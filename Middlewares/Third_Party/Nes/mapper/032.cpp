#include "nes_mapper.h"


u8 map32_regs[1];

// Mapper 32
void MAP32_Reset()
{
  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

//  patch = 0;

//  if(parent_NES->crc32() == 0xc0fed437) // Major League
//  {
//    set_mirroring(0,0,0,0);
//    patch = 1;
//  }

//  if(parent_NES->crc32() == 0xfd3fc292) // Ai Sensei no Oshiete - Watashi no Hoshi
//  {
//    set_CPU_banks(30,31,30,31);
//  }

  map32_regs[0] = 0;
}

void MAP32_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF000)
  {
    case 0x8000:
      {
        if(map32_regs[0] & 0x02)
        {
          set_CPU_bank6(data);
        }
        else
        {
          set_CPU_bank4(data);
        }
      }
      break;

    case 0x9000:
      {
        if(data & 0x01)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        map32_regs[0] = data;
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;
  }

  switch(addr & 0xF007)
  {
    case 0xB000:
      {
        set_PPU_bank0(data);
      }
      break;

    case 0xB001:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0xB002:
      {
        set_PPU_bank2(data);
      }
      break;

    case 0xB003:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0xB004:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0xB005:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0xB006:
      {
//        if ((patch == 1) && (data & 0x40))
//        {
//          set_mirroring(0,0,0,1);
//        }
        set_PPU_bank6(data);
      }
      break;

    case 0xB007:
      {
//        if ((patch == 1) && (data & 0x40))
//        {
//          set_mirroring(0,0,0,0);
//        }
        set_PPU_bank7(data);
      }
      break;
  }
}
void MAP32_Init()
{
	NES_Mapper->Reset = MAP32_Reset;
	NES_Mapper->Write = MAP32_MemoryWrite; 
}






















