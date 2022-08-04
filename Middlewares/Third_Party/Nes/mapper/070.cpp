#include "nes_mapper.h"


// Mapper 70
void MAP70_Reset()
{
//  patch = 0;

//  if(parent_NES->crc32() == 0xa59ca2ef || // Kamen Rider Club
//     parent_NES->crc32() == 0x10bb8f9a)   // Family Trainer - Manhattan Police
//  {
//    patch = 1;
//  }

//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

void MAP70_MemoryWrite(uint16 addr, uint8 data)
{
  uint8 chr_bank = data & 0x0F;
  uint8 prg_bank = (data & 0x70) >> 4;

  set_CPU_bank4(prg_bank*2+0);
  set_CPU_bank5(prg_bank*2+1);

  set_PPU_bank0(chr_bank*8+0);
  set_PPU_bank1(chr_bank*8+1);
  set_PPU_bank2(chr_bank*8+2);
  set_PPU_bank3(chr_bank*8+3);
  set_PPU_bank4(chr_bank*8+4);
  set_PPU_bank5(chr_bank*8+5);
  set_PPU_bank6(chr_bank*8+6);
  set_PPU_bank7(chr_bank*8+7);

//  if(patch)
//  {
//    if(data & 0x80)
//    {
//      set_mirroring(NES_PPU::MIRROR_HORIZ);
//    }
//    else
//    {
//      set_mirroring(NES_PPU::MIRROR_VERT);
//    }
//  }
//  else
  {
    if(data & 0x80)
    {
      set_mirroring(1,1,1,1);
    }
    else
    {
      set_mirroring(0,0,0,0);
    }
  }
}
void MAP70_Init()
{
	NES_Mapper->Reset = MAP70_Reset;
	NES_Mapper->Write = MAP70_MemoryWrite; 
}












