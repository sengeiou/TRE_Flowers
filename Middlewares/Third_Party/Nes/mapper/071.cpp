#include "nes_mapper.h"
 
// Mapper 71
void MAP71_Reset()
{
  // set CPU bank pointers
  //set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

//void MAP71_MemoryWriteSaveRAM(uint32 addr, uint8 data)
//{
//  set_CPU_bank4(data*2+0);
//  set_CPU_bank5(data*2+1);
//}

void MAP71_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr & 0xF000)
  {
    case 0x9000:
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

    case 0xC000:
    case 0xD000:
    case 0xE000:
    case 0xF000:
      {
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
      }
      break;
  }
}
void MAP71_Init()
{
	NES_Mapper->Reset = MAP71_Reset;
	NES_Mapper->Write = MAP71_MemoryWrite; 
}












