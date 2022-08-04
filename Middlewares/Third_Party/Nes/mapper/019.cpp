#include "nes_mapper.h"
 

// Mapper 19
void MAP19_Reset()
{
  // Init ExSound
  //parent_NES->apu->SelectExSound(16);

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks >= 8)
  {
    set_PPU_bank0(num_1k_VROM_banks-8);
    set_PPU_bank1(num_1k_VROM_banks-7);
    set_PPU_bank2(num_1k_VROM_banks-6);
    set_PPU_bank3(num_1k_VROM_banks-5);
    set_PPU_bank4(num_1k_VROM_banks-4);
    set_PPU_bank5(num_1k_VROM_banks-3);
    set_PPU_bank6(num_1k_VROM_banks-2);
    set_PPU_bank7(num_1k_VROM_banks-1);
  }

  MAPx->regs[0] = 0;
  MAPx->regs[1] = 0;
  MAPx->regs[2] = 0;

  // Rick
  MAPx->irq_counter = MAPx->irq_enabled = 0;
}

uint8 MAP19_MemoryReadLow(uint32 addr)
{
  if(addr == 0x4800)
  {
//    if(patch == 2)
//    {
//      uint8 retval =  parent_NES->ReadSaveRAM(MAPx->regs[2] & 0x7F);
//      if(MAPx->regs[2] & 0x80) MAPx->regs[2] = ((MAPx->regs[2] & 0x7F)+1) | 0x80;
//      return retval;
//    }
//    else
//    {
//      //return parent_NES->apu->ExRead(addr);
//    }
	  return 0;
  }
  else if((addr & 0xF800) == 0x5000) // addr $5000-$57FF
  {
    return (uint8)(MAPx->irq_counter & 0x00FF);
  }
  else if ((addr & 0xF800) == 0x5800) // addr $5800-$5FFF
  {
    return (uint8)((MAPx->irq_counter & 0x7F00) >> 8);
  }
  else
  {
    return (uint8)(addr >> 8);
  }
}

void MAP19_MemoryWriteLow(uint16 addr, uint8 data)
{
  switch(addr & 0xF800)
  {
    case 0x4800:
      {
        if(addr == 0x4800)
        {
//          if(patch == 2)
//          {
//            parent_NES->WriteSaveRAM(MAPx->regs[2] & 0x7F, data);
//            if(MAPx->regs[2] & 0x80) MAPx->regs[2] = ((MAPx->regs[2] & 0x7F)+1) | 0x80;
//          }
//          else
//          {
//            //parent_NES->apu->ExWrite(addr, data);
//          }
        }
      }
      break;

    case 0x5000: // addr $5000-$57FF
      {
        MAPx->irq_counter = (MAPx->irq_counter & 0xFF00) | data;
      }
      break;

    case 0x5800: // addr $5000-$57FF
      {
        MAPx->irq_counter = (MAPx->irq_counter & 0x00FF) | ((uint32)(data & 0x7F) << 8);
        MAPx->irq_enabled = (data & 0x80) >> 7;
//        if(patch)
//        {
//          MAPx->irq_counter ++;
//        }
      }
      break;
  }
}

void MAP19_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF800)
  {
    case 0x8000: // addr $8000-$87FF
      {
        if(data < 0xE0 || MAPx->regs[0] == 1)
        {
          set_PPU_bank0(data);
        }
        else
        {
          set_VRAM_bank(0, 0);
        }
      }
      break;

    case 0x8800: // addr $8800-$8FFF
      {
        if(data < 0xE0 || MAPx->regs[0] == 1)
        {
          set_PPU_bank1(data);
        }
        else
        {
          set_VRAM_bank(1, 1);
        }
      }
      break;

    case 0x9000: // addr $9000-$97FF
      {
        if(data < 0xE0 || MAPx->regs[0] == 1)
        {
          set_PPU_bank2(data);
        }
        else
        {
          set_VRAM_bank(2, 2);
        }
      }
      break;

    case 0x9800: // addr $9800-$9FFF
      {
        if(data < 0xE0 || MAPx->regs[0] == 1)
        {
          set_PPU_bank3(data);
        }
        else
        {
          set_VRAM_bank(3, 3);
        }
      }
      break;

    case 0xA000: // addr $A000-$A7FF
      {
        if(data < 0xE0 || MAPx->regs[1] == 1)
        {
          set_PPU_bank4(data);
        }
        else
        {
          set_VRAM_bank(4, 4);
        }
      }
      break;

    case 0xA800: // addr $A800-$AFFF
      {
        if(data < 0xE0 || MAPx->regs[1] == 1)
        {
          set_PPU_bank5(data);
        }
        else
        {
          set_VRAM_bank(5, 5);
        }
      }
      break;

    case 0xB000: // addr $B000-$B7FF
      {
        if(data < 0xE0 || MAPx->regs[1] == 1)
        {
          set_PPU_bank6(data);
        }
        else
        {
          set_VRAM_bank(6, 6);
        }
      }
      break;

    case 0xB800: // addr $B800-$BFFF
      {
        if(data < 0xE0 || MAPx->regs[1] == 1)
        {
          set_PPU_bank7(data);
        }
        else
        {
          set_VRAM_bank(7, 7);
        }
      }
      break;

    case 0xC000: // addr $C000-$C7FF
      {
        if(data <= 0xDF)
        {
          set_PPU_bank8(data);
        }
        else
        {
          set_VRAM_bank(8, data & 0x01);
        }
      }
      break;

    case 0xC800: // addr $C800-$CFFF
      {
        if(data <= 0xDF)
        {
          set_PPU_bank9(data);
        }
        else
        {
          set_VRAM_bank(9, data & 0x01);
        }
      }
      break;

    case 0xD000: // addr $D000-$D7FF
      {
        if(data <= 0xDF)
        {
          set_PPU_bank10(data);
        }
        else
        {
          set_VRAM_bank(10, data & 0x01);
        }
      }
      break;

    case 0xD800: // addr $D800-$DFFF
      {
        if(data <= 0xDF)
        {
          set_PPU_bank11(data);
        }
        else
        {
          set_VRAM_bank(11, data & 0x01);
        }
      }
      break;

    case 0xE000: // addr $E000-$E7FF
      {
        set_CPU_bank4(data & 0x3F);
      }
      break;

    case 0xE800: // addr $E800-$EFFF
      {
        set_CPU_bank5(data & 0x3F);
        MAPx->regs[0] = (data & 0x40) >> 6;
        MAPx->regs[1] = (data & 0x80) >> 7;
      }
      break;

    case 0xF000: // addr $F000-$F7FF
      {
        set_CPU_bank6(data & 0x3F);
      }
      break;

    case 0xF800:
      {
        if(addr == 0xF800)
        {
//          if(patch == 2)
//          {
//            MAPx->regs[2] = data;
//          }
//          else
//          {
//            //parent_NES->apu->ExWrite(addr, data);
//          }
        }
      }
      break;
  }
}

void MAP19_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    //if(MAPx->irq_counter >= (uint32)(0x7FFF - ((patch == 1) ? 112 : 113)))
    if(MAPx->irq_counter >= (uint32)(0x7FFF - 113))
    {
      MAPx->irq_counter = 0x7FFF;
      CPU_IRQ;
    }
    else
    {
      MAPx->irq_counter += 113;
      //MAPx->irq_counter += ((patch == 1) ? 112 : 113);
    }
  }
}
void MAP19_Init()
{
	NES_Mapper->Reset = MAP19_Reset;
	NES_Mapper->Write = MAP19_MemoryWrite; 
	NES_Mapper->HSync = MAP19_HSync; 
}


