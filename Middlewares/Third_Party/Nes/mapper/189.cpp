#include "nes_mapper.h"

 
// Mapper 189
void MAP189_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
  MAPx->irq_enabled = 0;
}

void MAP189_MemoryWriteLow(uint32 addr, uint8 data)
{
  if(addr >= 0x4100 && addr <= 0x41FF)
  {
    data = (data & 0x30) >> 4;
    set_CPU_banks(data*4,data*4+1,data*4+2,data*4+3);
  }
}

void MAP189_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        MAPx->regs[0] = data;
      }
      break;

    case 0x8001:
      {
        switch(MAPx->regs[0])
        {
          case 0x40:
            {
              set_PPU_bank0(data+0);
              set_PPU_bank1(data+1);
            }
            break;

          case 0x41:
            {
              set_PPU_bank2(data+0);
              set_PPU_bank3(data+1);
            }
            break;

          case 0x42:
            {
              set_PPU_bank4(data);
            }
            break;

          case 0x43:
            {
              set_PPU_bank5(data);
            }
            break;

          case 0x44:
            {
              set_PPU_bank6(data);
            }
            break;

          case 0x45:
            {
              set_PPU_bank7(data);
            }
            break;

          case 0x46:
            {
              set_CPU_bank6(data);
            }
            break;

          case 0x47:
            {
              set_CPU_bank5(data);
            }
            break;
        }
      }
      break;

    case 0xA000:
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
      }
      break;

    case 0xC000:
      {
        MAPx->irq_counter = data;
      }
      break;

    case 0xC001:
      {
        MAPx->irq_latch = data;
      }
      break;

    case 0xE000:
      {
        MAPx->irq_enabled = 0;
      }
      break;

    case 0xE001:
      {
        MAPx->irq_enabled = 1;
      }
      break;
  }
}

void MAP189_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 239))
    {
      if(spr_enabled() || bg_enabled())
      {
        if(!(--MAPx->irq_counter))
        {
          MAPx->irq_counter = MAPx->irq_latch;
          CPU_IRQ;
        }
      }
    }
  }
}
void MAP189_Init()
{
	NES_Mapper->Reset = MAP189_Reset;
	NES_Mapper->Write=MAP189_MemoryWrite;
	NES_Mapper->HSync=MAP189_HSync;
}




