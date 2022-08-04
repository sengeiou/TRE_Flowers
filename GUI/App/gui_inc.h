#ifndef _GUI_INC_H
#define _GUI_INC_H

#include "components.h"
#include "gui_inc_def.h"

typedef void(*page_call_handle)(void* arg, bool del);

static lv_obj_t* page_self_root;
static lv_style_t font_32;
static lv_style_t font_14;
static lv_style_t font_22;

extern page_call_handle page_call_handler;
extern SemaphoreHandle_t lvgl_mutex;
extern FileExplorer_t* file_manager;
extern StatusBar_t* status_bar;

extern const char* app_names[];

void gui_logo_init(lv_obj_t* root);
void gui_main_init(lv_obj_t* root);
void gui_apps_init(lv_obj_t* root);
void gui_about_init(lv_obj_t* root);
void gui_music_init(lv_obj_t* root);
void gui_book_init(lv_obj_t* root);
void gui_fm_init(lv_obj_t* root);
void gui_arduboy_init(lv_obj_t* root);
void gui_nes_init(lv_obj_t* root);
void gui_gps_init(lv_obj_t* root);
void gui_setting_init(lv_obj_t* root);
void gui_calc_init(lv_obj_t* root);
void gui_calendar_init(lv_obj_t* root);

void gui_logo_focus(void);
void gui_main_focus(void);
void gui_apps_focus(void);
void gui_about_focus(void);
void gui_music_focus(void);
void gui_book_focus(void);
void gui_fm_focus(void);
void gui_arduboy_focus(void);
void gui_nes_focus(void);
void gui_gps_focus(void);
void gui_setting_focus(void);
void gui_calc_focus(void);
void gui_calendar_focus(void);

void gui_task_state(uint8_t state);

#endif

