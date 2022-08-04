#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 180
void NES_mapper180::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  if(parent_NES->crc32() == 0xc68363f6) // Crazy Climber
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
}

void NES_mapper180::MemoryWrite(uint32 addr, uint8 data)
{
  set_CPU_bank6((data & 0x07)*2+0);
  set_CPU_bank7((data & 0x07)*2+1);
}
/////////////////////////////////////////////////////////////////////

#endif
