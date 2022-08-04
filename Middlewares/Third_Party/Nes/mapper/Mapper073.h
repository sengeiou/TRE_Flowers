#ifndef _MAPPER073_H_
#define _MAPPER073_H_
#include "nes_mapper.h" 


uint8 irq_enabled;
uint32 irq_counter;

void	Mapper073_Reset()
{          
	irq_counter = 0;
    irq_enabled = 0;	
}

void Mapper073_Write( uint8 data, uint16 addr)
{
  switch(addr)
  {
    case 0x8000:
      {
        irq_counter = (irq_counter & 0xFFF0) | (data & 0x0F);
      }
      break;

    case 0x9000:
      {
        irq_counter = (irq_counter & 0xFF0F) | ((data & 0x0F) << 4);
      }
      break;

    case 0xA000:
      {
        irq_counter = (irq_counter & 0xF0FF) | ((data & 0x0F) << 8);
      }
      break;

    case 0xB000:
      {
        irq_counter = (irq_counter & 0x0FFF) | ((data & 0x0F) << 12);
      }
      break;

    case 0xC000:
      {
        irq_enabled = data;
      }
      break;
  
    case 0xF000:
      {
//      byData <<= 1;
//      byData %= ( NesHeader.byRomSize << 1 );
//      ROMBANK0 = ROMPAGE( byData );
//      ROMBANK1 = ROMPAGE( byData + 1 );
		  
		set_cpu_bank0(data*2+0);
        set_cpu_bank1(data*2+1);		 
      }
      break;
  }	
}

void Mapper073_HSync( int scanline )
{
   if(irq_enabled & 0x02)
   {
		if(irq_counter > 0xFFFF - 114)
		{
		  CPU_IRQ;          //cpu  irqÖÐ¶Ï±êÖ¾//		  parent_NES->cpu->DoIRQ();    IRQ_REQ;
		  irq_enabled = 0;
		}
		else
		{
		  irq_counter += 114;
		}
   }
}

void Mapper073_Init()
{
	NES_Mapper->Reset = Mapper073_Reset;
	NES_Mapper->Write = Mapper073_Write;
    NES_Mapper->HSync = Mapper073_HSync;	
}

#endif





