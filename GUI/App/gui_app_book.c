#include "gui_app_book.h"
#include "fatfs.h"

#define TXT_BUFF_SIZE 512

#define COLOR_BLACK 	lv_color_black()
#define COLOR_WHITE 	lv_color_white()
#define COLOR_EYECARE	lv_color_hex(0XCCE8CF)

#define TXT_BUFF_CLEAR do{\
	last_read_dir = TXT_READ_NEXT;\
	txt_page_index = 0; \
	memset(book_info, 0, sizeof(BookInfo_t));\
	memset(txt_buff, 0, TXT_BUFF_SIZE); \
}while(0);

enum
{
	TXT_READ_PREV,
	TXT_READ_NEXT
};

enum
{
	COLOR_BLACK_ID = 0,
	COLOR_WHITE_ID,
	COLOR_EYECARE_ID
};

struct BookSettingObj
{
	uint16_t btn_bg_black:1;
	uint16_t btn_bg_white:2;
	uint16_t btn_bg_eyecare:2;
	uint16_t btn_font_black:3;
	uint16_t btn_font_white:3;
}booksetting_obj;

BookMenu_t* menu;
BookShelf_t* shelf;
BookInfo_t* book_info;

lv_obj_t* cont_setting;
lv_obj_t* label_txt;
lv_obj_t* mbox_warning;

static FIL fp;
static FRESULT fres;
static uint32_t readBytes;
int32_t txt_page_index = 0;
uint8_t* txt_buff;

bool exit_flag = false;
uint8_t last_read_dir;
/*******************************************************/
static void book_setting_display(uint8_t state);
static void msgbox_display(uint8_t state);

/*******************************************************/
static lv_color16_t get_color(uint8_t color_id)
{
	if(color_id == COLOR_BLACK_ID)
		return COLOR_BLACK;
	else if(color_id == COLOR_WHITE_ID)
		return COLOR_WHITE;
	else if(color_id == COLOR_EYECARE_ID)
		return COLOR_EYECARE;
	
	return COLOR_BLACK;
}

/**
 *@name txt_read
 *@desc 文件读取(数据头尾可能出现乱码)
 *@parem: dir 读取方向
 *@return:
 */
static void txt_read(uint8_t dir)
{
	int32_t index;
	uint8_t skip = 0;
	
	if(last_read_dir != dir)
	{
		last_read_dir = dir;
		skip = 1;
	}
	
	index = (dir == TXT_READ_NEXT) ? txt_page_index + skip : LV_MAX(txt_page_index-1, 0);
	index = LV_MAX(index, 0);
	
	FSIZE_t offset = index * TXT_BUFF_SIZE;
	//Debug("%d %ld\n", index, offset);
	
	//偏移文件指针
	fres = f_lseek (&fp, offset);
	if(fres != FR_OK)
	{
		Debug("File lseek failed\n");
		return;
	}
	
	memset(txt_buff, 0, TXT_BUFF_SIZE);
	fres = f_read(&fp, txt_buff, TXT_BUFF_SIZE, &readBytes);
	if(fres == FR_OK)
	{
		//显示文本
		lv_label_set_text(label_txt, "");
		lv_label_set_text_static(label_txt, (const char*)txt_buff);
		lv_obj_scroll_to_y(label_txt, 0, LV_ANIM_OFF);
	}
	
	if(f_eof(&fp) != 0 || f_tell(&fp) <= 0)
	{
		Debug("File begin/end\n");
		return;
	}
	
	book_info->read_page = index; //记录读取页数
	txt_page_index = (dir == TXT_READ_NEXT) ? index+1 : index;
}

/**
 *@name txt_file_load
 *@desc 文件载入
 *@parem: path 文件绝对路径
 *@return:
 */
static void txt_file_load(const char* path)
{
	f_close(&fp);
	readBytes = 0;
	
	if(f_open(&fp, path, FA_READ) != FR_OK)
		return;
	
	//偏移文件指针
	uint32_t pointer = (txt_page_index * TXT_BUFF_SIZE);
	fres = f_lseek (&fp, pointer);
	
	//读取一页
	txt_read(TXT_READ_NEXT);
}
/*******************************************************/
static void status_bar_hidecb()
{
	if(exit_flag)
	{
		exit_flag = !exit_flag;
		f_close(&fp); //关闭文件
		myfree(SRAMIN, txt_buff);
		myfree(SRAMIN, book_info);
		page_call_handler((void*)PAGE_APPS, DEL);
	}

}

static void menu_select_cb(uint8_t type)
{	
	if(type == BOOK_MENU_FILE)
	{
		//打开文件浏览器
		file_manager->display(ENABLE);
	}
	else if(type == BOOK_MENU_SHELF)
	{
		//打开书籍管理器
		status_bar->state_set(STATUS_BAR, ENABLE);
		shelf->display(ENABLE);
	}
	else if(type == BOOK_MENU_SAVE)
	{
		//保存当前书籍
		if(strcmp(book_info->file_name, "") == 0)
		{
			Debug("No file\n");
			msgbox_display(ENABLE);
		}
		else
		{
			shelf->save(book_info);
		}
	}
	else if(type == BOOK_MENU_SETTING)
	{
		//打开功能设置
		book_setting_display(ENABLE);
		lv_obj_clear_flag(label_txt, LV_OBJ_FLAG_HIDDEN);
	}
	else if(type == BOOK_MENU_NONE)
	{
		//无效
	}
}

static void shelf_close_cb()
{
	status_bar->state_set(STATUS_BAR, DISABLE);
}

static void shelf_select_cb(BookInfo_t* info)
{
	if(info == NULL) return;
	
	TXT_BUFF_CLEAR;
	//设置记录值
	txt_page_index = info->read_page;
	
#if 1
	Debug("select id: %d\n", txt_page_index);
	Debug("select path: %s\n", info->file_path);
	Debug("select name: %s\n", info->file_name);
#endif
	
	strcpy(book_info->file_name, info->file_name);
	strcpy(book_info->file_path, info->file_path);
	
	txt_file_load(book_info->file_path);
}

static void file_close_cb()
{
	
}

static void file_select_cb(const char* name, const char* path)
{
	TXT_BUFF_CLEAR;
	
	//临时保存文件信息
	strcpy(book_info->file_name, name);
	strcpy(book_info->file_path, path);
	
	txt_file_load(path);
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
				msgbox_display(DISABLE);
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				//释放内存
				exit_flag = true;
				shelf->destroy();
				menu->destroy();
				
				if(status_bar->is_show)
					status_bar->state_set(STATUS_BAR, DISABLE);
				else
					status_bar_hidecb();
					
			}
			else if(key == LV_KEY_LEFT)
			{
				//上一页
				txt_read(TXT_READ_PREV);
			}
			else if(key == LV_KEY_RIGHT)
			{
				//下一页
				txt_read(TXT_READ_NEXT);
			}
			else if(key == LV_KEY_HOME)
			{
				if(status_bar->is_show)
					status_bar->state_set(STATUS_BAR, DISABLE);
				else
					status_bar->state_set(STATUS_BAR, ENABLE);
			}
			else if(key == LV_KEY_END)
			{
				if(menu != NULL)
				{
					menu->display(ENABLE);
				}
			}
	
		}
			break;
	}
}

static void event_setting_handler(lv_event_t *e)
{
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			uint16_t id;
			lv_color_t color;
			
			if(key == LV_KEY_ENTER)
			{
				id = lv_obj_get_child_id(obj);
				color = lv_obj_get_style_bg_color(obj, LV_PART_MAIN);
				
				if(id == booksetting_obj.btn_bg_black || \
						id == booksetting_obj.btn_bg_white || \
						id == booksetting_obj.btn_bg_eyecare)
				{
					//设置背景颜色				
					lv_obj_set_style_bg_color(label_txt, color, LV_PART_MAIN);
					
					//修改配置
					device_cfg.book_bg_color = (uint8_t)lv_obj_get_user_data(obj);
					
				}
				else if(id == booksetting_obj.btn_font_black || \
								id == booksetting_obj.btn_font_white)
				{
					//设置字体颜色
					lv_obj_set_style_text_color(label_txt, color, LV_PART_MAIN);
					
					//修改配置
					device_cfg.book_font_color = (uint8_t)lv_obj_get_user_data(obj);
				}
				
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				//写入配置
				device_config_write();
				book_setting_display(DISABLE);
			}
	
		}
			break;
	}
}

static void event_focus_handler(lv_event_t *e)
{
	lv_obj_t* obj = lv_event_get_target(e);
	
	switch(e->code)
	{
		case LV_EVENT_FOCUSED:
			lv_obj_clear_flag(label_txt, LV_OBJ_FLAG_HIDDEN);
			break;
		case LV_EVENT_DEFOCUSED:
			lv_obj_add_flag(label_txt, LV_OBJ_FLAG_HIDDEN);
			break;
	}
}

static void book_setting_display(uint8_t state)
{
	lv_anim_t anim;
	
	if(state)
	{
		lv_group_focus_obj(cont_setting);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, NULL, 300, 0, 200, cont_setting, 80, -5, 0);
	}
	else
	{
		lv_group_focus_obj(label_txt);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, NULL, 300, 0, 50, cont_setting, -5, 80, 0);
	}
	lv_anim_start(&anim);
}

static void msgbox_display(uint8_t state)
{
	if(state == ENABLE)
	{
		if(lv_obj_has_flag(mbox_warning, LV_OBJ_FLAG_HIDDEN)) lv_obj_clear_flag(mbox_warning, LV_OBJ_FLAG_HIDDEN);
	}
	else
	{
		if(!lv_obj_has_flag(mbox_warning, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(mbox_warning, LV_OBJ_FLAG_HIDDEN);
	}
		
}

static void book_setting_init()
{
	lv_obj_t* btn_black;
	lv_obj_t* btn_white;
	lv_obj_t* btn_eyecare;
	lv_obj_t* label_back;
	lv_obj_t* label_font;

	cont_setting = lv_obj_create(page_self_root);
	lv_obj_set_size(cont_setting, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(cont_setting, LV_ALIGN_BOTTOM_MID, 0, 80); //-5

	lv_gridnav_add(cont_setting, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), cont_setting);

	//背景颜色
	label_back = lv_label_create(cont_setting);
	lv_obj_set_style_text_color(label_back, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_back, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label_back, "BackColor:");

	btn_black = lv_btn_create(cont_setting);
	booksetting_obj.btn_bg_black = lv_obj_get_child_id(btn_black); //记录控件ID
	lv_obj_add_event_cb(btn_black, event_setting_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn_black, COLOR_BLACK, LV_PART_MAIN);
	lv_group_remove_obj(btn_black);
	lv_obj_align_to(btn_black, label_back, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	lv_obj_set_user_data(btn_black, (void*)COLOR_BLACK_ID);

	btn_white = lv_btn_create(cont_setting);
	booksetting_obj.btn_bg_white = lv_obj_get_child_id(btn_white);
	lv_obj_add_event_cb(btn_white, event_setting_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn_white, COLOR_WHITE, LV_PART_MAIN);
	lv_group_remove_obj(btn_white);
	lv_obj_align_to(btn_white, btn_black, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	lv_obj_set_user_data(btn_white, (void*)COLOR_WHITE_ID);

	btn_eyecare = lv_btn_create(cont_setting);
	booksetting_obj.btn_bg_eyecare = lv_obj_get_child_id(btn_eyecare);
	lv_obj_add_event_cb(btn_eyecare, event_setting_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn_eyecare, COLOR_EYECARE, LV_PART_MAIN);
	lv_group_remove_obj(btn_eyecare);
	lv_obj_align_to(btn_eyecare, btn_white, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	lv_obj_set_user_data(btn_eyecare, (void*)COLOR_EYECARE_ID);

	//字体颜色
	label_font = lv_label_create(cont_setting);
	lv_obj_set_style_text_color(label_font, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label_font, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(label_font, "FontColor:");
	lv_obj_align_to(label_font, label_back, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	btn_black = lv_btn_create(cont_setting);
	booksetting_obj.btn_font_black = lv_obj_get_child_id(btn_black);
	lv_obj_add_event_cb(btn_black, event_setting_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn_black, COLOR_BLACK, LV_PART_MAIN);
	lv_group_remove_obj(btn_black);
	lv_obj_align_to(btn_black, label_font, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	lv_obj_set_user_data(btn_black, (void*)COLOR_BLACK_ID);

	btn_white = lv_btn_create(cont_setting);
	booksetting_obj.btn_font_white = lv_obj_get_child_id(btn_white);
	lv_obj_add_event_cb(btn_white, event_setting_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn_white, COLOR_WHITE, LV_PART_MAIN);
	lv_group_remove_obj(btn_white);
	lv_obj_align_to(btn_white, btn_black, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	lv_obj_set_user_data(btn_white, (void*)COLOR_WHITE_ID);
}

void gui_book_init(lv_obj_t* root)
{
	page_self_root = root;
	
	//文本显示
	label_txt = lv_label_create(page_self_root);
	lv_group_add_obj(lv_group_get_default(), label_txt);
	lv_obj_add_event_cb(label_txt, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(label_txt, event_focus_handler, LV_EVENT_FOCUSED, NULL);  //失去焦点
	lv_obj_add_event_cb(label_txt, event_focus_handler, LV_EVENT_DEFOCUSED, NULL); //获取焦点
	lv_obj_set_size(label_txt, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_bg_opa(label_txt, LV_OPA_MAX, LV_PART_MAIN);
	if(device_cfg.font_zh_flag) lv_obj_add_style(label_txt, &font_zh_full_16, 0); //中文字体样式
	lv_label_set_text(label_txt, "");
	lv_obj_align(label_txt, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_set_style_bg_color(label_txt, get_color(device_cfg.book_bg_color), LV_PART_MAIN); //初始化背景颜色
	lv_obj_set_style_text_color(label_txt, get_color(device_cfg.book_font_color), LV_PART_MAIN); //初始化字体颜色

	mbox_warning = lv_msgbox_create(page_self_root, "Warning!", "Please open a TXT file", NULL, true);
	lv_obj_add_flag(mbox_warning, LV_OBJ_FLAG_HIDDEN);
	lv_obj_center(mbox_warning);
	
	menu = lv_bookmenu_create(page_self_root);
	shelf = lv_bookshelf_create(page_self_root);
	book_setting_init();
	
	txt_buff = mymalloc(SRAMIN, TXT_BUFF_SIZE);
	book_info = mymalloc(SRAMIN, sizeof(BookInfo_t));
	memset(txt_buff, 0, TXT_BUFF_SIZE);
	memset(book_info, 0, sizeof(BookInfo_t));
}

void gui_book_focus(void)
{
	lv_group_focus_obj(page_self_root);
	
	//文件浏览
	file_manager->select_cb = file_select_cb;
	file_manager->close_cb = file_close_cb;
	file_manager->display(DISABLE);
	file_manager->select_once = true;
	file_manager->pfilter = ".TXT";
	file_manager->re_focus_obj = label_txt;
	
	//功能菜单
	menu->select = menu_select_cb;
	menu->re_focus_obj = label_txt;
	menu->display(DISABLE);
	
	//书架
	shelf->select = shelf_select_cb;
	shelf->close = shelf_close_cb;
	shelf->re_focus_obj = label_txt;
	shelf->display(ENABLE);
	
	//状态栏
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = status_bar_hidecb;
	status_bar->title_set(app_names[PAGE_APP_BOOK - 3]);
}



