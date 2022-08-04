#ifndef _GUI_APP_MUSIC_H
#define _GUI_APP_MUSIC_H

#include "gui_inc.h"

typedef struct PlayState
{
	const char* file_name;
	uint8_t volume;
	uint16_t kbps;
	uint16_t sec;
}PlayState_t;

#endif

