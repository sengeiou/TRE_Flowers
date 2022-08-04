#include "gui_app_gps.h"

static GpsTable_t* gps_tabv = NULL;

static void status_bar_hidecb()
{
	
}

static void close_cb()
{
	page_call_handler((void*)(PAGE_APPS), DEL);
}

void gui_gps_init(lv_obj_t* root)
{
	page_self_root = root;
	gps_tabv = gps_table_create(page_self_root);
	
}

void gui_gps_focus(void)
{
	gps_tabv->gps_close_cb = close_cb;
	gps_tabv->gps_display(ENABLE);

#if 0
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = status_bar_hidecb;
#endif
	
}


