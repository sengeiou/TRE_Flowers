#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 97
void NES_mapper97::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(num_8k_ROM_banks-2,num_8k_ROM_banks-1,0,1);

  // set PPU bank pointers ?
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void NES_mapper97::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr < 0xC000)
  {
    uint8 prg_bank = data & 0x0F;

    set_CPU_bank6(prg_bank*2+0);
    set_CPU_bank7(prg_bank*2+1);

    if((data & 0x80) == 0)
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
  }
}
/////////////////////////////////////////////////////////////////////

#endif
