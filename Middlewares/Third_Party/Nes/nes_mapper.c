#include "nes_mapper.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友ye781205的NES模拟器工程
//ALIENTEK STM32F407开发板
//NES MAP 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/1
//版本：V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 

//根据自己需要添加其他map文件用.  
#include "mapper/000.cpp"
#include "mapper/001.cpp"
#include "mapper/002.cpp"
#include "mapper/003.cpp"
#include "mapper/004.cpp"
#include "mapper/019.cpp"

#if 0
#include "mapper/000.cpp"
#include "mapper/001.cpp"
#include "mapper/002.cpp"
#include "mapper/003.cpp"
#include "mapper/004.cpp"
#include "mapper/006.cpp"
#include "mapper/007.cpp"
#include "mapper/008.cpp"
#include "mapper/011.cpp"
#include "mapper/013.cpp"
#include "mapper/015.cpp"
#include "mapper/016.cpp" 
#include "mapper/017.cpp"
#include "mapper/018.cpp"
#include "mapper/019.cpp"
#include "mapper/021.cpp"
#include "mapper/022.cpp"
#include "mapper/023.cpp"
#include "mapper/024.cpp"
#include "mapper/025.cpp"
#include "mapper/032.cpp"
#include "mapper/033.cpp"
#include "mapper/034.cpp"  
#include "mapper/064.cpp"  
#include "mapper/065.cpp"  
#include "mapper/066.cpp" 
#include "mapper/067.cpp"     
#include "mapper/069.cpp"    
#include "mapper/070.cpp"   
#include "mapper/071.cpp"  
#include "mapper/072.cpp"  
#include "mapper/073.cpp"   
#include "mapper/075.cpp" 
#include "mapper/076.cpp"
#include "mapper/078.cpp"
#include "mapper/079.cpp"
#include "mapper/085.cpp"
#include "mapper/087.cpp"
#include "mapper/088.cpp"
#include "mapper/099.cpp"
#include "mapper/113.cpp"  
#include "mapper/189.cpp"
#include "mapper/225.cpp"
#include "mapper/227.cpp"
#include "mapper/240.cpp" 
#include "mapper/245.cpp"
#endif


//支持的Mapper号,自己添加
#if 0
const int MapTab[] =  
{
	0,1,2,3,4,6,7,8,11,13,15,16,17,18,19,21,22,23,24,25,32,33,34,
	64,65,66,67,69,70,71,72,73,75,76,78,79,85,87,88,99,113,
	189,225,227,240,245,
	-1,//最后一个,-1,表示不支持的map号
};
#endif

const int MapTab[] ={0,1,2,3,4,19,-1};

#define MASK_BANK(bank,mask) (bank) = ((bank) & (mask))
#define VALIDATE_VROM_BANK(bank) \
		MASK_BANK(bank,VROM_mask); \
		if((bank) >= VROM_1K_SIZE) return;
		
		
uint32 VROM_mask;  

void Mapper_Reset(void){} 
uint8 Mapper_ReadLow(uint16 addr){	return 0;}  
void Mapper_WriteLow(uint16 addr,uint8 data){}
void Mapper_Write( uint16 addr , uint8 data){}
void Mapper_Read( uint8 data,uint16 addr ){}
void Mapper_HSync( int scanline ){}  
void Mapper_VSync(void)  {}	
	
 void Mapper_Init(void)
{
	uint32 probe; 
	NES_Mapper->Reset         = Mapper_Reset;
	NES_Mapper->Write         = Mapper_Write;
	NES_Mapper->Read          = Mapper_Read;
	NES_Mapper->WriteLow      = Mapper_WriteLow;
	NES_Mapper->ReadLow       = Mapper_ReadLow;	
	NES_Mapper->HSync         = Mapper_HSync;
	NES_Mapper->VSync         = Mapper_VSync; 
	VROM_mask = 0xFFFF; 
	for(probe = 0x8000; probe; probe >>= 1)
	{
		if((VROM_1K_SIZE-1) & probe) break;
		VROM_mask >>= 1;
	}
	
	switch (MapperNo)
	{
		case 0 :MAP0_Init();break;
		case 1 :MAP1_Init();break;
 		case 2 :MAP2_Init();break;
		case 3 :MAP3_Init();break;
 		case 4 :MAP4_Init();break;
//		case 19:MAP19_Init();break;
#if 0
		case 0 :MAP0_Init();break;
		case 1 :MAP1_Init();break;
 		case 2 :MAP2_Init();break;
 		case 3 :MAP3_Init();break;
 		case 4 :MAP4_Init();break;
 		case 6 :MAP6_Init();break;
 		case 7 :MAP7_Init();break;
 		case 8 :MAP8_Init();break;
 		case 11:MAP11_Init();break;
 		case 13:MAP13_Init();break;
 		case 15:MAP15_Init();break;
 		case 16:MAP16_Init();break;
 		case 17:MAP17_Init();break;
 		case 18:MAP18_Init();break;
 		case 19:MAP19_Init();break;
 		case 21:MAP21_Init();break;
 		case 22:MAP22_Init();break;
 		case 23:MAP23_Init();break;
 		case 24:MAP24_Init();break;
 		case 25:MAP25_Init();break;
 		case 32:MAP32_Init();break;
 		case 33:MAP33_Init();break;
 		case 34:MAP34_Init();break;
 		case 64:MAP64_Init();break;
 		case 65:MAP65_Init();break;
 		case 66:MAP66_Init();break;
 		case 67:MAP67_Init();break; 
 		case 69:MAP69_Init();break; 
 		case 70:MAP70_Init();break; 
 		case 71:MAP71_Init();break;
 		case 72:MAP72_Init();break;
 		case 73:MAP73_Init();break;
 		case 75:MAP75_Init();break;
 		case 76:MAP76_Init();break;
 		case 78:MAP78_Init();break;
 		case 79:MAP79_Init();break;
 		case 85:MAP85_Init();break;
 		case 87:MAP87_Init();break;
 		case 88:MAP88_Init();break;
 		case 99:MAP99_Init();break;
 		case 113:MAP113_Init();break;
 		case 189:MAP189_Init();break;
 		case 225:MAP225_Init();break;
 		case 227:MAP227_Init();break;
 		case 240:MAP240_Init();break;
 		case 245:MAP245_Init();break;
#endif
		default:break;
	}
	NES_Mapper->Reset();
} 
void asm_Mapper_Write(uint8 byData,uint16 wAddr)
{	
	NES_Mapper->Write(wAddr,byData);
}  
void asm_Mapper_ReadLow( uint16 wAddr) //添加的
{	
	NES_Mapper->ReadLow( wAddr);
} 
void asm_Mapper_WriteLow( uint8 byData ,uint16 wAddr) 
{	
	NES_Mapper->WriteLow(wAddr, byData );
}

void set_CPU_bank3(signed char page )  {map67_(page) ;} 
void set_CPU_bank4(signed char page )  {map89_(page) ;} 
void set_CPU_bank5(signed char page )  {mapAB_(page) ;}
void set_CPU_bank6(signed char page )  {mapCD_(page) ;} 
void set_CPU_bank7(signed char page )  {mapEF_(page) ;}
void set_CPU_banks(int bank0_num,int bank1_num,int bank2_num, int bank3_num)
{
    map89_(bank0_num);
	mapAB_(bank1_num);
	mapCD_(bank2_num);
	mapEF_(bank3_num);
}
void set_PPU_banks(uint32 bank0_num, uint32 bank1_num,
				   uint32 bank2_num, uint32 bank3_num,
				   uint32 bank4_num, uint32 bank5_num,
				   uint32 bank6_num, uint32 bank7_num)
{
  VALIDATE_VROM_BANK(bank0_num);
  VALIDATE_VROM_BANK(bank1_num);
  VALIDATE_VROM_BANK(bank2_num);
  VALIDATE_VROM_BANK(bank3_num);
  VALIDATE_VROM_BANK(bank4_num);
  VALIDATE_VROM_BANK(bank5_num);
  VALIDATE_VROM_BANK(bank6_num);
  VALIDATE_VROM_BANK(bank7_num);

  ppu->PPU_VRAM_banks[0] = VROM_banks + (bank0_num << 10); // * 0x400
  ppu->PPU_VRAM_banks[1] = VROM_banks + (bank1_num << 10);
  ppu->PPU_VRAM_banks[2] = VROM_banks + (bank2_num << 10);
  ppu->PPU_VRAM_banks[3] = VROM_banks + (bank3_num << 10);
  ppu->PPU_VRAM_banks[4] = VROM_banks + (bank4_num << 10);
  ppu->PPU_VRAM_banks[5] = VROM_banks + (bank5_num << 10);
  ppu->PPU_VRAM_banks[6] = VROM_banks + (bank6_num << 10);
  ppu->PPU_VRAM_banks[7] = VROM_banks + (bank7_num << 10);
	
  set_tile_banks( VROM_tiles + bank0_num * 0x400 ,
				  VROM_tiles + bank1_num * 0x400 ,
				  VROM_tiles + bank2_num * 0x400 ,
				  VROM_tiles + bank3_num * 0x400 ,
				  VROM_tiles + bank4_num * 0x400 ,
				  VROM_tiles + bank5_num * 0x400 ,
				  VROM_tiles + bank6_num * 0x400 ,
				  VROM_tiles + bank7_num * 0x400 );
}
void set_PPU_bank0(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[0] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(0, VROM_tiles + bank_num * 0x400);
}

void set_PPU_bank1(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[1] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(1, VROM_tiles + bank_num * 0x400 );
}

void set_PPU_bank2(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[2] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(2, VROM_tiles + bank_num * 0x400 );
}

void set_PPU_bank3(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[3] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(3, VROM_tiles + bank_num * 0x400 );
}

void set_PPU_bank4(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[4] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(4, VROM_tiles + bank_num * 0x400 );
} 
void set_PPU_bank5(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[5] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(5, VROM_tiles + bank_num * 0x400 );
}

void set_PPU_bank6(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[6] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(6, VROM_tiles + bank_num * 0x400 );
}

void set_PPU_bank7(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[7] = VROM_banks + (bank_num << 10); // * 0x400
	set_tile_bank(7, VROM_tiles + bank_num * 0x400);
}
// for mapper 19,68,90
void set_PPU_bank8(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[8] = VROM_banks + (bank_num << 10);
}
void set_PPU_bank9(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[9] = VROM_banks + (bank_num << 10);
}
void set_PPU_bank10(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[10] = VROM_banks + (bank_num << 10);
}
void set_PPU_bank11(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	ppu->PPU_VRAM_banks[11] = VROM_banks + (bank_num << 10);
}

// for mapper 1,4,5,6,13,19,80,85,96,119
void set_VRAM_bank(uint8 bank, uint32 bank_num)
{
	if(bank < 8)
	{        
		ppu->PPU_VRAM_banks[bank] = PPU_patterntables + ((bank_num & 0x0f) << 10);
	}
	else if(bank < 12)
	{
		set_name_table(bank, bank_num);
	}
}












