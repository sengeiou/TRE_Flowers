#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 94
void NES_mapper94::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

void NES_mapper94::MemoryWrite(uint32 addr, uint8 data)
{
  if((addr & 0xFFF0) == 0xFF00)
  {
    uint8 bank = (data & 0x1C) >> 2;
    set_CPU_bank4(bank*2+0);
    set_CPU_bank5(bank*2+1);
  }
}

/////////////////////////////////////////////////////////////////////

#endif
