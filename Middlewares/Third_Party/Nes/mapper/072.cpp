#include "nes_mapper.h"
 
// Mapper 72 (Created by rinao)
void MAP72_Reset()
{
  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void MAP72_MemoryWrite(uint16 addr, uint8 data)
{
  uint8 bank = data & 0x0f;
  if (data & 0x80)
  {
    set_CPU_banks(bank*2, bank*2+1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  }
  if (data & 0x40)
  {
    set_PPU_banks(bank*8, bank*8+1,bank*8+2,bank*8+3,bank*8+4,bank*8+5,bank*8+6,bank*8+7);
  }
}
void MAP72_Init()
{
	NES_Mapper->Reset = MAP72_Reset;
	NES_Mapper->Write = MAP72_MemoryWrite; 
}

















