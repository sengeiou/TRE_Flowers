
/////////////////////////////////////////////////////////////////////
// Mapper 182
void NES_mapper182::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  regs[0] = 0;

  irq_enabled = 0;
  irq_counter = 0;
}

void NES_mapper182::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr & 0xF003)
  {
    case 0x8001:
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
      break;

    case 0xA000:
      {
        regs[0] = data & 0x07;
      }
      break;

    case 0xC000:
      {
        switch(regs[0])
        {
          case 0x00:
            {
              set_PPU_bank0((data & 0xFE)+0);
              set_PPU_bank1((data & 0xFE)+1);
            }
            break;

          case 0x01:
            {
              set_PPU_bank5(data);
            }
            break;

          case 0x02:
            {
              set_PPU_bank2((data & 0xFE)+0);
              set_PPU_bank3((data & 0xFE)+1);
            }
            break;

          case 0x03:
            {
              set_PPU_bank7(data);
            }
            break;

          case 0x04:
            {
              set_CPU_bank4(data);
            }
            break;

          case 0x05:
            {
              set_CPU_bank5(data);
            }
            break;

          case 0x06:
            {
              set_PPU_bank4(data);
            }
            break;

          case 0x07:
            {
              set_PPU_bank6(data);
            }
            break;
        }
      }
      break;

    case 0xE003:
      {
        irq_counter = data;
        irq_enabled = data;
      }
      break;
  }
}

void NES_mapper182::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 240))
    {
      if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
      {
        if(!(--irq_counter))
        {
          irq_counter = 0;
          irq_enabled = 0;
          parent_NES->cpu->DoIRQ();
        }
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////

