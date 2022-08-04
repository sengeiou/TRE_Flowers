#include "nes_mapper.h"


// Mapper 8
void MAP8_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7); 
}

void MAP8_MemoryWrite(uint16 addr, uint8 data)
{
  uint8 prg_bank = (data & 0xF8) >> 3;
  uint8 chr_bank = data & 0x07;
  
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
}
void MAP8_Init()
{
	NES_Mapper->Reset = MAP8_Reset;
	NES_Mapper->Write = MAP8_MemoryWrite;  
}

