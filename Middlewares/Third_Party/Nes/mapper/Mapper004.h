#ifndef _MAPPER004_H_
#define _MAPPER004_H_
#include "nes_mapper.h" 

//////////////////////////////////////////////////////////////////////////
Mapper4Res *MAP4;

#define Map4_Chr_Swap()    ( MAP4->Map4_Regs[ 0 ] & 0x80 )
#define Map4_Prg_Swap()    ( MAP4->Map4_Regs[ 0 ] & 0x40 )

void Map4_Set_CPU_Banks(void);
void Map4_Set_PPU_Banks(void);

//*******************************************************************
void	Mapper004_Reset()
{
	int nPage;
    /* Initialize State Registers */
  for ( nPage = 0; nPage < 8; nPage++ )
  {
    MAP4->Map4_Regs[ nPage ] = 0x00;
  }
	 /* Set ROM Banks */
  MAP4->Map4_Prg0 = 0;
  MAP4->Map4_Prg1 = 1;
//	SetBank_CPU();

	/* Set PPU Banks */
//  if ( Neshd->byVRomSize > 0 )
  if( VROM_1K_SIZE )
  {
    MAP4->Map4_Chr01 = 0;
    MAP4->Map4_Chr23 = 2;
    MAP4->Map4_Chr4  = 4;
    MAP4->Map4_Chr5  = 5;
    MAP4->Map4_Chr6  = 6;
    MAP4->Map4_Chr7  = 7;
    Map4_Set_PPU_Banks();
  } else {
    MAP4->Map4_Chr01 = MAP4->Map4_Chr23 = 0;
    MAP4->Map4_Chr4 = MAP4->Map4_Chr5 = MAP4->Map4_Chr6 = MAP4->Map4_Chr7 = 0;
  }
	/* Initialize IRQ Registers */
  MAP4->Map4_IRQ_Enable = 0;
  MAP4->Map4_IRQ_Cnt = 0;
  MAP4->Map4_IRQ_Latch = 0;
  MAP4->Map4_IRQ_Request = 0;
  MAP4->Map4_IRQ_Present = 0;
  MAP4->Map4_IRQ_Present_Vbl = 0;

}
/*-------------------------------------------------------------------*/
/*  Mapper 4 Write Function                                          */
/*-------------------------------------------------------------------*/
void Mapper004_Write(uint8 byData ,uint16 wAddr )
{
 uint16 dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      MAP4->Map4_Regs[ 0 ] = byData;
      Map4_Set_PPU_Banks();
      Map4_Set_CPU_Banks();//******************************************************
      break;

    case 0x8001:
      MAP4->Map4_Regs[ 1 ] = byData;
      dwBankNum = MAP4->Map4_Regs[ 1 ];

      switch ( MAP4->Map4_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
//          if ( Neshd->byVRomSize > 0 )
			if( VROM_1K_SIZE )
          {
            dwBankNum &= 0xfe;
            MAP4->Map4_Chr01 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x01:
//          if ( Neshd->byVRomSize > 0 )
		    if( VROM_1K_SIZE )
          {
            dwBankNum &= 0xfe;
            MAP4->Map4_Chr23 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x02:
//          if ( Neshd->byVRomSize > 0 )
			if( VROM_1K_SIZE )
          {
            MAP4->Map4_Chr4 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x03:
 //         if ( Neshd->byVRomSize > 0 )
			if( VROM_1K_SIZE )
          {
            MAP4->Map4_Chr5 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x04:
//          if ( Neshd->byVRomSize > 0 )
			if( VROM_1K_SIZE )
          {
            MAP4->Map4_Chr6 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x05:
 //         if ( Neshd->byVRomSize > 0 )
			if( VROM_1K_SIZE )
          {
            MAP4->Map4_Chr7 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          MAP4->Map4_Prg0 = dwBankNum;
          Map4_Set_CPU_Banks();//*****************************************************
          break;

        case 0x07:
          MAP4->Map4_Prg1 = dwBankNum;
          Map4_Set_CPU_Banks();//*****************************************************
          break;
      }
      break;

    case 0xa000:
      MAP4->Map4_Regs[ 2 ] = byData;

      if ( !(RomHeader->flags_1&0x8 ))
      {
        if ( byData & 0x01 )
        {
			PPU_set_mirroring(0,0,1,1);//Ë®Æ½¾µÏñ
        } else {
			PPU_set_mirroring(0,1,0,1);//´¹Ö±¾µÏñ
        }	  
      }
      break;

    case 0xa001:
      MAP4->Map4_Regs[ 3 ] = byData;
      break;

    case 0xc000:
      MAP4->Map4_Regs[ 4 ] = byData;
      MAP4->Map4_IRQ_Latch = byData;
      break;

    case 0xc001:
      MAP4->Map4_Regs[ 5 ] = byData;
      if ( NES_scanline < 240 )
      {
          MAP4->Map4_IRQ_Cnt |= 0x80;
          MAP4->Map4_IRQ_Present = 0xff;
      } else {
          MAP4->Map4_IRQ_Cnt |= 0x80;
          MAP4->Map4_IRQ_Present_Vbl = 0xff;
          MAP4->Map4_IRQ_Present = 0;
      }
      break;

    case 0xe000:
      MAP4->Map4_Regs[ 6 ] = byData;
      MAP4->Map4_IRQ_Enable = 0;
			MAP4->Map4_IRQ_Request = 0;
      break;

    case 0xe001:
      MAP4->Map4_Regs[ 7 ] = byData;
      MAP4->Map4_IRQ_Enable = 1;
			MAP4->Map4_IRQ_Request = 0;
      break;
  }
}
//***********************************************************************************
void Mapper004_HSync( int scanline )
{                                           //  ( PPU.m_REG[1] & 0x08 || PPU.m_REG[1] & 0x10))	                        			
   if( (scanline >= 0 && scanline <= 239) &&( spr_enabled() || bg_enabled()))
  {
		if( MAP4->Map4_IRQ_Present_Vbl ) {
			MAP4->Map4_IRQ_Cnt = MAP4->Map4_IRQ_Latch;
			MAP4->Map4_IRQ_Present_Vbl = 0;
		}
		if( MAP4->Map4_IRQ_Present ) {
			MAP4->Map4_IRQ_Cnt = MAP4->Map4_IRQ_Latch;
			MAP4->Map4_IRQ_Present = 0;
		} else if( MAP4->Map4_IRQ_Cnt > 0 ) {
			MAP4->Map4_IRQ_Cnt--;
		}

		if( MAP4->Map4_IRQ_Cnt == 0 ) {
			if( MAP4->Map4_IRQ_Enable ) {
				MAP4->Map4_IRQ_Request = 0xFF;
			}
			MAP4->Map4_IRQ_Present = 0xFF;
		}
	}
	if( MAP4->Map4_IRQ_Request  ) {
		CPU_IRQ;//		IRQ_REQ;
	}
}			
/*-------------------------------------------------------------------*/
/*  Mapper 4 Set CPU Banks Function                                  */
/*-------------------------------------------------------------------*/
void Map4_Set_CPU_Banks(void)
{
  if ( Map4_Prg_Swap() )
  {                                     
//    ROMBANK0 = ROMLASTPAGE( 1 );       
//    ROMBANK1 = ROMPAGE( MAP4->Map4_Prg1 % ( Neshd->byRomSize << 1 ) );
//    ROMBANK2 = ROMPAGE( MAP4->Map4_Prg0 % ( Neshd->byRomSize << 1 ) );
//    ROMBANK3 = ROMLASTPAGE( 0 );
//******************************************************************************	  
	  set_cpu_bank0(-2);  
      set_cpu_bank1(MAP4->Map4_Prg1);
      set_cpu_bank2(MAP4->Map4_Prg0);
      set_cpu_bank3(-1);//ÓÐ·ûºÅ×Ö·û
//************************************************************************************
  } else {
//    ROMBANK0 = ROMPAGE( MAP4->Map4_Prg0 % ( Neshd->byRomSize << 1 ) );
//    ROMBANK1 = ROMPAGE( MAP4->Map4_Prg1 % ( Neshd->byRomSize << 1 ) );
//    ROMBANK2 = ROMLASTPAGE( 1 );
//    ROMBANK3 = ROMLASTPAGE( 0 );
//**********************************************************************************	  
      set_cpu_bank0(MAP4->Map4_Prg0);  
      set_cpu_bank1(MAP4->Map4_Prg1);
      set_cpu_bank2(-2);
      set_cpu_bank3(-1);
//**************************************************************************
  }
}

void Map4_Set_PPU_Banks(void)
{
	if( VROM_1K_SIZE ) {
		if( MAP4->Map4_Regs[0] & 0x80 ) {
			
			set_PPU_banks(MAP4->Map4_Chr4, MAP4->Map4_Chr5, MAP4->Map4_Chr6, MAP4->Map4_Chr7,
			              MAP4->Map4_Chr01,MAP4->Map4_Chr01+1,MAP4->Map4_Chr23,MAP4->Map4_Chr23+1);
//			SetVROM_1K_Bank(0,MAP4->Map4_Chr4);
//			SetVROM_1K_Bank(1,MAP4->Map4_Chr5);
//			SetVROM_1K_Bank(2,MAP4->Map4_Chr6);
//			SetVROM_1K_Bank(3,MAP4->Map4_Chr7);
//			SetVROM_1K_Bank(4,MAP4->Map4_Chr01);
//			SetVROM_1K_Bank(5,MAP4->Map4_Chr01+1);
//			SetVROM_1K_Bank(6,MAP4->Map4_Chr23);
//			SetVROM_1K_Bank(7,MAP4->Map4_Chr23+1);			
			
		} else {

			set_PPU_banks(MAP4->Map4_Chr01,MAP4->Map4_Chr01+1,MAP4->Map4_Chr23,MAP4->Map4_Chr23+1,
			              MAP4->Map4_Chr4,MAP4->Map4_Chr5,MAP4->Map4_Chr6,MAP4->Map4_Chr7);
//			SetVROM_1K_Bank(4,MAP4->Map4_Chr4);
//			SetVROM_1K_Bank(5,MAP4->Map4_Chr5);
//			SetVROM_1K_Bank(6,MAP4->Map4_Chr6);
//			SetVROM_1K_Bank(7,MAP4->Map4_Chr7);
//			SetVROM_1K_Bank(0,MAP4->Map4_Chr01);
//			SetVROM_1K_Bank(1,MAP4->Map4_Chr01+1);
//			SetVROM_1K_Bank(2,MAP4->Map4_Chr23);
//			SetVROM_1K_Bank(3,MAP4->Map4_Chr23+1);						
		}
	} else {
		if( MAP4->Map4_Regs[0] & 0x80 ) {
		
			  set_VRAM_bank(0, MAP4->Map4_Chr4);
			  set_VRAM_bank(1, MAP4->Map4_Chr5);
			  set_VRAM_bank(2, MAP4->Map4_Chr6);
			  set_VRAM_bank(3, MAP4->Map4_Chr7);
			  set_VRAM_bank(4, MAP4->Map4_Chr01+0);
			  set_VRAM_bank(5, MAP4->Map4_Chr01+1);
			  set_VRAM_bank(6, MAP4->Map4_Chr23+0);
			  set_VRAM_bank(7, MAP4->Map4_Chr23+1);		
//********************************************************************************				
//			SetCRAM_1K_Bank( 4, MAP4->Map4_Chr01+0);
//			SetCRAM_1K_Bank( 5, MAP4->Map4_Chr01+1);
//			SetCRAM_1K_Bank( 6, MAP4->Map4_Chr23+0);
//			SetCRAM_1K_Bank( 7, MAP4->Map4_Chr23+1);
//			SetCRAM_1K_Bank( 0, MAP4->Map4_Chr4 );
//			SetCRAM_1K_Bank( 1, MAP4->Map4_Chr5 );
//			SetCRAM_1K_Bank( 2, MAP4->Map4_Chr6 );
//			SetCRAM_1K_Bank( 3, MAP4->Map4_Chr7 );
			
		} else {

			  set_VRAM_bank(0, MAP4->Map4_Chr01+0);
			  set_VRAM_bank(1, MAP4->Map4_Chr01+1);
			  set_VRAM_bank(2, MAP4->Map4_Chr23+0);
			  set_VRAM_bank(3, MAP4->Map4_Chr23+1);
			  set_VRAM_bank(4, MAP4->Map4_Chr4);
			  set_VRAM_bank(5, MAP4->Map4_Chr5);
			  set_VRAM_bank(6, MAP4->Map4_Chr6);
			  set_VRAM_bank(7, MAP4->Map4_Chr7);
//******************************************************************************
//			SetCRAM_1K_Bank( 0, MAP4->Map4_Chr01+0);
//			SetCRAM_1K_Bank( 1, MAP4->Map4_Chr01+1);
//			SetCRAM_1K_Bank( 2, MAP4->Map4_Chr23+0);
//			SetCRAM_1K_Bank( 3, MAP4->Map4_Chr23+1);
//			SetCRAM_1K_Bank( 4, MAP4->Map4_Chr4);
//			SetCRAM_1K_Bank( 5, MAP4->Map4_Chr5);
//			SetCRAM_1K_Bank( 6, MAP4->Map4_Chr6);
//			SetCRAM_1K_Bank( 7, MAP4->Map4_Chr7);
			
		}
	}
}

//*****************************************************************
void Mapper004_Init()
{
	NES_Mapper->Reset = Mapper004_Reset;
	NES_Mapper->Write = Mapper004_Write;
	
	NES_Mapper->HSync = Mapper004_HSync;
}

#endif


