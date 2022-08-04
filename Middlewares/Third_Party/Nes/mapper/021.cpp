#include "nes_mapper.h"


/////////////////////////////////////////////////////////////////////
// Mapper 21
void MAP21_Reset()
{
  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  MAPx->regs[0] = 0;
  MAPx->regs[1] = 1;
  MAPx->regs[2] = 2;
  MAPx->regs[3] = 3;
  MAPx->regs[4] = 4;
  MAPx->regs[5] = 5;
  MAPx->regs[6] = 6;
  MAPx->regs[7] = 7;
  MAPx->regs[8] = 0;

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP21_MemoryWrite(uint16 addr, uint8 data)
{
  // MAPx->regs[0] ... 1K VROM bank at PPU $0000
  // MAPx->regs[1] ... 1K VROM bank at PPU $0400
  // MAPx->regs[2] ... 1K VROM bank at PPU $0800
  // MAPx->regs[3] ... 1K VROM bank at PPU $0C00
  // MAPx->regs[4] ... 1K VROM bank at PPU $1000
  // MAPx->regs[5] ... 1K VROM bank at PPU $1400
  // MAPx->regs[6] ... 1K VROM bank at PPU $1800
  // MAPx->regs[7] ... 1K VROM bank at PPU $1C00
  // MAPx->regs[8] ... $8000 Switching Mode

  switch (addr & 0xF0CF)
  {
    case 0x8000:
      {
        if(MAPx->regs[8] & 0x02)
        {
          set_CPU_bank6(data);
        }
        else
        {
          set_CPU_bank4(data);
        }
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
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
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x9002:
    case 0x9080:
      {
        MAPx->regs[8] = data;
      }
      break;

    case 0xB000:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0xF0) | (data & 0x0F);
        set_PPU_bank0(MAPx->regs[0]);
      }
      break;

    case 0xB002:
    case 0xB040:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank0(MAPx->regs[0]);
      }
      break;

    case 0xB001:
    case 0xB004:
    case 0xB080:
      {
        MAPx->regs[1] = (MAPx->regs[1] & 0xF0) | (data & 0x0F);
        set_PPU_bank1(MAPx->regs[1]);
      }
      break;

    case 0xB003:
    case 0xB006:
    case 0xB0C0:
      {
        MAPx->regs[1] = (MAPx->regs[1] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank1(MAPx->regs[1]);
      }
      break;

    case 0xC000:
      {
        MAPx->regs[2] = (MAPx->regs[2] & 0xF0) | (data & 0x0F);
        set_PPU_bank2(MAPx->regs[2]);
      }
      break;

    case 0xC002:
    case 0xC040:
      {
        MAPx->regs[2] = (MAPx->regs[2] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank2(MAPx->regs[2]);
      }
      break;

    case 0xC001:
    case 0xC004:
    case 0xC080:
      {
        MAPx->regs[3] = (MAPx->regs[3] & 0xF0) | (data & 0x0F);
        set_PPU_bank3(MAPx->regs[3]);
      }
      break;

    case 0xC003:
    case 0xC006:
    case 0xC0C0:
      {
        MAPx->regs[3] = (MAPx->regs[3] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank3(MAPx->regs[3]);
      }
      break;

    case 0xD000:
      {
        MAPx->regs[4] = (MAPx->regs[4] & 0xF0) | (data & 0x0F);
        set_PPU_bank4(MAPx->regs[4]);
      }
      break;

    case 0xD002:
    case 0xD040:
      {
        MAPx->regs[4] = (MAPx->regs[4] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank4(MAPx->regs[4]);
      }
      break;

    case 0xD001:
    case 0xD004:
    case 0xD080:
      {
        MAPx->regs[5] = (MAPx->regs[5] & 0xF0) | (data & 0x0F);
        set_PPU_bank5(MAPx->regs[5]);
      }
      break;

    case 0xD003:
    case 0xD006:
    case 0xD0C0:
      {
        MAPx->regs[5] = (MAPx->regs[5] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank5(MAPx->regs[5]);
      }
      break;

    case 0xE000:
      {
        MAPx->regs[6] = (MAPx->regs[6] & 0xF0) | (data & 0x0F);
        set_PPU_bank6(MAPx->regs[6]);
      }
      break;

    case 0xE002:
    case 0xE040:
      {
        MAPx->regs[6] = (MAPx->regs[6] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank6(MAPx->regs[6]);
      }
      break;

    case 0xE001:
    case 0xE004:
    case 0xE080:
      {
        MAPx->regs[7] = (MAPx->regs[7] & 0xF0) | (data & 0x0F);
        set_PPU_bank7(MAPx->regs[7]);
      }
      break;

    case 0xE003:
    case 0xE006:
    case 0xE0C0:
      {
        MAPx->regs[7] = (MAPx->regs[7] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank7(MAPx->regs[7]);
      }
      break;

    case 0xF000:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xF0) | (data & 0x0F);
      }
      break;

    case 0xF002:
    case 0xF040:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0x0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xF003:
    case 0xF0C0:
      {
        MAPx->irq_enabled = (MAPx->irq_enabled & 0x01) * 3;
      }
      break;

    case 0xF004:
    case 0xF080:
      {
        MAPx->irq_enabled = data & 0x03;
        if(MAPx->irq_enabled & 0x02)
        {
          MAPx->irq_counter = MAPx->irq_latch;
        }
      }
      break;
  }
}

void MAP21_HSync(int scanline)
{
  if(MAPx->irq_enabled & 0x02)
  {
    if(MAPx->irq_counter == 0xFF)
    {
      MAPx->irq_counter = MAPx->irq_latch;
      MAPx->irq_enabled = (MAPx->irq_enabled & 0x01) * 3;
      CPU_IRQ;
    }
    else
    {
      MAPx->irq_counter++;
    }
  }
}
void MAP21_Init()
{
	NES_Mapper->Reset = MAP21_Reset;
	NES_Mapper->Write = MAP21_MemoryWrite; 
	NES_Mapper->HSync = MAP21_HSync; 
}


