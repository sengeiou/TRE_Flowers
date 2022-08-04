#include "hdc1080.h"

I2C_HandleTypeDef *hdc1080_i2c;

static float calc_temp(uint16_t data)
{
	return (data / 65536.0f) * 165.0f - 40.0f;
}

static float calc_humi(uint16_t data)
{
	float tmp = 0.0f;
	tmp = (data / 65536.0f) * 100.0f;
	if (tmp>100.0) tmp = 100.0f;
	if (tmp<0) tmp = 0.0f;
	return tmp;
}

static uint16_t hdc1080_read_reg(uint8_t reg)
{
	uint8_t buf[2];
	buf[0] = reg;
	/* Send the read followed by address */
	HAL_I2C_Master_Transmit(hdc1080_i2c,HDC1080_BASE_ADD<<1,buf,1,1000);
	delay_ms(25);
	HAL_I2C_Master_Receive(hdc1080_i2c, HDC1080_BASE_ADD<<1|0x01, buf, 2, 1000);
	return buf[0]*256+buf[1];
}

static void hdc1080_write_reg(uint8_t reg, uint16_t val)
{
	uint8_t buf[3];
	buf[0] = reg;
	buf[1] = (uint8_t)((val >> 8) & 0xff);  // msb
	buf[2] = (uint8_t)(val & 0xff); 
	HAL_I2C_Master_Transmit(hdc1080_i2c,HDC1080_BASE_ADD<<1,buf,3,100);
}

void HDC1080_Init(I2C_HandleTypeDef *i2c)
{	
	
	hdc1080_i2c = i2c;
	
	//配置0x02寄存器
	switch(READ_MODE)
	{
		case 0:
			hdc1080_write_reg(CONFIGURATION, Config_Readmode_0);
			break;
		case 1:
			hdc1080_write_reg(CONFIGURATION, Config_Readmode_1);
			break;
		case 2:
			hdc1080_write_reg(CONFIGURATION, Config_Readmode_2);
			break;
		case 3:
			hdc1080_write_reg(CONFIGURATION, Config_Readmode_3);
			break;
	}
	
}

uint16_t HDC1080_ReadConfigation()
{
	return hdc1080_read_reg(CONFIGURATION);
}

uint16_t HDC1080_ReadManufacturerId()
{
	return hdc1080_read_reg(MANUFACTURER_ID);
}

uint16_t HDC1080_ReadDeviceId()
{
	return hdc1080_read_reg(DEVICE_ID);
}

float HDC1080_ReadHumidity()
{
	uint16_t rawH = hdc1080_read_reg(HUMIDITY);
	return calc_humi(rawH);
}

float HDC1080_ReadTemperature() 
{
	uint16_t rawT = hdc1080_read_reg(TEMPERATURE);
	return calc_temp(rawT);
}

bool HDC1080_ReadTempHumidity(float *temp, float *humi)
{
	uint8_t data[4];
	memset(data, 0x00, sizeof(data));
	
	uint8_t reg = TEMPERATURE;
	HAL_I2C_Master_Transmit(hdc1080_i2c,HDC1080_BASE_ADD<<1,&reg,1,1000);
	delay_ms(25); /* Temp转换时间 + Humi转换时间 = 20ms */
	if(HAL_I2C_Master_Receive(hdc1080_i2c, HDC1080_BASE_ADD<<1|0x01, data, sizeof(data), 1000) != HAL_OK) return false;
	
	*temp = calc_temp(data[0]<<8|data[1]);
	*humi = calc_humi(data[2]<<8|data[3]);
	return true;
}