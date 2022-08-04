#ifndef _BUZZER_H
#define _BUZZER_H

#include "main.h"
#include "sys.h"

typedef struct BuzzerConfig
{
	TIM_HandleTypeDef *tim;
	uint16_t tim_channel;
}buzzer_cfg_t;

extern buzzer_cfg_t buzzer_cfg;

void buzzer_on(void);
void buzzer_on_direct(void);
void buzzer_off(void);

#endif