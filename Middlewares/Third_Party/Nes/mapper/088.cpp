#include "nes_mapper.h"


u8 map88_regs[1];
 
// Mapper 88
void MAP88_Reset()
{
  //set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  if(num_1k_VROM_banks >= 8)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void MAP88_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        map88_regs[0] = data;
      }
      break;

    case 0x8001:
      {
        switch(map88_regs[0] & 0x07)
        {
          case 0x00:
            {
              set_PPU_bank0(data & 0xFE);
              set_PPU_bank1((data & 0xFE)+1);
            }
            break;

          case 0x01:
            {
              set_PPU_bank2(data & 0xFE);
              set_PPU_bank3((data & 0xFE)+1);
            }
            break;

          case 0x02:
            {
              set_PPU_bank4(data+0x40);
            }
            break;

          case 0x03:
            {
              set_PPU_bank5(data+0x40);
            }
            break;

          case 0x04:
            {
              set_PPU_bank6(data+0x40);
            }
            break;

          case 0x05:
            {
              set_PPU_bank7(data+0x40);
            }
            break;

          case 0x06:
            {
              set_CPU_bank4(data);
            }
            break;

          case 0x07:
            {
              set_CPU_bank5(data);
            }
            break;
        }
      }
      break;

    case 0xC000:
      {
        if(data)
        {
          set_mirroring(1,1,1,1);
        }
        else
        {
          set_mirroring(0,0,0,0);
        }
      }
      break;
  }
}
void MAP88_Init()
{
	NES_Mapper->Reset = MAP88_Reset;
	NES_Mapper->Write = MAP88_MemoryWrite; 
}


