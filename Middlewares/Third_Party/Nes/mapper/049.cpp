
/////////////////////////////////////////////////////////////////////
// Mapper 49
void NES_mapper49::Reset()
{
  // set CPU bank pointers
  prg0 = 0;
  prg1 = 1;
  set_CPU_banks(0,1,2,3);

  // set VROM banks
  chr01 = 0;
  chr23 = 2;
  chr4  = 4;
  chr5  = 5;
  chr6  = 6;
  chr7  = 7;
  set_PPU_banks(0,1,2,3,4,5,6,7);

  regs[0] = regs[1] = regs[2] = 0;
  irq_enabled = 0;
  irq_counter = 0;
  irq_latch = 0;
}

void NES_mapper49::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  if(regs[2] & 0x80)
  {
    regs[1] = data;
    MMC3_set_CPU_banks();
    MMC3_set_PPU_banks();
  }
}

void NES_mapper49::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xE001)
  {
    case 0x8000:
      {
        if((data & 0x40) != (regs[0] & 0x40))
        {
          MMC3_set_CPU_banks();
        }
        if((data & 0x80) != (regs[0] & 0x80))
        {
          regs[0] = data;
          MMC3_set_PPU_banks();
        }
        regs[0] = data;
      }
      break;

    case 0x8001:
      {
        switch(regs[0] & 0x07)
        {
          case 0x00:
            {
              chr01 = data & 0xFE;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x01:
            {
              chr23 = data & 0xFE;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x02:
            {
              chr4 = data;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x03:
            {
              chr5 = data;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x04:
            {
              chr6 = data;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x05:
            {
              chr7 = data;
              MMC3_set_PPU_banks();
            }
            break;

          case 0x06:
            {
              prg0 = data;
              MMC3_set_CPU_banks();
            }
            break;

          case 0x07:
            {
              prg1 = data;
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
        regs[2] = data;
      }
      break;

    case 0xC000:
      {
        irq_counter = data;
      }
      break;

    case 0xC001:
      {
        irq_latch = data;
      }
      break;

    case 0xE000:
      {
        irq_enabled = 0;
      }
      break;

    case 0xE001:
      {
        irq_enabled = 1;
      }
      break;
  }
}

void NES_mapper49::HSync(uint32 scanline)
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

void NES_mapper49::MMC3_set_CPU_banks()
{
  if(regs[1] & 0x01)
  {
    if(regs[0] & 0x40)
    {
      set_CPU_bank4(((num_8k_ROM_banks - 1) & 0xE) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank5((prg1 & 0x0F) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank6((prg0 & 0x0F) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank7(((num_8k_ROM_banks - 1) & 0xF) | ((regs[1] & 0xC0) >> 2));
    }
    else
    {
      set_CPU_bank4((prg0 & 0x0F) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank5((prg1 & 0x0F) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank6(((num_8k_ROM_banks - 1) & 0xE) | ((regs[1] & 0xC0) >> 2));
      set_CPU_bank7(((num_8k_ROM_banks - 1) & 0xF) | ((regs[1] & 0xC0) >> 2));
    }
  }
  else
  {
    set_CPU_bank4(((regs[1] & 0x70) >> 2) | 0);
    set_CPU_bank5(((regs[1] & 0x70) >> 2) | 1);
    set_CPU_bank6(((regs[1] & 0x70) >> 2) | 2);
    set_CPU_bank7(((regs[1] & 0x70) >> 2) | 3);
  }
}

void NES_mapper49::MMC3_set_PPU_banks()
{
  chr01 = (chr01 & 0x7F) | ((regs[1] & 0xC0) << 1);
  chr23 = (chr23 & 0x7F) | ((regs[1] & 0xC0) << 1);
  chr4 = (chr4 & 0x7F) | ((regs[1] & 0xC0) << 1);
  chr5 = (chr5 & 0x7F) | ((regs[1] & 0xC0) << 1);
  chr6 = (chr6 & 0x7F) | ((regs[1] & 0xC0) << 1);
  chr7 = (chr7 & 0x7F) | ((regs[1] & 0xC0) << 1);

  if(regs[0] & 0x80)
  {
    set_PPU_banks(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
  }
  else
  {
    set_PPU_banks(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);
  }
}

#define MAP49_ROM(ptr)  (((ptr)-parent_NES->ROM->get_ROM_banks())  >> 13)
#define MAP49_VROM(ptr) (((ptr)-parent_NES->ROM->get_VROM_banks()) >> 10)

void NES_mapper49::SNSS_fixup()
{
  NES_6502::Context context;
  parent_NES->cpu->GetContext(&context);

  if(regs[1])
  {
    prg0 = MAP49_ROM(context.mem_page[(regs[0] & 0x40) ? 6 : 4]);
    prg1 = MAP49_ROM(context.mem_page[5]);
  }

  if(regs[0] & 0x80)
  {
    chr01 = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
    chr23 = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
    chr4  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
    chr5  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[1]);
    chr6  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
    chr7  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[3]);
  }
  else
  {
    chr01 = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[0]);
    chr23 = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[2]);
    chr4  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[4]);
    chr5  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[5]);
    chr6  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[6]);
    chr7  = MAP49_VROM(parent_NES->ppu->PPU_VRAM_banks[7]);
  }
}

/////////////////////////////////////////////////////////////////////

