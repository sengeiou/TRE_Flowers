#include "nes_mapper.h"


u8 map245_regs[1];
// Mapper 245
// coded by r1
void MAP245_Reset()
{
  // set CPU bank pointers
 // set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

  map245_regs[0] = 0;
}

void MAP245_MemoryWrite(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x8000:
      {
        map245_regs[0] = data;
      }
      break;

    case 0x8001:
      {
        switch(map245_regs[0] & 7)
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
            break;

          case 6:
            {
              set_CPU_bank4(data);
            }
            break;

          case 7:
            {
              set_CPU_bank5(data);
            }
            break;
        }
      }
      break;

    case 0xA000:
      {
      }
      break;

    case 0xA001:
      {
        // write 0x80 to enable save ram ?
      }
      break;

    case 0xE000:
      {
      }
      break;
  }
}
void MAP245_Init()
{
	NES_Mapper->Reset = MAP245_Reset;
	NES_Mapper->Write=MAP245_MemoryWrite; 
}


