#ifndef _COM_ADJUST_SLIDER_H
#define _COM_ADJUST_SLIDER_H

#include "lvgl.h"
#include "gui_inc_def.h"

typedef struct AdjustBarParam
{
	uint8_t type;
	uint8_t range_min;
	uint8_t range_max;
	uint8_t init_val;
	uint8_t slide_gap;
	lv_obj_t* re_focus_obj;
	const char* title;
}AdjustBarParam_t;

typedef void(*lv_adjust_slider_cb)(uint8_t type, uint8_t val);

lv_obj_t* lv_adjust_slider_create(lv_obj_t* parent, AdjustBarParam_t* param, lv_adjust_slider_cb cb);

#endif


