#ifndef _GUI_INC_DEF_H
#define _GUI_INC_DEF_H

#include "devices.h"
#include "app_default_config.h"
#include <stdio.h>
#include <stdbool.h>
#include "os_task.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_disp.h"

#include "font_symbol_def.h"

#define GUI_ASSERT while(1){HAL_Delay(1000);}

//横屏显示
#define GUI_WIDTH  LCD_H
#define GUI_HEIGHT LCD_W

#define DEL true
#define NOTDEL false

#define ANIM_LINE_ADD(a, exec_cb, path_cb, ready_cb, time, back_time, delay, var, begin, end, repeatcnt) do{\
	lv_anim_init(a);\
	lv_anim_set_exec_cb(a, exec_cb);\
	if(path_cb != NULL) lv_anim_set_path_cb(a, path_cb);\
	if(ready_cb != NULL) lv_anim_set_ready_cb(a, ready_cb);\
	lv_anim_set_time(a, time);\
	lv_anim_set_playback_time(a, back_time);\
	lv_anim_set_delay(a, delay);\
	lv_anim_set_var(a, var);\
	lv_anim_set_values(a, begin, end);\
	lv_anim_set_repeat_count(a, repeatcnt);\
}while(0);

//声明字体
LV_FONT_DECLARE(songti_zh_16);
LV_FONT_DECLARE(font_symbol_14);
LV_FONT_DECLARE(font_symbol_32)

enum PAGE_ID
{
	PAGE_NULL = -1,
	PAGE_LOGO,
	PAGE_MAIN,
	PAGE_APPS,
	PAGE_APP_BOOK,
	PAGE_APP_ARDUBOY,
	PAGE_APP_NES,
	PAGE_APP_GPS,
	PAGE_APP_CALC,
	PAGE_APP_CALENDAR,
	PAGE_APP_SETTING,
	PAGE_APP_ABOUT,
};

extern lv_style_t font_zh_full_16;

static inline void set_angle(void* obj, int32_t v)
{
	lv_arc_set_value((lv_obj_t*)obj, (int32_t)v);
}

static inline void set_img_angle(void* img, int32_t v)
{
	lv_img_set_angle(img, v);
}

static inline void anim_opa_cb(void* var, int32_t v)
{
	lv_obj_set_style_opa(var, v, LV_PART_MAIN);
}

static inline void anim_y_cb(void* var, int32_t v)
{
	lv_obj_set_y(var, v);
}

static inline void anim_x_cb(void* var, int32_t v)
{
	lv_obj_set_x(var, v);
}

static inline void gui_delay(int t)
{
	while(t--)
	{
		lv_task_handler();
		vTaskDelay(1);
	}
}

static inline void init_font_style(lv_style_t *style, lv_color_t color, const lv_font_t *font)
{
	lv_style_reset(style);
	lv_style_init(style);

	lv_style_set_text_font(style, font);
	lv_style_set_text_color(style, color);
}

static inline void lv_read_mem(uint8_t *pmem)
{
	lv_mem_monitor_t gui_mem_monitor; 
	lv_mem_monitor(&gui_mem_monitor);
	//_mem = (gui_mem_monitor.total_size - gui_mem_monitor.free_size) * 1.0f / gui_mem_monitor.total_size * 100.0f;
	
	if(!pmem)
	{
		Debug("lv_mem: %d %%\n", gui_mem_monitor.used_pct);
	}
	else
		*pmem = gui_mem_monitor.used_pct;
}

static inline void init_outline_shadow_style(lv_style_t *style, lv_opa_t opa, lv_palette_t color, uint8_t w)
{
	lv_style_reset(style);
	lv_style_init(style);

	// Set background & radius
	lv_style_set_radius(style, 5);
	lv_style_set_bg_opa(style, opa);
	lv_style_set_bg_color(style, lv_palette_lighten(LV_PALETTE_GREY, 1));

	// Add shadow
	lv_style_set_shadow_width(style, w);
	lv_style_set_shadow_color(style, lv_palette_main(color));
}

#endif
