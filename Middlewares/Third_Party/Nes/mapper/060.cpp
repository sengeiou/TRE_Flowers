#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 60
void NES_mapper60::Reset()
{
  set_CPU_banks(0,1,2,3);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper60::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x80)
  {
    set_CPU_bank4(2 * ((addr & 0x70) >> 4) + 0);
    set_CPU_bank5(2 * ((addr & 0x70) >> 4) + 1);
    set_CPU_bank6(2 * ((addr & 0x70) >> 4) + 0);
    set_CPU_bank7(2 * ((addr & 0x70) >> 4) + 1);
  }
  else
  {
    set_CPU_bank4(4 * ((addr & 0x70) >> 5) + 0);
    set_CPU_bank5(4 * ((addr & 0x70) >> 5) + 1);
    set_CPU_bank6(4 * ((addr & 0x70) >> 5) + 2);
    set_CPU_bank7(4 * ((addr & 0x70) >> 5) + 3);
  }

  set_PPU_bank0(8 * (addr & 0x07) + 0);
  set_PPU_bank1(8 * (addr & 0x07) + 1);
  set_PPU_bank2(8 * (addr & 0x07) + 2);
  set_PPU_bank3(8 * (addr & 0x07) + 3);
  set_PPU_bank4(8 * (addr & 0x07) + 4);
  set_PPU_bank5(8 * (addr & 0x07) + 5);
  set_PPU_bank6(8 * (addr & 0x07) + 6);
  set_PPU_bank7(8 * (addr & 0x07) + 7);

  if(data & 0x08)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
}
/////////////////////////////////////////////////////////////////////

#endif
