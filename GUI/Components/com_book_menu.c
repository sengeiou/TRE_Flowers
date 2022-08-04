#include "com_book_menu.h"

static BookMenu_t* book_menu;
uint8_t select_type = BOOK_MENU_SHELF;

static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	
	if(!book_menu->is_show)
	{
		//关闭,选择回调
		if(book_menu->re_focus_obj != NULL) lv_group_focus_obj(book_menu->re_focus_obj);
		if(book_menu->select != NULL) book_menu->select(select_type);
	}
	
	if(lv_obj_has_flag(book_menu->menu, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(book_menu->menu, LV_OBJ_FLAG_HIDDEN);
}

static void menu_destroy()
{
	 myfree(SRAMCCM, book_menu);
}

static void menu_display(uint8_t state)
{
	lv_anim_t anim;
	if(book_menu->menu == NULL) return;
	
	if(state)
	{
		if(book_menu->is_show) return;
		lv_group_focus_obj(book_menu->menu);
		lv_obj_clear_flag(book_menu->menu, LV_OBJ_FLAG_HIDDEN);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 200, book_menu->menu, GUI_HEIGHT, 0, 0);
	}
	else
	{
		if(!(book_menu->is_show)) return;
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 50, book_menu->menu, 0, GUI_HEIGHT, 0);
	}
	
	book_menu->is_show = state;
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
				select_type = lv_obj_get_child_id(obj);
				menu_display(DISABLE);
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				select_type = BOOK_MENU_NONE;
				menu_display(DISABLE);
			}
			else if(key == LV_KEY_LEFT)
			{

			}
			else if(key == LV_KEY_RIGHT)
			{

			}
			else if(key == LV_KEY_HOME)
			{

			}
			else if(key == LV_KEY_END)
			{

			}
			
		}
			break;
	}
}


BookMenu_t* lv_bookmenu_create(lv_obj_t* parent)
{
	lv_obj_t* btn;
	lv_obj_t* label;
	
	book_menu = mymalloc(SRAMCCM, sizeof(BookMenu_t));
	memset(book_menu, 0, sizeof(BookMenu_t));
	book_menu->display = menu_display;
	book_menu->destroy = menu_destroy;
	
	book_menu->menu = lv_obj_create(parent);
	lv_obj_set_flex_flow(book_menu->menu, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_size(book_menu->menu, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(book_menu->menu, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(book_menu->menu, LV_OBJ_FLAG_HIDDEN);

	lv_gridnav_add(book_menu->menu, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), book_menu->menu);
	
	//书架
	btn = lv_btn_create(book_menu->menu);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_LIST);
	lv_obj_center(label);
	
	//保存记录
	btn = lv_btn_create(book_menu->menu);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_SAVE);
	lv_obj_center(label);

	//文件列表
	btn = lv_btn_create(book_menu->menu);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_FILE);
	lv_obj_center(label);
	
	//设置
	btn = lv_btn_create(book_menu->menu);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_SETTINGS);
	lv_obj_center(label);
	
	return book_menu;
}




