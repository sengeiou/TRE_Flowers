#ifndef _COM_BOOK_MENU_H
#define _COM_BOOK_MENU_H

#include "lvgl.h"
#include "gui_inc_def.h"

enum
{
	BOOK_MENU_SHELF,		//书架
	BOOK_MENU_SAVE,			//保存书签记录
	BOOK_MENU_FILE,			//文件浏览器
	BOOK_MENU_SETTING,	//设置
	BOOK_MENU_NONE,			//无操作
};

typedef struct BookMenu
{
	lv_obj_t* menu;
	lv_obj_t* re_focus_obj;
	void(*select)(uint8_t type);
	void(*display)(uint8_t state);
	void(*destroy)();
	
	bool is_show;
}BookMenu_t;

BookMenu_t* lv_bookmenu_create(lv_obj_t* parent);

#endif

