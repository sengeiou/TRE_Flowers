#include "nes_mapper.h"



// Mapper 99
void MAP99_Reset()
{
//  // set CPU bank pointers
//  if(num_8k_ROM_banks > 2)
//  {
//    set_CPU_banks(0,1,2,3);
//  }
//  else if(num_8k_ROM_banks > 1)
//  {
//    set_CPU_banks(0,1,0,1);
//  }
//  else
//  {
//    set_CPU_banks(0,0,0,0);
//  }

  // set VROM bank
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void MAP99_WriteHighRegs(uint16 addr, uint8 data)
{
  if(addr == 0x4016)
  {
    if(data & 0x04)
    {
      set_PPU_banks(8,9,10,11,12,13,14,15);
    }
    else
    {
      set_PPU_banks(0,1,2,3,4,5,6,7);
    }
  }
}
void MAP99_Init()
{
	NES_Mapper->Reset = MAP99_Reset;
	NES_Mapper->WriteLow=MAP99_WriteHighRegs;
}



















