
/////////////////////////////////////////////////////////////////////
// Mapper 100
void NES_mapper100::Reset()
{
  // clear registers FIRST!!!
  for(int i = 0; i < 8; i++) regs[i] = 0x00;

  // set CPU bank pointers
  prg0 = 0;
  prg1 = 1;
  prg2 = num_8k_ROM_banks - 2;
  prg3 = num_8k_ROM_banks - 1;
  MMC3_set_CPU_banks();

  // set VROM banks
  if(num_1k_VROM_banks)
  {
    chr0 = 0;
    chr1 = 1;
    chr2 = 2;
    chr3 = 3;
    chr4 = 4;
    chr5 = 5;
    chr6 = 6;
    chr7 = 7;
    MMC3_set_PPU_banks();
  }
  else
  {
    chr0 = chr2 = chr4 = chr5 = chr6 = chr7 = 0;
    chr1 = chr3 = 1;
  }

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper100::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xE001)
  {
    case 0x8000:
      {
        regs[0] = data;
      }
      break;

    case 0x8001:
      {
        uint32 bank_num;
        regs[1] = data;
        bank_num = regs[1];

        switch(regs[0] & 0xC7)
        {
          case 0x00:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr0 = bank_num;
                chr1 = bank_num + 1;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x01:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr2 = bank_num;
                chr3 = bank_num + 1;
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

          case 0x46:
            {
              prg2 = bank_num;
              MMC3_set_CPU_banks();
            }
            break;

          case 0x47:
            {
              prg3 = bank_num;
              MMC3_set_CPU_banks();
            }
            break;

          case 0x80:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr4 = bank_num;
                chr5 = bank_num + 1;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x81:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr6 = bank_num;
                chr7 = bank_num + 1;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x82:
            {
              if(num_1k_VROM_banks)
              {
                chr0 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x83:
            {
              if(num_1k_VROM_banks)
              {
                chr1 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x84:
            {
              if(num_1k_VROM_banks)
              {
                chr2 = bank_num;
                MMC3_set_PPU_banks();
              }
            }
            break;

          case 0x85:
            {
              if(num_1k_VROM_banks)
              {
                chr3 = bank_num;
                MMC3_set_PPU_banks();
              }
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
        regs[3] = data;
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

void NES_mapper100::HSync(uint32 scanline)
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
/////////////////////////////////////////////////////////////////////

