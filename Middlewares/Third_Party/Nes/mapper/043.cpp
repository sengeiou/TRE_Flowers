
/////////////////////////////////////////////////////////////////////
// Mapper 43
void NES_mapper43::Reset()
{
  set_CPU_banks(2,1,0,4,9);
  set_PPU_banks(0,1,2,3,4,5,6,7);

  //set_mirroring(NES_PPU::MIRROR_VERT);

  irq_enabled = 1;
  irq_counter = 0;
}

uint8 NES_mapper43::MemoryReadLow(uint32 addr)
{
  if(0x5000 <= addr && addr < 0x6000)
  {
    return ROM_banks[0x2000*8 + 0x1000 + (addr - 0x5000)];
  }
  return (uint8)(addr >> 8);
}

void NES_mapper43::MemoryWriteLow(uint32 addr, uint8 data)
{
  if((addr & 0xF0FF) == 0x4022)
  {
    switch(data & 0x07)
    {
      case 0x0:
      case 0x2:
      case 0x3:
      case 0x4:
        {
          set_CPU_bank6(4);
        }
        break;

      case 0x1:
        {
          set_CPU_bank6(3);
        }
        break;

      case 0x5:
        {
          set_CPU_bank6(7);
        }
        break;

      case 0x6:
        {
          set_CPU_bank6(5);
        }
        break;

      case 0x7:
        {
          set_CPU_bank6(6);
        }
        break;
    }
  }
}

void NES_mapper43::MemoryWrite(uint32 addr, uint8 data)
{
  if(addr == 0x8122)
  {
    if(data & 3)
    {
      irq_enabled = 1;
    }
    else
    {
      irq_counter = 0;
      irq_enabled = 0;
    }
  }
}

void NES_mapper43::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    irq_counter += 114;
    if(irq_counter >= 4096)
    {
      irq_counter -= 4096;
      parent_NES->cpu->DoIRQ();
    }
  }
}
/////////////////////////////////////////////////////////////////////

