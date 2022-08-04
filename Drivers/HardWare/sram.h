#ifndef _SRAM_H
#define _SRAM_H

#include "sys.h"

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))
#define Bank1_SRAM3_SIZE		((1 * 1024 * 1024))

#define LVGL_MEM_SIZE							((320 * 240 / 3) * 4 * 2)	//200K
#define ARDUBOY_MEM_SIZE					(256 * 128 * 2)						//64K
#define NES_ROM_MEM_SIZE					(512 * 1024)							//512K
#define NES_ROM_TITLE_MEM_SIZE		(8*1024)									//8K

#define LVGL_BUFF_BASE_ADDR 		(Bank1_SRAM3_ADDR) 																		
#define ARDUBOY_BUFF_BASE_ADDR 	(LVGL_BUFF_BASE_ADDR + LVGL_MEM_SIZE)
#define NES_ROM_BASE_ADDR				(ARDUBOY_BUFF_BASE_ADDR + ARDUBOY_MEM_SIZE)
#define NES_ROM_TITLE_BASE_ADDR	(NES_ROM_BASE_ADDR + NES_ROM_MEM_SIZE)

void SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);

#endif

