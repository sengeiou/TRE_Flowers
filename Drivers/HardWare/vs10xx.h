#ifndef _VS1053B_H
#define _VS1053B_H

#include <stdbool.h>
#include "sys.h"
#include "spi.h"

/* Pin configuration */
#define HSPI_VS1053						&hspi2
#define VS1053_DREQ_PORT			DREQ_GPIO_Port
#define VS1053_DREQ_PIN				DREQ_Pin

#define	VS1053_XRST_PORT			XRESET_GPIO_Port
#define	VS1053_XRST_PIN				XRESET_Pin

#define VS1053_XCS_PORT				XCS_GPIO_Port
#define VS1053_XCS_PIN				XCS_Pin
#define VS1053_XDCS_PORT			XDCS_GPIO_Port
#define VS1053_XDCS_PIN				XDCS_Pin

#define FLAC_PLUGIN_ENABLE		0


/* Functions */
bool VS1053_Init();
void VS1053_Reset();
bool VS1053_SoftReset();
bool VS1053_SetVolume(uint8_t volumeLeft, uint8_t volumeRight);
bool VS1053_SetMode(uint16_t mode);
bool VS1053_GetMode(uint16_t *mode);
bool VS1053_AutoResync();
bool VS1053_SetDecodeTime(uint16_t time);
bool VS1053_SendEndFill(uint16_t num);
bool VS1053_IsBusy();
bool VS1053_ApplyFlacPlugin();
bool VS1053_SciWrite(uint8_t address, uint16_t input);
bool VS1053_SciRead(uint8_t address, uint16_t *res);
bool VS1053_SdiWrite(uint8_t input);
bool VS1053_SdiWrite32(uint8_t *input32);
bool VS1053_SdiWriteLen( uint8_t *input32, uint16_t len);

bool VS1053_GetDecodeTime(uint16_t * t);
uint16_t VS_Get_HeadInfo(void);

extern uint8_t endFillByte;

#if FLAC_PLUGIN_ENABLE
	extern const uint8_t atab[8208];
	extern const uint16_t dtab[8208];
#endif

#endif

