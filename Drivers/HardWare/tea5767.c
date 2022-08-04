#include "tea5767.h"

static I2C_HandleTypeDef *hi2c;

static uint8_t tea5767_Write(uint8_t *_ucaBuf, uint8_t _count)
{	
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c,TEA5767_ADDRESS_WRITE,_ucaBuf,_count,1000);
	if(status == HAL_OK)
		return _count;
	else
		return 0;
}

static uint8_t tea5767_Read(uint8_t *_ucaBuf, uint8_t _count)
{
  HAL_StatusTypeDef status = HAL_I2C_Master_Receive(hi2c, TEA5767_ADDRESS_READ, _ucaBuf, _count, 1000);
	if(status == HAL_OK)
		return _count;
	else
		return 0;
}

static uint16_t CaclPLL(uint32_t _freq)
{
	return (uint16_t)(((_freq - 225000) * 4) / 32768);
}

static uint32_t CaclFreq(uint16_t _pll)
{
	return ((uint32_t)_pll * 32768) / 4 + 225000;
}

uint8_t tea5767_Set(uint32_t _Freq, uint8_t _ucMuteEn, uint8_t _ucSerchEn, uint8_t _ucSearchUp, uint8_t _ucAdcLevel)
{
	uint8_t buf[5];
	uint16_t pll;

	/* 美国/欧洲(87.5-108M), 日本(76-91MHz) */
	pll = CaclPLL(_Freq);

	/*
		第1个寄存器：
		MUTE (bit7) = 0
		SM(bit6) = 0
	*/
	buf[0] = ((pll >> 8) & 0x3F);
	if (_ucMuteEn == MUTE_ON)
	{
		buf[0] |= (1 << 7);
	}
	if (_ucSerchEn == 1)
	{
		buf[0] |= (1 << 6);
	}
		
	/*
		第2个寄存器：PLL的低8位
	*/
	buf[1] = pll;

	/*
	    第3个寄存器
		SUD(bit7) = 1, 向上搜索
		SSL(bit6 bit5) = 7  搜索停止级别
			00 不允许设置等级
			01 低级，ADC 输出 = 5
			10 中级，ADC 输出 = 7
			11 高级，ADC 输出 = 10
			
		HLSI(BIT4) = 0, 低边带低频注入
		MS (BIT3) = 0, 选择立体声
		MR (BIT2) = 0, R通道不静音
		ML (BIT1) = 0, L通道不静音
		SWP1（bit0) = 0 : 软件可编程端口
	*/
	if ((_ucAdcLevel < 1) || (_ucAdcLevel > 2))
	{
		_ucAdcLevel = 2;
	} 
	buf[2] = (2 << 5);
	if (_ucSearchUp == SEARCH_UP)
	{
		buf[2] |= (1 << 7);
	}

	/*
		第4个寄存器
		SWP2（bit7) = 0 : 软件可编程端口
		STBY（bit6) = 0 : 选择非待机模式
		BL (bit5) = 0, 选择美国/欧洲频带
		XTAL(bit4) = 1， 选择32768Hz晶振
		SMUTE(bit3) = 0, 软件静音关闭
		HCC(bit2) = 0, 高切控制关
		SNC(bit1) = 1, 立体声消噪功能打开
		SI(bit0) = 1, swport引脚作为就绪标志
	*/
	buf[3] = (1 << 4) | (1 << 1) | (1 << 0);

	/*
		第5个寄存器
		PLLREF(BIT7) = 0, PLL的6.5MHz参考频率关闭
		DTC(bit6) = 1, 去加重时间常数75uS
	*/
	buf[4] = (1 << 6);

	if (tea5767_Write(buf, 5) == 5) {
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t tea5767_ReadStatus(TEA5767_T *_tStatus)
{
	uint8_t buf[5];
	uint8_t ret = 0;

	if (tea5767_Read(buf, 5) == 5) {
		ret = 1;
		
		_tStatus->ucReady = (buf[0] >> 7) & 0x01;
		_tStatus->ucBandLimit = (buf[0] >> 6) & 0x01;
		_tStatus->usPll = ((buf[0] & 0x3f) << 8) + buf[1];		//PLL值
		_tStatus->ucStereo =  (buf[2] >> 7) & 0x01;
		_tStatus->ucIFCount = buf[2] & 0x7F;
		_tStatus->ucAdcLevel = (buf[3] >> 4) & 0x0F;

		_tStatus->ulFreq = CaclFreq(_tStatus->usPll);
	}

	return ret;
}

void tea5767_Init_Device(I2C_HandleTypeDef* handle)
{
	hi2c = handle;
}
