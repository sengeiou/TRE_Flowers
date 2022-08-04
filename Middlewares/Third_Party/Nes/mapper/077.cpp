
/////////////////////////////////////////////////////////////////////
// Mapper 77
void NES_mapper77::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // for Napoleon Senki
  parent_NES->ppu->vram_write_protect = 0;
}

void NES_mapper77::MemoryWrite(uint32 addr, uint8 data)
{
  uint8 prg_bank = data & 0x07;
  uint8 chr_bank = (data & 0xF0) >> 4;

  set_CPU_bank4(prg_bank*4+0);
  set_CPU_bank5(prg_bank*4+1);
  set_CPU_bank6(prg_bank*4+2);
  set_CPU_bank7(prg_bank*4+3);

  set_PPU_bank0(chr_bank*2+0);
  set_PPU_bank1(chr_bank*2+1);
}
/////////////////////////////////////////////////////////////////////

