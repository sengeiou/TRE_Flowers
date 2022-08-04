#ifndef _COM_MSGBOX_H
#define _COM_MSGBOX_H

#include "lvgl.h"
#include "gui_inc_def.h"

enum
{
	MSGBOX_OK,
	MSGBOX_CANCEL
};

typedef struct MsgParam
{
	const char* title;
	const char* info;
	lv_obj_t* parent;
	lv_obj_t* re_focus_obj;
	uint8_t msg_type;
}MsgParam_t;

typedef void(*msgbox_cb)(uint8_t index, uint8_t type);

lv_obj_t* lv_mymsgbox_create(MsgParam_t* param, msgbox_cb cb);

#endif


