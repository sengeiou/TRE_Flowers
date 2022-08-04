#ifndef _MAPPER002_H_
#define _MAPPER002_H_
#include "nes_mapper.h" 

void Mapper002_Reset()
{
}

void Mapper002_Write( uint8 data,uint16 addr)
{
    int num_8k_ROM_banks=RomHeader->num_16k_rom_banks * 2;
	data &= num_8k_ROM_banks-1;
    set_CPU_banks(data*2,(data*2)+1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

void Mapper002_Init()
{
	NES_Mapper->Reset = Mapper002_Reset;
	NES_Mapper->Write = Mapper002_Write;
}

#endif
