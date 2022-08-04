#ifndef _ADS1110_H
#define _ADS1110_H

#include <stdbool.h>
#include "main.h"
#include "sys.h"

//ED0
#define ADS_ADDR_W 0x90
#define ADS_ADDR_R 0x91

bool ADS1110_Init(I2C_HandleTypeDef *i2c);
bool ADS1110_Write(uint8_t cmd);
uint16_t ADS1110_Read();
float ADS1110_Get_Voltage();


#endif

