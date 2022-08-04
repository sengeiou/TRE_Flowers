
/////////////////////////////////////////////////////////////////////
// Mapper 105
void NES_mapper105::Reset()
{
  set_CPU_banks(0,1,2,3);

  regs[0] = 0x0C;
  regs[1] = 0x00;
  regs[2] = 0x00;
  regs[3] = 0x10;

  bits = 0;
  write_count = 0;

  irq_counter = 0;
  irq_enabled = 0;
  init_state = 0;
}

void NES_mapper105::MemoryWrite(uint32 addr, uint8 data)
{
  uint32 reg_num = (addr & 0x7FFF) >> 13;

  if (data & 0x80)
  {
    bits = write_count = 0;
    if (reg_num == 0)
    {
      regs[reg_num] |= 0x0C;
    }
  }
  else
  {
    bits |= (data & 1) << write_count++;
    if (write_count == 5)
    {
      regs[reg_num] = bits & 0x1F;
      bits = write_count = 0;
    }
  }

  if(regs[0] & 0x02)
  {
    if(regs[0] & 0x01)
    {
      set_mirroring(NES_PPU::MIRROR_HORIZ);
    }
    else
    {
      set_mirroring(NES_PPU::MIRROR_VERT);
    }
  }
  else
  {
    if(regs[0] & 0x01)
    {
      set_mirroring(1,1,1,1);
    }
    else
    {
      set_mirroring(0,0,0,0);
    }
  }

  switch (init_state)
  {
    case 0:
    case 1:
      {
        init_state++;
      }
      break;

    case 2:
      {
        if(regs[1] & 0x08)
        {
          if (regs[0] & 0x08)
          {
            if (regs[0] & 0x04)
            {
              set_CPU_bank4((regs[3] & 0x07) * 2 + 16);
              set_CPU_bank5((regs[3] & 0x07) * 2 + 17);
              set_CPU_bank6(30);
              set_CPU_bank7(31);
            }
            else
            {
              set_CPU_bank4(16);
              set_CPU_bank5(17);
              set_CPU_bank6((regs[3] & 0x07) * 2 + 16);
              set_CPU_bank7((regs[3] & 0x07) * 2 + 17);
            }
          }
          else
          {
            set_CPU_bank4((regs[3] & 0x06) * 2 + 16);
            set_CPU_bank5((regs[3] & 0x06) * 2 + 17);
            set_CPU_bank6((regs[3] & 0x06) * 2 + 18);
            set_CPU_bank7((regs[3] & 0x06) * 2 + 19);
          }
        }
        else
        {
          set_CPU_bank4((regs[1] & 0x06) * 2 + 0);
          set_CPU_bank5((regs[1] & 0x06) * 2 + 1);
          set_CPU_bank6((regs[1] & 0x06) * 2 + 2);
          set_CPU_bank7((regs[1] & 0x06) * 2 + 3);
        }

        if(regs[1] & 0x10)
        {
          irq_counter = 0;
          irq_enabled = 0;
        }
        else
        {
          irq_enabled = 1;
        }
      }
      break;
  }
}

void NES_mapper105::HSync(uint32 scanline)
{
  if(!scanline)
  {
    if(irq_enabled)
    {
      irq_counter += 29781;
    }
    if(((irq_counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000)
    {
      parent_NES->cpu->DoIRQ();
    }
  }
}

