#include "nes_mapper.h"


u8 map73_irq_enabled=0;
u8 map73_irq_counter=0;

// Mapper 73
void MAP73_Reset()
{
  // set CPU bank pointers
//  set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  map73_irq_enabled = 0;
  map73_irq_enabled = 0;
}

void MAP73_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        map73_irq_enabled = (map73_irq_enabled & 0xFFF0) | (data & 0x0F);
      }
      break;

    case 0x9000:
      {
        map73_irq_enabled = (map73_irq_enabled & 0xFF0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xA000:
      {
        map73_irq_enabled = (map73_irq_enabled & 0xF0FF) | ((data & 0x0F) << 8);
      }
      break;

    case 0xB000:
      {
        map73_irq_enabled = (map73_irq_enabled & 0x0FFF) | ((data & 0x0F) << 12);
      }
      break;

    case 0xC000:
      {
        map73_irq_enabled = data;
      }
      break;

    case 0xF000:
      {
        set_CPU_bank4(data*2+0);
        set_CPU_bank5(data*2+1);
      }
      break;
  }
}

void MAP73_HSync(int scanline)
{
  if(map73_irq_enabled & 0x02)
  {
    if(map73_irq_enabled > 0xFFFF - 114)
    {
      CPU_IRQ;
      map73_irq_enabled = 0;
    }
    else
    {
      map73_irq_enabled += 114;
    }
  }
}
void MAP73_Init()
{
	NES_Mapper->Reset = MAP73_Reset;
	NES_Mapper->Write = MAP73_MemoryWrite;
	NES_Mapper->HSync = MAP73_HSync;   
}

























