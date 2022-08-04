#include "nes_mapper.h"

uint8 prg_bank;
uint8 chr_bank;
  
// Mapper 13
void MAP13_Reset()
{
//  // set CPU bank pointers
//  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  set_VRAM_bank(0, 0);
  set_VRAM_bank(1, 1);
  set_VRAM_bank(2, 2);
  set_VRAM_bank(3, 3);
  set_VRAM_bank(4, 0);
  set_VRAM_bank(5, 1);
  set_VRAM_bank(6, 2);
  set_VRAM_bank(7, 3);

 // parent_NES->ppu->vram_size = 0x4000;
  prg_bank = chr_bank = 0;
}

void MAP13_MemoryWrite(uint16 addr, uint8 data)
{
  prg_bank = (data & 0x30) >> 4;
  chr_bank = data & 0x03;

  set_CPU_bank4(prg_bank*4+0);
  set_CPU_bank5(prg_bank*4+1);
  set_CPU_bank6(prg_bank*4+2);
  set_CPU_bank7(prg_bank*4+3);

  set_VRAM_bank(4, chr_bank * 4 + 0);
  set_VRAM_bank(5, chr_bank * 4 + 1);
  set_VRAM_bank(6, chr_bank * 4 + 2);
  set_VRAM_bank(7, chr_bank * 4 + 3);
}

//void MAP13_SNSS_fixup()
//{
//  set_VRAM_bank(4, chr_bank * 4 + 0);
//  set_VRAM_bank(5, chr_bank * 4 + 1);
//  set_VRAM_bank(6, chr_bank * 4 + 2);
//  set_VRAM_bank(7, chr_bank * 4 + 3);
//}
void MAP13_Init()
{
	NES_Mapper->Reset = MAP13_Reset;
	NES_Mapper->Write = MAP13_MemoryWrite;  
}























