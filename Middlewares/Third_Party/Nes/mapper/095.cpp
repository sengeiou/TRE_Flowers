#ifdef _NES_MAPPER_CPP_

/////////////////////////////////////////////////////////////////////
// Mapper 95
void NES_mapper95::Reset()
{
  regs[0] = 0x00;

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
}

void NES_mapper95::MemoryWrite(uint32 addr, uint8 data)
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

        if(regs[0] <= 0x05)
        {
          if(data & 0x20)
          {
            set_mirroring(1,1,1,1);
          }
          else
          {
            set_mirroring(0,0,0,0);
          }
          data &= 0x1F;
        }

        bank_num = data;

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
  }
}

#define MAP95_ROM(ptr)  (((ptr)-parent_NES->ROM->get_ROM_banks())  >> 13)
#define MAP95_VROM(ptr) (((ptr)-parent_NES->ROM->get_VROM_banks()) >> 10)

void NES_mapper95::SNSS_fixup()
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);

  prg0 = MAP95_ROM(context.mem_page[prg_swap() ? 6 : 4]);
  prg1 = MAP95_ROM(context.mem_page[5]);
  if(num_1k_VROM_banks)
  {
    if(chr_swap())
    {
      chr01 = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr23 = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr4  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr5  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[1]);
      chr6  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr7  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[3]);
    }
    else
    {
      chr01 = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
      chr23 = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
      chr4  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
      chr5  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[5]);
      chr6  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
      chr7  = MAP95_VROM(parent_NES->ppu->PPU_VRAM_banks[7]);
    }
  }
}
/////////////////////////////////////////////////////////////////////

#endif
