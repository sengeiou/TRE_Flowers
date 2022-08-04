#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 24
void MAP24_Reset()
{
//  // Init ExSound
//  parent_NES->apu->SelectExSound(1);

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP24_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF003)
  {
    case 0x8000:
      {
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
      }
      break;

    case 0xB003:
      {
        data = data & 0x0C;
        if(data == 0x00)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 0x04)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 0x08)
        {
          set_mirroring(0,0,0,0);
        }
        else if(data == 0x0C)
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0xC000:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0xD000:
      {
        set_PPU_bank0(data);
      }
      break;

    case 0xD001:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0xD002:
      {
        set_PPU_bank2(data);
      }
      break;

    case 0xD003:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0xE000:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0xE001:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0xE002:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0xE003:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xF000:
      {
        MAPx->irq_latch = data;
      }
      break;

    case 0xF001:
      {
        MAPx->irq_enabled = data & 0x03;
        if(MAPx->irq_enabled & 0x02)
        {
          MAPx->irq_counter = MAPx->irq_latch;
        }
      }
      break;

    case 0xF002:
      {
        if(MAPx->irq_enabled & 0x01)
        {
          MAPx->irq_enabled |= 0x02;
        }
        else
        {
          MAPx->irq_enabled &= 0x01;
        }
      }
      break;
  }
//  parent_NES->apu->ExWrite(addr, data);
}

void MAP24_HSync(int scanline)
{
  if(MAPx->irq_enabled & 0x02)
  {
    if(MAPx->irq_counter == 0xFF)
    {
      CPU_IRQ;
      MAPx->irq_counter = MAPx->irq_latch;
    }
    else
    {
      MAPx->irq_counter++;
    }
  }
}

void MAP24_Init()
{
	NES_Mapper->Reset = MAP24_Reset;
	NES_Mapper->Write = MAP24_MemoryWrite; 
	NES_Mapper->HSync = MAP24_HSync; 
}






















