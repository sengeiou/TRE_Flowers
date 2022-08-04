#include "com_arc_load.h"

static lv_obj_t* arc_cont;

lv_obj_t* lv_arc_load_get_label(lv_obj_t* obj)
{
	return lv_obj_get_child(obj, 1);
}

lv_obj_t* lv_arc_load_create(lv_obj_t* parent)
{
	arc_cont = lv_obj_create(parent);
	lv_obj_set_size(arc_cont, LV_PCT(80), 120);
	lv_obj_set_style_pad_all(arc_cont, 5, LV_PART_MAIN);
	lv_obj_center(arc_cont);
	
	lv_obj_t* arc = lv_arc_create(arc_cont);
	lv_obj_set_size(arc, 60, 60);
	lv_arc_set_rotation(arc, 270);
	lv_arc_set_bg_angles(arc, 0, 360);
	lv_obj_set_style_arc_width(arc, 6, LV_PART_MAIN);
	lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
	lv_obj_remove_style(arc, NULL, LV_PART_KNOB); 
	lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); 
	lv_obj_align(arc, LV_ALIGN_CENTER, 0, -10);

	lv_obj_t* label = lv_label_create(arc_cont);
	lv_obj_set_size(label, LV_PCT(80), LV_SIZE_CONTENT);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(label, "80%");
	lv_obj_align_to(label, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, arc);
	lv_anim_set_exec_cb(&a, set_angle);
	lv_anim_set_time(&a, 1000);
	lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
	lv_anim_set_repeat_delay(&a, 500);
	lv_anim_set_values(&a, 0, 100);
	lv_anim_start(&a);
	
	return arc_cont;
}


