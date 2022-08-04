
/////////////////////////////////////////////////////////////////////
// Mapper 236
void NES_mapper236::Reset()
{
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  bank = mode = 0;
}

void NES_mapper236::MemoryWrite(uint32 addr, uint8 data)
{
  if(0x8000 <= addr && addr <= 0xbfff)
  {
    bank = ((addr & 0x03) << 4) | (bank & 0x07);
  }
  else
  {
    bank = (addr & 0x07) | (bank & 0x30);
    mode = addr & 0x30;
  }
  if(addr & 0x20)
  {
    set_mirroring(NES_PPU::MIRROR_HORIZ);
  }
  else
  {
    set_mirroring(NES_PPU::MIRROR_VERT);
  }
  switch(mode)
  {
    case 0x00:
      {
        bank |= 0x08;
        set_CPU_bank4(bank*2+0);
        set_CPU_bank5(bank*2+1);
        set_CPU_bank6((bank|0x07)*2+0);
        set_CPU_bank7((bank|0x07)*2+1);
      }
      break;

    case 0x10:
      {
        bank &= 0x37;
        set_CPU_bank4(bank*2+0);
        set_CPU_bank5(bank*2+1);
        set_CPU_bank6((bank|0x07)*2+0);
        set_CPU_bank7((bank|0x07)*2+1);
      }
      break;

    case 0x20:
      {
        bank |= 0x08;
        set_CPU_bank4((bank&0xfe)*2+0);
        set_CPU_bank5((bank&0xfe)*2+1);
        set_CPU_bank6((bank&0xfe)*2+2);
        set_CPU_bank7((bank&0xfe)*2+3);
      }
      break;

    case 0x30:
      {
        bank |= 0x08;
        set_CPU_bank4(bank*2+0);
        set_CPU_bank5(bank*2+1);
        set_CPU_bank6(bank*2+0);
        set_CPU_bank7(bank*2+1);
      }
      break;
  }
}
/////////////////////////////////////////////////////////////////////

