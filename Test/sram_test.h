#ifndef _SRAM_TEST_H
#define _SRAM_TEST_H

#include "sys.h"
#include "sram.h"

#define EXT_SRAM_ADDR  	Bank1_SRAM3_ADDR
#define EXT_SRAM_SIZE		Bank1_SRAM3_SIZE

uint32_t bsp_TestExtSRAM(void);
void bsp_TestExtSRAM_Space(void);

#endif
