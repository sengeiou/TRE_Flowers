
/////////////////////////////////////////////////////////////////////
// Mapper 48
void NES_mapper48::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  set_PPU_banks(0,1,2,3,4,5,6,7);

  regs[0] = 0;
  irq_enabled = 0;
  irq_counter = 0;
}

void NES_mapper48::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        if(!regs[0])
        {
          if(data & 0x40)
          {
            set_mirroring(NES_PPU::MIRROR_HORIZ);
          }
          else
          {
            set_mirroring(NES_PPU::MIRROR_VERT);
          }
        }
        set_CPU_bank4(data);
      }
      break;

    case 0x8001:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x8002:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x8003:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0xA000:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0xA001:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0xA002:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0xA003:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xC000:
      {
        irq_counter = data;
      }
      break;

    case 0xC001:
      {
        irq_enabled = data & 0x01;
      }
      break;

    case 0xE000:
      {
        if (data & 0x40)
        {
          set_mirroring(NES_PPU::MIRROR_HORIZ);
        }
        else
        {
          set_mirroring(NES_PPU::MIRROR_VERT);
        }
        regs[0] = 1;
      }
      break;
  }
}

void NES_mapper48::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if((scanline >= 0) && (scanline <= 239))
    {
      if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
      {
        if(irq_counter == 0xFF)
        {
          parent_NES->cpu->DoIRQ();
          irq_enabled = 0;
        }
        else
        {
          irq_counter++;
        }
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////

