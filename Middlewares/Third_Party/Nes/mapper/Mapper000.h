#ifndef _MAPPER000_H_
#define _MAPPER000_H_
#include "nes_mapper.h" 

void Mapper000_Reset()
{

}
void Mapper000_Write( uint8 data,uint16 addr)
{

}
void Mapper000_HSync( int scanline )
{

}
void Mapper000_Init()
{
	NES_Mapper->Reset = Mapper000_Reset;
	NES_Mapper->Write = Mapper000_Write;
	NES_Mapper->HSync = Mapper000_HSync;
}

#endif
