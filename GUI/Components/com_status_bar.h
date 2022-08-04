#ifndef _COM_STATUS_BAR_H
#define _COM_STATUS_BAR_H

#include "sys.h"
#include "lvgl.h"
#include "gui_inc_def.h"

enum
{
	STATUS_BAR,
	ALARM,
	SOUND,
	TIME
};

typedef struct
{
	lv_obj_t *bar;

	void(*hide_cb)();
	
	void(*bat_set)(uint8_t val);
	void(*state_set)(uint8_t type, uint8_t state); //ENABLE ; DISABLE
	void(*parent_set)(lv_obj_t* parent);
	void(*title_set)(const char* title);
	
	uint8_t is_show;
}StatusBar_t;

StatusBar_t* status_bar_instance();

#endif

