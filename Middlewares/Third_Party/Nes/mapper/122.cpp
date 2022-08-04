#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 122
void NES_mapper122::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);
}

void NES_mapper122::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(addr = 0x6000)
  {
    uint8 chr_bank0 = data & 0x07;
    uint8 chr_bank1 = (data & 0x70) >> 4;

    set_PPU_bank0(chr_bank0*4+0);
    set_PPU_bank1(chr_bank0*4+1);
    set_PPU_bank2(chr_bank0*4+2);
    set_PPU_bank3(chr_bank0*4+3);
    set_PPU_bank4(chr_bank1*4+0);
    set_PPU_bank5(chr_bank1*4+1);
    set_PPU_bank6(chr_bank1*4+2);
    set_PPU_bank7(chr_bank1*4+3);
  }
}

/////////////////////////////////////////////////////////////////////

#endif
