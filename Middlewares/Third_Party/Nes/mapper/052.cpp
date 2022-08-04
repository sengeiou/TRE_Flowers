
/////////////////////////////////////////////////////////////////////
// Mapper 52
void NES_mapper52::Reset()
{
  // clear registers FIRST!!!
  for(int i = 0; i < 8; i++) regs[i] = 0x00;

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

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper52::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(!regs[1])
  {
    regs[1] = 1;
    regs[2] = data;
    MMC3_set_PPU_banks();
    MMC3_set_CPU_banks();
  }
}

void NES_mapper52::MemoryWrite(uint32 addr, uint8 data)
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
        uint32 bank_num = data;

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
        if(data & 0x80)
        {
          // enable save RAM $6000-$7FFF
        }
        else
        {
          // disable save RAM $6000-$7FFF
        }
      }
      break;

    case 0xC000:
      irq_counter = data;
      break;

    case 0xC001:
      irq_latch = data;
      break;

    case 0xE000:
      irq_enabled = 0;
      break;

    case 0xE001:
      irq_enabled = 1;
      break;
  }
}

void NES_mapper52::HSync(uint32 scanline)
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

void NES_mapper52::MMC3_set_CPU_banks()
{
  uint32 mask, bank;

  if(regs[2] & 0x08)
  {
    mask = 0x0f;
    bank = (regs[2] & 0x07) << 4;
  }
  else
  {
    mask = 0x1f;
    bank = (regs[2] & 0x06) << 4;
  }

  if(prg_swap())
  {
    set_CPU_banks(bank | (mask & (num_8k_ROM_banks-2)),
                  bank | (mask & prg1),
                  bank | (mask & prg0),
                  bank | (mask & (num_8k_ROM_banks-1)));
  }
  else
  {
    set_CPU_banks(bank | (mask & prg0),
                  bank | (mask & prg1),
                  bank | (mask & (num_8k_ROM_banks-2)),
                  bank | (mask & (num_8k_ROM_banks-1)));
  }
}

void NES_mapper52::MMC3_set_PPU_banks()
{
  uint32 mask, bank;

  if(regs[2] & 0x40)
  {
    mask = 0x7f;
    bank = (((regs[2] & 0x20) >> 3) | ((regs[2] & 0x10) >> 4) | ((regs[2] & 0x04) >> 1)) << 7;
  }
  else
  {
    mask = 0xff;
    bank = (((regs[2] & 0x20) >> 3) | ((regs[2] & 0x10) >> 4)) << 7;
  }

  if(chr_swap())
  {
    set_PPU_banks(bank | (mask & chr4),
                  bank | (mask & chr5),
                  bank | (mask & chr6),
                  bank | (mask & chr7),
                  bank | (mask & chr01),
                  bank | (mask & (chr01+1)),
                  bank | (mask & chr23),
                  bank | (mask & (chr23+1)));
  }
  else
  {
    set_PPU_banks(bank | (mask & chr01),
                  bank | (mask & (chr01+1)),
                  bank | (mask & chr23),
                  bank | (mask & (chr23+1)),
                  bank | (mask & chr4),
                  bank | (mask & chr5),
                  bank | (mask & chr6),
                  bank | (mask & chr7));
  }
}

#define MAP52_ROM(ptr)  (((ptr)-parent_NES->ROM->get_ROM_banks())  >> 13)
#define MAP52_VROM(ptr) (((ptr)-parent_NES->ROM->get_VROM_banks()) >> 10)

void NES_mapper52::SNSS_fixup() // HACK HACK HACK HACK
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);

  prg0 = MAP52_ROM(context.mem_page[prg_swap() ? 6 : 4]);
  prg1 = MAP52_ROM(context.mem_page[5]);
  if(num_1k_VROM_banks)
  {
    if(chr_swap())
    {
      chr01 = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr23 = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr4  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr5  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[1]);
      chr6  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr7  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[3]);
    }
    else
    {
      chr01 = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr23 = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr4  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr5  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[5]);
      chr6  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr7  = MAP52_VROM(parent_NES->ppu->PPU_VRAM_banks[7]);
    }
  }
}
/////////////////////////////////////////////////////////////////////

