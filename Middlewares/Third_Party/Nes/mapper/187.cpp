
/////////////////////////////////////////////////////////////////////
// Mapper 187
void NES_mapper187::Reset()
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
  else
  {
    chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
  }

  ex_prg0 = 0;
  ex_prg1 = 1;
  ex_bank_enabled = ex_bank_mode = 0;

  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper187::MemoryWriteLow(uint32 addr, uint8 data)
{
  if(addr == 0x5000)
  {
    ex_bank_mode = data;

    if(data & 0x80)
    {
      if(data & 0x20)
      {
        set_CPU_bank4((data & 0x1E)*2+0);
        set_CPU_bank5((data & 0x1E)*2+1);
        set_CPU_bank6((data & 0x1E)*2+2);
        set_CPU_bank7((data & 0x1E)*2+3);
      }
      else
      {
        set_CPU_bank5((data & 0x1F)*2+0);
        set_CPU_bank6((data & 0x1F)*2+1);
      }
    }
    else
    {
      set_CPU_banks(ex_prg0,ex_prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
    }
  }
}

void NES_mapper187::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        ex_bank_enabled = 0;
        regs[0] = data;
        MMC3_set_PPU_banks();
        //MMC3_set_CPU_banks();
      }
      break;

    case 0x8001:
      {
        uint32 bank_num;
        regs[1] = data;
        bank_num = regs[1];

        if(ex_bank_enabled)
        {
          if(regs[0] == 0x2A)
          {
            set_CPU_bank5(0x0F);
          }
          if(regs[0] == 0x28)
          {
            set_CPU_bank6(0x17);
          }
        }
        else
        {
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
                if((ex_bank_mode & 0xA0) != 0xA0)
                {
                  prg0 = bank_num;
                  MMC3_set_CPU_banks();
                }
                ex_prg0 = data;
              }
              break;

            case 0x07:
              {
                if((ex_bank_mode & 0xA0) != 0xA0)
                {
                  prg1 = bank_num;
                  MMC3_set_CPU_banks();
                }
                ex_prg1 = data;
              }
              break;
          }
        }
        if((regs[0] & 0x07) == 0x06) ex_prg0 = data;
        if((regs[0] & 0x07) == 0x07) ex_prg1 = data;
      }
      break;

    case 0x8003:
      {
        ex_bank_enabled = 1;
        if(!(data & 0xF0))
        {
          set_CPU_bank6(num_8k_ROM_banks-2);
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
    case 0xE002:
      regs[6] = data;
      irq_enabled = 0;
      break;

    case 0xE001:
    case 0xE003:
      regs[7] = data;
      irq_enabled = 1;
      break;
  }
}

void NES_mapper187::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 239))
    {
      if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
      {
        if(!(--irq_counter))
        {
          irq_counter = irq_latch;
          parent_NES->cpu->DoIRQ();
        }
      }
    }
  }
}

void NES_mapper187::MMC3_set_CPU_banks()
{
  if(prg_swap())
  {
    set_CPU_banks(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
  }
  else
  {
    set_CPU_banks(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  }
}

void NES_mapper187::MMC3_set_PPU_banks()
{
  if(num_1k_VROM_banks)
  {
    if(chr_swap())
    {
      set_PPU_banks(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
    }
    else
    {
      set_PPU_banks(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);
    }
  }
}
/////////////////////////////////////////////////////////////////////

