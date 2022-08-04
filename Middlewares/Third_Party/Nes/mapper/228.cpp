
/////////////////////////////////////////////////////////////////////
// Mapper 228
void NES_mapper228::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper228::MemoryWrite(uint32 addr, uint8 data)
{
  uint8 prg_bank = (addr & 0x0780) >> 7;
  uint8 chr_bank = ((addr & 0x000F) << 2) | (data & 0x03);

  switch((addr & 0x1800) >> 11)
  {
    case 1:
      {
        prg_bank |= 0x10;
      }
      break;

    case 3:
      {
        prg_bank |= 0x20;
      }
      break;
  }
  if(addr & 0x0020)
  {
    prg_bank = prg_bank << 1;
    if(addr & 0x0040)
    {
      prg_bank++;
    }
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+0);
    set_CPU_bank7(prg_bank*4+1);
  }
  else
  {
    set_CPU_bank4(prg_bank*4+0);
    set_CPU_bank5(prg_bank*4+1);
    set_CPU_bank6(prg_bank*4+2);
    set_CPU_bank7(prg_bank*4+3);
  }

  set_PPU_bank0(chr_bank*8+0);
  set_PPU_bank1(chr_bank*8+1);
  set_PPU_bank2(chr_bank*8+2);
  set_PPU_bank3(chr_bank*8+3);
  set_PPU_bank4(chr_bank*8+4);
  set_PPU_bank5(chr_bank*8+5);
  set_PPU_bank6(chr_bank*8+6);
  set_PPU_bank7(chr_bank*8+7);

  if(addr & 0x2000)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
}
/////////////////////////////////////////////////////////////////////

