#include "nes_mapper.h"


/////////////////////////////////////////////////////////////////////
// Mapper 64
void MAP64_Reset()
{
//  // set CPU bank pointers
//  set_CPU_bank4(num_8k_ROM_banks-1);
//  set_CPU_bank5(num_8k_ROM_banks-1);
//  set_CPU_bank6(num_8k_ROM_banks-1);
//  set_CPU_bank7(num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  MAPx->irq_latch = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_enabled = 0;

  MAPx->regs[0] = 0;
  MAPx->regs[1] = 0;
  MAPx->regs[2] = 0;
}

void MAP64_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF003)
  {
    case 0x8000:
      {
        MAPx->regs[0] = data & 0x0F;
        MAPx->regs[1] = data & 0x40;
        MAPx->regs[2] = data & 0x80;
      }
      break;

    case 0x8001:
      {
        switch(MAPx->regs[0])
        {
          case 0x00:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank4(data+0);
                set_PPU_bank5(data+1);
              }
              else
              {
                set_PPU_bank0(data+0);
                set_PPU_bank1(data+1);
              }
            }
            break;

          case 0x01:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank6(data+0);
                set_PPU_bank7(data+1);
              }
              else
              {
                set_PPU_bank2(data+0);
                set_PPU_bank3(data+1);
              }
            }
            break;

          case 0x02:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank0(data);
              }
              else
              {
                set_PPU_bank4(data);
              }
            }
            break;

          case 0x03:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank1(data);
              }
              else
              {
                set_PPU_bank5(data);
              }
            }
            break;

          case 0x04:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank2(data);
              }
              else
              {
                set_PPU_bank6(data);
              }
            }
            break;

          case 0x05:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank3(data);
              }
              else
              {
                set_PPU_bank7(data);
              }
            }
            break;

          case 0x06:
            {
              if(MAPx->regs[1])
              {
                set_CPU_bank5(data);
              }
              else
              {
                set_CPU_bank4(data);
              }
            }
            break;

          case 0x07:
            {
              if(MAPx->regs[1])
              {
                set_CPU_bank6(data);
              }
              else
              {
                set_CPU_bank5(data);
              }
            }
            break;

          case 0x08:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank5(data);
              }
              else
              {
                set_PPU_bank1(data);
              }
            }
            break;

          case 0x09:
            {
              if(MAPx->regs[2])
              {
                set_PPU_bank7(data);
              }
              else
              {
                set_PPU_bank3(data);
              }
            }
            break;

          case 0x0F:
            {
              if(MAPx->regs[1])
              {
                set_CPU_bank4(data);
              }
              else
              {
                set_CPU_bank6(data);
              }
            }
            break;
        }
      }
      break;

    case 0xA000:
      {
        if(!(data & 0x01))
        {
			set_mirroring(0,1,0,1);//´¹Ö±¾µÏñ
			//set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else
        {
			set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñ
			//set_mirroring(NES_PPU::MIRROR_HORIZ);
		}
      }
      break;

    case 0xC000:
      {
        MAPx->irq_latch = data;
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0xC001:
      {
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0xE000:
      {
        MAPx->irq_enabled = 0;
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0xE001:
      {
        MAPx->irq_enabled = 1;
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;
  }
}

void MAP64_HSync(int scanline)
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

void MAP64_Init()
{
	NES_Mapper->Reset = MAP64_Reset;
	NES_Mapper->Write = MAP64_MemoryWrite;
	NES_Mapper->HSync = MAP64_HSync;   
}













