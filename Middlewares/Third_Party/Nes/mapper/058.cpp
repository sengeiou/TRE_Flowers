#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 58
void NES_mapper58::Reset()
{
  set_CPU_banks(0,1,0,1);
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper58::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr & 0x40)
  {
    set_CPU_bank4(2 * (addr & 0x07) + 0);
    set_CPU_bank5(2 * (addr & 0x07) + 1);
    set_CPU_bank6(2 * (addr & 0x07) + 0);
    set_CPU_bank7(2 * (addr & 0x07) + 1);
  }
  else
  {
    set_CPU_bank4(4 * ((addr & 0x06) >> 1) + 0);
    set_CPU_bank5(4 * ((addr & 0x06) >> 1) + 1);
    set_CPU_bank6(4 * ((addr & 0x06) >> 1) + 2);
    set_CPU_bank7(4 * ((addr & 0x06) >> 1) + 3);
  }

  set_PPU_bank0(8 * ((addr & 0x38) >> 3) + 0);
  set_PPU_bank1(8 * ((addr & 0x38) >> 3) + 1);
  set_PPU_bank2(8 * ((addr & 0x38) >> 3) + 2);
  set_PPU_bank3(8 * ((addr & 0x38) >> 3) + 3);
  set_PPU_bank4(8 * ((addr & 0x38) >> 3) + 4);
  set_PPU_bank5(8 * ((addr & 0x38) >> 3) + 5);
  set_PPU_bank6(8 * ((addr & 0x38) >> 3) + 6);
  set_PPU_bank7(8 * ((addr & 0x38) >> 3) + 7);

  if(data & 0x02)
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
}
/////////////////////////////////////////////////////////////////////

#endif
