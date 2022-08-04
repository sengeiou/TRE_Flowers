#include "gui_main.h"
#include "lvgl_test.h"
#include "player.h"
#include "vs10xx.h"

#include "fatfs.h"
#include "key.h"

#define INFO_BUFF_SIZE 32

typedef struct MainObj
{
	lv_obj_t* cont_thp;
	lv_meter_indicator_t* indic_press;
	lv_obj_t* meter_press;
	lv_obj_t* label_press;
	lv_obj_t* label_date;
	lv_obj_t* roller_hour;
	lv_obj_t* roller_min;
	lv_obj_t* roller_sec;
	lv_obj_t* label_min_dot;
	lv_obj_t* label_sec_dot;
	lv_obj_t* bar_sec;
	lv_obj_t* label_temp;
	lv_obj_t* bar_temp;
	lv_obj_t* label_humi;
	lv_obj_t* bar_humi;
}MainObj_t;

static MainObj_t* lv_objs; 
char* info_buff; //32
static lv_timer_t* lv_timer;
int screen_lock_time;
bool alarm_clock_flag;
bool alarm_clock_exit;

static void objs_anim_set(uint8_t state);

static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	page_call_handler((void*)PAGE_APPS, NOTDEL);
}

static void file_close_cb()
{
	Debug("gui main load\n");
	
}

//最好还是创建一个闹钟任务
static void alarm_clock_detect(uint16_t time)
{
	static uint16_t last_alarm_clock = 0;
	static uint8_t on_count = 0; //统计次数
	uint8_t len;
	if(!device_cfg.alarm_flag) return; //未启用闹钟
	
	if(alarm_clock_flag)
	{
		//设置蜂鸣器
		buzzer_on();
		gui_delay(60);
		buzzer_off();
		gui_delay(40);
		buzzer_on();
		gui_delay(60);
		buzzer_off();
		
		if(on_count > 128) 
		{
			on_count = 0;
			alarm_clock_flag = false; //自动关闭
		}
		on_count++;
	}
	else if(last_alarm_clock != time)
	{
		len = sizeof(device_cfg.alarm_list) / sizeof(device_cfg.alarm_list[0]);
		for(int i=0;i<len;i++)
		{
			if(device_cfg.alarm_list[i] == time)
			{
				alarm_clock_flag = true; //启动闹钟
				last_alarm_clock = time;
			}
		}
	}

}

static void screen_lock_detect()
{
	
	if(screen_lock_time > 1)
	{
		screen_lock_time--;
	}
	else
	{
		return;
	}
	
	//等待关闭屏幕背光
	if(screen_lock_time == 10)
	{
		//设置最低亮度
		LCD_SetBlk(1);
	}
	else if(screen_lock_time == 1)
	{
		//关闭背光
		LCD_SetBlk(0);
	}
	
	Debug("lock time: %d\n", screen_lock_time);
}

static void timer_task(lv_timer_t *timer)
{
	static uint16_t count = 0;
	
	//时间指示标签
	if(lv_obj_get_style_opa(lv_objs->label_sec_dot, LV_PART_MAIN) == LV_OPA_MAX)
	{
		lv_obj_set_style_opa(lv_objs->label_min_dot, LV_OPA_30, LV_PART_MAIN);
		lv_obj_set_style_opa(lv_objs->label_sec_dot, LV_OPA_30, LV_PART_MAIN);
	}
	else
	{
		lv_obj_set_style_opa(lv_objs->label_min_dot, LV_OPA_MAX, LV_PART_MAIN);
		lv_obj_set_style_opa(lv_objs->label_sec_dot, LV_OPA_MAX, LV_PART_MAIN);
	}
	
	if(count++ > 4096) count = 0;
	if(count % 2 != 0) return;
	
	//更新时间
	if(ds3231_read())
	{
		//更新时间
		lv_roller_set_selected(lv_objs->roller_hour, rtc.Hour, LV_ANIM_ON);
		lv_roller_set_selected(lv_objs->roller_min, rtc.Min, LV_ANIM_ON);
		lv_roller_set_selected(lv_objs->roller_sec, rtc.Sec, LV_ANIM_ON);
		
		lv_bar_set_start_value(lv_objs->bar_sec, 60-rtc.Sec, LV_ANIM_OFF);
		lv_bar_set_value(lv_objs->bar_sec, 60+rtc.Sec, LV_ANIM_OFF);
		
		//更新日期
		memset(info_buff, 0, INFO_BUFF_SIZE);
		sprintf(info_buff, "%04d/%02d/%02d %s", (rtc.Year + 2000), rtc.Month, rtc.Date, DS3231_weeks[rtc.DaysOfWeek-1]);
		lv_label_set_text(lv_objs->label_date, info_buff);
		
		//闹钟检测
		alarm_clock_detect(rtc.Hour*100+rtc.Min);
	}
	
	//背光关闭检测
	screen_lock_detect();
	
	//环境信息检测
	if(count % 8 == 0)
	{
		float val = 0;
		//更新大气压
		memset(info_buff, 0, INFO_BUFF_SIZE);
		if(bmp280_read())
		{
			val = (bmp280_data->press / 100.0f);
			sprintf(info_buff, "%.1f \nhPa", val);
			lv_meter_set_indicator_end_value(lv_objs->meter_press, lv_objs->indic_press, val);
			lv_label_set_text(lv_objs->label_press, info_buff);
		}
		
		//更新温湿度
		if(hdc1080_read())
		{
			float temp = (hdc1080_data->temp + bmp280_data->th.temp) * 0.5f;
			memset(info_buff, 0, INFO_BUFF_SIZE);
			sprintf(info_buff, "%.1f *C", temp);
			lv_bar_set_value(lv_objs->bar_temp, temp, LV_ANIM_ON);
			lv_label_set_text(lv_objs->label_temp, info_buff);
			
			memset(info_buff, 0, INFO_BUFF_SIZE);
			sprintf(info_buff, "%.1f %%", hdc1080_data->humi);
			lv_bar_set_value(lv_objs->bar_humi, hdc1080_data->humi, LV_ANIM_ON);
			lv_label_set_text(lv_objs->label_humi, info_buff);
		}
	}
	
}

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			if(key == LV_KEY_ENTER)
			{
//				FM_Search_Auto(SEARCH_UP);
				
				lv_timer_del(lv_timer);
				myfree(SRAMCCM, info_buff);
				
				objs_anim_set(DISABLE);
				status_bar->state_set(STATUS_BAR, DISABLE);
				
			}else if(key == LV_KEY_BACKSPACE)
			{
//				FM_State_Set(MUTE_ON);
				if(alarm_clock_flag) alarm_clock_flag = false; //关闭闹钟
			}
			else if(key == LV_KEY_HOME)
			{
				//重置背光
				screen_lock_time = device_cfg.screen_close_delay;
				LCD_SetBlk(device_cfg.lcd_level);
			}
			
		}	
			break;
	}
}

static void objs_anim_set(uint8_t state)
{
	lv_anim_t anim;
	lv_anim_t anim1;
	lv_anim_t anim2;
	
	if(state == ENABLE)
	{
		ANIM_LINE_ADD(&anim, anim_x_cb, lv_anim_path_overshoot, NULL, 300, 0, 500, lv_objs->cont_thp, -200, 5, 0);
		ANIM_LINE_ADD(&anim1, anim_x_cb, lv_anim_path_overshoot, NULL, 300, 0, 500, lv_objs->meter_press, GUI_WIDTH, GUI_WIDTH-230, 0);
		ANIM_LINE_ADD(&anim2, anim_x_cb, lv_anim_path_overshoot, NULL, 300, 0, 500, lv_objs->label_date, \
																													lv_obj_get_x(lv_objs->label_date), \
																													lv_obj_get_x(lv_objs->label_date)-25, \
																													0);
	}
	else
	{
		ANIM_LINE_ADD(&anim, anim_x_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 50, lv_objs->cont_thp, 5, -200, 0);
		ANIM_LINE_ADD(&anim1, anim_x_cb, lv_anim_path_overshoot, NULL, 300, 0, 50, lv_objs->meter_press, GUI_WIDTH-230, GUI_WIDTH, 0);
		ANIM_LINE_ADD(&anim2, anim_x_cb, lv_anim_path_overshoot, NULL, 300, 0, 50, lv_objs->label_date, \
																													lv_obj_get_x(lv_objs->label_date), \
																													lv_obj_get_x(lv_objs->label_date)+25, \
																													0);
	}
	
	lv_anim_start(&anim);
	lv_anim_start(&anim1);
	lv_anim_start(&anim2);
}

void gui_main_init(lv_obj_t* root)
{	
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	//申请控件内存
	lv_objs = mymalloc(SRAMCCM, sizeof(MainObj_t));
	if(lv_objs != NULL) memset(lv_objs, 0, sizeof(MainObj_t));
	
#if 0
	
//	lv_obj_t* img = lv_img_create(page_self_root);
//	lv_img_set_src(img, "S:0.jpg");
//	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
	
#endif

	//时间容器
	lv_obj_t* cont_time_row = lv_obj_create(page_self_root);
	lv_obj_set_size(cont_time_row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(cont_time_row, LV_ALIGN_CENTER, 0, -35);
	lv_obj_set_flex_flow(cont_time_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont_time_row, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(cont_time_row, 5, 0);
	lv_obj_set_style_border_width(cont_time_row, 0, 0);
	lv_obj_set_style_shadow_width(cont_time_row, 20, 0);
	lv_obj_set_style_shadow_color(cont_time_row, lv_palette_main(LV_PALETTE_BLUE), 0);

	static lv_style_t style_main;
	static lv_style_t style_select;
	lv_style_init(&style_main);
	lv_style_init(&style_select);

	lv_style_set_width(&style_main, 58);
	lv_style_set_height(&style_main, 58);
	lv_style_set_border_width(&style_main, 0);
	lv_style_set_pad_all(&style_main, 0);
	lv_style_set_anim_time(&style_main, 100);
	lv_style_set_text_font(&style_main, &lv_font_montserrat_46);

	lv_style_set_bg_color(&style_select, lv_color_white());
	lv_style_set_text_color(&style_select, lv_palette_main(LV_PALETTE_BLUE));
	//lv_style_set_bg_opa(&style_select, LV_OPA_TRANSP);

	lv_objs->roller_hour = lv_roller_create(cont_time_row);
	lv_obj_add_style(lv_objs->roller_hour, &style_main, LV_PART_MAIN);
	lv_obj_add_style(lv_objs->roller_hour, &style_select, LV_PART_SELECTED);

	char* time_buf = (char*)mymalloc(SRAMCCM, 256);
	int index = 0;
	if (time_buf == NULL) return;
	memset(time_buf, 0, 256);

	for (int i = 0; i < 24; i++)
	{
		index = (i * 3);
		sprintf(time_buf + index, "%02d\n", i);
	}

	lv_roller_set_options(lv_objs->roller_hour, time_buf, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(lv_objs->roller_hour, 1);

	lv_objs->label_min_dot = lv_label_create(cont_time_row);
	lv_label_set_text(lv_objs->label_min_dot, ":");
	lv_obj_set_style_opa(lv_objs->label_min_dot, LV_OPA_MAX, LV_PART_MAIN);
	lv_obj_set_style_text_font(lv_objs->label_min_dot, &lv_font_montserrat_46, 0);

	index = 0;
	memset(time_buf, 0, 256);
	for (int i = 0; i < 60; i++)
	{
		index = (i * 3);
		sprintf(time_buf + index, "%02d\n", i);
	}


	lv_objs->roller_min = lv_roller_create(cont_time_row);
	lv_obj_add_style(lv_objs->roller_min, &style_main, LV_PART_MAIN);
	lv_obj_add_style(lv_objs->roller_min, &style_select, LV_PART_SELECTED);

	lv_roller_set_options(lv_objs->roller_min, time_buf, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(lv_objs->roller_min, 1);
	lv_roller_set_selected(lv_objs->roller_min, 0, LV_ANIM_ON);

	lv_objs->label_sec_dot = lv_label_create(cont_time_row);
	lv_label_set_text(lv_objs->label_sec_dot, ":");
	lv_obj_set_style_opa(lv_objs->label_sec_dot, LV_OPA_MAX, LV_PART_MAIN);
	lv_obj_set_style_text_font(lv_objs->label_sec_dot, &lv_font_montserrat_46, 0);

	lv_objs->roller_sec = lv_roller_create(cont_time_row);
	lv_obj_add_style(lv_objs->roller_sec, &style_main, LV_PART_MAIN);
	lv_obj_add_style(lv_objs->roller_sec, &style_select, LV_PART_SELECTED);

	lv_roller_set_options(lv_objs->roller_sec, time_buf, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(lv_objs->roller_sec, 1);
	lv_roller_set_selected(lv_objs->roller_sec, 0, LV_ANIM_ON);

	//秒钟进度条
	lv_objs->bar_sec = lv_bar_create(page_self_root);
	//lv_obj_remove_style_all(sec_bar);
	lv_obj_set_style_border_color(lv_objs->bar_sec, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_border_width(lv_objs->bar_sec, 1, LV_PART_MAIN);
	lv_obj_set_style_pad_all(lv_objs->bar_sec, 3, LV_PART_MAIN);
	lv_obj_set_style_anim_time(lv_objs->bar_sec, 200, LV_PART_MAIN);
	lv_obj_set_style_bg_color(lv_objs->bar_sec, lv_color_white(), LV_PART_MAIN);

	lv_obj_set_style_bg_opa(lv_objs->bar_sec, LV_OPA_COVER, LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(lv_objs->bar_sec, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

	lv_obj_set_size(lv_objs->bar_sec, 150, 10);
	lv_bar_set_mode(lv_objs->bar_sec, LV_BAR_MODE_RANGE);
	lv_bar_set_range(lv_objs->bar_sec, 0, 120); // <--↑-->
	lv_obj_align_to(lv_objs->bar_sec, cont_time_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	lv_bar_set_start_value(lv_objs->bar_sec, 59, LV_ANIM_OFF);
	lv_bar_set_value(lv_objs->bar_sec, 60, LV_ANIM_OFF);

	myfree(SRAMCCM, time_buf);

	//日期
	lv_objs->label_date = lv_label_create(page_self_root);
	lv_label_set_text(lv_objs->label_date, "2000/00/00  MON.");
	lv_obj_set_style_text_font(lv_objs->label_date, &lv_font_montserrat_18, 0);
	lv_obj_set_style_text_color(lv_objs->label_date, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align_to(lv_objs->label_date, lv_objs->bar_sec, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	//温湿度，大气压
	static lv_style_t style_indic;

	lv_style_init(&style_indic);
	lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
	lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);

	lv_objs->cont_thp = lv_obj_create(page_self_root);
	lv_obj_set_size(lv_objs->cont_thp, 160, 64);
	lv_obj_align(lv_objs->cont_thp, LV_ALIGN_BOTTOM_LEFT, 2, 0);
	lv_obj_set_style_pad_all(lv_objs->cont_thp, 5, 0);
	//lv_obj_set_style_border_width(cont_thp, 0, 0);

	//{温度}
	lv_objs->label_temp = lv_label_create(lv_objs->cont_thp);
	lv_label_set_text(lv_objs->label_temp, "0.0 *C");
	lv_obj_set_width(lv_objs->label_temp, 60);
	lv_obj_set_style_text_font(lv_objs->label_temp, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_color(lv_objs->label_temp, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_set_style_pad_top(lv_objs->label_temp, 6, 0);

	lv_objs->bar_temp = lv_bar_create(lv_objs->cont_thp);
	lv_obj_add_style(lv_objs->bar_temp, &style_indic, LV_PART_INDICATOR);
	lv_obj_set_size(lv_objs->bar_temp, 80, 15);
	lv_bar_set_range(lv_objs->bar_temp, -10, 100);
	lv_obj_align_to(lv_objs->bar_temp, lv_objs->label_temp, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_bar_set_value(lv_objs->bar_sec, -10, LV_ANIM_ON);

	//{湿度}
	lv_objs->label_humi = lv_label_create(lv_objs->cont_thp);
	lv_label_set_text(lv_objs->label_humi, "0.0 %");
	lv_obj_set_width(lv_objs->label_humi, 60);
	lv_obj_set_style_text_font(lv_objs->label_humi, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_color(lv_objs->label_humi, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align_to(lv_objs->label_humi, lv_objs->label_temp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->bar_humi = lv_bar_create(lv_objs->cont_thp);
	lv_obj_add_style(lv_objs->bar_humi, &style_indic, LV_PART_INDICATOR);
	lv_obj_set_size(lv_objs->bar_humi, 80, 15);
	lv_bar_set_range(lv_objs->bar_humi, 0, 100);
	lv_obj_align_to(lv_objs->bar_humi, lv_objs->label_humi, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

	//{大气压}
	lv_objs->meter_press = lv_meter_create(page_self_root);
	lv_obj_set_size(lv_objs->meter_press, 200, 200);
	lv_obj_align(lv_objs->meter_press, LV_ALIGN_BOTTOM_RIGHT, 90, 90);
	lv_obj_remove_style(lv_objs->meter_press, NULL, LV_PART_INDICATOR);

	lv_meter_scale_t* scale = lv_meter_add_scale(lv_objs->meter_press);
	lv_meter_set_scale_ticks(lv_objs->meter_press, scale, 8, 2, 10, lv_palette_main(LV_PALETTE_GREY));
	//lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 20, lv_color_hex3(0xeee), 15); //去掉文本
	lv_meter_set_scale_range(lv_objs->meter_press, scale, 0, 1500, 95, 175);

	lv_objs->indic_press = lv_meter_add_arc(lv_objs->meter_press, scale, 15, lv_palette_main(LV_PALETTE_GREEN), 0);
	lv_meter_set_indicator_end_value(lv_objs->meter_press, lv_objs->indic_press, 0);

	lv_objs->label_press = lv_label_create(lv_objs->meter_press);
	lv_label_set_text(lv_objs->label_press, "0.0 \nhPa");
	lv_obj_set_style_text_align(lv_objs->label_press, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(lv_objs->label_press, &lv_font_montserrat_18, 0);
	lv_obj_set_style_text_color(lv_objs->label_press, lv_palette_main(LV_PALETTE_GREEN), 0);
	lv_obj_align(lv_objs->label_press, LV_ALIGN_LEFT_MID, 35, -20);
	
}

void gui_main_focus(void)
{
	lv_group_focus_obj(page_self_root);
	
	file_manager->close_cb = file_close_cb;
	file_manager->display(DISABLE);
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, DISABLE);
	status_bar->title_set("");
	
	objs_anim_set(ENABLE);
	
	info_buff = mymalloc(SRAMCCM, INFO_BUFF_SIZE);
	if(info_buff != NULL) memset(info_buff, 0, INFO_BUFF_SIZE);
	
	screen_lock_time = device_cfg.screen_close_delay; //屏幕背光关闭时间
	lv_timer = lv_timer_create(timer_task, 500, NULL); //定时器更新
}



