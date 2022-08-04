#include "com_msgbox.h"

static const char* btns[] = { "Ok", "Cancel", "" };
static msgbox_cb msg_cb;
static MsgParam_t* msg_param;

static void event_cb(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_current_target(e);
	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			bool _close = false;
			
			if(key == LV_KEY_ENTER)
			{
				if(lv_msgbox_get_active_btn(obj) == MSGBOX_OK)
					if(msg_cb != NULL) msg_cb(lv_msgbox_get_active_btn(obj), msg_param->msg_type);
				_close = true;
			}
			else if(key == LV_KEY_BACKSPACE)
				_close = true;
			
			if(_close)
			{
				if(msg_param->re_focus_obj != NULL) lv_group_focus_obj(msg_param->re_focus_obj);
				myfree(SRAMCCM, msg_param);
				lv_msgbox_close(obj); //关闭消息框
			}

		}
			break;
	}

}

lv_obj_t* lv_mymsgbox_create(MsgParam_t* param, msgbox_cb cb)
{
	msg_param = mymalloc(SRAMCCM, sizeof(MsgParam_t));
	if(msg_param != NULL) memset(msg_param, 0, sizeof(MsgParam_t));
	
	memcpy(msg_param, param, sizeof(MsgParam_t));
	msg_cb = cb;
	
	lv_obj_t* mbox = lv_msgbox_create(param->parent, param->title, param->info, btns, false);
	lv_obj_center(mbox);
	
	lv_gridnav_add(mbox, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), mbox);
	lv_group_focus_obj(lv_msgbox_get_btns(mbox));
	lv_obj_add_event_cb(mbox, event_cb, LV_EVENT_KEY, NULL);
}
