#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdbool.h>
#include "sys.h"
#include "fatfs.h"
#include "vs10xx.h"
#include "sram.h"
#include "os_task.h"
#include "app_default_config.h"

#define VOLUME_MIN 30
#define VOLUME_DEFAULT 0x3f
#define VOLUME_MAX 130

typedef void(*player_end_handle)();

/* Functions */
bool MP3_Init(void);
bool MP3_Play(const char *filename);
void MP3_Stop(void);
void MP3_Pause(void);
void MP3_Resume(void);
void MP3_Feeder(void);
void MP3_SetVolume(uint8_t vol);

/* Flags */
extern bool isPlaying;
extern bool isFileOpen;
extern const char* mp3Path;;

extern player_end_handle player_end_handler;
#endif

