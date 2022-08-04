#include "gui_apps.h"
const char* app_names[] = {
    "Book","Arduboy","NES","GPS","Calculator","Calendar","Setting","About"
};

const char* app_names_symbol[] = {
	MY_SYMBOL_BOOK,
	MY_SYMBOL_ARDUBOY,
	MY_SYMBOL_NES,
	MY_SYMBOL_GPS,
	MY_SYMBOL_CALC,
	MY_SYMBOL_CALENDAR,
	MY_SYMBOL_SETTING,
	MY_SYMBOL_ABOUT
};

lv_obj_t* label_name;
lv_obj_t* cont_row_app;

static int id_select = 0;
static uint8_t main_flag = 0;

static void cont_app_switch(lv_obj_t* obj);
static void icon_add_shadow(lv_obj_t* obj);
static void icon_remove_shadow(lv_obj_t* obj);
static void objs_anim_set(uint8_t state);
	
static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	if(main_flag)
		page_call_handler((void*)PAGE_MAIN, DEL);
	else
		page_call_handler((void*)(PAGE_APPS + id_select + 1), DEL);
}

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			bool exit;
			
			if(key == LV_KEY_ENTER)
			{
				main_flag = 0;
				exit = true;
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				main_flag = 1;
				exit = true;
			}
			else if(key == LV_KEY_HOME || key == LV_KEY_LEFT)
			{
				id_select = LV_MAX(id_select--, 0);
				cont_app_switch(lv_obj_get_child(obj, id_select));
			}
			else if(key == LV_KEY_END || key == LV_KEY_RIGHT)
			{
				id_select = LV_MIN(id_select++, lv_obj_get_child_cnt(obj)-1);
				cont_app_switch(lv_obj_get_child(obj, id_select));
			}
			
			if(exit)
			{
				objs_anim_set(DISABLE);
				status_bar->state_set(STATUS_BAR, DISABLE);
			}
			
		}	
			break;
	}
}

static void objs_anim_set(uint8_t state)
{
	lv_anim_t anim;
	
	if(state == ENABLE)
	{
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, NULL, 500, 0, 500, label_name, 25, -10, 0);
	}
	else
	{
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 500, 0, 50, label_name, -10, lv_obj_get_height(label_name), 0);
	}
	lv_anim_start(&anim);
}

static void icon_add_shadow(lv_obj_t* obj)
{
	lv_obj_set_style_shadow_width(obj, 30, LV_PART_MAIN);
	lv_obj_set_style_shadow_color(obj, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
}

static void icon_remove_shadow(lv_obj_t* obj)
{
	lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN);
	lv_obj_set_style_shadow_color(obj, lv_color_white(), LV_PART_MAIN);
}

static void cont_app_switch(lv_obj_t* obj)
{
	static uint16_t old_id = 0;
	static uint8_t old_y = 28;
	static uint8_t new_y = 8;
	
	lv_anim_t anim;
	lv_anim_t anim1;

	lv_obj_t* parent = lv_obj_get_parent(obj);
	uint16_t id = lv_obj_get_index(obj);
	//uint8_t old_y = lv_obj_get_y(obj);
	
	lv_label_set_text(label_name, app_names[id]);

	if (id > 1)
			lv_obj_scroll_to_x(parent, (id - 1) * 105, LV_ANIM_ON);
	else
			lv_obj_scroll_to_x(parent, 0, LV_ANIM_ON);

	if (id > old_id)
	{
			if (id == 1)
			{
				icon_remove_shadow(lv_obj_get_child(parent, old_id));
				lv_obj_align(lv_obj_get_child(parent, old_id), LV_ALIGN_LEFT_MID, 0, 0);
			}
	}
	else
	{
			if (id == 0)
			{
				icon_add_shadow(obj);
				lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, new_y);
			}
	}

	icon_add_shadow(obj);
	ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, NULL, 250, 0, 50, obj, old_y, new_y, 0);
	lv_anim_start(&anim);
	
	if (old_id > 0 && id != old_id)
	{
		icon_remove_shadow(lv_obj_get_child(parent, old_id));
		ANIM_LINE_ADD(&anim1, anim_y_cb, lv_anim_path_overshoot, NULL, 250, 0, 0, lv_obj_get_child(parent, old_id), new_y, old_y, 0);
		lv_anim_start(&anim1);
	}

	old_id = id;
}

void gui_apps_init(lv_obj_t* root)
{
	page_self_root = root;
//	lv_group_add_obj(lv_group_get_default(), page_self_root);
//	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	cont_row_app = lv_obj_create(page_self_root);
	lv_group_add_obj(lv_group_get_default(), cont_row_app);
	lv_obj_add_event_cb(cont_row_app, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_size(cont_row_app, GUI_WIDTH, GUI_HEIGHT - 80);
	lv_obj_set_style_border_width(cont_row_app, 0, 0);
	lv_obj_set_style_bg_opa(cont_row_app, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT); //禁止显示滚动条
	lv_obj_set_style_bg_opa(cont_row_app, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
	lv_obj_set_style_pad_all(cont_row_app, 10, 0);
	lv_obj_center(cont_row_app);
	
	uint8_t i;
	uint8_t obj_size = 90;
	uint8_t len = sizeof(app_names) / sizeof(app_names[0]);
	lv_obj_t* old_obj = NULL;
	for (i = 0; i < len; i++) {
		lv_obj_t* obj;
		lv_obj_t* label;

		obj = lv_obj_create(cont_row_app);
		lv_group_add_obj(lv_group_get_default(), obj);
		lv_obj_add_event_cb(obj, event_key_handler, LV_EVENT_KEY, NULL);
		lv_obj_set_size(obj, obj_size, obj_size);
		lv_obj_set_style_radius(obj, LV_PCT(50), LV_PART_MAIN);
		lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
		if (old_obj) 
			lv_obj_align_to(obj, old_obj, LV_ALIGN_OUT_RIGHT_TOP, 15, 0);
		else 
			lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);

		label = lv_label_create(obj);
		lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_PART_MAIN);
		lv_obj_set_style_text_font(label, &font_symbol_32, LV_PART_MAIN);
		lv_label_set_text(label, app_names_symbol[i]);
		lv_obj_center(label);

		old_obj = obj;
	}
	
	label_name = lv_label_create(page_self_root);
	lv_label_set_text(label_name, app_names[0]);
	lv_obj_set_style_text_color(label_name, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_name, &lv_font_montserrat_22, LV_PART_MAIN);
	lv_obj_align(label_name, LV_ALIGN_BOTTOM_MID, 0, -10);
	
}

void gui_apps_focus(void)
{
	lv_group_focus_obj(cont_row_app);
	
	cont_app_switch(lv_obj_get_child(cont_row_app, id_select));
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->title_set("APPS");
	
	objs_anim_set(ENABLE);
}




