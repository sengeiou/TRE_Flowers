
/////////////////////////////////////////////////////////////////////
// Mapper 183
void NES_mapper183::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  for(uint8 i = 0; i < 8; i++)
  {
    regs[i] = i;
  }

  irq_enabled = 0;
  irq_counter = 0;
}

void NES_mapper183::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8800:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0xA800:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0xA000:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0xB000:
      {
        regs[0] = (regs[0] & 0xf0) | (data & 0x0f);
        set_PPU_bank0(regs[0]);
      }
      break;

    case 0xB004:
      {
        regs[0] = (regs[0] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank0(regs[0]);
      }
      break;

    case 0xB008:
      {
        regs[1] = (regs[1] & 0xf0) | (data & 0x0f);
        set_PPU_bank1(regs[1]);
      }
      break;

    case 0xB00C:
      {
        regs[1] = (regs[1] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank1(regs[1]);
      }
      break;

    case 0xC000:
      {
        regs[2] = (regs[2] & 0xf0) | (data & 0x0f);
        set_PPU_bank2(regs[2]);
      }
      break;

    case 0xC004:
      {
        regs[2] = (regs[2] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank2(regs[2]);
      }
      break;

    case 0xC008:
      {
        regs[3] = (regs[3] & 0xf0) | (data & 0x0f);
        set_PPU_bank3(regs[3]);
      }
      break;

    case 0xC00C:
      {
        regs[3] = (regs[3] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank3(regs[3]);
      }
      break;

    case 0xD000:
      {
        regs[4] = (regs[4] & 0xf0) | (data & 0x0f);
        set_PPU_bank4(regs[4]);
      }
      break;

    case 0xD004:
      {
        regs[4] = (regs[4] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank4(regs[4]);
      }
      break;

    case 0xD008:
      {
        regs[5] = (regs[5] & 0xf0) | (data & 0x0f);
        set_PPU_bank5(regs[5]);
      }
      break;

    case 0xD00C:
      {
        regs[5] = (regs[5] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank5(regs[5]);
      }
      break;

    case 0xE000:
      {
        regs[6] = (regs[6] & 0xf0) | (data & 0x0f);
        set_PPU_bank6(regs[6]);
      }
      break;

    case 0xE004:
      {
        regs[6] = (regs[6] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank6(regs[6]);
      }
      break;

    case 0xE008:
      {
        regs[7] = (regs[7] & 0xf0) | (data & 0x0f);
        set_PPU_bank7(regs[7]);
      }
      break;

    case 0xE00C:
      {
        regs[7] = (regs[7] & 0x0f) | ((data & 0x0f) << 4);
        set_PPU_bank7(regs[7]);
      }
      break;

    case 0x9008:
      {
        if(data == 1)
        {
          for(uint8 i = 0; i < 8; i++)
          {
            regs[i] = i;
          }
          set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
          set_PPU_banks(0,1,2,3,4,5,6,7);
        }
      }
      break;

    case 0x9800:
      {
        if(data == 0)
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if(data == 1)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if(data == 2)
        {
          set_mirroring(0,0,0,0);
        }
        else if(data == 3)
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0xF000:
      {
        irq_counter = (irq_counter & 0xFF00) | data;
      }
      break;

    case 0xF004:
      {
        irq_counter = (irq_counter & 0x00FF) | (data << 8);
      }
      break;

    case 0xF008:
      {
        irq_enabled = data;
      }
      break;
  }
}

void NES_mapper183::HSync(uint32 scanline)
{
  if(irq_enabled & 0x02)
  {
    if(irq_counter <= 113)
    {
      parent_NES->cpu->DoIRQ();
      irq_counter = 0;
    }
    else
    {
      irq_counter -= 113;
    }
  }
}
/////////////////////////////////////////////////////////////////////

