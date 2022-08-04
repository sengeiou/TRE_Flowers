#include "nes_mapper.h"
 
// Mapper 7
void MAP7_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,2,3);
} 
void MAP7_MemoryWrite(uint16 addr, uint8 data)
{
  uint32 bank;

  bank = (data & 0x07) << 2;
  set_CPU_banks(bank+0,bank+1,bank+2,bank+3); 
  if(data & 0x10)
  {
    set_mirroring(1,1,1,1);
  }
  else
  {
    set_mirroring(0,0,0,0);
  }
}
void MAP7_Init()
{
	NES_Mapper->Reset = MAP7_Reset;
	NES_Mapper->Write = MAP7_MemoryWrite;  
}















