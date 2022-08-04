#ifndef _DEVICES_H
#define _DEVICES_H

#include <string.h>
#include <stdbool.h>
#include "sys.h"
#include "lcd_2inch.h"
#include "ads1110.h"
#include "at24cxx.h"
#include "malloc.h"
#include "stm32_ds3231.h"
#include "w25q16.h"
#include "atgm336h.h"
#include "fm.h"
#include "buzzer.h"
#include "hdc1080.h"
#include "bmp280.h"
#include "fatfs.h"

#define MY_DEVICE_NAME "TRE Flowers"
#define MY_DEVICE_ID  0x00000001

#define FONT_ZH_ADDR_OFFSET 4 //字库地址偏移

#define EEPROM_BOOK_BASE_ADDR 0x0000 //书籍信息保存首地址 //AT24C128中分配4K == 4096
#define EEPROM_CONFIG_BASE_ADDR (EEPROM_BOOK_BASE_ADDR + 0x1000) //系统配置信息保存首地址 //AT24C128中分配1K == 1024

typedef struct DeviceConfig
{
	uint32_t device_id;				//设备ID
	uint8_t language_flag:1;	//语言标志//0:en 1:zh
	uint8_t font_zh_flag:1;		//中文字体烧录标志//0/1
	uint8_t sound_flag:1;			//蜂鸣器开启标志//0/1
	uint8_t alarm_flag:1; 		//闹钟开启标志//0/1
	uint8_t lcd_level:4;			//LCD背光亮度(0~10)
	uint8_t screen_close_delay; //屏幕关闭延时(30-180)秒
	uint8_t vs10_volume:4;			//耳机声音//(3-13) *10
	uint8_t book_bg_color:2;		//阅读背景颜色(0/1/2)(黑/白/护眼)
	uint8_t book_font_color:2;	//阅读字体颜色(0/1)(黑/白)
	
	uint16_t alarm_list[10];	//闹钟列表//10个//2*10字节 2359 ==> 23:59 ==> 23*100+59
}DeviceConfig_t;

typedef struct TH_Data
{
	float temp;
	float humi;
}TH_Data_t;

typedef struct BMP280_Data
{
	TH_Data_t th;
	float press;
}BMP280_Data_t;

extern DeviceConfig_t device_cfg;
extern _RTC rtc;
extern BMP280_Data_t *bmp280_data;
extern TH_Data_t *hdc1080_data;

extern const uint32_t font_install_flag;

void device_init(void);
void device_config_reset(void);
void device_config_write(void);
bool device_config_read(void);
bool bmp280_read(void);
bool ds3231_read(void);
bool hdc1080_read(void);
bool font_install_check(void);
void font_install_flag_clear(void);

#endif
