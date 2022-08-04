
/////////////////////////////////////////////////////////////////////
// Mapper 89
void NES_mapper89::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper89::MemoryWrite(uint32 addr, uint8 data)
{
  if((addr & 0xFF00) == 0xC000)
  {
    uint8 prg_bank = (data & 0x70) >> 4;
    uint8 chr_bank = ((data & 0x80) >> 4) | (data & 0x07);

    set_CPU_bank4(prg_bank*2+0);
    set_CPU_bank5(prg_bank*2+1);

    set_PPU_bank0(chr_bank*8+0);
    set_PPU_bank1(chr_bank*8+1);
    set_PPU_bank2(chr_bank*8+2);
    set_PPU_bank3(chr_bank*8+3);
    set_PPU_bank4(chr_bank*8+4);
    set_PPU_bank5(chr_bank*8+5);
    set_PPU_bank6(chr_bank*8+6);
    set_PPU_bank7(chr_bank*8+7);

    if (data & 0x08)
    {
      set_mirroring(1,1,1,1);
    }
    else
    {
      set_mirroring(0,0,0,0);
    }
  }
}

/////////////////////////////////////////////////////////////////////

