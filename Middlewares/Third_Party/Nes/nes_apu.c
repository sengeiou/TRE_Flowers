#include "nes_apu.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程
//ALIENTEK STM32F407开发板
//NES APU 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/1
//版本：V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 


/* look up table madness */ 
#define  APU_OVERSAMPLE
#define  APU_VOLUME_DECAY(x)  ((x) -= ((x) >> 7))
//需要用到的汇编的代码及参数
u8 K6502_Read( u16 wAddr );	  //6502.s
extern u32 clocks;			  //6502.s
//noise lookups for both modes */
//噪音查找两种模式 */
#ifndef REALTIME_NOISE     //1
static char noise_long_lut[APU_NOISE_32K];//APU_NOISE_32K  0x7FFF
static char noise_short_lut[APU_NOISE_93];//APU_NOISE_93   93
#endif /* !REALTIME_NOISE */

/* vblank长度表用于矩形、三角形、噪音 */
static const u8 vbl_length[32] =
{
    5, 127,  10, 1,  19, 2,  40,  3,  80, 4,  30,  5,   7,  6,  13,  7,  
	6,   8,  12, 9,  24, 10, 48,  11, 96, 12, 36,  13,  8,  14, 16,  15
};

/* 矩形通道的频率限制 */
static const int freq_limit[8] =
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/* 噪声频率查找表 */
static const int noise_freq[16] =
{
     4,    8,   16,   32,   64,   96,  128,  160,
   202,  254,  380,  508,  762, 1016, 2034, 4068
};

/* DMC transfer freqs */
const int dmc_clocks[16] =
{
   428, 380, 340, 320, 286, 254, 226, 214,
   190, 160, 142, 128, 106,  85,  72,  54
};

/* pos /底片的比率为矩形波脉冲 */
static const int duty_lut[4] = { 2, 4, 8, 12 };


/*** Simple queue routines简单队列例程*/
#define  APU_QEMPTY()   (apu->q_head == apu->q_tail)

void apu_enqueue(apudata_t *d)
{
   apu->queue[apu->q_head] = *d;

   apu->q_head = (apu->q_head + 1) & APUQUEUE_MASK; 
}

apudata_t *apu_dequeue(void)
{
   int loc;

   loc = apu->q_tail;
   apu->q_tail = (apu->q_tail + 1) & APUQUEUE_MASK;

   return &apu->queue[loc];
}

void apu_setchan(int chan, u8 enabled)
{
   apu->mix_enable[chan] = enabled;
}

/* emulation of the 15-bit shift register the
** NES uses to generate pseudo-random series
** for the white noise channel
   模拟的15位移位寄存器
　　NES用来生成伪随机序列
　　白噪声信道
*/
#ifdef REALTIME_NOISE    //1
char shift_register15(u8 xor_tap)
{
   static int sreg = 0x4000;
   int bit0, tap, bit14;

   bit0 = sreg & 1;
   tap = (sreg & xor_tap) ? 1 : 0;
   bit14 = (bit0 ^ tap);
   sreg >>= 1;
   sreg |= (bit14 << 14);
   return (bit0 ^ 1);
}
#else /* !REALTIME_NOISE */
void shift_register15(char *buf, int count)
{
   static int sreg = 0x4000;
   int bit0, bit1, bit6, bit14;

   if (count == APU_NOISE_93)
   {
      while (count--)
      {
         bit0 = sreg & 1;
         bit6 = (sreg & 0x40) >> 6;
         bit14 = (bit0 ^ bit6);
         sreg >>= 1;
         sreg |= (bit14 << 14);
         *buf++ = bit0 ^ 1;
      }
   }
   else /* 32K noise */
   {
      while (count--)
      {
         bit0 = sreg & 1;
         bit1 = (sreg & 2) >> 1;
         bit14 = (bit0 ^ bit1);
         sreg >>= 1;
         sreg |= (bit14 << 14);
         *buf++ = bit0 ^ 1;
      }
   }
}
#endif /* !REALTIME_NOISE */

/* RECTANGLE WAVE
** ==============
** reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
** reg2: 8 bits of freq
** reg3: 0-2=high freq, 7-4=vbl length counter
*/
#define  APU_RECTANGLE_OUTPUT chan->output_vol
int apu_rectangle(rectangle_t *chan)
{
   int output;

#ifdef APU_OVERSAMPLE   //1
   int num_times;
   int total;
#endif /* APU_OVERSAMPLE */

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || 0 == chan->vbl_length)
      return APU_RECTANGLE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length--;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
#if 0
   chan->env_phase -= 4; /* 240/60 */
   while (chan->env_phase < 0)
   {
      chan->env_phase += chan->env_delay;

      if (chan->holdnote)
         chan->env_vol = (chan->env_vol + 1) & 0x0F;
      else if (chan->env_vol < 0x0F)
         chan->env_vol++;
   }
#endif
   {
	   int env_phase = chan->env_phase;
	   int env_delay = chan->env_delay;
	   int holdnote = chan->holdnote;
	   int env_vol = chan->env_vol;
	   env_phase -= 4; /* 240/60 */
	   while (env_phase < 0)
	   {
		   if(env_delay==0)break;//*******我加的*****************1943这个游戏是0
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }

   /* TODO: using a table of max frequencies is not technically
   ** clean, but it is fast and (or should be) accurate 
   */
   if (chan->freq < 8 || (FALSE == chan->sweep_inc && chan->freq > chan->freq_limit))
      return APU_RECTANGLE_OUTPUT;

   /* frequency sweeping at a rate of (sweep_delay + 1) / 120 secs */
   if (chan->sweep_on && chan->sweep_shifts)
   {
      chan->sweep_phase -= 2; /* 120/60 */
      while (chan->sweep_phase < 0)
      {
         chan->sweep_phase += chan->sweep_delay;

         if (chan->sweep_inc) /* ramp up */
         {
            if (TRUE == chan->sweep_complement)
               chan->freq += ~(chan->freq >> chan->sweep_shifts);
            else
               chan->freq -= (chan->freq >> chan->sweep_shifts);
         }
         else /* ramp down */
         {
            chan->freq += (chan->freq >> chan->sweep_shifts);
         }
      }
   }

   chan->phaseacc -= apu->cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return APU_RECTANGLE_OUTPUT;

#ifdef APU_OVERSAMPLE  //1
   num_times = total = 0;

   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;
#endif /* APU_OVERSAMPLE */

   while (chan->phaseacc < 0)
   {
      chan->phaseacc += APU_TO_FIXED(chan->freq + 1);
      chan->adder = (chan->adder + 1) & 0x0F;

#ifdef APU_OVERSAMPLE  //1
      if (chan->adder < chan->duty_flip)
         total += output;
      else
         total -= output;

      num_times++;
#endif /* APU_OVERSAMPLE */
   }

#ifdef APU_OVERSAMPLE   //1
   chan->output_vol = total / num_times;
#else /* !APU_OVERSAMPLE */
   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;

   if (0 == chan->adder)
      chan->output_vol = output;
   else if (chan->adder == chan->duty_flip)
      chan->output_vol = -output;
#endif /* !APU_OVERSAMPLE */

   return APU_RECTANGLE_OUTPUT;
}

/* TRIANGLE WAVE
** =============
** reg0: 7=holdnote, 6-0=linear length counter
** reg2: low 8 bits of frequency
** reg3: 7-3=length counter, 2-0=high 3 bits of frequency
*/
#define  APU_TRIANGLE_OUTPUT  (chan->output_vol + (chan->output_vol >> 2))
int apu_triangle(triangle_t *chan)
{
   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || 0 == chan->vbl_length)
      return APU_TRIANGLE_OUTPUT;

   if (chan->counter_started)
   {
      if (chan->linear_length > 0)
         chan->linear_length--;
      if (chan->vbl_length && FALSE == chan->holdnote)
         chan->vbl_length--;
   }
   else if (FALSE == chan->holdnote && chan->write_latency)
   {
      if (--chan->write_latency == 0)
         chan->counter_started = TRUE;
   }

   if (0 == chan->linear_length || chan->freq < APU_TO_FIXED(4)) /* inaudible */
      return APU_TRIANGLE_OUTPUT;

   chan->phaseacc -= apu->cycle_rate; /* # of cycles per sample */
   while (chan->phaseacc < 0)
   {
      chan->phaseacc += chan->freq;
      chan->adder = (chan->adder + 1) & 0x1F;

      if (chan->adder & 0x10)
         chan->output_vol -= (2 << 8);
      else
         chan->output_vol += (2 << 8);
   }

   return APU_TRIANGLE_OUTPUT;
}


/* WHITE NOISE CHANNEL
** ===================
** reg0: 0-3=volume, 4=envelope, 5=hold
** reg2: 7=small(93 byte) sample,3-0=freq lookup
** reg3: 7-4=vbl length counter
*/
#define  APU_NOISE_OUTPUT  ((chan->output_vol + chan->output_vol + chan->output_vol) >> 2)

int apu_noise(noise_t *chan)
{
   int outvol;

#if defined(APU_OVERSAMPLE) && defined(REALTIME_NOISE)
#else /* !(APU_OVERSAMPLE && REALTIME_NOISE) */
   int noise_bit;
#endif /* !(APU_OVERSAMPLE && REALTIME_NOISE) */
#ifdef APU_OVERSAMPLE
   int num_times;
   int total;
#endif /* APU_OVERSAMPLE */

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || 0 == chan->vbl_length)
      return APU_NOISE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length--;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
#if 0
   chan->env_phase -= 4; /* 240/60 */
   while (chan->env_phase < 0)
   {
      chan->env_phase += chan->env_delay;

      if (chan->holdnote)
         chan->env_vol = (chan->env_vol + 1) & 0x0F;
      else if (chan->env_vol < 0x0F)
         chan->env_vol++;
   }
#endif
   {
	   int env_phase = chan->env_phase;
	   int env_delay = chan->env_delay;
	   int holdnote = chan->holdnote;
	   int env_vol = chan->env_vol;
	   env_phase -= 4; /* 240/60 */
	   while (env_phase < 0)
	   {
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }


   chan->phaseacc -= apu->cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return APU_NOISE_OUTPUT;
   
#ifdef APU_OVERSAMPLE
   num_times = total = 0;

   if (chan->fixed_envelope)
      outvol = chan->volume << 8; /* fixed volume */
   else
      outvol = (chan->env_vol ^ 0x0F) << 8;
#endif /* APU_OVERSAMPLE */

   while (chan->phaseacc < 0)
   {
      
	   if(chan->freq==0)break;//*******我加的*****************这个游戏是0/////////////////////////
	   chan->phaseacc += chan->freq;

#ifdef REALTIME_NOISE

#ifdef APU_OVERSAMPLE
      if (shift_register15(chan->xor_tap))
         total += outvol;
      else
         total -= outvol;

      num_times++;
#else /* !APU_OVERSAMPLE */
      noise_bit = shift_register15(chan->xor_tap);
#endif /* !APU_OVERSAMPLE */

#else /* !REALTIME_NOISE */
      chan->cur_pos++;

      if (chan->short_sample)
      {
         if (APU_NOISE_93 == chan->cur_pos)
            chan->cur_pos = 0;
      }
      else
      {
         if (APU_NOISE_32K == chan->cur_pos)
            chan->cur_pos = 0;
      }

#ifdef APU_OVERSAMPLE
      if (chan->short_sample)
         noise_bit = noise_short_lut[chan->cur_pos];
      else
         noise_bit = noise_long_lut[chan->cur_pos];

      if (noise_bit)
         total += outvol;
      else
         total -= outvol;

      num_times++;
#endif /* APU_OVERSAMPLE */
#endif /* !REALTIME_NOISE */
   }

#ifdef APU_OVERSAMPLE
   chan->output_vol = total / num_times;
#else /* !APU_OVERSAMPLE */
   if (chan->fixed_envelope)
      outvol = chan->volume << 8; /* fixed volume */
   else
      outvol = (chan->env_vol ^ 0x0F) << 8;

#ifndef REALTIME_NOISE
   if (chan->short_sample)
      noise_bit = noise_short_lut[chan->cur_pos];
   else
      noise_bit = noise_long_lut[chan->cur_pos];
#endif /* !REALTIME_NOISE */

   if (noise_bit)
      chan->output_vol = outvol;
   else
      chan->output_vol = -outvol;
#endif /* !APU_OVERSAMPLE */

   return APU_NOISE_OUTPUT;
}


void apu_dmcreload(dmc_t *chan)
{
   chan->address = chan->cached_addr;
   chan->dma_length = chan->cached_dmalength;
   chan->irq_occurred = FALSE;
}

/* DELTA MODULATION CHANNEL
** =========================
** reg0: 7=irq gen, 6=looping, 3-0=pointer to clock table
** reg1: output dc level, 6 bits unsigned
** reg2: 8 bits of 64-byte aligned address offset : $C000 + (value * 64)
** reg3: length, (value * 16) + 1
*/
#define  APU_DMC_OUTPUT ((chan->output_vol + chan->output_vol + chan->output_vol) >> 2)
int apu_dmc(dmc_t *chan)
{
   int delta_bit;

   APU_VOLUME_DECAY(chan->output_vol);

   /* only process when channel is alive */
   if (chan->dma_length)
   {
      chan->phaseacc -= apu->cycle_rate; /* # of cycles per sample */
      
      while (chan->phaseacc < 0)
      {
         chan->phaseacc += chan->freq;
         
         delta_bit = (chan->dma_length & 7) ^ 7;
         
         if (7 == delta_bit)
         {
          chan->cur_byte =K6502_Read(chan->address);//chan->cur_byte = nes6502_getbyte(chan->address);*********************
            /* steal a cycle from CPU偷从CPU周期*/
            clocks++;  // nes6502_burn(1);//要CPU时钟数加1**********************************************************************

            if (0xFFFF == chan->address)
               chan->address = 0x8000;
            else
               chan->address++;
         }

         if (--chan->dma_length == 0)
         {
            /* if loop bit set, we're cool to retrigger sample */
            if (chan->looping)
               apu_dmcreload(chan);
            else
            {
               /* check to see if we should generate an irq */
               if (chan->irq_gen)
               {
                  chan->irq_occurred = TRUE;

               }

               /* bodge for timestamp queue */
               chan->enabled = FALSE;
               break;
            }
         }

         /* positive delta */
         if (chan->cur_byte & (1 << delta_bit))
         {
            if (chan->regs[1] < 0x7D)
            {
               chan->regs[1] += 2;
               chan->output_vol += (2 << 8);
            }
         }
         /* negative delta */
         else            
         {
            if (chan->regs[1] > 1)
            {
               chan->regs[1] -= 2;
               chan->output_vol -= (2 << 8);
            }
         }
      }
   }

   return APU_DMC_OUTPUT;
}


void apu_regwrite(u32 address, u8 value)
{  
   int chan = (address & 4) ? 1 : 0;
   rectangle_t * rect = chan ? &(apu->rectangle[1]) : &(apu->rectangle[0]);

   switch (address)
   {
   /* rectangles */
   case APU_WRA0:
   case APU_WRB0:
      /*
      chan = (address & 4) ? 1 : 0;
      apu->rectangle[chan].regs[0] = value;

      apu->rectangle[chan].volume = value & 0x0F;
      apu->rectangle[chan].env_delay = apu->decay_lut[value & 0x0F];
      apu->rectangle[chan].holdnote = (value & 0x20) ? TRUE : FALSE;
      apu->rectangle[chan].fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      apu->rectangle[chan].duty_flip = duty_lut[value >> 6];
      */
      rect->regs[0] = value;
	  rect->volume = value & 0x0F;
      rect->env_delay = apu->decay_lut[value & 0x0F];
      rect->holdnote = (value & 0x20) ? TRUE : FALSE;
      rect->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      rect->duty_flip = duty_lut[value >> 6];
      break;

   case APU_WRA1:
   case APU_WRB1:
      /*
      chan = (address & 4) ? 1 : 0;
      apu->rectangle[chan].regs[1] = value;
      apu->rectangle[chan].sweep_on = (value & 0x80) ? TRUE : FALSE;
      apu->rectangle[chan].sweep_shifts = value & 7;
      apu->rectangle[chan].sweep_delay = apu->decay_lut[(value >> 4) & 7];
      
      apu->rectangle[chan].sweep_inc = (value & 0x08) ? TRUE : FALSE;
      apu->rectangle[chan].freq_limit = freq_limit[value & 7];
	  */
      rect->regs[1] = value;
      rect->sweep_on = (value & 0x80) ? TRUE : FALSE;
      rect->sweep_shifts = value & 7;
      rect->sweep_delay = apu->decay_lut[(value >> 4) & 7];
      
      rect->sweep_inc = (value & 0x08) ? TRUE : FALSE;
      rect->freq_limit = freq_limit[value & 7];
      break;

   case APU_WRA2:
   case APU_WRB2:
      /*
      chan = (address & 4) ? 1 : 0;
      apu->rectangle[chan].regs[2] = value;
//      if (apu->rectangle[chan].enabled)
         apu->rectangle[chan].freq = (apu->rectangle[chan].freq & ~0xFF) | value;
      */
      rect->regs[2] = value;
         rect->freq = (apu->rectangle[chan].freq & ~0xFF) | value;
      break;

   case APU_WRA3:
   case APU_WRB3:
      /*
      chan = (address & 4) ? 1 : 0;
      apu->rectangle[chan].regs[3] = value;

      apu->rectangle[chan].vbl_length = apu->vbl_lut[value >> 3];
      apu->rectangle[chan].env_vol = 0;
      apu->rectangle[chan].freq = ((value & 7) << 8) | (apu->rectangle[chan].freq & 0xFF);
      apu->rectangle[chan].adder = 0;
      */
      rect->regs[3] = value;

      rect->vbl_length = apu->vbl_lut[value >> 3];
      rect->env_vol = 0;
      rect->freq = ((value & 7) << 8) | (rect->freq & 0xFF);
      rect->adder = 0;
      break;

   /* triangle */
   case APU_WRC0:
/*
      apu->triangle.regs[0] = value;
      apu->triangle.holdnote = (value & 0x80) ? TRUE : FALSE;

      if (FALSE == apu->triangle.counter_started && apu->triangle.vbl_length)
         apu->triangle.linear_length = apu->trilength_lut[value & 0x7F];
*/
	   {
		   triangle_t * tri = &(apu->triangle);
		   tri->regs[0] = value;
		   tri->holdnote = (value & 0x80) ? TRUE : FALSE;
		   
		   if (FALSE == tri->counter_started && tri->vbl_length)
			   tri->linear_length = apu->trilength_lut[value & 0x7F];
	   }
      break;

   case APU_WRC2:
/*
      apu->triangle.regs[1] = value;
      apu->triangle.freq = APU_TO_FIXED((((apu->triangle.regs[2] & 7) << 8) + value) + 1);
*/
	   {
		   triangle_t * tri = &(apu->triangle);
		   tri->regs[1] = value;
           tri->freq = APU_TO_FIXED((((tri->regs[2] & 7) << 8) + value) + 1);
	   }
      break;

   case APU_WRC3:
#if 0
      apu->triangle.regs[2] = value;
  
      /* this is somewhat of a hack.  there appears to be some latency on 
      ** the Real Thing between when trireg0 is written to and when the 
      ** linear length counter actually begins its countdown.  we want to 
      ** prevent the case where the program writes to the freq regs first, 
      ** then to reg 0, and the counter accidentally starts running because 
      ** of the sound queue's timestamp processing.
      **
      ** set latency to a couple hundred cycles -- should be plenty of time 
      ** for the 6502 code to do a couple of table dereferences and load up 
      ** the other triregs
      */

      /* 06/13/00 MPC -- seems to work OK */
      apu->triangle.write_latency = (int) (228 / APU_FROM_FIXED(apu->cycle_rate));

      apu->triangle.freq = APU_TO_FIXED((((value & 7) << 8) + apu->triangle.regs[1]) + 1);
      apu->triangle.vbl_length = apu->vbl_lut[value >> 3];
      apu->triangle.counter_started = FALSE;
      apu->triangle.linear_length = apu->trilength_lut[apu->triangle.regs[0] & 0x7F];
#endif
	   {
		   triangle_t * tri = &(apu->triangle);
		   tri->regs[2] = value;
		   tri->write_latency = (int) (228 / APU_FROM_FIXED(apu->cycle_rate));
		   
		   tri->freq = APU_TO_FIXED((((value & 7) << 8) + tri->regs[1]) + 1);
		   tri->vbl_length = apu->vbl_lut[value >> 3];
		   tri->counter_started = FALSE;
		   tri->linear_length = apu->trilength_lut[tri->regs[0] & 0x7F];
	   }
	  break;

   /* noise */
   case APU_WRD0:
/*
      apu->noise.regs[0] = value;
      apu->noise.env_delay = apu->decay_lut[value & 0x0F];
      apu->noise.holdnote = (value & 0x20) ? TRUE : FALSE;
      apu->noise.fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      apu->noise.volume = value & 0x0F;
*/
	   {
		   noise_t * noise = &(apu->noise);
		   noise->regs[0] = value;
		   noise->env_delay = apu->decay_lut[value & 0x0F];
		   noise->holdnote = (value & 0x20) ? TRUE : FALSE;
		   noise->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
		   noise->volume = value & 0x0F;
	   }
      break;

   case APU_WRD2:
#if 0
      apu->noise.regs[1] = value;
      apu->noise.freq = APU_TO_FIXED(noise_freq[value & 0x0F]);

#ifdef REALTIME_NOISE
      apu->noise.xor_tap = (value & 0x80) ? 0x40: 0x02;
#else /* !REALTIME_NOISE */
      /* detect transition from long->short sample */
      if ((value & 0x80) && FALSE == apu->noise.short_sample)
      {
         /* recalculate short noise buffer */
         shift_register15(noise_short_lut, APU_NOISE_93);
         apu->noise.cur_pos = 0;
      }
      apu->noise.short_sample = (value & 0x80) ? TRUE : FALSE;
#endif /* !REALTIME_NOISE */
#endif
	  {
		  noise_t * noise = &(apu->noise);
		  noise->regs[1] = value;
		  noise->freq = APU_TO_FIXED(noise_freq[value & 0x0F]);
		  
#ifdef REALTIME_NOISE
		  noise->xor_tap = (value & 0x80) ? 0x40: 0x02;
#else /* !REALTIME_NOISE */
		  /* detect transition from long->short sample */
		  if ((value & 0x80) && FALSE == noise->short_sample)
		  {
			  /* recalculate short noise buffer */
			  shift_register15(noise_short_lut, APU_NOISE_93);
			  noise->cur_pos = 0;
		  }
		  noise->short_sample = (value & 0x80) ? TRUE : FALSE;
#endif /* !REALTIME_NOISE */
	  }
      break;

   case APU_WRD3:
#if 0
      apu->noise.regs[2] = value;

      apu->noise.vbl_length = apu->vbl_lut[value >> 3];
      apu->noise.env_vol = 0; /* reset envelope */
#endif
	  {
		  noise_t * noise = &(apu->noise);
		  noise->regs[2] = value;
		  
		  noise->vbl_length = apu->vbl_lut[value >> 3];
		  noise->env_vol = 0; /* reset envelope */
	  }
      break;

   /* DMC */
   case APU_WRE0:
	   /*
      apu->dmc.regs[0] = value;

      apu->dmc.freq = APU_TO_FIXED(dmc_clocks[value & 0x0F]);
      apu->dmc.looping = (value & 0x40) ? TRUE : FALSE;

      if (value & 0x80)
         apu->dmc.irq_gen = TRUE;
      else
      {
         apu->dmc.irq_gen = FALSE;
         apu->dmc.irq_occurred = FALSE;
      }
	  */
	   {
		   dmc_t * dmc = &(apu->dmc);
		   
		   dmc->regs[0] = value;
		   
		   dmc->freq = APU_TO_FIXED(dmc_clocks[value & 0x0F]);
		   dmc->looping = (value & 0x40) ? TRUE : FALSE;
		   
		   if (value & 0x80)
			   dmc->irq_gen = TRUE;
		   else
		   {
			   dmc->irq_gen = FALSE;
			   dmc->irq_occurred = FALSE;
		   }
	   }
      break;

   case APU_WRE1: /* 7-bit DAC */
#if 0
      /* add the _delta_ between written value and
      ** current output level of the volume reg
      */
      value &= 0x7F; /* bit 7 ignored */
      apu->dmc.output_vol += ((value - apu->dmc.regs[1]) << 8);
      apu->dmc.regs[1] = value;
#endif
	  {
		  dmc_t * dmc = &(apu->dmc);
		  value &= 0x7F; /* bit 7 ignored */
		  dmc->output_vol += ((value - apu->dmc.regs[1]) << 8);
		  dmc->regs[1] = value;
	  }
      break;

   case APU_WRE2:
	   /*
      apu->dmc.regs[2] = value;
      apu->dmc.cached_addr = 0xC000 + (u16) (value << 6);
	  */
	   {
		   dmc_t * dmc = &(apu->dmc);
		   dmc->regs[2] = value;
		   dmc->cached_addr = 0xC000 + (u16) (value << 6);
	   }
      break;

   case APU_WRE3:
	   /*
      apu->dmc.regs[3] = value;
      apu->dmc.cached_dmalength = ((value << 4) + 1) << 3;
	  */
	   {
		   dmc_t * dmc = &(apu->dmc);
		   dmc->regs[3] = value;
		   dmc->cached_dmalength = ((value << 4) + 1) << 3;
	   }
      break;

   case APU_SMASK:
#if 0
      /* bodge for timestamp queue */
      apu->dmc.enabled = (value & 0x10) ? TRUE : FALSE;

      apu->enable_reg = value;

      for (chan = 0; chan < 2; chan++)
      {
         if (value & (1 << chan))
            apu->rectangle[chan].enabled = TRUE;
         else
         {
            apu->rectangle[chan].enabled = FALSE;
            apu->rectangle[chan].vbl_length = 0;
         }
      }

      if (value & 0x04)
         apu->triangle.enabled = TRUE;
      else
      {
         apu->triangle.enabled = FALSE;
         apu->triangle.vbl_length = 0;
         apu->triangle.linear_length = 0;
         apu->triangle.counter_started = FALSE;
         apu->triangle.write_latency = 0;
      }

      if (value & 0x08)
         apu->noise.enabled = TRUE;
      else
      {
         apu->noise.enabled = FALSE;
         apu->noise.vbl_length = 0;
      }

      if (value & 0x10)
      {
         if (0 == apu->dmc.dma_length)
            apu_dmcreload(&apu->dmc);
      }
      else
         apu->dmc.dma_length = 0;

      apu->dmc.irq_occurred = FALSE;
#endif
	  {
		  dmc_t * dmc = &(apu->dmc);
		  triangle_t * tri = &(apu->triangle);
		  noise_t * noise = &(apu->noise);

		  dmc->enabled = (value & 0x10) ? TRUE : FALSE;
		  
		  apu->enable_reg = value;
		  
		  for (chan = 0; chan < 2; chan++)
		  {
			  if (value & (1 << chan))
				  apu->rectangle[chan].enabled = TRUE;
			  else
			  {
				  apu->rectangle[chan].enabled = FALSE;
				  apu->rectangle[chan].vbl_length = 0;
			  }
		  }
		  
		  if (value & 0x04)
			  tri->enabled = TRUE;
		  else
		  {
			  tri->enabled = FALSE;
			  tri->vbl_length = 0;
			  tri->linear_length = 0;
			  tri->counter_started = FALSE;
			  tri->write_latency = 0;
		  }
		  
		  if (value & 0x08)
			  noise->enabled = TRUE;
		  else
		  {
			  noise->enabled = FALSE;
			  noise->vbl_length = 0;
		  }
		  
		  if (value & 0x10)
		  {
			  if (0 == dmc->dma_length)
				  apu_dmcreload(&apu->dmc);
		  }
		  else
			  dmc->dma_length = 0;
		  
		  dmc->irq_occurred = FALSE;
	  }
      break;

      /* unused, but they get hit in some mem-clear loops */
   case 0x4009:
   case 0x400D:
      break;
   
   default:
      //DCR
      if(apu->ext)
      {
        apu_memwrite* mw = apu->ext->mem_write;

        while(mw->max_range != -1)//整数转换导致改变的迹象
        {
          if((mw->min_range <= address) && ((mw->max_range >= address)))
          {
            mw->write_func(address, value);
            break;
          }
          mw++;
        }
      }
      break;
   }
}

/* Read from $4000-$4017 */
u8 Apu_Read4015(u32 address)//***********************************************************************
{
   uint8 value=0;
	
#ifdef FRAME_IRQ
	if(!(frame_irq_enabled & 0xC0))  
#endif   
	{
		/* Return 1 in 0-5 bit pos if a channel is playing */
		if (apu->rectangle[0].enabled && apu->rectangle[0].vbl_length)value |= 0x01; 
		if (apu->rectangle[1].enabled && apu->rectangle[1].vbl_length)value |= 0x02; 
		if (apu->triangle.enabled && apu->triangle.vbl_length)value |= 0x04; 
		if (apu->noise.enabled && apu->noise.vbl_length)value |= 0x08;  
		/* bodge for timestamp queue */
		if (apu->dmc.enabled)value |= 0x10;  
		if (apu->dmc.irq_occurred)value |= 0x80;   
	} 
#ifdef FRAME_IRQ   
	return value | 0x40;
#else
	return value;
#endif   
}
void Apu_Write4017(uint8 value,uint32 address )
{	
#ifdef FRAME_IRQ
	apudata_t d;
	
    if(!frame_irq_disenabled)
	{
      frame_irq_enabled = value;
	}
	if(apu->ext)
      {
        apu_memwrite* mw = apu->ext->mem_write;

        while(mw->max_range != -1)
        {
          if((mw->min_range <= address) && ((mw->max_range >= address)))
          {
            d.timestamp = clocks;//d.timestamp = nes6502_getcycles(FALSE);
            d.address = address|0x4000;
            d.value = value;
            apu_enqueue(&d);
            break;
          }
          mw++;
        }
      }	
#endif
}

void Apu_Write4015(u8 value,u32 address )
{
	apudata_t d;
	apu->dmc.enabled = (value & 0x10) ? TRUE : FALSE;
	d.timestamp = clocks;  //d.timestamp = nes6502_getcycles(0);*********************
	d.address = address|0x4000;
	d.value = value;
	apu_enqueue(&d);	
}

void Apu_Write(u8 value,u32 address )
{
	apudata_t d; 
	d.timestamp = clocks;  //d.timestamp = nes6502_getcycles(0);*********************
	d.address = address|0x4000;
	d.value = value;
	apu_enqueue(&d); 
} 
void apu_getpcmdata(void **data, int *num_samples, int *sample_bits)
{
   *data = apu->buffer;
   *num_samples = apu->num_samples;
   *sample_bits = apu->sample_bits;
}
//                              _local_sample_size = 8 
//    apu_process(buf, buf_len/(_local_sample_size/8));
void apu_process(u8 *buffer, int num_samples)
{
   apudata_t *d;
   u32 elapsed_cycles;
   static int prev_sample = 0;
   int next_sample, accum;  
	
   /* grab it, keep it local for speed */
   elapsed_cycles = (u32) apu->elapsed_cycles;

   if (NULL == buffer)
   {
      /* just go through the motions... */
      while (num_samples--)
      {
         while ((FALSE == APU_QEMPTY()) && (apu->queue[apu->q_tail].timestamp <= elapsed_cycles))
         {
            d = apu_dequeue();
            apu_regwrite(d->address, d->value);
         }
         elapsed_cycles += APU_FROM_FIXED(apu->cycle_rate);
      }
   }
   else
   {
	  // Rick
	  u8 * mix_enable = apu->mix_enable;

      /* bleh */
      apu->buffer = buffer; 

      while (num_samples--)
      {
         while ((FALSE == APU_QEMPTY()) && (apu->queue[apu->q_tail].timestamp <= elapsed_cycles))
         {
            d = apu_dequeue();
            apu_regwrite(d->address, d->value);
         }

         elapsed_cycles += APU_FROM_FIXED(apu->cycle_rate);

         accum = 0;
         if (mix_enable[0]) accum += apu_rectangle(&apu->rectangle[0]);
         if (mix_enable[1]) accum += apu_rectangle(&apu->rectangle[1]);
         if (mix_enable[2]) accum += apu_triangle(&apu->triangle);
         if (mix_enable[3]) accum += apu_noise(&apu->noise);
         if (mix_enable[4]) accum += apu_dmc(&apu->dmc);

         if (apu->ext && mix_enable[5]) accum += apu->ext->process();

         /* do any filtering */
         if (APU_FILTER_NONE != apu->filter_type)
         {
            next_sample = accum;

            if (APU_FILTER_LOWPASS == apu->filter_type)
            {
               accum += prev_sample;
               accum >>= 1;
            }
            else
               accum = (accum + accum + accum + prev_sample) >> 2;

            prev_sample = next_sample;
         }
         /* little extra kick for the kids */
         accum <<= 1;

         /* prevent clipping */
         if (accum > 0x7FFF)accum = 0x7FFF;
         else if (accum < -0x8000)accum = -0x8000;
		 
				 *buffer++ = (accum >> 8) ^ 0x80;//音频数据存入缓冲
         //*buffer++=(u16)accum;//音频数据存入缓冲
      }
   } 
   /* resync cycle counter 重新同步循环计数器*/
   apu->elapsed_cycles =clocks; // apu->elapsed_cycles = nes6502_getcycles(0);//*****************************************************************
}


void apu_reset(void)
{
	u32 address;

	apu->elapsed_cycles = 0;
	memset(&apu->queue, 0, APUQUEUE_SIZE * sizeof(apudata_t));
	apu->q_head = apu->q_tail = 0;

	/* use to avoid bugs =) */
	for (address = 0x00; address <= 0x13; address++)apu_regwrite(address, 0);
	apu_regwrite(0x15, 0x00);
	if (apu->ext)apu->ext->reset();
}

void apu_build_luts(int num_samples)
{
   int i;

   /* lut used for enveloping and frequency sweeps */
   for (i = 0; i < 16; i++)
      apu->decay_lut[i] = num_samples * (i + 1);

   /* used for note length, based on vblanks and size of audio buffer */
   for (i = 0; i < 32; i++)
      apu->vbl_lut[i] = vbl_length[i] * num_samples;

   /* triangle wave channel's linear length table */
   for (i = 0; i < 128; i++)
      apu->trilength_lut[i] = (int) (0.25 * (i * num_samples));

#ifndef REALTIME_NOISE
   /* generate noise samples */
   shift_register15(noise_long_lut, APU_NOISE_32K);
   shift_register15(noise_short_lut, APU_NOISE_93);
#endif /* !REALTIME_NOISE */
}


void apu_setparams(int sample_rate, int refresh_rate, int frag_size, int sample_bits)
{
   apu->sample_rate = sample_rate;
   apu->refresh_rate = refresh_rate;
   apu->sample_bits = sample_bits;

   apu->num_samples = sample_rate / refresh_rate;
   frag_size = frag_size; /* quell warnings 平息警告*/

   /* turn into fixed point! */
   apu->cycle_rate = (int) (APU_BASEFREQ * 65536.0 / (float) sample_rate);

   /* build various lookup tables for apu */
   apu_build_luts(apu->num_samples);

   if (apu->ext)
      apu->ext->paramschanged();
}
/*硬件初始化模拟声音,创建波形/声音 */
//      _local_sample_rate = 11025;                               _local_sample_size = 8;  
//  apu_create(_local_sample_rate, 60,               0,             _local_sample_size);
//void apu_create(int sample_rate, int refresh_rate, int frag_size, int sample_bits)
void apu_init(void)	
{
	int channel; 
	memset(apu, 0, sizeof(apu_t));
	/* set the stupid flag to tell difference between two rectangles */
	apu->rectangle[0].sweep_complement = TRUE;
	apu->rectangle[1].sweep_complement = FALSE;
	apu->ext = NULL;
	//apu_setparams(sample_rate, refresh_rate, frag_size, sample_bits);
	apu_setparams(APU_SAMPLE_RATE,60,0,8);//APU_SAMPLE_RATE        //采样率
	apu_reset();
	for(channel=0;channel<6;channel++)apu_setchan(channel,TRUE);
	apu->filter_type=APU_FILTER_LOWPASS;	//设置筛选器类型 
}
//apu声音输出
void apu_soundoutput(void)          
{	 
	u16 i;
	apu_process(wave_buffers,APU_PCMBUF_SIZE);
	for(i=0;i<30;i++)if(wave_buffers[i]!=wave_buffers[i+1])break;//判断前30个数据,是不是都相等?
	if(i==30&&wave_buffers[i])//都相等,且不等于0
	{
		for(i=0;i<APU_PCMBUF_SIZE;i++)wave_buffers[i]=0;//是暂停状态输出的重复无效数据,直接修改为0.从而不输出杂音.		
	}
	clocks=0;
	nes_apu_fill_buffer(0,(u8*)wave_buffers);
}


