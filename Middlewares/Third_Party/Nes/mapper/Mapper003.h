#ifndef _MAPPER003_H_
#define _MAPPER003_H_
#include "nes_mapper.h" 
 
void	Mapper003_Reset()
{

}
void Mapper003_Write( uint8 data, uint16 addr)
{
	
  uint32 base;

  data &= (VROM_1K_SIZE>>1)-1;

  base = ((uint32)data) << 3;
  set_PPU_banks(base+0,base+1,base+2,base+3,base+4,base+5,base+6,base+7);
		
}

void Mapper003_Init()
{
	NES_Mapper->Reset = Mapper003_Reset;
	NES_Mapper->Write = Mapper003_Write;
}

#endif





