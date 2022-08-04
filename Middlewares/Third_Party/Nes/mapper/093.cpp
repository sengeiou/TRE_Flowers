
/////////////////////////////////////////////////////////////////////
// Mapper 93
void NES_mapper93::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper93::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(addr == 0x6000)
  {
    set_CPU_bank4(data*2+0);
    set_CPU_bank5(data*2+1);
  }
}

/////////////////////////////////////////////////////////////////////

