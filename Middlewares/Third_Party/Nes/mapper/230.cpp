
/////////////////////////////////////////////////////////////////////
// Mapper 230
void NES_mapper230::Reset()
{
  // Contra - 22 Games switch
  if(rom_switch)
  {
    rom_switch = 0;
  }
  else
  {
    rom_switch = 1;
  }

  // set CPU bank pointers
  if(rom_switch)
  {
    set_CPU_banks(0,1,14,15);
  }
  else
  {
    set_CPU_banks(16,17,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  }
}

void NES_mapper230::MemoryWrite(uint32 addr, uint8 data)
{
  if(rom_switch)
  {
    set_CPU_bank4((data & 0x07)*2+0);
    set_CPU_bank5((data & 0x07)*2+1);
  }
  else
  {
    if(data & 0x40)
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    if(data & 0x20)
    {
      set_CPU_bank4((data & 0x1F)*2+16);
      set_CPU_bank5((data & 0x1F)*2+17);
      set_CPU_bank6((data & 0x1F)*2+16);
      set_CPU_bank7((data & 0x1F)*2+17);
    }
    else
    {
      set_CPU_bank4((data & 0x1E)*2+16);
      set_CPU_bank5((data & 0x1E)*2+17);
      set_CPU_bank6((data & 0x1E)*2+18);
      set_CPU_bank7((data & 0x1E)*2+19);
    }
  }
}
/////////////////////////////////////////////////////////////////////

