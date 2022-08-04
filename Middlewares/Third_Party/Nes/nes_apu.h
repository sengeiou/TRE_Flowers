#ifndef __NES_APU_H
#define __NES_APU_H 
#include "nes_main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程
//ALIENTEK STM32F407开发板
//NES APU 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/1
//版本：V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 

#define NES_APU_BUF_NUM			20						//6个APU BUF
#define APU_SAMPLE_RATE 		22050					//APU音频输出为22.050Khz 
#define APU_PCMBUF_SIZE  		APU_SAMPLE_RATE/60		//44100/60=735		22050/60=367  11025/60=183


#undef   NULL
#define  NULL 0  
#ifndef  TRUE
#define  TRUE     1
#endif
#ifndef  FALSE
#define  FALSE    0
#endif

/* define this for realtime generated noise */
#define  REALTIME_NOISE

#define  APU_WRA0       0x4000
#define  APU_WRA1       0x4001
#define  APU_WRA2       0x4002
#define  APU_WRA3       0x4003
#define  APU_WRB0       0x4004
#define  APU_WRB1       0x4005
#define  APU_WRB2       0x4006
#define  APU_WRB3       0x4007
#define  APU_WRC0       0x4008
#define  APU_WRC2       0x400A
#define  APU_WRC3       0x400B
#define  APU_WRD0       0x400C
#define  APU_WRD2       0x400E
#define  APU_WRD3       0x400F
#define  APU_WRE0       0x4010
#define  APU_WRE1       0x4011
#define  APU_WRE2       0x4012
#define  APU_WRE3       0x4013

#define  APU_SMASK      0x4015

/* length of generated noise */
#define  APU_NOISE_32K  0x7FFF
#define  APU_NOISE_93   93

#define  APU_BASEFREQ   1789772.7272727272727272

/* to/from 16.16 fixed point */
#define  APU_TO_FIXED(x)    ((x) << 16)
#define  APU_FROM_FIXED(x)  ((x) >> 16)


/* channel structures */
/* As much data as possible is precalculated,
** to keep the sample processing as lean as possible
*/
 
typedef struct rectangle_s
{
	u8 regs[4];

	u8 enabled;

	int phaseacc;
	int freq;
	int output_vol;
	u8 fixed_envelope;
	u8 holdnote;
	u8 volume;

	int sweep_phase;
	int sweep_delay;
	u8 sweep_on;
	u8 sweep_shifts;
	u8 sweep_length;
	u8 sweep_inc;

	/* this may not be necessary in the future */
	int freq_limit;

	/* rectangle 0 uses a complement addition for sweep
	** increases, while rectangle 1 uses subtraction
	*/
	u8 sweep_complement;

	int env_phase;
	int env_delay;
	u8 env_vol;

	int vbl_length;
	u8 adder;
	int duty_flip;
} rectangle_t;

typedef struct triangle_s
{
	u8 regs[3];

	u8 enabled;

	int freq;
	int phaseacc;
	int output_vol;

	u8 adder;

	u8 holdnote;
	u8 counter_started;
	/* quasi-hack */
	int write_latency;

	int vbl_length;
	int linear_length;
} triangle_t;
typedef struct noise_s
{
	u8 regs[3];

	u8 enabled;

	int freq;
	int phaseacc;
	int output_vol;

	int env_phase;
	int env_delay;
	u8 env_vol;
	u8 fixed_envelope;
	u8 holdnote;

	u8 volume;

	int vbl_length;

#ifdef REALTIME_NOISE
	u8 xor_tap;
#else
	u8 short_sample;
	int cur_pos;
#endif /* REALTIME_NOISE */
} noise_t;

typedef struct dmc_s
{
	u8 regs[4];

	/* bodge for timestamp queue */
	u8 enabled;

	int freq;
	int phaseacc;
	int output_vol;

	u32 address;
	u32 cached_addr;
	int dma_length;
	int cached_dmalength;
	u8 cur_byte;

	u8 looping;
	u8 irq_gen;
	u8 irq_occurred; 
} dmc_t;

enum        //定义枚举变量
{
	APU_FILTER_NONE,
	APU_FILTER_LOWPASS,
	APU_FILTER_WEIGHTED
};

typedef struct
{
	//u32 min_range, max_range;
	signed long int min_range, max_range;
	u8 (*read_func)(u32 address);
} apu_memread;

typedef struct
{
	//u32 min_range, max_range;
	signed long int min_range, max_range;
	void (*write_func)(u32 address, u8 value);
} apu_memwrite;

/* external sound chip stuff */
typedef struct apuext_s
{
	void  (*init)(void);
	void  (*shutdown)(void);
	void  (*reset)(void);
	//DCR
	void  (*paramschanged)(void);
	int (*process)(void);
	apu_memread *mem_read;
	apu_memwrite *mem_write;
} apuext_t; 

/* APU queue structure */
#define  APUQUEUE_SIZE  256  //4096
#define  APUQUEUE_MASK  (APUQUEUE_SIZE - 1)

/* apu ring buffer member */
typedef struct apudata_s
{
	u32 timestamp, address;
	u8 value;
} apudata_t;


typedef struct apu_s
{
	rectangle_t rectangle[2];
	triangle_t triangle;
	noise_t noise;
	dmc_t dmc;
	u8 enable_reg;

	apudata_t queue[APUQUEUE_SIZE];//APUQUEUE_SIZE
	int q_head, q_tail;
	u32 elapsed_cycles;

	void *buffer; /* pointer to output buffer输出缓冲区指针 */
	int num_samples;

	u8 mix_enable[6];
	int filter_type;

	int cycle_rate;

	int sample_rate;
	int sample_bits;
	int refresh_rate;

	int decay_lut[16];
	int vbl_lut[32];
	int trilength_lut[128];
	/* external sound chip 外部声音芯片*/
	apuext_t *ext;
} apu_t;

extern apu_t *apu;
extern u8 *wave_buffers; 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern void apu_init(void);
extern void apu_setparams(int sample_rate, int refresh_rate, int frag_size,int sample_bits);
extern void apu_process(u8 *buffer, int num_samples);
extern void apu_reset(void);
	
extern u8 Apu_Read4015(u32 address);
extern void Apu_Write(u8 value,u32 address);
	
void apu_soundoutput(void);
void apu_write4017(u32 address, u8 value);  
#ifdef __cplusplus
}
#endif  
#endif  





