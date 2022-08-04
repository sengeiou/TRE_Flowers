#include "sram_test.h"

uint32_t bsp_TestExtSRAM(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};
	
	Debug("STM32F407ZG FSMC SRAM Test\r\n");
	
	/* 写SRAM */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}
	
	/* 读SRAM */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}
	Debug("SDRAM check round 1 error = %d\n", err);
	if (err > 0)
	{
		return (4 * err);
	}

	#if 1
	/* 对SRAM 的数据求反并写入 */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE/4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* 再次比较SRAM的数据 */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i<EXT_SRAM_SIZE/4;i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	Debug("SDRAM check round 2 error = %d\n", err);
	if (err>0)
	{
	return (4 * err);
	}
	#endif

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	Debug("SDRAM check round 3 error = %d\n", err);
	if (err > 0)
	{
		return err;
	}
	
	if (err == 0) {
      Debug("SRAM Test success\r\n");
  } else {
      Debug("SRAM Test fail\r\n");
  }
	
	return 0;
}

//u32 testsram[250000] __attribute__((at(Bank1_SRAM3_ADDR)));
void bsp_TestExtSRAM_Space(void)
{
	u32 i=0;  	  
	u8 temp=0;	   
	u8 sval=0;	//在地址0读到的数据
	
//	u32 ts=0;
//	for(ts=0;ts<250000;ts++)testsram[ts]=ts;
//	for(ts=0;ts<250000;ts++) Debug("%d\n", testsram[ts]);
	
	//每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节
	for(i=0;i<1024*1024;i+=4096)
	{
		FSMC_SRAM_WriteBuffer(&temp,i,1);
		temp++;
	}
	
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<1024*1024;i+=4096) 
	{
		FSMC_SRAM_ReadBuffer(&temp,i,1);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//后面读出的数据一定要比第一次读到的数据大.	   		   
 	}
	
	Debug("SRAM: %d KB\n", (temp-sval+1)*4);
	
}




