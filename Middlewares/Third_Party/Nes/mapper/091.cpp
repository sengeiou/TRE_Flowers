
/////////////////////////////////////////////////////////////////////
// Mapper 91
void NES_mapper91::Reset()
{
  // set CPU bank pointers
  set_CPU_bank4(num_8k_ROM_banks-2);
  set_CPU_bank5(num_8k_ROM_banks-1);
  set_CPU_bank6(num_8k_ROM_banks-2);
  set_CPU_bank7(num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,0,0,0,0,0,0,0);
  }
}

void NES_mapper91::MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
  switch(addr & 0xF00F)
  {
    case 0x6000:
      {
        set_PPU_bank0(data*2+0);
        set_PPU_bank1(data*2+1);
      }
      break;

    case 0x6001:
      {
        set_PPU_bank2(data*2+0);
        set_PPU_bank3(data*2+1);
      }
      break;

    case 0x6002:
      {
        set_PPU_bank4(data*2+0);
        set_PPU_bank5(data*2+1);
      }
      break;

    case 0x6003:
      {
        set_PPU_bank6(data*2+0);
        set_PPU_bank7(data*2+1);
      }
      break;

    case 0x7000:
      {
        set_CPU_bank4(data);
      }
      break;

    case 0x7001:
      {
        set_CPU_bank5(data);
      }
      break;

    case 0x7002:
      {
        irq_counter = data;
      }
      break;

    case 0x7003:
      {
        irq_enabled = data;
      }
      break;
   }
}

void NES_mapper91::HSync(uint32 scanline)
{
  if(irq_enabled)
  {
    if(0 <= scanline && scanline <= 240)
    {
      if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
      {
        if(!irq_counter)
        {
          irq_enabled = 0;
          parent_NES->cpu->DoIRQ();
        }
        else
        {
          irq_counter--;
        }
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////

