#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 255
void NES_mapper255::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  set_mirroring(NES_PPU::MIRROR_VERT);

  //regs[0] = regs[2] = 0x0F;
  //regs[1] = regs[3] = 0x00;
}

uint8 NES_mapper255::MemoryReadLow(uint32 addr)
{
  if(addr >= 0x5800)
  {
    return regs[addr & 0x0003] & 0x0F;
  }
  else
  {
    return (uint8)(addr >> 8);
  }
}

void NES_mapper255::MemoryWriteLow(uint32 addr, uint8 data)
{
  if(addr >= 0x5800)
  {
    regs[addr & 0x0003] = data & 0x0F;
  }
}

void NES_mapper255::MemoryWrite(uint32 addr, uint8 data)
{
  uint8 prg_bank = (uint8)((addr & 0x0F80) >> 7);
  uint8 chr_bank = (uint8)(addr & 0x003F);
  uint8 rom_bank = (uint8)((addr & 0x4000) >> 14);

  if(addr & 0x2000)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }

  if(addr & 0x1000)
  {//16K PRG_ROM
    if(addr & 0x0040)
    {//upper half
      set_CPU_bank4(0x80*rom_bank+prg_bank*4+2);
      set_CPU_bank5(0x80*rom_bank+prg_bank*4+3);
      set_CPU_bank6(0x80*rom_bank+prg_bank*4+2);
      set_CPU_bank7(0x80*rom_bank+prg_bank*4+3);
    }
    else
    {//lower half
      set_CPU_bank4(0x80*rom_bank+prg_bank*4+0);
      set_CPU_bank5(0x80*rom_bank+prg_bank*4+1);
      set_CPU_bank6(0x80*rom_bank+prg_bank*4+0);
      set_CPU_bank7(0x80*rom_bank+prg_bank*4+1);
    }
  }
  else
  {//32K PRG_ROM
    set_CPU_bank4(0x80*rom_bank+prg_bank*4+0);
    set_CPU_bank5(0x80*rom_bank+prg_bank*4+1);
    set_CPU_bank6(0x80*rom_bank+prg_bank*4+2);
    set_CPU_bank7(0x80*rom_bank+prg_bank*4+3);
  }

  set_PPU_bank0(0x200*rom_bank+chr_bank*8+0);
  set_PPU_bank1(0x200*rom_bank+chr_bank*8+1);
  set_PPU_bank2(0x200*rom_bank+chr_bank*8+2);
  set_PPU_bank3(0x200*rom_bank+chr_bank*8+3);
  set_PPU_bank4(0x200*rom_bank+chr_bank*8+4);
  set_PPU_bank5(0x200*rom_bank+chr_bank*8+5);
  set_PPU_bank6(0x200*rom_bank+chr_bank*8+6);
  set_PPU_bank7(0x200*rom_bank+chr_bank*8+7);
}
/////////////////////////////////////////////////////////////////////

#endif
