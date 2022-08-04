#include "lvgl_test.h"


static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

void gui_test(lv_obj_t* root)
{
//	lv_obj_t * label1 = lv_label_create(lv_scr_act());
//	char version[64];
//	sprintf(version, "LVGL: %d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
//	lv_label_set_text(label1, version);
//	lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 50);
	
	lv_obj_set_size(root, 320, 240);

	lv_obj_t * obj = lv_obj_create(root);
	lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
	lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

	lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_values(&a, 25, 100);
	lv_anim_set_time(&a, 1000);
	lv_anim_set_playback_delay(&a, 250);
	lv_anim_set_playback_time(&a, 1000);
	lv_anim_set_repeat_delay(&a, 250);
	lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

	lv_anim_set_exec_cb(&a, anim_size_cb);
	lv_anim_start(&a);
	lv_anim_set_exec_cb(&a, anim_x_cb);
	lv_anim_set_values(&a, 0, 230);
	lv_anim_set_delay(&a, 2000);
	lv_anim_start(&a);
}

void gui_font_test(lv_obj_t* root)
{	
	lv_obj_t * label2 = lv_label_create(root);
	lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
	lv_obj_set_width(label2, 150);
	lv_label_set_text(label2, "HReader123");
	lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
}

void gui_png_test(lv_obj_t* root)
{ 
	lv_fs_file_t f;
	lv_fs_res_t res;
	res = lv_fs_open(&f, "S:lvgl/0.jpg", LV_FS_MODE_RD);
	if(res == LV_FS_RES_OK) Debug("file exist!\n");
	
	lv_fs_close(&f);
	
	lv_obj_t * img;
	img = lv_img_create(root);

	lv_img_set_src(img, "S:0.jpg");
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
}

void gui_dir_test()
{
	lv_fs_dir_t dir;
	lv_fs_res_t res;
	char fn[256];
	res = lv_fs_dir_open(&dir, "S:/");
	if(res != LV_FS_RES_OK) Debug("open dir error\n");
	
	while(1) {
		res = lv_fs_dir_read(&dir, fn);
		if(res != LV_FS_RES_OK) {
				Debug("open dir error\n");
				break;
		}

		/*fn is empty, if not more files to read*/
		if(strlen(fn) == 0) {
				break;
		}

		printf("%s\n", fn);
	}
	
	lv_fs_dir_close(&dir);
}