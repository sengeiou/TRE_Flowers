#ifndef _COM_FILE_EXPLORER_H
#define _COM_FILE_EXPLORER_H

#include "sys.h"
#include "lvgl.h"
#include "string.h"
#include "stdlib.h"
#include "gui_inc_def.h"

typedef struct
{
	lv_obj_t* file_root;
	lv_obj_t* re_focus_obj; //指定重新获取焦点的控件
	char* pfilter; //扩展名过滤,统一大写
	
	void(*close_cb)();
	void(*select_cb)(const char* file_name, const char* file_path);
	
	void(*display)(uint8_t state);
	void(*load_dir)(const char* path);
	void(*parent_set)(lv_obj_t* parent);
	
	uint8_t is_show;
	bool select_once; //选择文件后是否关闭文件弹窗
}FileExplorer_t;

FileExplorer_t* file_explorer_instance();

#endif