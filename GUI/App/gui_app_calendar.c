#include "gui_app_calendar.h"

lv_obj_t* calendar;
lv_obj_t* btn_matrix;
lv_obj_t* btn_prev;
lv_obj_t* btn_next;
static lv_obj_t* label_date;

/*************************************************/
static void select_date_refresh(void);
static void today_date_refresh(void);

/*************************************************/
static void status_bar_hidecb()
{
	page_call_handler((void*)(PAGE_APPS), DEL);
}

static void local_date_set()
{
	if(ds3231_read())
	{
		lv_calendar_date_t lv_today;
		
		lv_today.year = (rtc.Year + 2000);
		lv_today.month = rtc.Month;
		lv_today.day = rtc.Date;
		
		//设置本地日期
		lv_calendar_set_today_date(calendar, lv_today.year, lv_today.month , lv_today.day);
		lv_calendar_set_showed_date(calendar, lv_today.year, lv_today.month);
		
		buzzer_on();
		gui_delay(200);
		buzzer_off();

		today_date_refresh();
	}
}

static void date_init_task(lv_timer_t *timer)
{
	local_date_set();
}

static void today_date_refresh()
{
	char str[32];
	const lv_calendar_date_t* today = lv_calendar_get_today_date(calendar);
	sprintf(str, "%04d/%02d/%02d", today->year, today->month, today->day);
	lv_label_set_text(label_date, str);
}

static void select_date_refresh()
{
	lv_calendar_date_t date;
	char str[32];
	
	if(lv_calendar_get_pressed_date(calendar, &date)) {
		sprintf(str, "%04d/%02d/%02d", date.year, date.month, date.day);
		lv_label_set_text(label_date, str);
	}
}

static void event_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			if(key == LV_KEY_ENTER)
			{
				local_date_set();
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				status_bar->state_set(STATUS_BAR, DISABLE);
			}
			else if(key == LV_KEY_HOME)
			{
				lv_event_send(btn_prev, LV_EVENT_CLICKED, NULL);
			}
			else if(key == LV_KEY_END)
			{
				lv_event_send(btn_next, LV_EVENT_CLICKED, NULL);
			}
			else
			{
				select_date_refresh();
			}
		}	
			break;
		case LV_EVENT_PRESSED:
			select_date_refresh();
		break;
	}
}

void gui_calendar_init(lv_obj_t* root)
{
	page_self_root = root;
	
	calendar = lv_calendar_create(page_self_root);
	lv_group_add_obj(lv_group_get_default(), calendar);
	lv_obj_set_size(calendar, LV_PCT(95), 185);
	lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 5);

	//LV_USE_CALENDAR_HEADER_ARROW (Enable)
	lv_calendar_header_arrow_create(calendar);
	
	//日期按钮组
	btn_matrix = lv_obj_get_child(calendar, 1);
	lv_obj_add_event_cb(btn_matrix, event_handler, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(btn_matrix, event_handler, LV_EVENT_PRESSED, NULL);
	
	//月份按钮
	lv_obj_t* header = lv_obj_get_child(calendar, 0);
	lv_obj_t* header_label = lv_obj_get_child(header, 1);
	btn_prev = lv_obj_get_child(header, 0);
	btn_next = lv_obj_get_child(header, 2);
	
	//当前日期
	label_date = lv_label_create(page_self_root);
	lv_obj_set_size(label_date, LV_PCT(90), LV_SIZE_CONTENT);
	lv_obj_set_style_text_align(label_date, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_long_mode(label_date, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_align(label_date, LV_ALIGN_BOTTOM_MID, 0, -2);
	lv_label_set_text(label_date, "2022/01/01");
}

void gui_calendar_focus(void)
{
	lv_group_focus_obj(btn_matrix);
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = status_bar_hidecb;
	status_bar->title_set(app_names[PAGE_APP_CALENDAR - 3]);
	
	lv_timer_t * timer = lv_timer_create_basic();
	lv_timer_set_cb(timer, date_init_task);
	lv_timer_set_repeat_count(timer, 1);
}





