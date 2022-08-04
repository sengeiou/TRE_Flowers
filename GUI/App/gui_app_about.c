#include "gui_app_about.h"

static void static_bar_hidecb()
{
	page_call_handler((void*)(PAGE_APPS), DEL);
}

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			if(key == LV_KEY_BACKSPACE)
			{
				status_bar->state_set(STATUS_BAR, DISABLE);
			}
			
		}	
			break;
	}
}

void gui_about_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	const char* data = "https://www.wiyixiao4.com/blog";
	lv_obj_t* lv_qr = lv_qrcode_create(page_self_root, 120, lv_color_hex3(0x33f), lv_color_hex3(0xeef));
	lv_qrcode_update(lv_qr, data, strlen(data));
	lv_obj_align(lv_qr, LV_ALIGN_LEFT_MID, 20, 10);

	lv_obj_t* label_info = lv_label_create(page_self_root);
	lv_label_set_text(label_info, "IC:\nSTM32F407ZGT6\nIS62WV51216BLL\nHDC1080\nBMP280\nDS3231\nATGM336H\nW25Q256\nAT24C128\nVS1053B\nST7789V\nADS1110");
	lv_obj_align(label_info, LV_ALIGN_BOTTOM_RIGHT, -5, -10);
	
}

void gui_about_focus(void)
{
	lv_group_focus_obj(page_self_root);
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = static_bar_hidecb;
	status_bar->title_set(app_names[PAGE_APP_ABOUT - 3]);
}



