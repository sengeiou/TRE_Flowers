#include "ads1110.h"

static I2C_HandleTypeDef *i2c_dev;

bool ADS1110_Write(uint8_t cmd)
{
	return (HAL_I2C_Master_Transmit(i2c_dev,ADS_ADDR_W,&cmd,1,100) == HAL_OK);
}

uint16_t ADS1110_Read()
{
	uint8_t temp[3];
	ADS1110_Write(0x9c); //单次转换, 数据传输速率15sps, 增益1, 16位转换
	delay_ms(10);
	HAL_I2C_Master_Receive(i2c_dev, ADS_ADDR_R, temp, 3, 1000);
	//Debug("%x\n", temp[2]);
	return (temp[0] << 8 ) | temp[1];
}

bool ADS1110_Init(I2C_HandleTypeDef *i2c)
{
	i2c_dev = i2c;
	if (HAL_I2C_IsDeviceReady(i2c_dev, ADS_ADDR_W, 1, 100) == HAL_OK)
	{
		return true; 
	}
	
	return false;
}

//(10+1)k电阻分压检测
float ADS1110_Get_Voltage()
{
	return (ADS1110_Read() * 1.0f / 32768.0f) * 2.048f * 11.0f;
}