#include "nes_mapper.h"

  
/////////////////////////////////////////////////////////////////////
// Mapper 17
void MAP17_Reset()
{ 
//  // set PPU bank pointers
//  if(num_1k_VROM_banks)
//  {
//    set_PPU_banks(0,1,2,3,4,5,6,7);
//  }

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP17_MemoryWriteLow(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x42FE:
      {
        if((data & 0x10) == 0)
        {
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x42FF:
      {
        if((data & 0x10) == 0)
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

    case 0x4501:
      {
        MAPx->irq_enabled = 0;
      }
      break;

    case 0x4502:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xFF00) | data;
      }
      break;

    case 0x4503:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0x00FF) | ((uint32)data << 8);
        MAPx->irq_counter = MAPx->irq_latch;
        MAPx->irq_enabled = 1;
      }
      break;

    case 0x4504:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x4505:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x4506:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0x4507:
      {
        set_CPU_bank7(data);
      }
      break;

    case 0x4510:
      {
        set_PPU_bank0(data);
      }
      break;

    case 0x4511:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0x4512:
      {
        set_PPU_bank2(data);
      }
      break;

    case 0x4513:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0x4514:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0x4515:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0x4516:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0x4517:
      {
        set_PPU_bank7(data);
      }
      break;
  }
}

void MAP17_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    if(MAPx->irq_counter >= 0xFFFF - 113)
    {
      CPU_IRQ;
      MAPx->irq_counter = 0;
      MAPx->irq_enabled = 0;
    }
    else
    {
      MAPx->irq_counter += 113;
    }
  }
}
void MAP17_Init()
{
	NES_Mapper->Reset = MAP17_Reset;
	NES_Mapper->WriteLow = MAP17_MemoryWriteLow; 
	NES_Mapper->HSync = MAP17_HSync; 
}


















