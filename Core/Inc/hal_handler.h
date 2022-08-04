#ifndef _HAL_HANDLER_H
#define _HAL_HANDLER_H

void dma1_stream4_handler();

typedef void (*dma2_stream5_handle)();

typedef void (*gpio_exti1_handle)();

typedef void (*tim_t7_handle)();



extern tim_t7_handle tim_t7_handler;

#endif
