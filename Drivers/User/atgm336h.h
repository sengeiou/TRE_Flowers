#ifndef _ATGM336H_H
#define _ATGM336H_H

#include "app_default_config.h"
#include "os_task.h"
#include "libNMEA.h"
#include "sys.h"

void atgm336_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *dma);
void atgm336_start(void);
void atgm336_stop(void);
NMEA_data* get_gps_data();

#endif
