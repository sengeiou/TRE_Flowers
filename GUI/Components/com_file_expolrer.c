#include "com_file_explorer.h"
#include "malloc.h"
#include "fatfs.h"
#include "utf8.h"
#include <ctype.h>

#define ROOT_PATH ""
#define PARENT_PATH "Parent dir/"
#define LSIT_ITEM_MAX (10) //每页显示文件数量

#define FILE_DEBUG 1

enum
{
	SCAN_PREV,
	SCAN_NEXT
};

enum
{
	ITEM_NONE,
	ITEM_DIR,
	ITEM_FILE
};

/************************/
FileExplorer_t *file_explorer = NULL;
char* file_path;

lv_obj_t* list_file;
lv_obj_t* label_prev;
lv_obj_t* label_next;
lv_obj_t* label_path;
lv_obj_t* label_size;
lv_obj_t* label_sd_size;

const char* unitArr[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
char file_size_buf[64];

void* ready_exec_cb;
char file_name_buf[256]; //文件名 
char file_abolute_buf[256]; //文件绝对路径
char file_path_buf[256]; //文件夹路径
int file_page_index = 0; //当前分页索引
int file_page_count = 0; //当前目录列表总分页(含文件和文件夹)

/************************/
static void explorer_display(uint8_t state);
static void scan_dir(uint8_t s_dir);
static char* calc_file_size(uint32_t val);
static void calc_sd_size(void);
static void calc_file_page(void);

static void list_parentdir_set(const char* path);
static void explorer_load_dir(const char* path);

/************************/
static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	if(ready_exec_cb != NULL && !(file_explorer->is_show)) 
	{
		lv_obj_add_flag(file_explorer->file_root, LV_OBJ_FLAG_HIDDEN);
		if(ready_exec_cb == file_explorer->select_cb)
		{
			file_explorer->select_cb(file_name_buf, file_abolute_buf);
		}
		else if(ready_exec_cb == file_explorer->close_cb)
		{
			file_explorer->close_cb();
		}
		
		if(file_explorer->re_focus_obj != NULL) lv_group_focus_obj(file_explorer->re_focus_obj);
	}
	else if(file_explorer->is_show)
	{
		calc_sd_size(); //获取SD卡容量
		calc_file_page(); //获取当前目录分页数量
	}
}

static bool check_expname(const char* path)
{
	char temp[10];
	const char* exp = NULL;
	if(file_explorer->pfilter == NULL) return true; //默认不检测后缀名
	exp = strrchr(path, '.'); //获取扩展名
	for(int i=0;i<strlen(exp);i++) temp[i] = toupper(*(exp+i));
#if FILE_DEBUG
	Debug("File expname: %s\n", temp);
#endif
	if(strcmp(temp, file_explorer->pfilter) == 0)
		return true;
	
	return false;
}

static uint8_t check_path(const char* path)
{
	FRESULT fr;
	FILINFO fno;
	uint8_t res = ITEM_NONE;
	
	fr = f_stat (path, &fno);
	
	switch(fr)
	{
		case FR_OK:
			if(fno.fattrib & AM_DIR)
				res = ITEM_DIR;
			else
				res = ITEM_FILE;
			break;
		case FR_NO_FILE:
			break;
	}
	
	if(strcmp(path, ROOT_PATH) == 0) res = ITEM_DIR; //根目录
	return res;
}

static void calc_sd_size()
{
	char* buf = mymalloc(SRAMCCM, 128);
	memset(buf, 0, 128);
	
	uint32_t total_size = 0, free_size = 0;
	get_fatsize(&free_size, &total_size);
#if FILE_DEBUG
	Debug("SD: %df/%d\n", free_size, total_size);
#endif
	sprintf(buf, "%s", calc_file_size(free_size));
	sprintf(&buf[strlen(buf)], "/%s", calc_file_size(total_size));
	
	lv_label_set_text(label_sd_size, buf);
	myfree(SRAMCCM, buf);
}

static void calc_file_page()
{
	FRESULT res;
	DIR dir;
	FILINFO fno;
	int count = 0;
	
	res = f_opendir(&dir, file_path_buf);
	if(res == FR_OK)
	{	
		for(;;)
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;
			}
			
			count++;
		}
		
		f_closedir(&dir);
		
		//计算分页总数
		file_page_count = ceil(count * 1.0f / LSIT_ITEM_MAX);
#if FILE_DEBUG
		Debug("Page count: %d %d\n", count, file_page_count);
#endif
	}
	else
	{
		Debug("open dir failed!\n");
	}
}

static char* calc_file_size(uint32_t val)
{
	uint8_t index = 0;
	float out_size = 0;
	memset(file_size_buf, 0, sizeof(file_size_buf));
	
	do{
		if(val <= 0) break;
		index = floor(log(val) / log(1024));
		out_size = (val) / pow(1024, index);
	}while(0);

	sprintf(file_size_buf, "%.2f %s", out_size, unitArr[index]);
	return file_size_buf;
}

static void get_absolute_path(lv_obj_t* obj)
{
	uint16_t len;
	const char* str;
	unsigned char* gbk_buf;
	if(obj == NULL) return;
	
	str = lv_list_get_btn_text(list_file, obj);
	len = strlen(str);
	
	gbk_buf = (unsigned char*)mymalloc(SRAMCCM, len<<1);

	memset(gbk_buf, 0, len<<1);
	memset(file_abolute_buf, 0, sizeof(file_abolute_buf)); 
	
	encUtf8ToGbkLen((unsigned char*)str, len, gbk_buf); //to GBK
	sprintf(file_abolute_buf, "%s/%s", file_path_buf, gbk_buf); //copy file path
	
	myfree(SRAMCCM, gbk_buf);
}

static void list_parentdir_set(const char* path)
{
	if(path == NULL || strcmp(path, ROOT_PATH) == 0) path = "/";
	lv_obj_t* obj = lv_obj_get_child(list_file, 0);
	lv_label_set_text(lv_obj_get_child(obj, 1), path);
}

static void list_item_clear()
{
	uint8_t i;
	lv_obj_t* obj;
	
	//从第一项开始
	for(i=1;i<lv_obj_get_child_cnt(list_file);i++)
	{
		obj = lv_obj_get_child(list_file, i);
		
		lv_img_set_src(lv_obj_get_child(obj, 0), LV_SYMBOL_DUMMY); //重置图标
		lv_label_set_text(lv_obj_get_child(obj, 1), ""); //重置文本
	}
	
	obj = lv_obj_get_child(list_file, 0);
	lv_gridnav_set_focused(list_file, obj, LV_ANIM_ON); //重新设置焦点
}

static void list_item_update(const char* name, uint16_t len, uint8_t type, uint8_t id)
{
	lv_obj_t* obj;
	obj = lv_obj_get_child(list_file, (id + 1)); //从第一项开始
	unsigned char* utf8_buf = mymalloc(SRAMCCM, len);

	memset(utf8_buf, 0, len);
	encGbkToUtf8Len((unsigned char*)name, (len >> 1), utf8_buf); //to UTF-8
	
	lv_img_set_src(lv_obj_get_child(obj, 0), (type == ITEM_DIR) ? LV_SYMBOL_DIRECTORY: LV_SYMBOL_FILE); //设置图标
	lv_label_set_text(lv_obj_get_child(obj, 1), (const char*)utf8_buf); //设置文本
	
	myfree(SRAMCCM, utf8_buf);
}

static lv_obj_t* list_item_selected(lv_obj_t* obj)
{
	for (int i = 0; i < lv_obj_get_child_cnt(obj); i++)
	{
		lv_obj_t* child = lv_obj_get_child(obj, i);
		if (lv_obj_has_state(child, LV_STATE_FOCUSED))
		{
			return child;
		}
	}
	
	return NULL;
}

static void scan_dir(uint8_t s_dir)
{
	FRESULT res;
	DIR dir;
	static FILINFO fno;

	uint16_t begin_index;
	uint16_t end_index;
	uint16_t i;
	uint8_t id;
	lv_obj_t* obj;
	
	if(s_dir == SCAN_PREV)
		file_page_index = LV_MAX(file_page_index--, 0);
	else
		file_page_index = LV_MIN(file_page_index++, file_page_count-1);
	
	begin_index = (file_page_index * LSIT_ITEM_MAX);
	end_index = (begin_index + LSIT_ITEM_MAX);
#if FILE_DEBUG
	Debug("%d, %d, %d\n", file_page_index, begin_index, end_index);
#endif
	
	res = f_opendir(&dir, file_path_buf);
	if(res == FR_OK)
	{
		id = 0;
		list_item_clear();
		
		//移动文件索引到当前列表页
		for(i=0;i<end_index;i++)
		{
			res = f_readdir(&dir, &fno);
			
			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;
			}
			
			if(i < begin_index)
				continue;
			else
			{
				if(fno.fattrib & AM_DIR)
				{
					//文件夹
//					Debug("Dir: %s\n", fno.fname);
					list_item_update(fno.fname, sizeof(fno.fname), ITEM_DIR, id);
				}
				else
				{
					//文件
//					Debug("File: %s\n", fno.fname);
					list_item_update(fno.fname, sizeof(fno.fname), ITEM_FILE, id);
				}
				
				id++;
			}
		}

		f_closedir(&dir);
		
	}
	else
	{
		Debug("open dir failed!\n");
	}
	
	if(file_page_index <= 0)
	{
		//首页
		lv_obj_add_flag(label_prev, LV_OBJ_FLAG_HIDDEN);
		(file_page_count > 1) ? lv_obj_clear_flag(label_next, LV_OBJ_FLAG_HIDDEN) : lv_obj_add_flag(label_next, LV_OBJ_FLAG_HIDDEN);
	}
	else if(file_page_index > 0 && (file_page_index < file_page_count-1))
	{
		//中间页
		lv_obj_clear_flag(label_prev, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(label_next, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		//末页
		lv_obj_clear_flag(label_prev, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(label_next, LV_OBJ_FLAG_HIDDEN);
	}
	
}

/**
 *@目录加载函数
 *@path: 绝对路径
 */
static void explorer_load_dir(const char* path)
{
	if(path == NULL || (check_path(path) != ITEM_DIR)) return;
	
	list_parentdir_set(strcmp(path, ROOT_PATH) == 0 ? ROOT_PATH : PARENT_PATH); //若非根目录则提示有上级目录
	
	//重置变量
	file_page_index = 0;
	file_page_count = 0;
	memset(file_path_buf, 0, sizeof(file_path_buf));
	strcpy(file_path_buf, path); //拷贝当前路径
	
	lv_label_set_text(label_path, strcmp(file_path_buf, ROOT_PATH) == 0 ? "/" : file_path_buf); //显示当前路径
	calc_file_page(); //统计文件数量
	
	//扫描文件夹，默认向前扫描
	scan_dir(SCAN_PREV);
	
}

static void load_parent_dir()
{
	int index = 0;
#if FILE_DEBUG
	Debug("Current dir: %s\n", file_path_buf);
#endif
	
	//查找上级目录路径
	do{
		if(strcmp(file_path_buf, "") == 0) break;
		char* ptr = strrchr(file_path_buf, '/');
		if(ptr)
			memset(ptr, 0, strlen(ptr));
	}while(0);

#if FILE_DEBUG
	Debug("Parent dir: %s\n", file_path_buf);
#endif
	
	explorer_load_dir(file_path_buf);
}

static void event_list_handler(lv_event_t* e)
{
	uint32_t key;
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_KEY)
	{
		key = lv_indev_get_key(lv_indev_get_act());
		lv_obj_t* obj = list_item_selected(lv_event_get_target(e)); //获取列表选中项
		
		if(obj == NULL) return;
		
		if(key == LV_KEY_ENTER)
		{
			//选中目录或者文件
			if(file_explorer->select_cb == NULL) return;
			
			if(lv_obj_get_index(obj) == 0)
			{
				load_parent_dir(); //返回上级目录
				return;
			}
			
			//进入子目录或打开文件
			{
				get_absolute_path(obj); 
				uint8_t type = check_path(file_abolute_buf);
				
				if(type == ITEM_DIR)
				{
					//打开文件夹
					explorer_load_dir(file_abolute_buf);
				}
				else
				{
					if(!check_expname(file_abolute_buf))
					{
						Debug("File expname check failed\n");
						return;
					}
					
					memset(file_name_buf, 0, sizeof(file_name_buf));
					strcpy(file_name_buf, lv_list_get_btn_text(list_file, obj));
					
					//打开文件
					if(file_explorer->select_once)
					{
						ready_exec_cb = file_explorer->select_cb;
						explorer_display(DISABLE);
					}
					else
					{
						file_explorer->select_cb(file_name_buf, file_abolute_buf);
					}
				}
			}
		}
		else if(key == LV_KEY_BACKSPACE)
		{
			//关闭弹窗
			if(file_explorer->close_cb == NULL) return;
			ready_exec_cb = file_explorer->close_cb;
			explorer_display(DISABLE);
		}
		else if(key == LV_KEY_UP || key == LV_KEY_DOWN )
		{
			//计算选中文件大小
			FRESULT fr;
			FILINFO fno;
			
			if(lv_obj_get_index(obj) == 0)
			{
				lv_label_set_text(label_size, "");
				return;
			}
			
			get_absolute_path(obj);
			fr = f_stat(file_abolute_buf, &fno);
			if(fr == FR_OK)
			{
				lv_label_set_text(label_size, calc_file_size(fno.fsize)); //设置文件大小
			}
			else
			{
				lv_label_set_text(label_size, "");
			}
		}
		else if(key == LV_KEY_HOME)
		{
			//向前扫描
			scan_dir(SCAN_PREV);
		}
		else if(key == LV_KEY_END)
		{
			//向后扫描
			scan_dir(SCAN_NEXT);
		}
	}
}

static void explorer_set_parent(lv_obj_t* parent)
{
	lv_obj_set_parent(file_explorer->file_root, parent);
	
	//重置
	file_explorer->select_cb = NULL;
	file_explorer->close_cb = NULL;
	file_explorer->re_focus_obj = NULL;
	file_explorer->pfilter = NULL;
	file_explorer->select_once = false;
}

static void explorer_init()
{
	file_explorer->file_root = lv_obj_create(lv_scr_act());
	lv_obj_set_size(file_explorer->file_root, lv_pct(95), lv_pct(84));
	lv_obj_align(file_explorer->file_root, LV_ALIGN_BOTTOM_MID, 0, -5);
	lv_obj_set_style_pad_all(file_explorer->file_root, 5, 0);
	lv_obj_add_flag(file_explorer->file_root, LV_OBJ_FLAG_HIDDEN);
	
	label_prev = lv_label_create(file_explorer->file_root);
	lv_label_set_text(label_prev, LV_SYMBOL_LEFT);
	lv_obj_set_style_text_color(label_prev, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_align(label_prev, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_add_flag(label_prev, LV_OBJ_FLAG_HIDDEN);
	
	label_next = lv_label_create(file_explorer->file_root);
	lv_label_set_text(label_next, LV_SYMBOL_RIGHT);
	lv_obj_set_style_text_color(label_next, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_align(label_next, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_flag(label_next, LV_OBJ_FLAG_HIDDEN);

	label_path = lv_label_create(file_explorer->file_root);
	lv_obj_set_size(label_path, lv_pct(80), LV_SIZE_CONTENT);
	lv_label_set_long_mode(label_path, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(label_path, ROOT_PATH);
	lv_obj_set_style_text_align(label_path, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label_path, LV_ALIGN_TOP_MID, 0, 0);
	lv_group_remove_obj(label_path);

	list_file = lv_list_create(file_explorer->file_root);
	lv_gridnav_add(list_file, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), list_file);
	lv_obj_add_event_cb(list_file, event_list_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_flex_flow(list_file, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_size(list_file, lv_pct(100), lv_pct(80));
	lv_obj_align(list_file, LV_ALIGN_BOTTOM_MID, 0, -20);
	lv_obj_set_style_bg_color(list_file, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
	
	
	label_size = lv_label_create(file_explorer->file_root);
	lv_obj_set_size(label_size, LV_PCT(40), LV_SIZE_CONTENT);
	lv_label_set_long_mode(label_size, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(label_size, "");
	lv_obj_align(label_size, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	
	label_sd_size = lv_label_create(file_explorer->file_root);
	lv_label_set_text(label_sd_size, "");
	lv_obj_align(label_sd_size, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

	//初始化列表
	uint8_t i;
	for (i = 0; i < (LSIT_ITEM_MAX+1); i++) {
		lv_obj_t * obj = lv_list_add_btn(list_file, LV_SYMBOL_DUMMY, "");
		lv_obj_set_style_bg_opa(obj, 0, 0);
		lv_group_remove_obj(obj);  

		lv_obj_t* label = lv_obj_get_child(obj, 1);
		lv_obj_add_style(label, &font_zh_full_16, 0);
//		lv_obj_center(label);

	}
}

static void explorer_display(uint8_t state)
{
	lv_anim_t anim;
	
	if(file_explorer->file_root == NULL) return;
	
	if(state == ENABLE)
	{
		lv_group_focus_obj(list_file);
		lv_obj_clear_flag(file_explorer->file_root, LV_OBJ_FLAG_HIDDEN);
		
		if(file_explorer->is_show) return;
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 200, 0, 50, file_explorer->file_root, GUI_HEIGHT, -5, 0);
	}
	else
	{
		if(!(file_explorer->is_show)) return;
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 200, 0, 50, file_explorer->file_root, -5, GUI_HEIGHT, 0);
	}

	file_explorer->is_show = state;
	lv_anim_start(&anim);
}

FileExplorer_t* file_explorer_instance()
{
	if(file_explorer == NULL)
	{
		file_explorer = (FileExplorer_t*)mymalloc(SRAMCCM, sizeof(FileExplorer_t));
		memset(file_explorer, 0, sizeof(FileExplorer_t));
		file_explorer->display = explorer_display;
		file_explorer->load_dir = explorer_load_dir;
		file_explorer->parent_set = explorer_set_parent;
		
		explorer_init();
		
		//初始化文件列表，根目录
		memset(file_path_buf, 0, sizeof(file_path_buf));
		strcpy(file_path_buf, ROOT_PATH);
		list_parentdir_set(ROOT_PATH);
		calc_file_page();
		scan_dir(SCAN_PREV);
	}
	
	return file_explorer;
}

