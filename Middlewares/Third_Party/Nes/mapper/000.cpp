#include "nes_mapper.h"


void MAP0_Reset()
{  
	if(VROM_1K_SIZE)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}
}

void MAP0_Init(void)
{
	NES_Mapper->Reset=MAP0_Reset;
}

















