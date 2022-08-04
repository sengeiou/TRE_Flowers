#ifndef _COM_BOOK_SHELF_H
#define _COM_BOOK_SHELF_H

#include "lvgl.h"
#include "gui_inc_def.h"

#define BOOK_COUNT_MAX 10 //最多保存10本 
#define BOOK_INFO_SET 0xff //写入标志

typedef struct BookInfo
{
	uint8_t set_flag;	//写入标志
	int32_t read_page; //书籍已读页数,用于加载时偏移文件读取指针
	char file_path[256]; //书籍绝对路径
	char file_name[128]; //书籍名称
}BookInfo_t;

typedef struct BookShelf
{
	lv_obj_t* shelf;
	lv_obj_t* re_focus_obj;
	void(*select)(BookInfo_t* info);
	void(*close)();
	void(*display)(uint8_t state);
	void(*destroy)();
	void(*save)(BookInfo_t* info);
	
	bool is_show;

}BookShelf_t;

BookShelf_t* lv_bookshelf_create(lv_obj_t* parent);

#endif

