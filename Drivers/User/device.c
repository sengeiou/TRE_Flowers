#include "devices.h"
#include "sram_test.h"
#include "app_default_config.h"

#define DEVICE_DEBUG 0

DeviceConfig_t device_cfg;
FATFS sd_fat;
FRESULT fR;

BMP280_HandleTypedef bmp280;
bool bme280p = false;

_RTC rtc = {
    .Year = 22, .Month = 8, .Date = 1,
    .DaysOfWeek = MONDAY,//1-7
    .Hour = 8, .Min = 35, .Sec = 19
};
BMP280_Data_t *bmp280_data;
TH_Data_t *hdc1080_data;

uint8_t *eeprom_test;

const uint32_t font_install_flag = 0xA5A55A5A;
/******************************************************/


/******************************************************/
void device_config_reset()
{
	device_cfg.device_id = MY_DEVICE_ID;
	device_cfg.language_flag = 0; //默认英文
	device_cfg.font_zh_flag = 0; //默认未安装字库
	device_cfg.sound_flag = 1; //默认蜂鸣器开启
	device_cfg.alarm_flag = 0; //默认不开启闹钟
	device_cfg.lcd_level = 6; //默认LCD背光等级6
	device_cfg.screen_close_delay = 60; //默认屏幕关闭延时60秒
	device_cfg.vs10_volume = 6; //默认vs1053B声音大小60
	device_cfg.book_bg_color = 1; 	//默认阅读背景白色
	device_cfg.book_font_color = 0; //默认阅读字体黑色
	
	//初始化闹钟
	uint8_t alarm_len = sizeof(device_cfg.alarm_list) / sizeof(device_cfg.alarm_list[0]);
	for(int i=0;i<alarm_len;i++)
	{
		device_cfg.alarm_list[i] = 5959; //59:59
	}
	
	//写入配置
	Debug("write default config\n");
	device_config_write();
}

/******************************************************/
void font_install_flag_clear()
{
	uint8_t len = sizeof(uint32_t);
	uint8_t* clear_flag = mymalloc(SRAMCCM, len);
	if(clear_flag != NULL) memset(clear_flag, 0, len);
	W25QXX_Write(clear_flag, 0, len);
	
	myfree(SRAMCCM, clear_flag);
	Debug("font installation flag cleared\n");
}

bool font_install_check()
{
	bool res = false;
	uint8_t len = sizeof(uint32_t);
	uint8_t* read_flag = mymalloc(SRAMCCM, len);
	if(read_flag != NULL) memset(read_flag, 0, len);
	
	W25QXX_Read(read_flag, 0, len);
	res = (font_install_flag == (read_flag[0] << 24) || (read_flag[1] << 16) || (read_flag[2] << 8) || (read_flag[3]));
	
	Debug("read flag 0x%x%x%x%x\n", read_flag[0], read_flag[1], read_flag[2], read_flag[3]);
	myfree(SRAMCCM, read_flag);
	return res;
}

bool bmp280_read()
{
	bmp280_data->th.temp = 0.0f;
	bmp280_data->th.humi = 0.0f;
	bmp280_data->press = 0.0f;
	
	bool res = bmp280_read_float(&bmp280, &(bmp280_data->th.temp),\
																		&(bmp280_data->press),\
																		&(bmp280_data->th.humi));
	
#if DEVICE_DEBUG
	Debug("Pressure: %.2f Pa, Temperature: %.2f C", bmp280_data->press, bmp280_data->th.temp);
	if (bme280p) {
		printf(", Humidity: %.2f\n", bmp280_data->th.humi);
	}
	else {
		printf("\n");
	}
#endif
	
	return res;
}

bool ds3231_read()
{
	bool res = DS3231_GetTime(&rtc);
#if DEVICE_DEBUG
	Debug("%d-%d-%d %d:%d:%d %d\n", rtc.Year, rtc.Month, rtc.Date, rtc.Hour, rtc.Min, rtc.Sec, rtc.DaysOfWeek);
#endif
	return res;
}

bool hdc1080_read()
{
	hdc1080_data->temp = 0.0f;
	hdc1080_data->humi = 0.0f;
	
	bool res = HDC1080_ReadTempHumidity(&(hdc1080_data->temp), &(hdc1080_data->humi));
#if DEVICE_DEBUG
	Debug("HDC1080 T/H: %.2f, %.2f\n", hdc1080_data->temp, hdc1080_data->humi);
#endif
	
	return res;
}

bool device_config_read()
{
	at24_read(EEPROM_CONFIG_BASE_ADDR, (uint8_t*)&device_cfg, sizeof(DeviceConfig_t), 1000);
	return (device_cfg.device_id == MY_DEVICE_ID);
}

void device_config_write()
{
	bool res = at24_write(EEPROM_CONFIG_BASE_ADDR, (uint8_t*)&device_cfg, sizeof(DeviceConfig_t), 1000);
	if(res)
	{
		Debug("config write success\n");
	}
}

void device_init()
{
	Debug("System clock: %d\n", SysClockGet());
	Debug("**********************************************************\n");
	
	//Mem Init
	Debug("--Mem init...\n");
	my_mem_init(SRAMIN);
	my_mem_init(SRAMCCM);
	
	//SDRAM Init
	Debug("--SDRAM init...\n");
	bsp_TestExtSRAM();
	bsp_TestExtSRAM_Space();
	my_mem_init(SRAMEX);
	
	//AT24C0x Init
	Debug("--EEPROM init...\n");
	if(at24_isConnected()) Debug("AT24Cxx init ok\n");
#if 0
	uint8_t head[2];
	uint8_t tail[2];
	//eeprom_test = mymalloc(SRAMEX,16384);
	//memset(eeprom_test, 0xaa, 16384);
	//at24_write(0, eeprom_test, 16384, 10000);
	//HAL_Delay(10);
	at24_read(0, head, 2, 100);
	HAL_Delay(10);
	at24_read(16382, tail, 2, 100);
	HAL_Delay(10);
	for(int i=0;i<2;i++)
	{
		Debug("%x,%x\n", head[i], tail[i]);
	}
#endif
	
	//Flash Init
	Debug("--Flash init...\n");
	W25QXX_Init(&hspi3);
	Debug("Flash: %x\n", W25QXX_ReadID());
#if 0
	uint8_t w25q_read[32];
	uint8_t w25q_str[32] = "hello w25q256 ic";
	W25QXX_Write(w25q_str, 0, sizeof(w25q_str));
	W25QXX_Read(w25q_read, 0, sizeof(w25q_str));
	Debug("%s\n", w25q_read);
#endif
	
	//ADS1110 Init
	Debug("--ADC init...\n");
	if(ADS1110_Init(&hi2c1)) Debug("ADS1110 init ok\n");
	
	//HDC1080 Init
	Debug("--HDC1080 init...\n");
	HDC1080_Init(&hi2c1);
	Debug("HDC1080 Info: ManufacturerID: %x, DeviceID: %x\n", HDC1080_ReadManufacturerId(), HDC1080_ReadDeviceId());
	
	//BMP280 Init
	Debug("--BMP280 init...\n");
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;
	while (!bmp280_init(&bmp280, &bmp280.params)) {
		Debug("BMP280 initialization failed\n");
		delay_ms(2000);
	}
	bme280p = bmp280.id == BME280_CHIP_ID;
	Debug("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
	
	//DS3231 Init
	Debug("--DS3231 init...\n");
	DS3231_Init(&hi2c1);
	DS3231_ClearAlarm1();
#if CONFIG_INIT_DS3231
	DS3231_SetTime(&rtc); //设置初始时间
#endif
	
	//Buzzer Init
	Debug("--Buzzer init...\n");
	buzzer_cfg.tim = &htim10;
	buzzer_cfg.tim_channel = TIM_CHANNEL_1;
	
	//FM Init
	Debug("--Tea5767 init...\n");
	FM_init();
	
	//SD Init
	Debug("--SD init...\n");
	fR= f_mount(&sd_fat,SDPath,1);
	if(fR == FR_OK)
	{
		Debug("SD mount success\n");
	}
	else
	{
		for(;;)
		{
			Debug("SD mount failed\n");
			delay_ms(2000);
		}
	}
	
	//Device config init
	Debug("--Config init...\n");
	bool config_res = device_config_read();
	if(!config_res)
	{
		device_config_reset();
	}
	
	config_res = device_config_read();
	if(config_res)
	{
		Debug("read device config success\n");
		Debug("device id: 0x%08x\n", device_cfg.device_id);
	}
	else
	{
		Debug("read device config failed\n");
		for(;;)
		{
			Debug("Device config initialization failed\n");
			delay_ms(2000);
		}
	}
	
	//LCD Init
	Debug("--LCD init...\n");
	LCD_Init(&hspi1);
	LCD_SetBlk(device_cfg.lcd_level);
	LCD_Clear(WHITE);

	Debug("\r\nHardware init complete\n");
	Debug("-->Hardware init complete\n");	
	Debug("**********************************************************\n");
	
	//检查字体是否安装
	if(font_install_check())
		device_cfg.font_zh_flag = true;
	else
		device_cfg.font_zh_flag = false;

	bmp280_data = mymalloc(SRAMCCM, sizeof(BMP280_Data_t));
	if(bmp280_data != NULL) memset(bmp280_data, 0, sizeof(BMP280_Data_t));
	
	hdc1080_data = mymalloc(SRAMCCM, sizeof(TH_Data_t));
	if(hdc1080_data != NULL) memset(hdc1080_data, 0, sizeof(TH_Data_t));
}










