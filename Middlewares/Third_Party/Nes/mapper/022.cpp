#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 22
void MAP22_Reset()
{
  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void MAP22_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x9000:
      {
        data &= 0x03;
        if(data == 0)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 1)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 2)
        {
          set_mirroring(1,1,1,1);
        }
        else
        {
          set_mirroring(0,0,0,0);
        }
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0xB000:
      {
        set_PPU_bank0(data >> 1);
      }
      break;

    case 0xB001:
      {
        set_PPU_bank1(data >> 1);
      }
      break;

    case 0xC000:
      {
        set_PPU_bank2(data >> 1);
      }
      break;

    case 0xC001:
      {
        set_PPU_bank3(data >> 1);
      }
      break;

    case 0xD000:
      {
        set_PPU_bank4(data >> 1);
      }
      break;

    case 0xD001:
      {
        set_PPU_bank5(data >> 1);
      }
      break;

    case 0xE000:
      {
        set_PPU_bank6(data >> 1);
      }
      break;

    case 0xE001:
      {
        set_PPU_bank7(data >> 1);
      }
      break;
  }
}
void MAP22_Init()
{
	NES_Mapper->Reset = MAP22_Reset;
	NES_Mapper->Write = MAP22_MemoryWrite; 
}


















