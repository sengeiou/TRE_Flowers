
/////////////////////////////////////////////////////////////////////
// Mapper 242
void NES_mapper242::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void NES_mapper242::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x0001)
  {
    set_CPU_bank4(((addr & 0x78) >> 1) + 0);
    set_CPU_bank5(((addr & 0x78) >> 1) + 1);
    set_CPU_bank6(((addr & 0x78) >> 1) + 2);
    set_CPU_bank7(((addr & 0x78) >> 1) + 3);
  }
}
/////////////////////////////////////////////////////////////////////

