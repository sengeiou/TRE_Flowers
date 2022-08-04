#include "nes_mapper.h"


/////////////////////////////////////////////////////////////////////
// Mapper 67
void MAP67_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_bank0(0);
  set_PPU_bank1(1);
  set_PPU_bank2(2);
  set_PPU_bank3(3);
  set_PPU_bank4(num_8k_ROM_banks-4);
  set_PPU_bank5(num_8k_ROM_banks-3);
  set_PPU_bank6(num_8k_ROM_banks-2);
  set_PPU_bank7(num_8k_ROM_banks-1);

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP67_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF800)
  {
    case 0x8800:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x9800:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0xA800:
      {
        set_PPU_bank4(data*2+0);
        set_PPU_bank5(data*2+1);
      }
      break;

    case 0xB800:
      {
        set_PPU_bank6(data*2+0);
        set_PPU_bank7(data*2+1);
      }
      break;

    case 0xC800:
      {
        MAPx->irq_counter = MAPx->irq_latch;
        MAPx->irq_latch = data;
      }
      break;

    case 0xD800:
      {
        MAPx->irq_enabled = data & 0x10;
      }
      break;

    case 0xE800:
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
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0xF800:
      {
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
      }
      break;
  }
}

void MAP67_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 239))
    {
      if(spr_enabled() || bg_enabled())
      {
        if(--MAPx->irq_counter == 0xF6)
        {
          MAPx->irq_counter = MAPx->irq_latch;
          CPU_IRQ;
        }
      }
    }
  }
}

void MAP67_Init()
{
	NES_Mapper->Reset = MAP67_Reset;
	NES_Mapper->Write = MAP67_MemoryWrite;
	NES_Mapper->HSync = MAP67_HSync;   
}






















