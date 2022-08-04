#include "fm.h"

TEA5767_T cerrent_FM;

unsigned long Freq; //初始化频率

static void calc_freq(int dir)
{
	if(dir == SEARCH_UP)
	{
		Freq += FREQ_STEP;
		if(Freq > FM_FREQ_MAX) Freq = FM_FREQ_MIN;
	}
	else
	{
		Freq -= FREQ_STEP;
		if(Freq < FM_FREQ_MIN) Freq = FM_FREQ_MAX;
	}
}

void FM_test(void)
{
	tea5767_Set(94800000, MUTE_OFF, SEARCH_ON, SEARCH_UP, FM_ADC_LEVEL);
	delay_ms(1000);
	tea5767_ReadStatus(&cerrent_FM);
}

void FM_Search_Manual(int dir)
{
	calc_freq(dir);
	
	Debug("FM: %lu\n", Freq);
	tea5767_Set(Freq, MUTE_OFF, SEARCH_OFF, dir, FM_ADC_LEVEL);
	delay_ms(30);
	tea5767_ReadStatus(&cerrent_FM);
	
	if(cerrent_FM.ucReady==1)
	{
		Debug("FM found\n");
	}
	
}

void FM_Search_Auto(int dir)
{	
	while(1)
	{
		calc_freq(dir);
		Debug("FM: %lu\n", Freq);
		tea5767_Set(Freq, MUTE_OFF, SEARCH_ON, SEARCH_UP, FM_ADC_LEVEL);
		delay_ms(50);
		tea5767_ReadStatus(&cerrent_FM);
		
		if(cerrent_FM.ucBandLimit==1)
		{
			Debug("FM bandLimit\n");
			break;
		}
		else if(cerrent_FM.ucReady==1)
		{
			Debug("FM found\n");
			break;
		}
	}
}

void FM_State_Set(int state)
{
	tea5767_Set(Freq,state,SEARCH_OFF,SEARCH_UP,FM_ADC_LEVEL);
	delay_ms(30);
	tea5767_ReadStatus(&cerrent_FM);
}

void FM_init()
{
	Freq = 106400000;//94800000; //从配置中读取
	tea5767_Init_Device(&FM_I2C);
	tea5767_Set(Freq, MUTE_ON, SEARCH_ON, SEARCH_UP, FM_ADC_LEVEL);
	
	//GUI_draw(&cerrent_FM);
}