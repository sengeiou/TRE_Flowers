#include "nes_mapper.h"


// Mapper 25
void MAP25_Reset()
{
	int i;
//  patch = 0;

//  if(parent_NES->crc32() == 0xea74c587 || // Teenage Mutant Ninja Turtles
//     parent_NES->crc32() == 0x5f82cb7d || // Teenage Mutant Ninja Turtles 2
//     parent_NES->crc32() == 0xa2e68da8)   // Racer Mini Yonku - Japan Cup
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

  for( i = 0; i < 8; i++)
    MAPx->regs[i] = 0;

  MAPx->regs[8] = 0;
  MAPx->regs[9] = num_8k_ROM_banks-2;
  MAPx->regs[10] = 0;

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP25_MemoryWrite(uint16 addr, uint8 data)
{
  // MAPx->regs[0] ... 1K VROM bank at PPU $0000
  // MAPx->regs[1] ... 1K VROM bank at PPU $0400
  // MAPx->regs[2] ... 1K VROM bank at PPU $0800
  // MAPx->regs[3] ... 1K VROM bank at PPU $0C00
  // MAPx->regs[4] ... 1K VROM bank at PPU $1000
  // MAPx->regs[5] ... 1K VROM bank at PPU $1400
  // MAPx->regs[6] ... 1K VROM bank at PPU $1800
  // MAPx->regs[7] ... 1K VROM bank at PPU $1C00
  // MAPx->regs[8] ... 8K ROM bank at CPU $8000
  // MAPx->regs[9] ... 8K ROM bank at CPU $C000
  // MAPx->regs[10] .. ROM Swap flag

  switch(addr & 0xF000)
  {
    case 0x8000:
      {
        if(MAPx->regs[10] & 0x02)
        {
          MAPx->regs[9] = data;
          set_CPU_bank6(data);
        }
        else
        {
          MAPx->regs[8] = data;
          set_CPU_bank4(data);
        }
      }
      break;

    case 0xA000:
      {
        set_CPU_bank5(data);
      }
      break;
  }

  switch(addr & 0xF00F)
  {
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

    case 0x9001:
    case 0x9004:
      {
        if((MAPx->regs[10] & 0x02) != (data & 0x02))
        {
          uint8 swap = MAPx->regs[8];
          MAPx->regs[8] = MAPx->regs[9];
          MAPx->regs[9] = swap;
          set_CPU_bank4(MAPx->regs[8]);
          set_CPU_bank6(MAPx->regs[9]);
        }
        MAPx->regs[10] = data;
      }
      break;

    case 0xB000:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0xF0) | (data & 0x0F);
        set_PPU_bank0(MAPx->regs[0]);
      }
      break;

    case 0xB001:
    case 0xB004:
      {
        MAPx->regs[1] = (MAPx->regs[1] & 0xF0) | (data & 0x0F);
        set_PPU_bank1(MAPx->regs[1]);
      }
      break;

    case 0xB002:
    case 0xB008:
      {
        MAPx->regs[0] = (MAPx->regs[0] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank0(MAPx->regs[0]);
      }
      break;

    case 0xB003:
    case 0xB00C:
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

    case 0xC001:
    case 0xC004:
      {
        MAPx->regs[3] = (MAPx->regs[3] & 0xF0) | (data & 0x0F);
        set_PPU_bank3(MAPx->regs[3]);
      }
      break;

    case 0xC002:
    case 0xC008:
      {
        MAPx->regs[2] = (MAPx->regs[2] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank2(MAPx->regs[2]);
      }
      break;

    case 0xC003:
    case 0xC00C:
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

    case 0xD001:
    case 0xD004:
      {
        MAPx->regs[5] = (MAPx->regs[5] & 0xF0) | (data & 0x0F);
        set_PPU_bank5(MAPx->regs[5]);
      }
      break;

    case 0xD002:
    case 0xD008:
      {
        MAPx->regs[4] = (MAPx->regs[4] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank4(MAPx->regs[4]);
      }
      break;

    case 0xD003:
    case 0xD00C:
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

    case 0xE001:
    case 0xE004:
      {
        MAPx->regs[7] = (MAPx->regs[7] & 0xF0) | (data & 0x0F);
        set_PPU_bank7(MAPx->regs[7]);
      }
      break;

    case 0xE002:
    case 0xE008:
      {
        MAPx->regs[6] = (MAPx->regs[6] & 0x0F) | ((data & 0x0F) << 4);
        set_PPU_bank6(MAPx->regs[6]);
      }
      break;

    case 0xE003:
    case 0xE00C:
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

    case 0xF001:
    case 0xF004:
      {
        MAPx->irq_enabled = data & 0x03;
        if(MAPx->irq_enabled & 0x02)
        {
          MAPx->irq_counter = MAPx->irq_latch;
        }
      }
      break;

    case 0xF002:
    case 0xF008:
      {
        MAPx->irq_latch = (MAPx->irq_latch & 0x0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xF003:
    case 0xF00C:
      {
        MAPx->irq_enabled = (MAPx->irq_enabled & 0x01) * 3;
      }
      break;
  }
}

void MAP25_HSync(int scanline)
{
  if(MAPx->irq_enabled & 0x02)
  {
    if(MAPx->irq_counter == 0xFF)
    {
      MAPx->irq_counter = MAPx->irq_latch;
      CPU_IRQ;
    }
//    else if(patch && MAPx->irq_counter == 0x00)
//    {
//      MAPx->irq_counter = MAPx->irq_latch;
//      CPU_IRQ;
//    }
    else
    {
      MAPx->irq_counter++;
    }
  }
} 
void MAP25_Init()
{
	NES_Mapper->Reset = MAP25_Reset;
	NES_Mapper->Write = MAP25_MemoryWrite; 
	NES_Mapper->HSync = MAP25_HSync; 
}



