#include "nes_mapper.h"
 
// Mapper 18
void MAP18_Reset()
{
//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  MAPx->regs[0] = 0;
  MAPx->regs[1] = 1;
  MAPx->regs[2] = num_8k_ROM_banks-2;
  MAPx->regs[3] = num_8k_ROM_banks-1;
  MAPx->regs[4] = 0;
  MAPx->regs[5] = 0;
  MAPx->regs[6] = 0;
  MAPx->regs[7] = 0;
  MAPx->regs[8] = 0;
  MAPx->regs[9] = 0;
  MAPx->regs[10] = 0;

  MAPx->irq_enabled = 0;
  MAPx->irq_latch = 0;
  MAPx->irq_counter = 0;
}

void MAP18_MemoryWrite(uint16 addr, uint8 data)
{
  // MAPx->regs[ 0] ... 8K PROM bank at CPU $8000
  // MAPx->regs[ 1] ... 8K PROM bank at CPU $A000
  // MAPx->regs[ 2] ... 8K PROM bank at CPU $C000
  // MAPx->regs[ 3] ... 1K VROM bank at PPU $0000
  // MAPx->regs[ 4] ... 1K VROM bank at PPU $0400
  // MAPx->regs[ 5] ... 1K VROM bank at PPU $0800
  // MAPx->regs[ 6] ... 1K VROM bank at PPU $0C00
  // MAPx->regs[ 7] ... 1K VROM bank at PPU $1000
  // MAPx->regs[ 8] ... 1K VROM bank at PPU $1400
  // MAPx->regs[ 9] ... 1K VROM bank at PPU $1800
  // MAPx->regs[10] ... 1K VROM bank at PPU $1C00

  switch(addr)
  {
    case 0x8000:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0xF0) | (data & 0x0F);
        set_CPU_bank4(MAPx->regs[0]);
      }
      break;

    case 0x8001:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0x0F) | ((data & 0x0F) << 4);
        set_CPU_bank4(MAPx->regs[0]);
      }
      break;

    case 0x8002:
      {
        MAPx->regs[1] = (MAPx->regs[1] & 0xF0) | (data & 0x0F);
        set_CPU_bank5(MAPx->regs[1]);
      }
      break;

    case 0x8003:
      {
        MAPx->regs[1] = (MAPx->regs[1] & 0x0F) | ((data & 0x0F) << 4);
        set_CPU_bank5(MAPx->regs[1]);
      }
      break;

    case 0x9000:
      {
        MAPx->regs[2] = (MAPx->regs[2] & 0xF0) | (data & 0x0F);
        set_CPU_bank6(MAPx->regs[2]);
      }
      break;

    case 0x9001:
      {
        MAPx->regs[2] = (MAPx->regs[2] & 0x0F) | ((data & 0x0F) << 4);
        set_CPU_bank6(MAPx->regs[2]);
      }
      break;

    case 0xA000:
      {
        MAPx->regs[3] = (MAPx->regs[3] & 0xF0) | (data & 0x0F);
        set_PPU_bank0(MAPx->regs[3]);
      }
      break;

    case 0xA001:
      {
        MAPx->regs[3] = (MAPx->regs[3] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank0(MAPx->regs[3]);
      }
      break;

    case 0xA002:
      {
        MAPx->regs[4] = (MAPx->regs[4] & 0xF0) | (data & 0x0F);
        set_PPU_bank1(MAPx->regs[4]);
      }
      break;

    case 0xA003:
      {
        MAPx->regs[4] = (MAPx->regs[4] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank1(MAPx->regs[4]);
      }
      break;

    case 0xB000:
      {
        MAPx->regs[5] = (MAPx->regs[5] & 0xF0) | (data & 0x0F);
        set_PPU_bank2(MAPx->regs[5]);
      }
      break;

    case 0xB001:
      {
        MAPx->regs[5] = (MAPx->regs[5] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank2(MAPx->regs[5]);
      }
      break;

    case 0xB002:
      {
        MAPx->regs[6] = (MAPx->regs[6] & 0xF0) | (data & 0x0F);
        set_PPU_bank3(MAPx->regs[6]);
      }
      break;

    case 0xB003:
      {
        MAPx->regs[6] = (MAPx->regs[6] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank3(MAPx->regs[6]);
      }
      break;

    case 0xC000:
      {
        MAPx->regs[7] = (MAPx->regs[7] & 0xF0) | (data & 0x0F);
        set_PPU_bank4(MAPx->regs[7]);
      }
      break;

    case 0xC001:
      {
        MAPx->regs[7] = (MAPx->regs[7] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank4(MAPx->regs[7]);
      }
      break;

    case 0xC002:
      {
        MAPx->regs[8] = (MAPx->regs[8] & 0xF0) | (data & 0x0F);
        set_PPU_bank5(MAPx->regs[8]);
      }
      break;

    case 0xC003:
      {
        MAPx->regs[8] = (MAPx->regs[8] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank5(MAPx->regs[8]);
      }
      break;

    case 0xD000:
      {
        MAPx->regs[9] = (MAPx->regs[9] & 0xF0) | (data & 0x0F);
        set_PPU_bank6(MAPx->regs[9]);
      }
      break;

    case 0xD001:
      {
        MAPx->regs[9] = (MAPx->regs[9] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank6(MAPx->regs[9]);
      }
      break;

    case 0xD002:
      {
        MAPx->regs[10] = (MAPx->regs[10] & 0xF0) | (data & 0x0F);
        set_PPU_bank7(MAPx->regs[10]);
      }
      break;

    case 0xD003:
      {
        MAPx->regs[10] = (MAPx->regs[10] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank7(MAPx->regs[10]);
      }
      break;

    case 0xE000:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xFFF0) | (data & 0x0F);
      }
      break;

    case 0xE001:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xE002:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
      }
      break;

    case 0xE003:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
      }
      break;

    case 0xF000:
      {
        MAPx->irq_counter = MAPx->irq_latch;
      }
      break;

    case 0xF001:
      {
        MAPx->irq_enabled = data & 0x01;
      }
      break;

    case 0xF002:
      {
        data &= 0x03;
        if(data == 0)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 1)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else
        {
          set_mirroring(0,0,0,0);
        }
      }
      break;
  }
}

void MAP18_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    if(MAPx->irq_counter <= 113)
    {
      CPU_IRQ;
      MAPx->irq_counter =  0;
      MAPx->irq_enabled = 0;
    }
    else
    {
      MAPx->irq_counter -= 113;
    }
  }
}


void MAP18_Init()
{
	NES_Mapper->Reset = MAP18_Reset;
	NES_Mapper->Write = MAP18_MemoryWrite;  
	NES_Mapper->HSync = MAP18_HSync; 
}











