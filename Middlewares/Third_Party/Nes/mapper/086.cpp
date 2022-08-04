
/////////////////////////////////////////////////////////////////////
// Mapper 86
void NES_mapper86::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper86::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(addr == 0x6000)
  {
    uint8 chr_bank = data & 0x03 | (data & 0x40) >> 4;
    uint8 prg_bank = (data & 0x30) >> 4;

    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);

    set_PPU_bank0(chr_bank*8+0);
    set_PPU_bank1(chr_bank*8+1);
    set_PPU_bank2(chr_bank*8+2);
    set_PPU_bank3(chr_bank*8+3);
    set_PPU_bank4(chr_bank*8+4);
    set_PPU_bank5(chr_bank*8+5);
    set_PPU_bank6(chr_bank*8+6);
    set_PPU_bank7(chr_bank*8+7);
  }
}
/////////////////////////////////////////////////////////////////////

