#include "com_status_bar.h"
#include "malloc.h"

#define BAT_MAX 4.2F
#define BAT_MIN 2.8F
#define BAT_AVAILABLE (BAT_MAX - BAT_MIN)
#define BAT_PCT_MIN 10 //低于10%报警

StatusBar_t *statusbar = NULL;
uint8_t time_update = DISABLE;

static struct OBJS
{
	uint16_t time_id:1;
	uint16_t bat_id:1;
	uint16_t alarm_id:2;
	uint16_t sound_id:2;
	uint16_t title_id:3;
}objs;

static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	if(statusbar->hide_cb != NULL && !(statusbar->is_show)) statusbar->hide_cb();
}

static void statusbar_set_state(uint8_t type, uint8_t state)
{
	lv_obj_t *obj;
	lv_anim_t anim;
	switch(type)
	{
		case STATUS_BAR:
			obj = statusbar->bar;
			if(state == ENABLE)
			{
				if(statusbar->is_show) break;
				ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 500, statusbar->bar, -32, 0, 0);
			}
			else
			{
				if(!(statusbar->is_show)) break;
				ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 200, statusbar->bar, 0, -32, 0);
			}
			
			statusbar->is_show = state;
			lv_anim_start(&anim);
			
			break;
		case ALARM:
			obj = lv_obj_get_child(statusbar->bar, objs.alarm_id);
			
			device_cfg.alarm_flag = state;
			lv_label_set_text(obj, device_cfg.alarm_flag ? MY_SYMBOL_ALARM_ON : MY_SYMBOL_ALARM_OFF);
			break;
		case SOUND:
			obj = lv_obj_get_child(statusbar->bar, objs.sound_id);
			
			device_cfg.sound_flag = state;
			lv_label_set_text(obj, device_cfg.sound_flag ? MY_SYMBOL_SOUND_ON : MY_SYMBOL_SOUND_OFF);
			break;
		case TIME:
			time_update = state;
			break;
	}
}

static void statusbar_set_bat(uint8_t val)
{
	lv_obj_t* obj = lv_obj_get_child(statusbar->bar, objs.bat_id);
	lv_bar_set_value(obj, val, LV_ANIM_ON);
}

static void statusbar_title_set(const char* title)
{
	lv_obj_t* obj = lv_obj_get_child(statusbar->bar, objs.title_id);
	lv_label_set_text(obj, title);
}

static void statusbar_time_task(lv_timer_t *timer)
{
	static uint8_t count = 0;
	static uint8_t low_bat_count = 0;
	
	lv_obj_t* obj = lv_obj_get_child(statusbar->bar, objs.time_id);
	
	if(time_update)
	{
		if(ds3231_read())
		{
			char buff[16];
			sprintf(buff, "%02d : %02d", rtc.Hour, rtc.Min);
			lv_label_set_text(obj, buff);
		}
	}
	else
	{
		lv_label_set_text(obj, MY_DEVICE_NAME);
	}

	if(count % 2 == 0)
	{
		//更新电量
		uint8_t bat = (ADS1110_Get_Voltage() - BAT_MIN) / BAT_AVAILABLE * 100;
		statusbar_set_bat(bat);
#if CONFIG_LOG_BAT_INFO
		Debug("battery: %.2f %d %%\n", ADS1110_Get_Voltage(), bat);
#endif
		
		//低电量提醒
		if(bat <= BAT_PCT_MIN)
		{
			buzzer_on_direct();
			gui_delay(30);
			buzzer_off();
		}
	}

	if(count++ > 200) count = 0;
}

static void statusbar_set_parent(lv_obj_t* parent)
{
	lv_obj_set_parent(statusbar->bar, parent);
	
	//重置
	statusbar->hide_cb = NULL;
}

static void statusbar_init()
{
	//状态栏
	statusbar->bar = lv_obj_create(lv_scr_act());
	lv_obj_set_size(statusbar->bar, 320-2, 30);
	lv_obj_set_style_border_width(statusbar->bar, 1, 0);
	lv_obj_set_style_pad_all(statusbar->bar, 0, 0);
	lv_obj_set_style_opa(statusbar->bar, LV_OPA_COVER, 0);
	lv_obj_set_style_border_side(statusbar->bar, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, 0);
	lv_obj_align(statusbar->bar, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_clear_flag(statusbar->bar, LV_OBJ_FLAG_SCROLLABLE); //取消滚动
	
	//时间
	lv_obj_t* label_time = lv_label_create(statusbar->bar);
	objs.time_id = lv_obj_get_child_id(label_time);
	lv_label_set_text(label_time, "08 : 53");
	lv_obj_set_size(label_time, LV_SIZE_CONTENT, 24);
	lv_obj_set_style_text_font(label_time, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_color(label_time, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align(label_time, LV_ALIGN_LEFT_MID, 5, 3);
	
	//电池电量
	lv_obj_t* bat_bar = lv_bar_create(statusbar->bar);
	objs.bat_id = lv_obj_get_child_id(bat_bar);
	//lv_obj_remove_style_all(bat_bar);
	lv_obj_set_style_border_color(bat_bar, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_border_width(bat_bar, 2, LV_PART_MAIN);
	lv_obj_set_style_pad_all(bat_bar, 3, LV_PART_MAIN);
	lv_obj_set_style_radius(bat_bar, 6, LV_PART_MAIN);
	lv_obj_set_style_anim_time(bat_bar, 200, LV_PART_MAIN);
	lv_obj_set_style_bg_color(bat_bar, lv_color_white(), LV_PART_MAIN);

	lv_obj_set_style_bg_opa(bat_bar, LV_OPA_COVER, LV_PART_INDICATOR);
	lv_obj_set_style_radius(bat_bar, 4, LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(bat_bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

	lv_obj_set_size(bat_bar, 26, 16);
	lv_bar_set_range(bat_bar, 0, 100);
	lv_obj_align(bat_bar, LV_ALIGN_RIGHT_MID, -5, 0);
	lv_bar_set_value(bat_bar, 60, LV_ANIM_ON);
	
	//闹钟
	lv_obj_t* label_alarm = lv_label_create(statusbar->bar);
	objs.alarm_id = lv_obj_get_child_id(label_alarm);
	lv_label_set_text(label_alarm, device_cfg.alarm_flag ? MY_SYMBOL_ALARM_ON : MY_SYMBOL_ALARM_OFF); //读取配置判断是否已开启闹钟
	lv_obj_set_size(label_alarm, 22, 22);
	lv_obj_set_style_text_font(label_alarm, &font_symbol_14, 0);
	lv_obj_set_style_text_color(label_alarm, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align_to(label_alarm, bat_bar, LV_ALIGN_OUT_LEFT_MID, -2, 4);
	
	//声音
	lv_obj_t* label_sound = lv_label_create(statusbar->bar);
	objs.sound_id = lv_obj_get_child_id(label_sound);
	lv_label_set_text(label_sound, device_cfg.sound_flag ? MY_SYMBOL_SOUND_ON : MY_SYMBOL_SOUND_OFF); //读取配置判断是否已开启声音
	lv_obj_set_size(label_sound, 22, 22);
	lv_obj_set_style_text_font(label_sound, &font_symbol_14, 0);
	lv_obj_set_style_text_color(label_sound, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align_to(label_sound, label_alarm, LV_ALIGN_OUT_LEFT_MID, -2, 0);
	
	//页面名称
	lv_obj_t* label_title = lv_label_create(statusbar->bar);
	objs.title_id = lv_obj_get_child_id(label_title);
	lv_label_set_text(label_title, "");
	lv_obj_set_size(label_title, LV_SIZE_CONTENT, 24);
	lv_obj_set_style_text_font(label_title, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_color(label_title, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align(label_title, LV_ALIGN_CENTER, 0, 3);
	
	lv_timer_t* lv_timer = lv_timer_create(statusbar_time_task, 1200, NULL);
	
}

StatusBar_t* status_bar_instance()
{
	if(statusbar == NULL)
	{
		statusbar = (StatusBar_t*)mymalloc(SRAMCCM, sizeof(StatusBar_t));
		memset(statusbar, 0, sizeof(StatusBar_t));
		statusbar->bat_set = statusbar_set_bat;
		statusbar->state_set = statusbar_set_state;
		statusbar->parent_set = statusbar_set_parent;
		statusbar->title_set = statusbar_title_set;
		
		statusbar_init();
	}
	
	return statusbar;
}

