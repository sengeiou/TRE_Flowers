#include "nes_mapper.h"

// Mapper 80
void MAP80_Reset()
{
//  patch = 0;

//  if(parent_NES->crc32() == 0x9832d15a)   // Fudou Myouou Den (J).nes
//  {
//    // for Hudoumyouou Den
//    patch = 1;
//  }

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void MAP80_MemoryWriteSaveRAM(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x7EF0:
      {
        set_PPU_bank0(data & 0x7F);
        set_PPU_bank1((data & 0x7F)+1);

//        if(patch)
//        {
//          if (data & 0x80)
//          {
//            set_VRAM_bank(0x8, 1);
//            set_VRAM_bank(0x9, 1);
//          }
//          else
//          {
//            set_VRAM_bank(0x8, 0);
//            set_VRAM_bank(0x9, 0);
//          }
//        }
      }
      break;

    case 0x7EF1:
      {
        set_PPU_bank2(data & 0x7F);
        set_PPU_bank3((data & 0x7F)+1);

//        if(patch)
//        {
//          if (data & 0x80)
//          {
//            set_VRAM_bank(0xA, 1);
//            set_VRAM_bank(0xB, 1);
//          }
//          else
//          {
//            set_VRAM_bank(0xA, 0);
//            set_VRAM_bank(0xB, 0);
//          }
//        }
      }
      break;

    case 0x7EF2:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0x7EF3:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0x7EF4:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0x7EF5:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0x7EF6:
      {
        if(data & 0x01)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
      }
      break;

    case 0x7EFA:
    case 0x7EFB:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x7EFC:
    case 0x7EFD:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x7EFE:
    case 0x7EFF:
      {
        set_CPU_bank6(data);
      }
      break;
  }
}
void MAP80_Init()
{
	NES_Mapper->Reset = MAP80_Reset; 
	//NES_Mapper->Write=MAP80_MemoryWriteSaveRAM;
}
























