
/////////////////////////////////////////////////////////////////////
// Mapper 117
void NES_mapper117::Reset()
{
  // set CPU bank pointers
  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }

  irq_line = 0;
  irq_enabled1 = 0;
  irq_enabled2 = 1;
}

void NES_mapper117::MemoryWrite(uint32 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
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
        set_CPU_bank6(data);
      }
      break;

    case 0xA000:
      {
        set_PPU_bank0(data);
      }
      break;

    case 0xA001:
      {
        set_PPU_bank1(data);
      }
      break;

    case 0xA002:
      {
        set_PPU_bank2(data);
      }
      break;

    case 0xA003:
      {
        set_PPU_bank3(data);
      }
      break;

    case 0xA004:
      {
        set_PPU_bank4(data);
      }
      break;

    case 0xA005:
      {
        set_PPU_bank5(data);
      }
      break;

    case 0xA006:
      {
        set_PPU_bank6(data);
      }
      break;

    case 0xA007:
      {
        set_PPU_bank7(data);
      }
      break;

    case 0xA008:
    case 0xA009:
    case 0xA00a:
    case 0xA00b:
    case 0xA00c:
    case 0xA00d:
    case 0xA00e:
    case 0xA00f:
       break;

    case 0xC001:
    case 0xC002:
    case 0xC003:
      {
        irq_enabled1 = irq_line = data;
      }
      break;

    case 0xE000:
      {
        irq_enabled2 = data & 1;
      }
      break;
  }
}

void NES_mapper117::HSync(uint32 scanline)
{
  if(irq_enabled1 && irq_enabled2 && irq_line == scanline)
  {
      irq_enabled1 = 0;
      parent_NES->cpu->DoIRQ();
  }
}
/////////////////////////////////////////////////////////////////////

