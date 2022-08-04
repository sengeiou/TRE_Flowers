#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 46
void NES_mapper46::Reset()
{
  regs[0] = regs[1] = regs[2] = regs[3] = 0;

  set_rom_banks();
  set_mirroring(NES_PPU::MIRROR_VERT);
}

void NES_mapper46::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  regs[0] = data & 0x0F;
  regs[1] = (data & 0xF0) >> 4;
  set_rom_banks();
}

void NES_mapper46::MemoryWrite(uint32 addr, uint8 data)
{
  regs[2] = data & 0x01;
  regs[3] = (data & 0x70) >> 4;
  set_rom_banks();
}

void NES_mapper46::set_rom_banks()
{
  set_CPU_bank4(regs[0]*8+regs[2]*4+0);
  set_CPU_bank5(regs[0]*8+regs[2]*4+1);
  set_CPU_bank6(regs[0]*8+regs[2]*4+2);
  set_CPU_bank7(regs[0]*8+regs[2]*4+3);
  set_PPU_bank0(regs[1]*64+regs[3]*8+0);
  set_PPU_bank1(regs[1]*64+regs[3]*8+1);
  set_PPU_bank2(regs[1]*64+regs[3]*8+2);
  set_PPU_bank3(regs[1]*64+regs[3]*8+3);
  set_PPU_bank4(regs[1]*64+regs[3]*8+4);
  set_PPU_bank5(regs[1]*64+regs[3]*8+5);
  set_PPU_bank6(regs[1]*64+regs[3]*8+6);
  set_PPU_bank7(regs[1]*64+regs[3]*8+7);
}
/////////////////////////////////////////////////////////////////////

#endif
