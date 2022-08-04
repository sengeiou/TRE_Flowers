#ifndef _TAA5767_H
#define _TAA5767_H

#include "sys.h"
#include "i2c.h"

#define TEA5767_ADDRESS_WRITE 0xC0
#define TEA5767_ADDRESS_READ	0xC1

#define FM_FREQ_INIT	87500000
#define FM_FREQ_MIN		87500000	
#define FM_FREQ_MAX		108000000
#define FREQ_STEP			100000
#define FM_ADC_LEVEL ADC_LEVEL_10

#define HLSI 0//低位注入
#define MS 0//立体声
#define BL 0//us/eurpel band
#define XTAL 1
#define PLLREF 0//fref 32.768Khz

enum
{
	MUTE_OFF = 0,
	MUTE_ON = 1
};

enum
{
	SEARCH_OFF = 0,
	SEARCH_ON = 1
};

enum
{
	SEARCH_DOWN = 0,
	SEARCH_UP = 1
};
/* 所搜索频率的信号质量级别 */
enum
{
	ADC_LEVEL_5 = 1,
	ADC_LEVEL_7 = 2,
	ADC_LEVEL_10 = 3
};
//目前状态
enum
{
	standby=0,
	loading=1,
	saving=2,
	searching=3
};

/* 定义一个TEA5767结构体 */
typedef struct
{
	uint8_t ucReady;					/* 准备好标志，1有一个频道被搜到或者一个制式已经符合，0没有频道被搜到 */
	uint8_t ucBandLimit;			/* 波段极限标志， 1达到频带极限 */
	uint16_t usPll;						/* 当前搜到频率的PLL值 */
	uint8_t ucStereo;					/* 立体声标志位，1立体声接收  0单声道接收 */
	uint8_t ucIFCount;				/* 中频计数器结果，正确调谐时值在31H至3EH之间 */
	uint8_t ucAdcLevel;				/* ADC输出级别，即信号质量的级别 */
	uint32_t ulFreq;					/* 频率 */
	uint8_t Fr_searched;      /*计数已搜到频率*/
	uint8_t cur_statue;       /*目前状况 */
}TEA5767_T;

void tea5767_Init_Device(I2C_HandleTypeDef* handle);
uint8_t tea5767_Set(uint32_t _Freq, uint8_t _ucMuteEn, uint8_t _ucSerchEn, uint8_t _ucSearchUp, uint8_t _ucAdcLevel);
uint8_t tea5767_ReadStatus(TEA5767_T *_tStatus);

#endif