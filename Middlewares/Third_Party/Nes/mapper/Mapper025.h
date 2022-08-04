#ifndef _MAPPER025_H_
#define _MAPPER025_H_
#include "nes_mapper.h" 


Mapper25Res *MAP25;

void Mapper025_Reset()
{
	
	uint8 i;
//	patch = 0;

//  if(parent_NES->crc32() == 0xea74c587 || // Teenage Mutant Ninja Turtles
//     parent_NES->crc32() == 0x5f82cb7d || // Teenage Mutant Ninja Turtles 2
//     parent_NES->crc32() == 0xa2e68da8)   // Racer Mini Yonku - Japan Cup
//  {
//    patch = 1;
//  }

  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(VROM_1K_SIZE)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  for( i = 0; i < 8; i++)
    MAP25->regs[i] = 0;

  MAP25->regs[8] = 0;
  MAP25->regs[9] = VROM_8K_SIZE-2;
  MAP25->regs[10] = 0;

  MAP25->irq_enabled = 0;
  MAP25->irq_counter = 0;
  MAP25->irq_latch = 0;
}
void Mapper025_Write( uint8 data, uint16 addr)
{
	switch(addr & 0xF000)
  {
    case 0x8000:
      {
        if(MAP25->regs[10] & 0x02)
        {
          MAP25->regs[9] = data;
          set_cpu_bank2(data);
        }
        else
        {
          MAP25->regs[8] = data;
          set_cpu_bank0(data);			
        }
      }
      break;

    case 0xA000:
      {
        set_cpu_bank1(data);
      }
      break;
  }

  switch(addr & 0xF00F)
  {
    case 0x9000:
      {
        data &= 0x03;
        if(data == 0)
        {
 //         set_mirroring(NES_PPU::MIRROR_VERT);
			PPU_set_mirroring(0,1,0,1);//´¹Ö±¾µÏñ
        }
        else if(data == 1)
        {
 //         set_mirroring(NES_PPU::MIRROR_HORIZ);
			PPU_set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñ
        }
        else if(data == 2)
        {
          PPU_set_mirroring(0,0,0,0);
        }
        else
        {
          PPU_set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x9001:
    case 0x9004:
      {
        if((MAP25->regs[10] & 0x02) != (data & 0x02))
        {
          uint8 swap = MAP25->regs[8];
          MAP25->regs[8] = MAP25->regs[9];
          MAP25->regs[9] = swap;
          set_cpu_bank0(MAP25->regs[8]);
          set_cpu_bank2(MAP25->regs[9]);							       
							
        }
        MAP25->regs[10] = data;
      }
      break;

    case 0xB000:
      {
        MAP25->regs[0] = (MAP25->regs[0] & 0xF0) | (data & 0x0F);
        set_PPU_bank0(MAP25->regs[0]);
      }
      break;

    case 0xB001:
    case 0xB004:
      {
        MAP25->regs[1] = (MAP25->regs[1] & 0xF0) | (data & 0x0F);
        set_PPU_bank1(MAP25->regs[1]);
      }
      break;

    case 0xB002:
    case 0xB008:
      {
        MAP25->regs[0] = (MAP25->regs[0] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank0(MAP25->regs[0]);
      }
      break;

    case 0xB003:
    case 0xB00C:
      {
        MAP25->regs[1] = (MAP25->regs[1] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank1(MAP25->regs[1]);
      }
      break;

    case 0xC000:
      {
        MAP25->regs[2] = (MAP25->regs[2] & 0xF0) | (data & 0x0F);
        set_PPU_bank2(MAP25->regs[2]);
      }
      break;

    case 0xC001:
    case 0xC004:
      {
        MAP25->regs[3] = (MAP25->regs[3] & 0xF0) | (data & 0x0F);
        set_PPU_bank3(MAP25->regs[3]);
      }
      break;

    case 0xC002:
    case 0xC008:
      {
        MAP25->regs[2] = (MAP25->regs[2] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank2(MAP25->regs[2]);
      }
      break;

    case 0xC003:
    case 0xC00C:
      {
        MAP25->regs[3] = (MAP25->regs[3] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank3(MAP25->regs[3]);
      }
      break;

    case 0xD000:
      {
        MAP25->regs[4] = (MAP25->regs[4] & 0xF0) | (data & 0x0F);
        set_PPU_bank4(MAP25->regs[4]);
      }
      break;

    case 0xD001:
    case 0xD004:
      {
        MAP25->regs[5] = (MAP25->regs[5] & 0xF0) | (data & 0x0F);
        set_PPU_bank5(MAP25->regs[5]);
      }
      break;

    case 0xD002:
    case 0xD008:
      {
        MAP25->regs[4] = (MAP25->regs[4] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank4(MAP25->regs[4]);
      }
      break;

    case 0xD003:
    case 0xD00C:
      {
        MAP25->regs[5] = (MAP25->regs[5] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank5(MAP25->regs[5]);
      }
      break;

    case 0xE000:
      {
        MAP25->regs[6] = (MAP25->regs[6] & 0xF0) | (data & 0x0F);
        set_PPU_bank6(MAP25->regs[6]);
      }
      break;

    case 0xE001:
    case 0xE004:
      {
        MAP25->regs[7] = (MAP25->regs[7] & 0xF0) | (data & 0x0F);
        set_PPU_bank7(MAP25->regs[7]);
      }
      break;

    case 0xE002:
    case 0xE008:
      {
        MAP25->regs[6] = (MAP25->regs[6] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank6(MAP25->regs[6]);
      }
      break;

    case 0xE003:
    case 0xE00C:
      {
        MAP25->regs[7] = (MAP25->regs[7] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank7(MAP25->regs[7]);
      }
      break;

    case 0xF000:
      {
        MAP25->irq_latch = (MAP25->irq_latch & 0xF0) | (data & 0x0F);
      }
      break;

    case 0xF001:
    case 0xF004:
      {
        MAP25->irq_enabled = data & 0x03;
        if(MAP25->irq_enabled & 0x02)
        {
          MAP25->irq_counter = MAP25->irq_latch;
        }
      }
      break;

    case 0xF002:
    case 0xF008:
      {
        MAP25->irq_latch = (MAP25->irq_latch & 0x0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xF003:
    case 0xF00C:
      {
        MAP25->irq_enabled = (MAP25->irq_enabled & 0x01) * 3;
      }
      break;
  }
}
void Mapper025_HSync( int scanline )
{	
	
  if(MAP25->irq_enabled & 0x02)
  {
    if((MAP25->irq_counter == 0xFF)||( MAP25->irq_counter == 0x00))
    {
      MAP25->irq_counter = MAP25->irq_latch;
      CPU_IRQ;//     MAP25->parent_NES->cpu->DoIRQ();
    }
    else
    {
      MAP25->irq_counter++;
    }
  }
}
void Mapper025_Init()
{
	NES_Mapper->Reset = Mapper025_Reset;
	NES_Mapper->Write = Mapper025_Write;
	NES_Mapper->HSync = Mapper025_HSync;
}

#endif
