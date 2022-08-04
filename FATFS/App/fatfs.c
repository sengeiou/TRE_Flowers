/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
void get_fatsize(uint32_t* freeSize,uint32_t* totalSize)
{
	FRESULT res;
	FATFS *fs = &SDFatFS;
	DWORD freeClust = 0,freeNum = 0,totalNum = 0;
	
	res = f_getfree(SDPath,&freeClust,&fs);
	if(res == FR_OK)
	{
		totalNum = (fs->n_fatent-2)*fs->csize;
		freeNum = freeClust*fs->csize;
		
		//B
		*freeSize = (freeNum>>1)<<10;
		*totalSize = (totalNum>>1)<<10;
	}
	
	return ;
}
/* USER CODE END Application */
