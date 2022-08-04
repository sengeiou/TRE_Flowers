#ifndef _HDC1080_H
#define _HDC1080_H

#include "main.h"
#include "sys.h"
#include "math.h"
#include "string.h"
#include <stdbool.h>


/**
 *0：关闭Heater,自由选择读取温度、湿度
 *1：关闭Heater,同时采集读取温度、湿度
 *2: 开启Heater,自由选择读取温度、湿度
 *3: 开启Heater,同时采集读取温度、湿度
 */
#define READ_MODE 3		

typedef enum{
	HDC1080_BASE_ADD			= 0x40,		//HDC1080的IIC地址为1000000（7位地址）+ 读(1)或写(0)方向位	
	Manufacturer_ID_value = 0x5449,	//制造商ID值
	Device_ID_value				= 0x1050,	//设备ID值
	Config_Readmode_0		  = 0x0000,	//关闭Heater,自由选择采集温度或者湿度
	Config_Readmode_1			= 0x1000, //关闭Heater,同时采集温度和湿度(温度在前),温度分辨率为14位,湿度分辨率为14位
	Config_Readmode_2			= 0x2000,	//开启Heater,自由选择采集温度或者湿度
	Config_Readmode_3			= 0x3000, //开启Heater,同时采集温度和湿度(温度在前),温度分辨率为14位,湿度分辨率为14位
}HDC1080_Config;

typedef enum {
	TEMPERATURE 		= 0x00,
	HUMIDITY 				= 0x01,
	CONFIGURATION 	= 0x02,
	MANUFACTURER_ID = 0xFE,
	DEVICE_ID 			= 0xFF,
	SERIAL_ID_FIRST = 0xFB,
	SERIAL_ID_MID 	= 0xFC,
	SERIAL_ID_LAST 	= 0xFD,
}HDC1080_Pointers;


void HDC1080_Init(I2C_HandleTypeDef *i2c);
uint16_t HDC1080_ReadConfigation(void);
uint16_t HDC1080_ReadManufacturerId(void);
uint16_t HDC1080_ReadDeviceId(void);
float HDC1080_ReadTemperature(void);
float HDC1080_ReadHumidity(void);
bool HDC1080_ReadTempHumidity(float *temp, float *humi);


#endif