
/////////////////////////////////////////////////////////////////////
// Mapper 226
void NES_mapper226::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  regs[0] = 0;
  regs[1] = 0;
}

void NES_mapper226::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x001)
  {
    regs[1] = data;
  }
  else
  {
    regs[0] = data;
  }

  if(regs[0] & 0x40)
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }

  uint8 prg_bank = ((regs[0] & 0x1E) >> 1) | ((regs[0] & 0x80) >> 3) | ((regs[1] & 0x01) << 5);

  if(regs[0] & 0x20)
  {
    if(regs[0] & 0x01)
    {
      set_CPU_bank4(prg_bank*4+2);
      set_CPU_bank5(prg_bank*4+3);
      set_CPU_bank6(prg_bank*4+2);
      set_CPU_bank7(prg_bank*4+3);
    }
    else
    {
      set_CPU_bank4(prg_bank*4+0);
      set_CPU_bank5(prg_bank*4+1);
      set_CPU_bank6(prg_bank*4+0);
      set_CPU_bank7(prg_bank*4+1);
    }
  }
  else
  {
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }
}
/////////////////////////////////////////////////////////////////////

