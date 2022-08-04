
/////////////////////////////////////////////////////////////////////
// Mapper 231
void NES_mapper231::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void NES_mapper231::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x0020)
  {
    uint8 prg_bank = (uint8)(addr >> 1) & 0x0F;
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }
  else
  {
    uint8 prg_bank = addr & 0x001E;
    set_CPU_bank4(prg_bank*2+0);
    set_CPU_bank5(prg_bank*2+1);
    set_CPU_bank6(prg_bank*2+0);
    set_CPU_bank7(prg_bank*2+1);
  }

  if(addr & 0x0080)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
}
/////////////////////////////////////////////////////////////////////

