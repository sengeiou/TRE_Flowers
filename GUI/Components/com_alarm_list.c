#include "com_alarm_list.h"

#define BTNMX_CLEAR_ID 	10
#define BTNMX_RESET_ID 	11
#define BTNMX_OK_ID 		12

static lv_obj_t* com_parent;
static lv_obj_t* list_alarm;
static lv_obj_t* re_focus;

static lv_obj_t* time_cont;
static lv_obj_t* label_time;

static char time_txt[8];

static const char* btnm_map[] = { "1", "2", "3", "4", "5", "\n",
																	"6", "7", "8", "9", "0", "\n",
																	"Clear", "Reset", "Ok", ""};

/******************************************************************/
static void lv_time_btnmx_create(lv_obj_t* parent, uint8_t id);

/******************************************************************/
static void event_key_handler(lv_event_t *e)
{
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			if(key == LV_KEY_ENTER)
			{	
				//设置闹钟
				Debug("alarm list item: %d\n", lv_obj_get_child_id(obj));
				lv_time_btnmx_create(com_parent, lv_obj_get_child_id(obj));
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				//关闭弹窗
				lv_group_focus_obj(re_focus);
				lv_obj_del(list_alarm);
			}	
		}
			break;
	}

}

static void event_btnmx_handler(lv_event_t *e)
{
	static uint8_t index = 0;
	
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		uint32_t id = lv_btnmatrix_get_selected_btn(obj);
		
		if(id == BTNMX_CLEAR_ID)
		{
			Debug("alarm clock clear\n");
			index = 0;
			memset(time_txt, 0, sizeof(time_txt));
			lv_label_set_text(label_time, time_txt);
		}
		else if(id == BTNMX_RESET_ID)
		{
			Debug("alarm clock reset\n");
			memcpy(time_txt, "59:59", strlen("59:59"));
			lv_label_set_text(label_time, time_txt);
		}
		else if(id == BTNMX_OK_ID)
		{
			if (strlen(time_txt) < 5) return;
			char* dot = strchr(time_txt, ':');
			uint8_t min = atoi(dot + 1);

			memset(dot, 0, strlen(dot));
			uint8_t hour = atoi(time_txt);
			
			do
			{
				if(hour == 59 && min == 59) break;

				if (hour > 23 || min > 59)
				{
					Debug("alarm clock input error\n");
					lv_label_set_text(label_time, "input error");
					return;
				}
			}while(0);

			Debug("alarm clock set %d:%d\n", hour, min); //(包含59:59，无效默认时间)
			
			uint8_t index = (uint8_t)lv_obj_get_user_data(time_cont);
			uint16_t old_alarm_clock = device_cfg.alarm_list[index];
			uint16_t new_alarm_clock = (hour * 100) + min;
			Debug("old alarm clock: %d\n", old_alarm_clock);
			
			if(old_alarm_clock != new_alarm_clock)
			{
				Debug("new alarm clock: %d\n", new_alarm_clock);
				
				//设置新的闹钟
				lv_obj_t* obj = lv_obj_get_child(list_alarm, index);
				obj = lv_obj_get_child(obj, 1);
				lv_label_set_text_fmt(obj, "%d - %d : %d", index, new_alarm_clock/100, new_alarm_clock%100);
				
				//写入配置
				device_cfg.alarm_list[index] = new_alarm_clock;
				device_config_write();
			}
			else
			{
				Debug("alarm clock hold\n");
			}
			
			//删除弹窗
			lv_group_focus_obj(list_alarm);
			lv_obj_del_async(time_cont);
			
		}
		else
		{
			if (index >= 5) return;

			const char* txt = lv_btnmatrix_get_btn_text(obj, id);
			time_txt[index] = *txt;

			if (index == 1) time_txt[++index] = ':';
			index++;
			
			lv_label_set_text(label_time, time_txt);
		}

	}
}

static void lv_time_btnmx_create(lv_obj_t* parent, uint8_t id)
{
	time_cont = lv_obj_create(parent);
	lv_gridnav_add(time_cont, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), time_cont);
	lv_obj_set_size(time_cont, LV_PCT(90), LV_SIZE_CONTENT);
	lv_obj_set_style_pad_all(time_cont, 5, LV_PART_MAIN);
	lv_obj_align(time_cont, LV_ALIGN_BOTTOM_MID, 0, -10);
	
	label_time = lv_label_create(time_cont);
	lv_obj_set_size(label_time, LV_PCT(80), LV_SIZE_CONTENT);
	lv_obj_set_style_text_color(label_time, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_time, &lv_font_montserrat_18, 0);
	lv_obj_set_style_text_align(label_time, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(label_time, "");
	lv_obj_align(label_time, LV_ALIGN_TOP_MID, 0, 5);

	lv_obj_t* btnm1 = lv_btnmatrix_create(time_cont);
	lv_gridnav_add(btnm1, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), btnm1);
	lv_btnmatrix_set_map(btnm1, btnm_map);
	lv_btnmatrix_set_btn_width(btnm1, 10, 2);
	lv_obj_align_to(btnm1, label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	lv_obj_set_user_data(time_cont, (void*)id); //设置用户数据，保存对应的闹钟列表项索引
	memset(time_txt, 0, sizeof(time_txt));
	sprintf(time_txt, "%02d:%02d", device_cfg.alarm_list[id] / 100, device_cfg.alarm_list[id] % 100); //初始化标签文本
	lv_label_set_text(label_time, time_txt);
	
	lv_obj_add_event_cb(btnm1, event_btnmx_handler, LV_EVENT_VALUE_CHANGED, NULL);
	lv_group_focus_obj(btnm1);
}

lv_obj_t* lv_alarm_list_create(lv_obj_t* parent, lv_obj_t* re_focus_obj)
{
	lv_obj_t* label;
	
	com_parent = parent;
	re_focus = re_focus_obj;
	
	list_alarm = lv_list_create(parent);
	lv_gridnav_add(list_alarm, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), list_alarm);
	lv_obj_set_size(list_alarm, lv_pct(90), lv_pct(70));
	lv_obj_align(list_alarm, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_bg_color(list_alarm, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
	
	uint8_t list_len = sizeof(device_cfg.alarm_list) / sizeof(device_cfg.alarm_list[0]);
	char* buf = mymalloc(SRAMCCM, 32);
	if(buf != NULL) memset(buf, 0, 32);
	
	for (int i = 0; i < list_len; i++)
	{
		lv_obj_t* item = lv_list_add_btn(list_alarm, LV_SYMBOL_BELL, "");
		lv_obj_add_event_cb(item, event_key_handler, LV_EVENT_KEY, NULL);
		lv_obj_set_style_bg_opa(item, 0, 0);
		lv_group_remove_obj(item);

		label = lv_obj_get_child(item, 1);
		memset(buf, 0, 32);
		sprintf(buf, "%d - %d : %d", i, device_cfg.alarm_list[i]/100, device_cfg.alarm_list[i]%100);
		lv_label_set_text(label, buf);
	}
	
	lv_group_focus_obj(list_alarm);
	myfree(SRAMCCM, buf);
	
	return list_alarm;
}

