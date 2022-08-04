#ifndef _MAPPER001_H_
#define _MAPPER001_H_
#include "nes_mapper.h" 

Mapper1Res *MAP1;
void Map1_set_ROM_banks(void);

void Mapper001_Reset()
{ 
	uint32 size_in_K;

  MAP1->Map1_Cnt = 0;
  MAP1->Map1_Latch = 0x00;

  MAP1->Map1_Regs[ 0 ] = 0x0c;
  MAP1->Map1_Regs[ 1 ] = 0x00;
  MAP1->Map1_Regs[ 2 ] = 0x00;
  MAP1->Map1_Regs[ 3 ] = 0x00;

  size_in_K = ( RomHeader->num_16k_rom_banks << 1 ) * 8;//16kB ROM的数目

  if ( size_in_K == 1024 )
  {
    MAP1->Map1_Size = Map1_1024K;
  } 
  else if(size_in_K == 512)
  {
    MAP1->Map1_Size = Map1_512K;
  }
  else
  {
    MAP1->Map1_Size = Map1_SMALL;
  }

  MAP1->Map1_256K_base = 0; // use first 256K使用第一个256 k
  MAP1->Map1_swap = 0;

  if( MAP1->Map1_Size == Map1_SMALL )
  {
    // set two high pages to last two banks两个高的页面设置为最后
    MAP1->Map1_HI1 = ( RomHeader->num_16k_rom_banks << 1 ) - 2;
    MAP1->Map1_HI2 = ( RomHeader->num_16k_rom_banks << 1 ) - 1;
  }
  else
  {
    // set two high pages to last two banks of current 256K region
    MAP1->Map1_HI1 = ( 256 / 8 ) - 2;
    MAP1->Map1_HI2 = ( 256 / 8 ) - 1;
  }

  // set CPU bank pointers
  MAP1->Map1_bank1 = 0;
  MAP1->Map1_bank2 = 1;
  MAP1->Map1_bank3 = MAP1->Map1_HI1;
  MAP1->Map1_bank4 = MAP1->Map1_HI2;

  /* Set ROM Banks */
  Map1_set_ROM_banks();//************************************************************
	
}
void Map1_set_ROM_banks(void)
{
//  ROMBANK0 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank1 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );  
//  ROMBANK1 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank2 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );
//  ROMBANK2 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank3 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );
//  ROMBANK3 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank4 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) ); 

//******************************************************************************************************
//   set_CPU_banks((MMC1_256K_base << 5) + (MMC1_bank1 & ((256/8)-1)),	
//                (MMC1_256K_base << 5) + (MMC1_bank2 & ((256/8)-1)),
//                (MMC1_256K_base << 5) + (MMC1_bank3 & ((256/8)-1)),
//                (MMC1_256K_base << 5) + (MMC1_bank4 & ((256/8)-1)));
	set_cpu_bank0(( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank1 & ((256/8)-1))) ); 
	set_cpu_bank1(( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank2 & ((256/8)-1))) );
	set_cpu_bank2(( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank3 & ((256/8)-1))) );
	set_cpu_bank3(( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank4 & ((256/8)-1))) );
}

void Mapper001_Write( uint8 byData, uint16 wAddr)//VROM_1K_SIZE
{
	uint32 dwRegNum;

  // if write is to a different reg, reset
  if( ( wAddr & 0x6000 ) != ( MAP1->Map1_Last_Write_Addr & 0x6000 ) )
  {
    MAP1->Map1_Cnt = 0;
    MAP1->Map1_Latch = 0x00;
  }
  MAP1->Map1_Last_Write_Addr = wAddr;

  // if bit 7 set, reset and return
  if ( byData & 0x80 )
  {
    MAP1->Map1_Cnt = 0;
    MAP1->Map1_Latch = 0x00;
    return;
  }

  if ( byData & 0x01 ) MAP1->Map1_Latch |= ( 1 << MAP1->Map1_Cnt );
  MAP1->Map1_Cnt++;
  if( MAP1->Map1_Cnt < 5 ) return;

  dwRegNum = ( wAddr & 0x7FFF ) >> 13;
  MAP1->Map1_Regs[ dwRegNum ] = MAP1->Map1_Latch;

  MAP1->Map1_Cnt = 0;
  MAP1->Map1_Latch = 0x00;

  switch( dwRegNum )
  {
    case 0:
      {
        // set mirroring
        if( MAP1->Map1_Regs[0] & 0x02 )
        {
          if( MAP1->Map1_Regs[0] & 0x01 )
          {
            PPU_set_mirroring(0,0,1,1);//水平镜像InfoNES_Mirroring( 0 );   //0011
          }
          else
          {
            PPU_set_mirroring(0,1,0,1);//垂直镜像InfoNES_Mirroring( 1 );   //0101
          }
        }
        else
        {
          // one-screen mirroring
          if( MAP1->Map1_Regs[0] & 0x01 )
          {
            PPU_set_mirroring(1,1,1,1);//InfoNES_Mirroring( 2 );   //1111
          }
          else
          {
            PPU_set_mirroring(0,0,0,0);//InfoNES_Mirroring( 3 );   //0000
          }
        }
      }
      break;

    case 1:
      {
        uint8 byBankNum = MAP1->Map1_Regs[1];

        if ( MAP1->Map1_Size == Map1_1024K )
        {
          if ( MAP1->Map1_Regs[0] & 0x10 )
          {
            if ( MAP1->Map1_swap )
            {
              MAP1->Map1_256K_base = (MAP1->Map1_Regs[1] & 0x10) >> 4;
              if(MAP1->Map1_Regs[0] & 0x08)
              {
                MAP1->Map1_256K_base |= ((MAP1->Map1_Regs[2] & 0x10) >> 3);
              }
              Map1_set_ROM_banks();//**********************************
              MAP1->Map1_swap = 0;
            }
            else
            {
              MAP1->Map1_swap = 1;
            }
          }
          else
          {
            // use 1st or 4th 256K banks
            MAP1->Map1_256K_base = ( MAP1->Map1_Regs[1] & 0x10 ) ? 3 : 0;
            Map1_set_ROM_banks();
          }
        }
        else if((MAP1->Map1_Size == Map1_512K) && (!RomHeader->num_8k_vrom_banks))
        {
          MAP1->Map1_256K_base = (MAP1->Map1_Regs[1] & 0x10) >> 4;
          Map1_set_ROM_banks();
        }
        else if ( RomHeader->num_8k_vrom_banks )
        {
          // set VROM bank at $0000
          if ( MAP1->Map1_Regs[0] & 0x10 )
          {
            // swap 4K
            byBankNum <<= 2;
//            PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
//            InfoNES_SetupChr();
//*********************************************************************************			  
			set_PPU_bank0(byBankNum+0);
            set_PPU_bank1(byBankNum+1);
            set_PPU_bank2(byBankNum+2);
            set_PPU_bank3(byBankNum+3);
			  
			  
          }
          else
          {
            // swap 8K
            byBankNum <<= 2;
//            PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 4 ] = VROMPAGE( (byBankNum+4) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 5 ] = VROMPAGE( (byBankNum+5) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 6 ] = VROMPAGE( (byBankNum+6) % (Neshd->byVRomSize << 3) );
//            PPUBANK[ 7 ] = VROMPAGE( (byBankNum+7) % (Neshd->byVRomSize << 3) );
//            InfoNES_SetupChr();
//******************************************************************************************
			  // swap 8K
            set_PPU_banks(byBankNum+0,byBankNum+1,byBankNum+2,byBankNum+3,
                          byBankNum+4,byBankNum+5,byBankNum+6,byBankNum+7);
          }
        }
      }
      break;

    case 2:
      {
        uint8 byBankNum = MAP1->Map1_Regs[2];

        if((MAP1->Map1_Size == Map1_1024K) && (MAP1->Map1_Regs[0] & 0x08))
        {
          if(MAP1->Map1_swap)
          {
            MAP1->Map1_256K_base =  (MAP1->Map1_Regs[1] & 0x10) >> 4;
            MAP1->Map1_256K_base |= ((MAP1->Map1_Regs[2] & 0x10) >> 3);
            Map1_set_ROM_banks();
            MAP1->Map1_swap = 0;
          }
          else
          {
            MAP1->Map1_swap = 1;
          }
        }

        if(!RomHeader->num_8k_vrom_banks) 
        {
          if ( MAP1->Map1_Regs[ 0 ] & 0x10 )
          {
            byBankNum <<= 2;

//            PPUBANK[ 4 ] = CRAMPAGE( byBankNum+0 );
//            PPUBANK[ 5 ] = CRAMPAGE( byBankNum+1 );
//            PPUBANK[ 6 ] = CRAMPAGE( byBankNum+2 );
//            PPUBANK[ 7 ] = CRAMPAGE( byBankNum+3 );
//            InfoNES_SetupChr();			  
//************************************************************
            set_VRAM_bank(4, byBankNum+0);
            set_VRAM_bank(5, byBankNum+1);
            set_VRAM_bank(6, byBankNum+2);
            set_VRAM_bank(7, byBankNum+3);
			  
            break;
          }
        }

        // set 4K VROM bank at $1000
        if(MAP1->Map1_Regs[0] & 0x10)
        {
          // swap 4K
          byBankNum <<= 2;
//          PPUBANK[ 4 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
//          PPUBANK[ 5 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
//          PPUBANK[ 6 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
//          PPUBANK[ 7 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
//          InfoNES_SetupChr(); 
//**************************************************************************************
			// swap 4K
          set_PPU_bank4(byBankNum+0);
          set_PPU_bank5(byBankNum+1);
          set_PPU_bank6(byBankNum+2);
          set_PPU_bank7(byBankNum+3);
			
			
        }
      }
      break;

    case 3:
      {
        uint8 byBankNum = MAP1->Map1_Regs[3];

        // set ROM bank
        if ( MAP1->Map1_Regs[0] & 0x08 )
        {
          // 16K of ROM
          byBankNum <<= 1;

          if ( MAP1->Map1_Regs[0] & 0x04 )
          {
            // 16K of ROM at $8000
            MAP1->Map1_bank1 = byBankNum;
            MAP1->Map1_bank2 = byBankNum+1;
            MAP1->Map1_bank3 = MAP1->Map1_HI1;
            MAP1->Map1_bank4 = MAP1->Map1_HI2;
          }
          else
          {
            // 16K of ROM at $C000
            if(MAP1->Map1_Size == Map1_SMALL)
            {
              MAP1->Map1_bank1 = 0;
              MAP1->Map1_bank2 = 1;
              MAP1->Map1_bank3 = byBankNum;
              MAP1->Map1_bank4 = byBankNum+1;
            }
          }  
        }
        else
        {
          // 32K of ROM at $8000
          byBankNum <<= 1;

          MAP1->Map1_bank1 = byBankNum;
          MAP1->Map1_bank2 = byBankNum+1;
          if(MAP1->Map1_Size == Map1_SMALL)
          {
            MAP1->Map1_bank3 = byBankNum+2;
            MAP1->Map1_bank4 = byBankNum+3;
          }
        }
		Map1_set_ROM_banks();//*********************************************
      }
      break;
  }
}


void Mapper001_Init()
{
	NES_Mapper->Reset = Mapper001_Reset;
	NES_Mapper->Write = Mapper001_Write;
}

#endif





