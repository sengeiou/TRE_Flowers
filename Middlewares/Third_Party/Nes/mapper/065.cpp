#include "nes_mapper.h"
 

/////////////////////////////////////////////////////////////////////
// Mapper 65
void MAP65_Reset()
{
//  patch = 0;

//  // for Kaiketsu Yanchamaru 3
//  if(parent_NES->crc32() == 0xe30b7f64) // Kaiketsu Yanchamaru 3 - Taiketsu! Zouringen
//  {
//    patch = 1;
//  }

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
}

void MAP65_MemoryWrite(uint16 addr, uint8 data)
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
        //if(!patch)
        {
          if(data & 0x40)
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
      }
      break;

    case 0x9001:
      {
//        if(patch)
//        {
//          if(data & 0x80)
//          {
//            set_mirroring(NES_PPU::MIRROR_HORIZ);
//          }
//          else
//          {
//            set_mirroring(NES_PPU::MIRROR_VERT);
//          }
//        }
      }
      break;

    case 0x9003:
      {
        //if(!patch)
        {
          MAPx->irq_enabled = data & 0x80;
        }
      }
      break;

    case 0x9004:
      {
        //if(!patch)
        {
          MAPx->irq_counter = MAPx->irq_latch;
        }
      }
      break;

    case 0x9005:
      {
//        if(patch)
//        {
//          MAPx->irq_counter = (uint8)(data << 1);
//          MAPx->irq_enabled = data;
//        }
//        else
        {
          MAPx->irq_latch = (MAPx->irq_latch & 0x00FF) | ((uint32)data << 8);
        }
      }
      break;

    case 0x9006:
      {
//        if(patch)
//        {
//          MAPx->irq_enabled = 1;
//        }
//        else
        {
          MAPx->irq_latch = (MAPx->irq_latch & 0xFF00) | data;
        }
      }
      break;

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
        set_PPU_bank6(data);
      }
      break;

    case 0xB007:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0xC000:
      {
        set_CPU_bank6(data);
      }
      break;
  }
}

void MAP65_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
//    if(patch)
//    {
//      if(MAPx->irq_counter == 0)
//      {
//        CPU_IRQ;
//        MAPx->irq_enabled = 0;
//      }
//      else
//      {
//        MAPx->irq_counter--;
//      }
//    }
//    else
    {
      if(MAPx->irq_counter <= 113)
      {
        CPU_IRQ;
        MAPx->irq_enabled = 0;
        MAPx->irq_counter = 0xFFFF;
      }
      else
      {
        MAPx->irq_counter -= 113;
      }
    }
  }
}
void MAP65_Init()
{
	NES_Mapper->Reset = MAP65_Reset;
	NES_Mapper->Write = MAP65_MemoryWrite;
	NES_Mapper->HSync = MAP65_HSync;   
}





