#ifndef _COM_GPS_TABVIEW_H
#define _COM_GPS_TABVIEW_H

#include "lvgl.h"
#include "gui_inc_def.h"

typedef struct
{
	lv_obj_t* gps_root;

	void(*gps_display)(uint8_t state);
	void(*gps_close_cb)();
	
	bool is_show;
	
}GpsTable_t;

GpsTable_t* gps_table_create(lv_obj_t* parent);

#endif


