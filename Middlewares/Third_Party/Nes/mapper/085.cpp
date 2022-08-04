#include "nes_mapper.h"



// Mapper 85
void MAP85_Reset()
{
//  // Init ExSound
//  parent_NES->apu->SelectExSound(2);

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
  else
  {
    set_VRAM_bank(0, 0);
    set_VRAM_bank(1, 0);
    set_VRAM_bank(2, 0);
    set_VRAM_bank(3, 0);
    set_VRAM_bank(4, 0);
    set_VRAM_bank(5, 0);
    set_VRAM_bank(6, 0);
    set_VRAM_bank(7, 0);
  }

  MAPx->irq_enabled = 0;
  MAPx->irq_counter = 0;
  MAPx->irq_latch = 0;
}

void MAP85_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF038)
  {
    case 0x8000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x8008:
    case 0x8010:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x9000:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0x9010:
    case 0x9030:
      {
        //parent_NES->apu->ExWrite(addr, data);
      }
      break;

    case 0xA000:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank0(data);
        }
        else
        {
          set_VRAM_bank(0, data);
        }
      }
      break;

    case 0xA008:
    case 0xA010:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank1(data);
        }
        else
        {
          set_VRAM_bank(1, data);
        }
      }
      break;

    case 0xB000:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank2(data);
        }
        else
        {
          set_VRAM_bank(2, data);
        }
      }
      break;

    case 0xB008:
    case 0xB010:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank3(data);
        }
        else
        {
          set_VRAM_bank(3, data);
        }
      }
      break;

    case 0xC000:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank4(data);
        }
        else
        {
          set_VRAM_bank(4, data);
        }
      }
      break;

    case 0xC008:
    case 0xC010:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank5(data);
        }
        else
        {
          set_VRAM_bank(5, data);
        }
      }
      break;

    case 0xD000:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank6(data);
        }
        else
        {
          set_VRAM_bank(6, data);
        }
      }
      break;

    case 0xD008:
    case 0xD010:
      {
        if(num_1k_VROM_banks)
        {
          set_PPU_bank7(data);
        }
        else
        {
          set_VRAM_bank(7, data);
        }
      }
      break;

    case 0xE000:
      {
        data &= 0x03;
        if(data == 0x00)
        {
             set_mirroring(0,1,0,1);//´¹Ö±¾µÏñInfoNES_Mirroring( 1 );   //0101
             //set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 0x01)
        {
             set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñInfoNES_Mirroring( 0 );   //0011
             //set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 0x02)
        {
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0xE008:
    case 0xE010:
      {
        MAPx->irq_latch = data;
      }
      break;

    case 0xF000:
      {
        MAPx->irq_enabled = data & 0x03;
        if(MAPx->irq_enabled & 0x02)
        {
          MAPx->irq_counter = MAPx->irq_latch;
        }
      }
      break;

    case 0xF008:
    case 0xF010:
      {
        MAPx->irq_enabled = (MAPx->irq_enabled & 0x01) * 3;
      }
      break;
  }
}

void MAP85_HSync(int scanline)
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
void MAP85_Init()
{
	NES_Mapper->Reset = MAP85_Reset;
	NES_Mapper->Write = MAP85_MemoryWrite;
	NES_Mapper->HSync = MAP85_HSync;   
}




