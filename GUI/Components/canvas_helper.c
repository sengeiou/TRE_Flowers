#include "canvas_helper.h"

void canvas_draw_fv_line
(lv_obj_t* canvas, int16_t x, int16_t y, uint8_t h, lv_color_t c)
{
	int end = y + h;
	for (int a = LV_MAX(0, y); a < LV_MIN(end, GUI_WIDTH); a++)
	{
		lv_canvas_set_px_color(canvas, x, a, c);
	}
}

void canvas_draw_fh_line
(lv_obj_t* canvas, int16_t x, int16_t y, uint8_t w, lv_color_t c)
{
	int end = x + w;
	for (int a = LV_MAX(0, x); a < LV_MIN(end, GUI_HEIGHT); a++)
	{
		lv_canvas_set_px_color(canvas, a, y, c);
	}
}

void canvas_file_circle_helper
(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta,
    lv_color_t color)
{
	// used to do circles and roundrects!
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1)
		{
			canvas_draw_fv_line(canvas, x0 + x, y0 - y, 2 * y + 1 + delta, color);
			canvas_draw_fv_line(canvas, x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}

		if (cornername & 0x2)
		{
			canvas_draw_fv_line(canvas, x0 - x, y0 - y, 2 * y + 1 + delta, color);
			canvas_draw_fv_line(canvas, x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

void canvas_fill_circle(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, lv_color_t color)
{
	canvas_draw_fv_line(canvas, x0, y0 - r, 2 * r + 1, color);
	canvas_file_circle_helper(canvas, x0, y0, r, 3, 0, color);
}

void canvas_draw_circle(lv_obj_t* canvas, int16_t x0, int16_t y0, uint8_t r, lv_color_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	lv_canvas_set_px_color(canvas, x0, y0 + r, color);
	lv_canvas_set_px_color(canvas, x0, y0 - r, color);
	lv_canvas_set_px_color(canvas, x0 + r, y0, color);
	lv_canvas_set_px_color(canvas, x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;

		lv_canvas_set_px_color(canvas, x0 + x, y0 + y, color);
		lv_canvas_set_px_color(canvas, x0 - x, y0 + y, color);
		lv_canvas_set_px_color(canvas, x0 + x, y0 - y, color);
		lv_canvas_set_px_color(canvas, x0 - x, y0 - y, color);
		lv_canvas_set_px_color(canvas, x0 + y, y0 + x, color);
		lv_canvas_set_px_color(canvas, x0 - y, y0 + x, color);
		lv_canvas_set_px_color(canvas, x0 + y, y0 - x, color);
		lv_canvas_set_px_color(canvas, x0 - y, y0 - x, color);
	}
}

void canvas_clear(lv_obj_t* canvas, lv_color_t color)
{
	lv_canvas_fill_bg(canvas, color, LV_OPA_COVER);
}



