#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 34
void MAP34_Reset()
{
//  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

//void MAP34_MemoryWriteSaveRAM(uint16 addr, uint8 data)
//{
//  switch(addr)
//  {
//    case 0x7FFD:
//      {
//        set_CPU_banks(data*4,data*4+1,data*4+2,data*4+3);
//      }
//      break;

//    case 0x7FFE:
//      {
//        set_PPU_bank0(data*4+0);
//        set_PPU_bank1(data*4+1);
//        set_PPU_bank2(data*4+2);
//        set_PPU_bank3(data*4+3);
//      }
//      break;

//    case 0x7FFF:
//      {
//        set_PPU_bank4(data*4+0);
//        set_PPU_bank5(data*4+1);
//        set_PPU_bank6(data*4+2);
//        set_PPU_bank7(data*4+3);
//      }
//      break;
//  }
//}

void MAP34_MemoryWrite(uint16 addr, uint8 data)
{
  set_CPU_banks(data*4,data*4+1,data*4+2,data*4+3);
}
void MAP34_Init()
{
	NES_Mapper->Reset = MAP34_Reset;
	NES_Mapper->Write = MAP34_MemoryWrite;  
}


























