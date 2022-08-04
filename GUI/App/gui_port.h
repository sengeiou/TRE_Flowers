#ifndef _GUI_PORT_H
#define _GUI_PORT_H

#include "gui_inc.h"

#define PAGE_INIT_DEF(TAG)  gui_##TAG##_init
#define PAGE_FOCUS_DEF(TAG) gui_##TAG##_focus

typedef void(*page_init_handle)(lv_obj_t* root);
typedef void(*page_focus_handle)();

typedef struct
{
	uint8_t page_id;
	lv_obj_t* root;
	page_init_handle init_handler;
	page_focus_handle focus_handler;
	
}PageStruct_t;

void gui_port_init(void);
void gui_task_state(uint8_t state);

#endif
