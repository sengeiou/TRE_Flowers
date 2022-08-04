
/////////////////////////////////////////////////////////////////////
// Mapper 42
void NES_mapper42::Reset()
{
  // set CPU bank pointers
  set_CPU_bank3(0);
  set_CPU_bank4(num_8k_ROM_banks-4);
  set_CPU_bank5(num_8k_ROM_banks-3);
  set_CPU_bank6(num_8k_ROM_banks-2);
  set_CPU_bank7(num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);
}

void NES_mapper42::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xE003)
  {
    case 0xE000:
      {
        set_CPU_bank3(data & 0x0F);
      }
      break;

    case 0xE001:
      {
        if(data & 0x08)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
      }
      break;

    case 0xE002:
      {
        if(data & 0x02)
        {
          irq_enabled = 1;
        }
        else
        {
          irq_enabled = 0;
          irq_counter = 0;
        }
      }
      break;
  }
}

void NES_mapper42::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if(irq_counter < 215)
    {
      irq_counter++;
    }
    if(irq_counter == 215)
    {
      parent_NES->cpu->DoIRQ();
      irq_enabled = 0;
    }
  }
}
/////////////////////////////////////////////////////////////////////

