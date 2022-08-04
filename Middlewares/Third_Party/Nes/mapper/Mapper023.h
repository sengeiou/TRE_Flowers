#ifndef _MAPPER023_H_
#define _MAPPER023_H_
#include "nes_mapper.h" 

Mapper23Res *MAP23;

void Mapper023_Reset()
{
//	patch = 0xFFFF;

//  if(parent_NES->crc32() == 0x93794634) // Akumajou Special - Boku Dracula Kun
//  {
//    patch = 0xF00C;
//  }	
	// set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  MAP23->regs[0] = 0;
  MAP23->regs[1] = 1;
  MAP23->regs[2] = 2;
  MAP23->regs[3] = 3;
  MAP23->regs[4] = 4;
  MAP23->regs[5] = 5;
  MAP23->regs[6] = 6;
  MAP23->regs[7] = 7;
  MAP23->regs[8] = 0;

  MAP23->irq_enabled = 0;
  MAP23->irq_counter = 0;
  MAP23->irq_latch = 0;
}
void Mapper023_Write( uint8 data,uint16 addr)
{
	switch(addr /*& patch*/)
  {
    case 0x8000:
    case 0x8004:
    case 0x8008:
    case 0x800C:
      {
        if(MAP23->regs[8])
        {
          set_cpu_bank2(data);
        }
        else
        {
          set_cpu_bank0(data);
        }
      }
      break;

    case 0x9000:
      {
        if(data != 0xFF)
        {
          data &= 0x03;
          if(data == 0)
          {
 //           set_mirroring(NES_PPU::MIRROR_VERT);
			  PPU_set_mirroring(0,1,0,1);//´¹Ö±¾µÏñ;   //0101
          }
          else if(data == 1)
          {
 //           set_mirroring(NES_PPU::MIRROR_HORIZ);
			  PPU_set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñ  //0011			  
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
      }
      break;

    case 0x9008:
      {
        MAP23->regs[8] = data & 0x02;
      }
      break;

    case 0xA000:
    case 0xA004:
    case 0xA008:
    case 0xA00C:
      {
        set_cpu_bank1(data); 		  
      }
      break;

    case 0xB000:
      {
        MAP23->regs[0] = (MAP23->regs[0] & 0xF0) | (data & 0x0F);
        set_PPU_bank0(MAP23->regs[0]);
      }
      break;

    case 0xB001:
    case 0xB004:
      {
        MAP23->regs[0] = (MAP23->regs[0] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank0(MAP23->regs[0]);
      }
      break;

    case 0xB002:
    case 0xB008:
      {
        MAP23->regs[1] = (MAP23->regs[1] & 0xF0) | (data & 0x0F);
        set_PPU_bank1(MAP23->regs[1]);
      }
      break;

    case 0xB003:
    case 0xB00C:
      {
        MAP23->regs[1] = (MAP23->regs[1] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank1(MAP23->regs[1]);
      }
      break;

    case 0xC000:
      {
        MAP23->regs[2] = (MAP23->regs[2] & 0xF0) | (data & 0x0F);
        set_PPU_bank2(MAP23->regs[2]);
      }
      break;

    case 0xC001:
    case 0xC004:
      {
        MAP23->regs[2] = (MAP23->regs[2] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank2(MAP23->regs[2]);
      }
      break;

    case 0xC002:
    case 0xC008:
      {
        MAP23->regs[3] = (MAP23->regs[3] & 0xF0) | (data & 0x0F);
        set_PPU_bank3(MAP23->regs[3]);
      }
      break;

    case 0xC003:
    case 0xC00C:
      {
        MAP23->regs[3] = (MAP23->regs[3] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank3(MAP23->regs[3]);
      }
      break;

    case 0xD000:
      {
        MAP23->regs[4] = (MAP23->regs[4] & 0xF0) | (data & 0x0F);
        set_PPU_bank4(MAP23->regs[4]);
      }
      break;

    case 0xD001:
    case 0xD004:
      {
        MAP23->regs[4] = (MAP23->regs[4] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank4(MAP23->regs[4]);
      }
      break;

    case 0xD002:
    case 0xD008:
      {
        MAP23->regs[5] = (MAP23->regs[5] & 0xF0) | (data & 0x0F);
        set_PPU_bank5(MAP23->regs[5]);
      }
      break;

    case 0xD003:
    case 0xD00C:
      {
        MAP23->regs[5] = (MAP23->regs[5] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank5(MAP23->regs[5]);
      }
      break;

    case 0xE000:
      {
        MAP23->regs[6] = (MAP23->regs[6] & 0xF0) | (data & 0x0F);
        set_PPU_bank6(MAP23->regs[6]);
      }
      break;

    case 0xE001:
    case 0xE004:
      {
        MAP23->regs[6] = (MAP23->regs[6] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank6(MAP23->regs[6]);
      }
      break;

    case 0xE002:
    case 0xE008:
      {
        MAP23->regs[7] = (MAP23->regs[7] & 0xF0) | (data & 0x0F);
        set_PPU_bank7(MAP23->regs[7]);
      }
      break;

    case 0xE003:
    case 0xE00C:
      {
        MAP23->regs[7] = (MAP23->regs[7] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank7(MAP23->regs[7]);
      }
      break;

    case 0xF000:
      {
        MAP23->irq_latch = (MAP23->irq_latch & 0xF0) | (data & 0x0F);
      }
      break;

    case 0xF004:
      {
        MAP23->irq_latch = (MAP23->irq_latch & 0x0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xF008:
      {
        MAP23->irq_enabled = data & 0x03;
        if(MAP23->irq_enabled & 0x02)
        {
          MAP23->irq_counter = MAP23->irq_latch;
        }
      }
      break;

    case 0xF00C:
      {
        MAP23->irq_enabled = (MAP23->irq_enabled & 0x01) * 3;
      }
      break;
  }
}
void Mapper023_HSync( int scanline )
{
  if(MAP23->irq_enabled & 0x02)
  {
    if(MAP23->irq_counter == 0xFF)
    {
      CPU_IRQ;//parent_NES->cpu->DoIRQ();
      MAP23->irq_counter = MAP23->irq_latch;
      MAP23->irq_enabled = (MAP23->irq_enabled & 0x01) * 3;
    }
    else
    {
      MAP23->irq_counter++;
    }
  }
}
void Mapper023_Init()
{
	NES_Mapper->Reset = Mapper023_Reset;
	NES_Mapper->Write = Mapper023_Write;
	NES_Mapper->HSync = Mapper023_HSync;
}

#endif

