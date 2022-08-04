
/////////////////////////////////////////////////////////////////////
// Mapper 44
void NES_mapper44::Reset()
{
  // clear registers FIRST!!!
  for(int i = 0; i < 8; i++) regs[i] = 0x00;

  // set game select register
  rom_bank = 0;

  // set CPU bank pointers
  prg0 = 0;
  prg1 = 1;
  MMC3_set_CPU_banks();

  // set VROM banks
  if(num_1k_VROM_banks)
  {
    chr01 = 0;
    chr23 = 2;
    chr4  = 4;
    chr5  = 5;
    chr6  = 6;
    chr7  = 7;
    MMC3_set_PPU_banks();
  }
  else
  {
    chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
  }

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper44::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xE001)
  {
    case 0x8000:
      {
        regs[0] = data;
        MMC3_set_PPU_banks();
        MMC3_set_CPU_banks();
      }
      break;

    case 0x8001:
      {
        uint32 bank_num;
        regs[1] = data;
        bank_num = regs[1];

        switch(regs[0] & 0x07)
        {
          case 0x00:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr01 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x01:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr23 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x02:
            {
              if(num_1k_VROM_banks)
              {
                chr4 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x03:
            {
              if(num_1k_VROM_banks)
              {
                chr5 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x04:
            {
              if(num_1k_VROM_banks)
              {
                chr6 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x05:
            {
              if(num_1k_VROM_banks)
              {
                chr7 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x06:
            {
              prg0 = bank_num;
              MMC3_set_CPU_banks();
            }
            break;

          case 0x07:
            {
              prg1 = bank_num;
              MMC3_set_CPU_banks();
            }
            break;
        }
      }
      break;

    case 0xA000:
      {
        regs[2] = data;

        if(parent_NES->ROM->get_mirroring() != NES_PPU::MIRROR_FOUR_SCREEN)
        {
          if(data & 0x01)
          {
            set_mirroring(NES_PPU::MIRROR_HORIZ);
          }
          else
          {
            set_mirroring(NES_PPU::MIRROR_VERT);
          }
        }
      }
      break;

    case 0xA001:
      {
        rom_bank = data & 0x07;
        if(rom_bank == 7)
        {
          rom_bank = 6;
        }
        MMC3_set_CPU_banks();
        MMC3_set_PPU_banks();
      }
      break;

    case 0xC000:
      regs[4] = data;
      irq_counter = regs[4];
      break;

    case 0xC001:
      regs[5] = data;
      irq_latch = regs[5];
      break;

    case 0xE000:
      regs[6] = data;
      irq_enabled = 0;
      break;

    case 0xE001:
      regs[7] = data;
      irq_enabled = 1;
      break;
  }
}

void NES_mapper44::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 239))
    {
      if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
      {
        if(!(irq_counter--))
        {
          irq_counter = irq_latch;
          parent_NES->cpu->DoIRQ();
        }
      }
    }
  }
}

void NES_mapper44::MMC3_set_CPU_banks()
{
  if(prg_swap())
  {
    set_CPU_bank4(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
    set_CPU_bank5(((rom_bank == 6) ? 0x1f & prg1 : 0x0f & prg1) | (rom_bank << 4));
    set_CPU_bank6(((rom_bank == 6) ? 0x1f & prg0 : 0x0f & prg0) | (rom_bank << 4));
    set_CPU_bank7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));
  }
  else
  {
    set_CPU_bank4(((rom_bank == 6) ? 0x1f & prg0: 0x0f & prg0) | (rom_bank << 4));
    set_CPU_bank5(((rom_bank == 6) ? 0x1f & prg1: 0x0f & prg1) | (rom_bank << 4));
    set_CPU_bank6(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
    set_CPU_bank7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));
  }
}

void NES_mapper44::MMC3_set_PPU_banks()
{
  if(num_1k_VROM_banks)
  {
    if(chr_swap())
    {
      set_PPU_bank0(((rom_bank == 6) ? 0xff & chr4 : 0x7f & chr4) | (rom_bank << 7));
      set_PPU_bank1(((rom_bank == 6) ? 0xff & chr5 : 0x7f & chr5) | (rom_bank << 7));
      set_PPU_bank2(((rom_bank == 6) ? 0xff & chr6 : 0x7f & chr6) | (rom_bank << 7));
      set_PPU_bank3(((rom_bank == 6) ? 0xff & chr7 : 0x7f & chr7) | (rom_bank << 7));
      set_PPU_bank4(((rom_bank == 6) ? 0xff & chr01 : 0x7f & chr01) | (rom_bank << 7));
      set_PPU_bank5(((rom_bank == 6) ? 0xff & (chr01+1) : 0x7f & (chr01+1)) | (rom_bank << 7));
      set_PPU_bank6(((rom_bank == 6) ? 0xff & chr23: 0x7f & chr23) | (rom_bank << 7));
      set_PPU_bank7(((rom_bank == 6) ? 0xff & (chr23+1) : 0x7f & (chr23+1)) | (rom_bank << 7));
    }
    else
    {
      set_PPU_bank0(((rom_bank == 6) ? 0xff & chr01 : 0x7f & chr01) | (rom_bank << 7));
      set_PPU_bank1(((rom_bank == 6) ? 0xff & (chr01+1) : 0x7f & (chr01+1)) | (rom_bank << 7));
      set_PPU_bank2(((rom_bank == 6) ? 0xff & chr23: 0x7f & chr23) | (rom_bank << 7));
      set_PPU_bank3(((rom_bank == 6) ? 0xff & (chr23+1) : 0x7f & (chr23+1)) | (rom_bank << 7));
      set_PPU_bank4(((rom_bank == 6) ? 0xff & chr4 : 0x7f & chr4) | (rom_bank << 7));
      set_PPU_bank5(((rom_bank == 6) ? 0xff & chr5 : 0x7f & chr5) | (rom_bank << 7));
      set_PPU_bank6(((rom_bank == 6) ? 0xff & chr6 : 0x7f & chr6) | (rom_bank << 7));
      set_PPU_bank7(((rom_bank == 6) ? 0xff & chr7 : 0x7f & chr7) | (rom_bank << 7));
    }
  }
}

/////////////////////////////////////////////////////////////////////

