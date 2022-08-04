#ifndef _CANVAS_HELPER_H
#define _CANVAS_HELPER_H

#include "lvgl.h"
#include "gui_inc_def.h"

void canvas_draw_fv_line
(lv_obj_t* canvas, int16_t x, int16_t y, uint8_t h, lv_color_t c);

void canvas_draw_fh_line
(lv_obj_t* canvas, int16_t x, int16_t y, uint8_t w, lv_color_t c);

void canvas_file_circle_helper
(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta,
    lv_color_t color);

void canvas_fill_circle(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, lv_color_t color);

void canvas_draw_circle(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, lv_color_t color);

void canvas_clear(lv_obj_t* canvas, lv_color_t color);



#endif

