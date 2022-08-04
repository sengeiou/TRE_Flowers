#include "nes_mapper.h"


// Mapper 113
void MAP113_Reset()
{
  // set CPU bank pointers
  //set_CPU_banks(0,1,2,3);

  // set PPU bank pointers
  if(num_1k_VROM_banks)
  {
    set_PPU_banks(0,1,2,3,4,5,6,7);
  }
}

void MAP113_MemoryWriteLow(uint16 addr, uint8 data)
{
  switch(addr)
  {
    case 0x4100:
    case 0x4111:
    case 0x4120:
    case 0x4900:
      {
        uint8 prg_bank, chr_bank;

        prg_bank = data >> 3;
        if (num_8k_ROM_banks <= 8 && num_1k_VROM_banks == 8*16)
        {
          chr_bank = ((data >> 3) & 0x08) + (data & 0x07);
        }
        else
        {
          chr_bank = data & 0x07;
        }
        //uint8 prg_bank = (data & 0x03) | ((data & 0x80) >> 5);
        //uint8 chr_bank = (data & 0x70) >> 4;

        set_CPU_bank4(prg_bank*4+0);
        set_CPU_bank5(prg_bank*4+1);
        set_CPU_bank6(prg_bank*4+2);
        set_CPU_bank7(prg_bank*4+3);

        set_PPU_bank0(chr_bank*8+0);
        set_PPU_bank1(chr_bank*8+1);
        set_PPU_bank2(chr_bank*8+2);
        set_PPU_bank3(chr_bank*8+3);
        set_PPU_bank4(chr_bank*8+4);
        set_PPU_bank5(chr_bank*8+5);
        set_PPU_bank6(chr_bank*8+6);
        set_PPU_bank7(chr_bank*8+7);
      }
      break;
  }
}

void MAP113_MemoryWrite(uint16 addr, uint8 data)
{ 
  switch(addr)
  {
    case 0x8008:
    case 0x8009:
      {
        uint8 prg_bank, chr_bank;

        prg_bank = data >> 3;
        if (num_8k_ROM_banks <= 8 && num_1k_VROM_banks == 8*16)
        {
          chr_bank = ((data >> 3) & 0x08) + (data & 0x07);
        }
        else
        {
          chr_bank = data & 0x07;
        }
        prg_bank = (data & 0x03) | ((data & 0x80) >> 5);
        chr_bank = (data & 0x70) >> 4;

        set_CPU_bank4(prg_bank*4+0);
        set_CPU_bank5(prg_bank*4+1);
        set_CPU_bank6(prg_bank*4+2);
        set_CPU_bank7(prg_bank*4+3);

        set_PPU_bank0(chr_bank*8+0);
        set_PPU_bank1(chr_bank*8+1);
        set_PPU_bank2(chr_bank*8+2);
        set_PPU_bank3(chr_bank*8+3);
        set_PPU_bank4(chr_bank*8+4);
        set_PPU_bank5(chr_bank*8+5);
        set_PPU_bank6(chr_bank*8+6);
        set_PPU_bank7(chr_bank*8+7);
      }
      break;
  }
} 
void MAP113_Init()
{
	NES_Mapper->Reset = MAP113_Reset;
	NES_Mapper->Write=MAP113_MemoryWrite;
	NES_Mapper->WriteLow=MAP113_MemoryWriteLow;
}












