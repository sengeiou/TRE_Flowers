
/////////////////////////////////////////////////////////////////////
// Mapper 229
void NES_mapper229::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper229::MemoryWrite(uint32 addr, uint8 data)
{
  addr &= 0x0FFF;

  if(addr & 0x0020)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }

  if(addr & 0x001E)
  {
    uint8 prg_bank = (uint8)(addr & 0x001F);
    uint32 chr_bank = addr & 0x0FFF;
    set_CPU_bank4(prg_bank*2+0);
    set_CPU_bank5(prg_bank*2+1);
    set_CPU_bank6(prg_bank*2+0);
    set_CPU_bank7(prg_bank*2+1);
    set_PPU_bank0(chr_bank*8+0);
    set_PPU_bank1(chr_bank*8+1);
    set_PPU_bank2(chr_bank*8+2);
    set_PPU_bank3(chr_bank*8+3);
    set_PPU_bank4(chr_bank*8+4);
    set_PPU_bank5(chr_bank*8+5);
    set_PPU_bank6(chr_bank*8+6);
    set_PPU_bank7(chr_bank*8+7);
  }
  else
  {
    set_CPU_banks(0,1,2,3);
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}
/////////////////////////////////////////////////////////////////////

