#include "nes_mapper.h"

u8 map33_irq_enabled=0;
u8 map33_irq_counter=0;

// Mapper 33
void MAP33_Reset()
{
//  patch = 0;

//  if(parent_NES->crc32() == 0x5e9bc161 || // Akira
//     parent_NES->crc32() == 0xecdbafa4 || // Bakushou!! Jinsei Gekijou
//     parent_NES->crc32() == 0x59cd0c31 || // Don Doko Don
//     parent_NES->crc32() == 0x837c1342 || // Golf Ko Open
//     parent_NES->crc32() == 0x42d893e4 || // Operation Wolf
//     parent_NES->crc32() == 0x07ee6d8f || // Power Blazer
//     parent_NES->crc32() == 0x5193fb54)   // Takeshi no Sengoku Fuuunji
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

  map33_irq_enabled = 0;
  map33_irq_counter = 0;
}

void MAP33_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
//        if(patch) // old #33 games
//        {
//          if(data & 0x40)
//          {
//            set_mirroring(NES_PPU::MIRROR_HORIZ);
//          }
//          else
//          {
//            set_mirroring(NES_PPU::MIRROR_VERT);
//          }
//          set_CPU_bank4(data & 0x1F);
//        }
//        else
        {
          set_CPU_bank4(data);
        }
      }
      break;

    case 0x8001:
      {
//        if(patch) // old #33 games
//        {
//          set_CPU_bank5(data & 0x1F);
//        }
//        else
        {
          set_CPU_bank5(data);
        }
      }
      break;

    case 0x8002:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x8003:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0xA000:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0xA001:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0xA002:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0xA003:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xC000:
      {
        map33_irq_counter = data;
      }
      break;

    case 0xC001:
    case 0xC002:
    case 0xE001:
    case 0xE002:
      {
        map33_irq_enabled = data;
      }
      break;

    case 0xE000:
      {
        if (data & 0x40)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
      }
      break;
  }
}

void MAP33_HSync(int scanline)
{
  if(map33_irq_enabled)
  {
    if(spr_enabled() || bg_enabled())
    {
      if (scanline >= 0 && scanline <= 239)
      {
        if(map33_irq_counter == 0x00)
        {
          CPU_IRQ;
          map33_irq_counter = 0;
          map33_irq_enabled = 0;
        }
        else
        {
          map33_irq_counter++;
        }
      }
    }
  }
}
void MAP33_Init()
{
	NES_Mapper->Reset = MAP33_Reset;
	NES_Mapper->Write = MAP33_MemoryWrite; 
	NES_Mapper->HSync = MAP33_HSync; 
}








