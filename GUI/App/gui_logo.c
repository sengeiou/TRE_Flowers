#include "gui_logo.h"

LV_IMG_DECLARE(MorningGlory);

static void anim_w_cb(void *var, int32_t v)
{
	lv_obj_set_width(var, v);
}

static void a2_ready_cb(lv_anim_t* a2)
{
	lv_anim_del(a2->var, NULL);
	gui_delay(1000);
	page_call_handler((void*)PAGE_MAIN, DEL);
}

static void a1_ready_cb(lv_anim_t *a1)
{
	lv_obj_t* label_logo = lv_label_create(page_self_root);
	lv_obj_set_style_text_color(label_logo, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_logo, &lv_font_montserrat_22, LV_PART_MAIN);
	lv_label_set_text(label_logo, MY_DEVICE_NAME);
	lv_obj_align(label_logo, LV_ALIGN_RIGHT_MID, -20, 0);
	
	lv_anim_del(a1->var, NULL);
	
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, label_logo);
	lv_anim_set_values(&a, GUI_WIDTH, -20);
	lv_anim_set_time(&a, 600);
	lv_anim_set_delay(&a, 50);
	lv_anim_set_user_data(&a, (void*)label_logo);
	lv_anim_set_ready_cb(&a, a2_ready_cb);
	lv_anim_set_exec_cb(&a, anim_x_cb);
	lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	lv_anim_start(&a);
	
}

static void logo_anim_start()
{	
	lv_obj_t* logo_img = lv_img_create(page_self_root);
	lv_img_set_src(logo_img, &MorningGlory);
	lv_obj_align(logo_img, LV_ALIGN_LEFT_MID, 30, 0);
	
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, logo_img);
	lv_anim_set_values(&a, LV_OPA_0, LV_OPA_MAX);
	lv_anim_set_time(&a, 600);
	lv_anim_set_delay(&a, 50);
	lv_anim_set_user_data(&a, (void*)logo_img);
	lv_anim_set_ready_cb(&a, a1_ready_cb);
	lv_anim_set_exec_cb(&a, anim_opa_cb);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
	lv_anim_start(&a);
}

void gui_logo_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_obj_clear_flag(page_self_root, LV_OBJ_FLAG_SCROLLABLE);

	//init_font_style(&font_32, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_32);
}

void gui_logo_focus(void)
{
	logo_anim_start();
}