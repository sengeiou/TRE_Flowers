
/////////////////////////////////////////////////////////////////////
// Mapper 41
void NES_mapper41::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void NES_mapper41::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(addr < 0x6800)
  {
    regs[0] = (uint8)(addr & 0xFF);

    set_CPU_bank4((regs[0] & 0x07)*4+0);
    set_CPU_bank5((regs[0] & 0x07)*4+1);
    set_CPU_bank6((regs[0] & 0x07)*4+2);
    set_CPU_bank7((regs[0] & 0x07)*4+3);
    if(regs[0] & 0x20)
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
  }
}

void NES_mapper41::MemoryWrite(uint32 addr, uint8 data)
{
  if(regs[0] & 0x04)
  {
    uint8 chr_bank = ((regs[0] & 0x18) >> 1) | (data & 0x03);

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

