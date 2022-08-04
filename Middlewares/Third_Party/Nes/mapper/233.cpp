
/////////////////////////////////////////////////////////////////////
// Mapper 233
void NES_mapper233::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);
}

void NES_mapper233::MemoryWrite(uint32 addr, uint8 data)
{
  if(data & 0x20)
  {
    uint8 prg_bank = data & 0x1F;
    set_CPU_bank4(prg_bank*2+0);
    set_CPU_bank5(prg_bank*2+1);
    set_CPU_bank6(prg_bank*2+0);
    set_CPU_bank7(prg_bank*2+1);
  }
  else
  {
    uint8 prg_bank = (data & 0x1E) >> 1;
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }

  if((data & 0xC0) == 0x00)
  {
    set_mirroring(0,0,0,1);
  }
  else if((data & 0xC0) == 0x40)
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
  else if((data & 0xC0) == 0x80)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(1,1,1,1);
  }
}
/////////////////////////////////////////////////////////////////////

