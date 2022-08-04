
/////////////////////////////////////////////////////////////////////
// Mapper 118
void NES_mapper118::Reset()
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
    chr0 = chr1 = chr2 = chr3 = chr4 = chr5 = chr6 = chr7 = 0;
  }

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper118::MemoryWrite(uint32 addr, uint8 data)
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

        if((regs[0] & 0x07) < 6)
        {
          if(data & 0x80)
          {
            set_mirroring(0,0,0,0);
          }
          else
          {
            set_mirroring(1,1,1,1);
          }
        }

        switch(regs[0] & 0x07)
        {
          case 0x00:
            {
              if(num_1k_VROM_banks)
              {
                bank_num &= 0xfe;
                chr0 = bank_num;
                chr1 = bank_num+1;
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
                chr3 = bank_num+1;
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

void NES_mapper118::HSync(uint32 scanline)
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

#define MAP118_ROM(ptr)  (((ptr)-parent_NES->ROM->get_ROM_banks())  >> 13)
#define MAP118_VROM(ptr) (((ptr)-parent_NES->ROM->get_VROM_banks()) >> 10)

void NES_mapper118::SNSS_fixup()
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);

  prg0 = MAP118_ROM(context.mem_page[prg_swap() ? 6 : 4]);
  prg1 = MAP118_ROM(context.mem_page[5]);
  if(num_1k_VROM_banks)
  {
    if(chr_swap())
    {
      chr0 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr1 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[5]);
      chr2 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr3 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[7]);
      chr4 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr5 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[1]);
      chr6 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr7 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[3]);
    }
    else
    {
      chr0 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr1 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[1]);
      chr2 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr3 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[3]);
      chr4 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr5 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[5]);
      chr6 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr7 = MAP118_VROM(parent_NES->ppu->PPU_VRAM_banks[7]);
    }
  }
}
/////////////////////////////////////////////////////////////////////

