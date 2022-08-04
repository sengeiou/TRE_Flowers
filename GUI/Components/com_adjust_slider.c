#include "com_adjust_slider.h"

static lv_obj_t* lv_cont;
static lv_obj_t* label_val;
static uint8_t last_val;

static AdjustBarParam_t* slider_param;
lv_adjust_slider_cb slider_cb;

static void event_handler(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	
	if(code == LV_EVENT_KEY)
	{
		const uint32_t key = lv_indev_get_key(lv_indev_get_act());
		if(key == LV_KEY_ENTER)
		{
			Debug("slider key enter %d\n", (uint8_t)lv_obj_get_user_data(obj));
		}
		else if(key == LV_KEY_BACKSPACE)
		{
			if(last_val != lv_slider_get_value(obj)) device_config_write(); //返回时写入配置
			
			lv_group_focus_obj(slider_param->re_focus_obj);
			myfree(SRAMCCM, slider_param);
			
			lv_obj_del_async(lv_cont);
		}
	}
	else if(code == LV_EVENT_VALUE_CHANGED)
	{
		uint8_t val = lv_slider_get_value(obj);
		lv_label_set_text_fmt(label_val, "%d", val * slider_param->slide_gap); //valx滑动间隔
		if(slider_cb != NULL) slider_cb(slider_param->type, val * slider_param->slide_gap);
	}
}

lv_obj_t* lv_adjust_slider_create(lv_obj_t* parent, AdjustBarParam_t* param, lv_adjust_slider_cb cb)
{
	slider_param = mymalloc(SRAMCCM, sizeof(AdjustBarParam_t));
	if(param == NULL) return NULL;
	
	memcpy(slider_param, param, sizeof(AdjustBarParam_t));
	slider_cb = cb;
	last_val = param->init_val;
	
	lv_cont = lv_obj_create(parent);
	lv_obj_set_size(lv_cont, LV_PCT(80), 90);
	lv_obj_set_style_pad_all(lv_cont, 5, LV_PART_MAIN);
	lv_gridnav_add(lv_cont, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), lv_cont);
	lv_obj_center(lv_cont);

	lv_obj_t* label = lv_label_create(lv_cont);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, param->title);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

	lv_obj_t* slider = lv_slider_create(lv_cont);
	lv_obj_set_user_data(slider, (void*)param->type); //设置用户数据
	lv_obj_set_size(slider, LV_PCT(80), 10);
	lv_slider_set_range(slider, param->range_min, param->range_max);
	lv_obj_align_to(slider, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
	lv_slider_set_value(slider, param->init_val, LV_ANIM_ON);
	
	lv_obj_add_event_cb(slider, event_handler, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(slider, event_handler, LV_EVENT_VALUE_CHANGED, NULL);

	label_val = lv_label_create(lv_cont);
	lv_obj_set_style_text_color(label_val, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text_fmt(label_val, "%d", (param->init_val * param->slide_gap));
	lv_obj_align_to(label_val, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
	
	lv_group_focus_obj(slider);

	return lv_cont;
}









