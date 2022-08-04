
/////////////////////////////////////////////////////////////////////
// Mapper 83
void NES_mapper83::Reset()
{
  regs[0] = regs[1] = regs[2] = 0;

  // set CPU bank pointers
  if(num_8k_ROM_banks >= 32)
  {
    set_CPU_banks(0,1,30,31);
    regs[1] = 0x30;
  }
  else
  {
    set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
  }

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  irq_enabled = 0;
  irq_counter = 0;
}

uint8 NES_mapper83::MemoryReadLow(uint32 addr)
{
  if((addr & 0x5100) == 0x5100)
  {
    return regs[2];
  }
  else
  {
    return addr >> 8;
  }
}

void NES_mapper83::MemoryWriteLow(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x5101:
    case 0x5102:
    case 0x5103:
      {
        regs[2] = data;
      }
      break;
  }
}

void NES_mapper83::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
    case 0xB000:
    case 0xB0FF:
    case 0xB1FF:
      {
        regs[0] = data;
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
        set_CPU_bank6(((data&0x30)|0x0F)*2+0);
        set_CPU_bank7(((data&0x30)|0x0F)*2+1);
        
      }
      break;

    case 0x8100:
      {
        if(num_1k_VROM_banks <= 32*8)
        {
          regs[1] = data;
        }
        if((data & 0x03) == 0x00)
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
        else if((data & 0x03) == 0x01)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else if((data & 0x03) == 0x02)
        {
          set_mirroring(0,0,0,0);
        }
        else
        {
          set_mirroring(1,1,1,1);
        }
      }
      break;

    case 0x8200:
      {
        irq_counter = (irq_counter & 0xFF00) | (uint32)data;
      }
      break;

    case 0x8201:
      {
        irq_counter = (irq_counter & 0x00FF) | ((uint32)data << 8);
        irq_enabled = data;
      }
      break;

    case 0x8300:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x8301:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x8302:
      {
        set_CPU_bank6(data);
      }
      break;

    case 0x8310:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank0(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank0((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank1((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8311:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank1(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank2((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank3((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8312:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank2(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8313:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank3(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8314:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank4(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8315:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank5(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8316:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank6(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8317:
      {
        if((regs[1] & 0x30) == 0x30)
        {
          set_PPU_bank7(((data & 0x30) << 4)^data);
        }
        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
        {
          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
        }
      }
      break;

    case 0x8318:
      {
        set_CPU_bank4(((regs[0]&0x30)|data)*2+0);
        set_CPU_bank5(((regs[0]&0x30)|data)*2+1);
      }
      break;
  }
}

void NES_mapper83::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if(irq_counter <= 114)
    {
      parent_NES->cpu->DoIRQ();
      irq_enabled = 0;
    }
    else
    {
      irq_counter -= 114;
    }
  }
}
/////////////////////////////////////////////////////////////////////

