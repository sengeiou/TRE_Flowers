#include "com_gps_tabview.h"
#include "canvas_helper.h"
#include "atgm336h.h"
#include "malloc.h"
#include <math.h>
#include "utc.h"
#include "com_msgbox.h"

//canvas绘图参数
#define CANVAS_MAP_W			(220)
#define CANVAS_MAP_H			(220)
#define CANVAS_MAP_WHF		(110)
#define CANVAS_MAP_HHF		(110)
#define CANVAS_CIRCLE_R		(110-5)
#define CANVAS_BUFF_SIZE	(CANVAS_MAP_W * CANVAS_MAP_H * sizeof(lv_color_t))
	
#define PI 3.1415926

enum
{
	SAT_PREV=0,
	SAT_NEXT
};

typedef struct 
{
	uint8_t degree;
	double minute;
	float sec;
}Pos_t;

struct OBJS
{
	uint16_t label_date:1;								//0
	uint16_t label_utc_t:1; 							//1
	uint16_t label_longitude:2;    				//2
	uint16_t label_latitude:2;						//3
	uint16_t label_sat_use:3;							//4
	uint16_t label_geoidal_sep:3;					//5
	uint16_t label_now_t:4;								//6
	uint16_t label_speed_over_ground:3; 	//7
	uint16_t btn_sync_t:4;								//8
}objs;

GpsTable_t *gps_table;
lv_color_t* canvas_map_buf;
lv_timer_t* lv_timer;
NMEA_data* gps_data;

int sat_index_select = 0;

lv_obj_t* tabview;
//lv_obj_t* tab1;
//lv_obj_t* tab2;
lv_obj_t* canvas_map;
lv_obj_t* label_sat_info;

NmeaTime_t local_time; //本地时间
NmeaTime_t utc_data; //utc时间

Pos_t longitude_data; //经度
Pos_t latitude_data; //纬度

/*********************************************************************/
static void tab_satellite_map_update();
static void tab_satellite_map_init(lv_obj_t* parent);

/*********************************************************************/
static void msgbox_event_cb(uint8_t index, uint8_t type)
{	
	switch(index)
	{
		case MSGBOX_OK:
			if(type == 0)
			{
				//本地时间校准
				Debug("%04d/%02d/%02d %02d:%02d:%02d %d\n", \
								local_time.year,\
								local_time.month,\
								local_time.day,\
								local_time.hour,\
								local_time.min,\
								local_time.sec,
								lv_calendar_get_week(local_time.year, local_time.month, local_time.day));
				
				//设置时间
				rtc.Year = local_time.year - 2000;
				rtc.Month = local_time.month;
				rtc.Date = local_time.day;
				rtc.Hour = local_time.hour;
				rtc.Min = local_time.min;
				rtc.Sec = local_time.sec;
				rtc.DaysOfWeek = lv_calendar_get_week(local_time.year, local_time.month, local_time.day)+1; //1-7
				
				DS3231_SetTime(&rtc);
			}
			break;
		case MSGBOX_CANCEL:
			break;
	}
}

static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	
	if(!(gps_table->is_show))
	{
		//关闭接收任务
		atgm336_stop();
		
		//释放内存
		lv_timer_del(lv_timer);
		myfree(SRAMCCM, gps_table);
		myfree(SRAMEX, canvas_map_buf);
		memset(&utc_data, 0, sizeof(NmeaTime_t));
		memset(&longitude_data, 0, sizeof(Pos_t));
		memset(&latitude_data, 0, sizeof(Pos_t));
		
		//执行回调
		if(gps_table->gps_close_cb != NULL) gps_table->gps_close_cb();
	}
	else
	{
		//开启接收任务
		atgm336_start();
	}
}

/**
 *@查找控件
 *@parem: uint8_t id
 *@return: lv_obj_t*
 */
static lv_obj_t* find_obj(uint8_t id)
{
	lv_obj_t* tab_cont = lv_obj_get_child(lv_tabview_get_content(tabview), lv_tabview_get_tab_act(tabview));
	return lv_obj_get_child(tab_cont, id);
}

/**
 *@更新选中卫星信息
 *@parem:
 *@return:
 */
static void sat_info_update()
{
	GSV_data* gsv = list_at(nmea_data.gsv_list, sat_index_select)->val;
	if(gsv == NULL) return;
	if(gsv->type == 0) //GPS
		lv_obj_set_style_text_color(label_sat_info, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	else //BD
		lv_obj_set_style_text_color(label_sat_info, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
		
	lv_label_set_text_fmt(label_sat_info, "%s\nPRN:%d\nSNR:%d\nElev:%d\nAzi:%d", \
																					gsv->type == 0 ? "GPS" : "BD", \
																					gsv->prn, \
																					gsv->snr, \
																					gsv->elev, \
																					gsv->azi);
}

/**
 *@绘制星位图背景
 *@parem:
 *@return:
 */
static void sat_draw_background()
{
	canvas_clear(canvas_map, lv_color_white());
	
	canvas_draw_fv_line(canvas_map, CANVAS_MAP_HHF, 0, CANVAS_MAP_H, lv_palette_main(LV_PALETTE_GREY));
	canvas_draw_fh_line(canvas_map, 0, CANVAS_MAP_HHF, CANVAS_MAP_H, lv_palette_main(LV_PALETTE_GREY));
	
	//简单画一下等分线
	canvas_draw_circle(canvas_map, CANVAS_MAP_HHF, CANVAS_MAP_HHF, CANVAS_CIRCLE_R, lv_palette_main(LV_PALETTE_BLUE)); 
	canvas_draw_circle(canvas_map, CANVAS_MAP_HHF, CANVAS_MAP_HHF, CANVAS_CIRCLE_R - 35, lv_palette_main(LV_PALETTE_BLUE));
	canvas_draw_circle(canvas_map, CANVAS_MAP_HHF, CANVAS_MAP_HHF, CANVAS_CIRCLE_R - 70, lv_palette_main(LV_PALETTE_BLUE));
}

/**
 *@绘制卫星位置
 *@parem:
 *@return:
 */
static void sat_draw_position()
{
	GSV_data* gsv;
	lv_color_t color;
	int x;
	int y;
	float len; //卫星投影点到圆心的距离
	
	for(int i=0;i<nmea_data.gsv_list->len;i++)
	{
		if(nmea_data.sat_in_use <= 0)
		{
			sat_draw_background();
			lv_label_set_text(label_sat_info, "NULL");
			break;
		}
		gsv = list_at(nmea_data.gsv_list, i)->val;
		//Debug("%d-%d: %d,%d; ", gsv->type, gsv->prn, gsv->elev, gsv->azi);

		#if 0
		gsv->elev = 35;
		gsv->azi = 33;
		#endif
		
		len = cos((gsv->elev * 1.0f * PI) / 180.0f)* CANVAS_CIRCLE_R;
		x = sin((gsv->azi * 1.0f * PI) / 180.0f) * len;
		y = -cos((gsv->azi * 1.0f * PI) / 180.0f) * len;
		
		//Debug("x,y: %d,%d\n", x, y);
		
		x += CANVAS_MAP_WHF; //加上原点相对屏幕边缘偏移
		y += CANVAS_MAP_HHF;
		
		if(gsv->type == 0)
			color = lv_palette_main(LV_PALETTE_BLUE);
		else
			color = lv_palette_main(LV_PALETTE_RED);
		
		canvas_fill_circle(canvas_map, x, y, 3, color);
		if(i == sat_index_select)
		{
			canvas_draw_circle(canvas_map, x, y, 6, lv_color_black());
			sat_info_update();
		}
	}
	
}

/**
 *@当前卫星信息显示切换
 *@parem: uint8_t dir
 *@return:
 */
static void sat_select(uint8_t dir)
{
	if(nmea_data.sat_in_use <= 0) return;
	
	if(dir == SAT_PREV)
	{
		sat_index_select = LV_MAX(sat_index_select--, 0);
	}
	else
	{
		sat_index_select++;
		if(sat_index_select >= nmea_data.gsv_list->len)
		{
			sat_index_select = 0;
		}
	}
	
	sat_info_update();
}

/**
 *@更新星位图
 *@parem:
 *@return:
 */
static void tab_satellite_map_update()
{
	if(canvas_map == NULL || gps_data == NULL) return;
	sat_draw_background();
	sat_draw_position();
	
}

/**
 *@更新定位信息
 *@parem:
 *@return:
 */
static void sat_data_update()
{
#if 0
	Debug("%02d/%02d/%02d\n", utc_data.year, utc_data.month, utc_data.day);
	Debug("%02d:%02d:%02d\n", utc_data.hour, utc_data.min, utc_data.sec);
	Debug("latitude/longitude: %f/%f\n", nmea_data.latitude, nmea_data.longitude);
#endif
	
	char* buf = NULL;
	uint8_t buf_len = 128;
	lv_obj_t* obj;
	buf = mymalloc(SRAMCCM, buf_len);
	if(buf == NULL) return;
	
	do{
		//Date
		obj = find_obj(objs.label_date);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Date:\n%02d/%02d/%02d", utc_data.year, utc_data.month, utc_data.day);
		lv_label_set_text(obj, buf);
		
		//UTC time
		obj = find_obj(objs.label_utc_t);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "UTC Time:\n%02d:%02d:%02d", utc_data.hour, utc_data.min, utc_data.sec);
		lv_label_set_text(obj, buf);
		
		//Now Time
		obj = find_obj(objs.label_now_t);
		if(obj == NULL) break;
		if(!ds3231_read()) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Now Time(RTC):\n%04d:/%02d/%02d\n%02d:%02d:%02d", \
																							rtc.Year+2000, \
																							rtc.Month,\
																							rtc.Date,\
																							rtc.Hour,\
																							rtc.Min,\
																							rtc.Sec);
		lv_label_set_text(obj, buf);	
		
		//Longitude
		obj = find_obj(objs.label_longitude);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Longitude:\n%c %d*%d'%.3f''", nmea_data.longitude_direction, \
																							longitude_data.degree, \
																							(int)longitude_data.minute,\
																							longitude_data.sec);
		lv_label_set_text(obj, buf);
		
		//Latitude
		obj = find_obj(objs.label_latitude);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Latitude:\n%c %d*%d'%.3f''", nmea_data.latitude_direction, \
																						 latitude_data.degree, \
																						 (int)latitude_data.minute, \
																						 latitude_data.sec);
		lv_label_set_text(obj, buf);
		
		//Sat in use
		obj = find_obj(objs.label_sat_use);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Sat use:\n%d", nmea_data.sat_in_use);
		lv_label_set_text(obj, buf);
		
		//Altitude
		obj = find_obj(objs.label_geoidal_sep);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Altitude:\n%.1f m", nmea_data.altitude);
		lv_label_set_text(obj, buf);

#if 0
		//Speed over ground
		obj = find_obj(objs.label_geoidal_sep);
		if(obj == NULL) break;
		memset(buf, 0, buf_len);
		sprintf(buf, "Speed over ground:\n%.1f km/h", nmea_data.speed_kmph);
		lv_label_set_text(obj, buf);	
#endif		
		
	}while(0);

	myfree(SRAMCCM, buf);
}

static void gps_time_task(lv_timer_t *timer)
{
	gps_data = get_gps_data();
	
	if(gps_data != NULL)
	{
		
		uint16_t tab_id = lv_tabview_get_tab_act(tabview);
		
		if(tab_id == 0)
		{
			//计算UTC时间
			utc_data.year = nmea_data.UT_date % 100;
			utc_data.month = (nmea_data.UT_date / 100) % 100;
			utc_data.day = (nmea_data.UT_date / 10000);
			utc_data.hour = (nmea_data.UTC_time / 10000);
			utc_data.min = (int)(nmea_data.UTC_time / 100) % 100;
			utc_data.sec = (int)(nmea_data.UTC_time) % 100;
			
			//转为本地时间
			local_time = UTCToBeijing(utc_data);
			local_time.year += 2000;
			
			//计算经纬度(度*分*秒)
			longitude_data.degree = nmea_data.longitude / 100;
			longitude_data.sec = modf(nmea_data.longitude, &longitude_data.minute) * 60.0f;
			longitude_data.minute = (int)longitude_data.minute % 100;
			
			latitude_data.degree = nmea_data.latitude / 100;
			latitude_data.sec = modf(nmea_data.latitude, &latitude_data.minute) * 60.0f;
			latitude_data.minute = (int)latitude_data.minute % 100;
		
			
			//更新信息
			sat_data_update();
		}
		else
		{
			tab_satellite_map_update();
		}
		
#if 0
		Debug("time/date: %f/%d\n", nmea_data.UTC_time, nmea_data.UT_date);
		Debug("latitude/longitude: %f/%f\n", nmea_data.latitude, nmea_data.longitude);
		Debug("altitude/speed_kmph: %.2f, %.2f\n", nmea_data.altitude, nmea_data.speed_kmph);
		Debug("speed_direction/speed_knots: %.2f, %.2f\n",nmea_data.speed_direction, nmea_data.speed_knots);
		Debug("HDOP/VDOP: %.2f, %.2f\n", nmea_data.HDOP, nmea_data.VDOP);
		Debug("sat_in_view_gp: %d\r\n", nmea_data.sat_in_view_gp);
		Debug("sat_in_view_bd: %d\r\n", nmea_data.sat_in_view_bd);
		Debug("gsv_len: %d\n", nmea_data.gsv_list->len);
		Debug("sat_in_use: %d\r\n", nmea_data.sat_in_use);
		
		for(int i=0;i<nmea_data.gsv_list->len;i++)
		{
			GSV_data* gsv = list_at(nmea_data.gsv_list, i)->val;
			Debug("%d-%d: %d,%d\n", gsv->type, gsv->prn, gsv->elev, gsv->azi);
		}
#endif
		
	}
	else
	{
		Debug("gps data is null\n");
	}
}

static void gps_table_display(uint8_t state)
{
	lv_anim_t anim;
	if(gps_table->gps_root == NULL) return;
	
	if(state)
	{
		if(gps_table->is_show) return;
		lv_group_focus_obj(tabview);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 200, gps_table->gps_root, GUI_HEIGHT, 0, 0);
	}
	else
	{
		if(!(gps_table->is_show)) return;
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 50, gps_table->gps_root, 0, GUI_HEIGHT, 0);
	}
	
	gps_table->is_show = state;
	lv_anim_start(&anim);
}

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
				Debug("btn click\n");
				//时间同步确认弹窗
				MsgParam_t param = {
					.title = "Time sync",
					.info = "Do you want to synchronize time?",
					.msg_type = 0,
					.re_focus_obj = tabview,
				};
				lv_mymsgbox_create(&param, msgbox_event_cb);
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				gps_table->gps_display(DISABLE);
			}
			else if(key == LV_KEY_LEFT)
			{
				sat_select(SAT_PREV);
			}
			else if(key == LV_KEY_RIGHT)
			{
				sat_select(SAT_NEXT);
			}
			else if(key == LV_KEY_HOME)
			{
				lv_tabview_set_act(tabview, 0, LV_ANIM_ON);
			}
			else if(key == LV_KEY_END)
			{
				sat_draw_background(); 
				lv_tabview_set_act(tabview, 1, LV_ANIM_ON);
			}
			
		}
			break;
	}
}

static void tab_satellite_map_init(lv_obj_t* parent)
{
	canvas_map = lv_canvas_create(parent);
	lv_obj_align(canvas_map, LV_ALIGN_LEFT_MID, 0, 0);
	
	label_sat_info = lv_label_create(parent);
	lv_obj_set_style_text_color(label_sat_info, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_sat_info, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label_sat_info, "NULL");
	lv_obj_align(label_sat_info, LV_ALIGN_RIGHT_MID, -20, 0);
	
	canvas_map_buf = mymalloc(SRAMEX, CANVAS_BUFF_SIZE);
	if(canvas_map_buf == NULL) return;
	memset(canvas_map_buf, 0, CANVAS_BUFF_SIZE);
		
	lv_canvas_set_buffer(canvas_map, canvas_map_buf, CANVAS_MAP_W, CANVAS_MAP_H, LV_IMG_CF_TRUE_COLOR);
	lv_canvas_fill_bg(canvas_map, lv_color_white(), LV_OPA_COVER);
	
	tab_satellite_map_update();
}

static void tab_satellite_data_init(lv_obj_t* parent)
{
	lv_obj_t* label_obj;

	//Date
	label_obj = lv_label_create(parent);
	objs.label_date = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Date:\n00/00/00");
	lv_obj_align(label_obj, LV_ALIGN_LEFT_MID, 20, -90);

	//UTC Time
	label_obj = lv_label_create(parent);
	objs.label_utc_t = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "UTC Time:\n00:00:00");
	lv_obj_align(label_obj, LV_ALIGN_LEFT_MID, 20, -48);

	//Longitude
	label_obj = lv_label_create(parent);
	objs.label_longitude = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Longitude:\n 0*0'0''");
	lv_obj_align(label_obj, LV_ALIGN_LEFT_MID, 20, 0);

	//Latitude
	label_obj = lv_label_create(parent);
	objs.label_latitude = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Latitude:\n 0*0'0''");
	lv_obj_align(label_obj, LV_ALIGN_LEFT_MID, 20, 48);

	//Sat in use
	label_obj = lv_label_create(parent);
	objs.label_sat_use = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Sat use:\n0");
	lv_obj_align(label_obj, LV_ALIGN_LEFT_MID, 20, 90);

	//Altitude
	label_obj = lv_label_create(parent);
	objs.label_geoidal_sep = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Altitude:\n0.0 m");
	lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
	lv_obj_align(label_obj, LV_ALIGN_RIGHT_MID, -20, -90);

	//Now Time
	label_obj = lv_label_create(parent);
	objs.label_now_t = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Now time:\n00:00:00");
	lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
	lv_obj_align(label_obj, LV_ALIGN_RIGHT_MID, -20, -48);
	
#if 0
	//Speed over ground
	label_obj = lv_label_create(parent);
	objs.label_speed_over_ground = lv_obj_get_child_id(label_obj);
	objs.label_geoidal_sep = lv_obj_get_child_id(label_obj);
	lv_label_set_text(label_obj, "Speed over ground:\n0.0 km/h");
	lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
	lv_obj_align(label_obj, LV_ALIGN_RIGHT_MID, -20, 0);
#endif

	//Time Sync
	lv_obj_t* btn_time_sync = lv_btn_create(parent);
	objs.btn_sync_t = lv_obj_get_child_id(label_obj);
	lv_obj_align(btn_time_sync, LV_ALIGN_RIGHT_MID, -20, 0);

	label_obj = lv_label_create(btn_time_sync);
	lv_label_set_text(label_obj, "Time Sync");
	lv_obj_center(label_obj);
}

static void gps_table_init(lv_obj_t* root)
{
	gps_table->gps_root = lv_obj_create(root);
	lv_obj_set_size(gps_table->gps_root, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_pad_all(gps_table->gps_root, 0, 0);
	lv_obj_set_style_border_opa(gps_table->gps_root, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align(gps_table->gps_root, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_clear_flag(gps_table->gps_root, LV_OBJ_FLAG_SCROLLABLE);
	
	tabview = lv_tabview_create(gps_table->gps_root, LV_DIR_TOP, 2);
	lv_group_add_obj(lv_group_get_default(), tabview);
	lv_obj_add_event_cb(tabview, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(tabview, lv_color_white(), LV_PART_MAIN);
	lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 0);
	
	//TAB1
	lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "-");
	lv_obj_clear_flag(tab1, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(tab1, 0, 0);
	
	//TAB2
	lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "-");
	lv_obj_clear_flag(tab2, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(tab2, 0, 0);
	
	tab_satellite_data_init(tab1);
	tab_satellite_map_init(tab2);
	
	lv_timer = lv_timer_create(gps_time_task, 1000, NULL); //定时读取GPS信息更新
}

GpsTable_t* gps_table_create(lv_obj_t* parent)
{
	gps_table = (GpsTable_t*)mymalloc(SRAMCCM, sizeof(GpsTable_t));
	memset(gps_table, 0, sizeof(GpsTable_t));
	gps_table->gps_display = gps_table_display;
	
	gps_table_init(parent);
	
	return gps_table;
}

