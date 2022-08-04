
/////////////////////////////////////////////////////////////////////
// Mapper 51
void NES_mapper51::Reset()
{
  bank = 0;
  mode = 1;
  Sync_Prg_Banks();
  parent_NES->ppu->vram_write_protect = 0;
}

void NES_mapper51::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  mode = ((data & 0x10) >> 3) | ((data & 0x02) >> 1);
  Sync_Prg_Banks();
}

void NES_mapper51::MemoryWrite(uint32 addr, uint8 data)
{
  bank = (data & 0x0f) << 2;
  if(0xC000 <= addr && addr <= 0xDFFF)
  {
    mode = (mode & 0x01) | ((data & 0x10) >> 3);
  }
  Sync_Prg_Banks();
}

void NES_mapper51::Sync_Prg_Banks()
{
  switch(mode)
  {
    case 0:
      {
        set_mirroring(NES_PPU::MIRROR_VERT);
        set_CPU_bank3(bank | 0x2c | 3);
        set_CPU_bank4(bank | 0x00 | 0);
        set_CPU_bank5(bank | 0x00 | 1);
        set_CPU_bank6(bank | 0x0c | 2);
        set_CPU_bank7(bank | 0x0c | 3);
      }
      break;

    case 1:
      {
        set_mirroring(NES_PPU::MIRROR_VERT);
        set_CPU_bank3(bank | 0x20 | 3);
        set_CPU_bank4(bank | 0x00 | 0);
        set_CPU_bank5(bank | 0x00 | 1);
        set_CPU_bank6(bank | 0x00 | 2);
        set_CPU_bank7(bank | 0x00 | 3);
      }
      break;

    case 2:
      {
        set_mirroring(NES_PPU::MIRROR_VERT);
        set_CPU_bank3(bank | 0x2e | 3);
        set_CPU_bank4(bank | 0x02 | 0);
        set_CPU_bank5(bank | 0x02 | 1);
        set_CPU_bank6(bank | 0x0e | 2);
        set_CPU_bank7(bank | 0x0e | 3);
      }
      break;

    case 3:
      {
        set_mirroring(NES_PPU::MIRROR_HORIZ);
        set_CPU_bank3(bank | 0x20 | 3);
        set_CPU_bank4(bank | 0x00 | 0);
        set_CPU_bank5(bank | 0x00 | 1);
        set_CPU_bank6(bank | 0x00 | 2);
        set_CPU_bank7(bank | 0x00 | 3);
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

