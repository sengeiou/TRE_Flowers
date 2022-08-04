#include "nes_mapper.h"
 
/////////////////////////////////////////////////////////////////////
// Mapper 6
void MAP6_Reset()
{
//  // set CPU bank pointers
//  set_CPU_banks(0,1,14,15); 
  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
  else
  {
    set_VRAM_bank(0, 0);
    set_VRAM_bank(0, 1);
    set_VRAM_bank(0, 2);
    set_VRAM_bank(0, 3);
    set_VRAM_bank(0, 4);
    set_VRAM_bank(0, 5);
    set_VRAM_bank(0, 6);
    set_VRAM_bank(0, 7);
  }
 // parent_NES->ppu->vram_size = 0x8000;
}

void MAP6_MemoryWriteLow(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x42FE:
      {
        if(data & 0x10)
        {
          set_mirroring(1,1,1,1);
        }
        else
        {
          set_mirroring(0,0,0,0);
        }
      }
      break;

    case 0x42FF:
      {
        if(data & 0x10)
        {
			set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñ
			//set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
			set_mirroring(0,1,0,1);//´¹Ö±¾µÏñ
			//set_mirroring(NES_PPU::MIRROR_VERT);
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
        MAPx->irq_counter = (MAPx->irq_counter & 0xFF00) | (uint32)data;
      }
      break;

    case 0x4503:
      {
        MAPx->irq_counter = (MAPx->irq_counter & 0x00FF) | ((uint32)data << 8);
        MAPx->irq_enabled = 1;
      }
      break;
  }
}

void MAP6_MemoryWrite(uint16 addr, uint8 data)
{
  uint8 prg_bank = (data & 0x3C) >> 2;
  uint8 chr_bank = data & 0x03;

  set_CPU_bank4(prg_bank*2+0);
  set_CPU_bank5(prg_bank*2+1);

  set_VRAM_bank(0, chr_bank * 8 + 0);
  set_VRAM_bank(1, chr_bank * 8 + 1);
  set_VRAM_bank(2, chr_bank * 8 + 2);
  set_VRAM_bank(3, chr_bank * 8 + 3);
  set_VRAM_bank(4, chr_bank * 8 + 4);
  set_VRAM_bank(5, chr_bank * 8 + 5);
  set_VRAM_bank(6, chr_bank * 8 + 6);
  set_VRAM_bank(7, chr_bank * 8 + 7);
}

void MAP6_HSync(int scanline)
{
  if(MAPx->irq_enabled)
  {
    MAPx->irq_counter += 133;
    if(MAPx->irq_counter >= 0xFFFF)
    {
      CPU_IRQ;
      MAPx->irq_counter = 0;
    }
  }
}
void MAP6_Init()
{
	NES_Mapper->Reset = MAP6_Reset;
	NES_Mapper->Write = MAP6_MemoryWrite; 
	NES_Mapper->HSync = MAP6_HSync;
	NES_Mapper->WriteLow=MAP6_MemoryWriteLow;
}
