#ifndef _MAPPER066_H_
#define _MAPPER066_H_
#include "nes_mapper.h" 


void Mapper066_Reset()
{
    set_PPU_banks(0,1,2,3,4,5,6,7);
}

void Mapper066_Write( uint8 byData, uint16 wAddr)
{
      uint8 chr_bank = byData & 0x0F;
	  uint8 prg_bank = (byData & 0xF0) >> 4;

	  set_cpu_bank0(prg_bank*4+0);
	  set_cpu_bank1(prg_bank*4+1);
	  set_cpu_bank2(prg_bank*4+2);
	  set_cpu_bank3(prg_bank*4+3);	
	

	  set_PPU_bank0(chr_bank*8+0);
	  set_PPU_bank1(chr_bank*8+1);
	  set_PPU_bank2(chr_bank*8+2);
	  set_PPU_bank3(chr_bank*8+3);
	  set_PPU_bank4(chr_bank*8+4);
	  set_PPU_bank5(chr_bank*8+5);
	  set_PPU_bank6(chr_bank*8+6);
	  set_PPU_bank7(chr_bank*8+7);
}

void Mapper066_Init()
{
	NES_Mapper->Reset = Mapper066_Reset;
	NES_Mapper->Write = Mapper066_Write;
}

#endif



