#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 101
void NES_mapper101::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper101::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  data &= 0x03;
  set_PPU_bank0(data*8+0);
  set_PPU_bank1(data*8+1);
  set_PPU_bank2(data*8+2);
  set_PPU_bank3(data*8+3);
  set_PPU_bank4(data*8+4);
  set_PPU_bank5(data*8+5);
  set_PPU_bank6(data*8+6);
  set_PPU_bank7(data*8+7);
}

void NES_mapper101::MemoryWrite(uint32 addr, uint8 data)
{
  data &= 0x03;
  set_PPU_bank0(data*8+0);
  set_PPU_bank1(data*8+1);
  set_PPU_bank2(data*8+2);
  set_PPU_bank3(data*8+3);
  set_PPU_bank4(data*8+4);
  set_PPU_bank5(data*8+5);
  set_PPU_bank6(data*8+6);
  set_PPU_bank7(data*8+7);
}
/////////////////////////////////////////////////////////////////////

#endif
