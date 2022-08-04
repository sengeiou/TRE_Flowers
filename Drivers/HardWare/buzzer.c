#include "buzzer.h"
#include "devices.h"

buzzer_cfg_t buzzer_cfg;

void buzzer_on_direct()
{
	HAL_TIM_PWM_Start(buzzer_cfg.tim,buzzer_cfg.tim_channel); 
}

void buzzer_on()
{
	if(!device_cfg.sound_flag) return;
	buzzer_on_direct();
}

void buzzer_off()
{
	HAL_TIM_PWM_Stop(buzzer_cfg.tim,buzzer_cfg.tim_channel);
}
