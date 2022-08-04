#include "com_book_shelf.h"
#include "devices.h"

static BookShelf_t* book_shelf;
static lv_obj_t* label_name;
static lv_obj_t* cont_shelf;
static BookInfo_t* info;

/**************************************************************/
static void event_key_handler(lv_event_t *e);
static void shelf_add_book(uint8_t id);


/**************************************************************/
static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	
	if(!book_shelf->is_show)
	{
		//关闭
		if(book_shelf->re_focus_obj != NULL) lv_group_focus_obj(book_shelf->re_focus_obj);
		if(book_shelf->close != NULL) book_shelf->close();
	}

	if(lv_obj_has_flag(book_shelf->shelf, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(book_shelf->shelf, LV_OBJ_FLAG_HIDDEN);
}

/**
 *@name shelf_search_bookinfo
 *@desc 查找书籍信息地址
 *@param pinfo 待比较的书籍信息指针
 *@param pid 查找到的书籍序号id
 *@return addr 查找到的地址
 */
static uint16_t shelf_search_bookinfo(BookInfo_t* pinfo, uint8_t* pid)
{
	uint16_t addr = 0;
	uint16_t info_size = sizeof(BookInfo_t);
	uint16_t len = info_size * BOOK_COUNT_MAX;
	bool find_blank_block = true; //如果没有查找到已存储记录，则查找未存储区块
	*pid = 0;
	BookInfo_t* info_temp = mymalloc(SRAMCCM, info_size);
	if(info_temp == NULL) return addr;
	memset(info_temp, 0, info_size);
	
	for(addr = EEPROM_BOOK_BASE_ADDR; addr < len; addr += info_size)
	{
		at24_read(addr, (uint8_t*)info_temp, info_size, 1000);
		if(strcmp(pinfo->file_name, info_temp->file_name) == 0)
		{
			//查找到已保存记录
			pinfo->set_flag = BOOK_INFO_SET;
			find_blank_block = false;
			break;
		}		
		(*pid)++;
	}
	
	if(find_blank_block)
	{
		*pid = 0;
		memset(info_temp, 0, info_size);
		for(addr = EEPROM_BOOK_BASE_ADDR; addr < len; addr += info_size)
		{
			at24_read(addr, (uint8_t*)info_temp, info_size, 1000);
			if(info_temp->set_flag != BOOK_INFO_SET)
			{
				//查找到未保存的存储区块
				break;
			}
			
			(*pid)++;
		}
	}
	
	myfree(SRAMCCM, info_temp);
	return addr;
}

/**
 *@name bookinfo_set
 *@desc 保存书籍信息
 *@param pinfo 书籍信息指针
 *@id 书籍序号id
 *@return
 */
static void bookinfo_set(BookInfo_t* pinfo, uint8_t id)
{
	uint16_t info_size = sizeof(BookInfo_t);
	uint16_t addr = (id * info_size);
	//写入信息
	at24_write(addr, (uint8_t*)pinfo, info_size, 1000);
}

/**
 *@name bookinfo_set
 *@desc 读取书籍信息
 *@id 书籍序号id
 *@return
 */
static void bookinfo_get(uint8_t id)
{
	uint16_t addr = id * sizeof(BookInfo_t);
	
	memset(info, 0, sizeof(BookInfo_t));
	at24_read(addr, (uint8_t*)info, sizeof(BookInfo_t), 1000);
}

static void shelf_add_book(uint8_t id)
{
	lv_obj_t* btn;
	lv_obj_t* label;
	
	btn = lv_btn_create(cont_shelf);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	
	Debug("book id: %d\n", id);
	lv_obj_set_user_data(btn, (void*)id); //设置编号

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_FILE);
	lv_obj_center(label);
}

static void shelf_del_book(uint8_t id)
{
	Debug("book del: %d\n", id);
	BookInfo_t* del = mymalloc(SRAMCCM, sizeof(BookInfo_t));
	memset(del, 0, sizeof(BookInfo_t));
	bookinfo_set(del, id);
	
	myfree(SRAMCCM, del);
	Debug("book del success\n")
;}

static void shelf_save(BookInfo_t* info)
{
	uint16_t info_addr;
	uint8_t info_id;
	uint8_t shelf_book_count;
	if(info == NULL) return;

#if 1
	Debug("save page: %d\n", info->read_page);
	Debug("save path: %s\n", info->file_path);
	Debug("save name: %s\n", info->file_name);
#endif
	
	info_addr = shelf_search_bookinfo(info, &info_id);
	Debug("book info[addr, id]: %d, %d\n", info_addr, info_id);
	
	shelf_book_count = lv_obj_get_child_cnt(cont_shelf);
	if(shelf_book_count >= BOOK_COUNT_MAX)
	{
		//达到保存最大数量
		Debug("book save ==> overflow\n");
		return;
	}
	
	//根据写入标志判断是更新还是添加
	if(info->set_flag == BOOK_INFO_SET)
	{
		//已保存的书籍，更新操作
		Debug("book save ==> update\n");
	}
	else
	{
		//未保存的书籍，添加操作
		Debug("book save ==> add\n");
		info->set_flag = BOOK_INFO_SET; //设置写入标志
		shelf_add_book(info_id); //添加书籍控件
	}
	bookinfo_set(info, info_id); //(写入||更新)

	Debug("book save success\n");
}

static void shelf_item_switch(lv_obj_t* obj)
{
	uint8_t id;
	for (int i = 0; i < lv_obj_get_child_cnt(obj); i++)
	{
		lv_obj_t* child = lv_obj_get_child(obj, i);
		if (lv_obj_has_state(child, LV_STATE_FOCUSED))
		{
			id = (uint8_t)lv_obj_get_user_data(child);
			bookinfo_get(id);
			
			//设置标签名称
			lv_label_set_text(label_name, info->file_name);
			
			break;
		}
	}
}

/**
 *@name shelf_update
 *@desc 书架初始化
 */
static void shelf_init()
{
	uint8_t id = 0;
	uint16_t addr = 0;
	uint16_t info_size = sizeof(BookInfo_t);
	uint16_t len = info_size * BOOK_COUNT_MAX;
	BookInfo_t* book = mymalloc(SRAMIN, info_size);
	if(book == NULL) return;
	memset(book, 0, info_size);
	
	for(addr = EEPROM_BOOK_BASE_ADDR; addr < len; addr += info_size)
	{
		at24_read(addr, (uint8_t*)book, info_size, 1000);
		if(book->set_flag == BOOK_INFO_SET)
		{
			//添加书籍控件
			shelf_add_book(id);
		}
		
		id++;
	}
	
	myfree(SRAMIN, book);
}

static void shelf_destroy()
{
	myfree(SRAMCCM, book_shelf);
	myfree(SRAMCCM, info);
}

static void shelf_display(uint8_t state)
{
	lv_anim_t anim;
	if(book_shelf->shelf == NULL) return;
	
	if(state)
	{
		if(book_shelf->is_show) return;
		lv_group_focus_obj(cont_shelf);
		lv_obj_clear_flag(book_shelf->shelf, LV_OBJ_FLAG_HIDDEN);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 200, book_shelf->shelf, GUI_HEIGHT, 35, 0);
	}
	else
	{
		if(!(book_shelf->shelf)) return;
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 50, book_shelf->shelf, 35, GUI_HEIGHT, 0);
	}
	
	book_shelf->is_show = state;
	lv_anim_start(&anim);
}

static void event_key_handler(lv_event_t *e)
{	
	uint8_t obj_id;
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			if(key == LV_KEY_ENTER)
			{

			}
			else if(key == LV_KEY_BACKSPACE)
			{
				book_shelf->display(DISABLE);
			}
			else if(key == LV_KEY_LEFT || key == LV_KEY_RIGHT || key == LV_KEY_UP || key == LV_KEY_DOWN)
			{
				if(obj == cont_shelf)
				{
					shelf_item_switch(obj); //读取当前选择书籍信息更新
				}
			}
			else if(key == LV_KEY_HOME)
			{
				//删除书籍
				if(obj != cont_shelf)
				{
					obj_id = (uint8_t)lv_obj_get_user_data(obj);
					shelf_del_book(obj_id);
					lv_obj_del(obj);
				}
			}
			else if(key == LV_KEY_END)
			{
				//读取文件记录，打开书籍
				if(obj != cont_shelf)
				{
					//发送当前书籍信息打开
					if(info->set_flag)
					{
						if(book_shelf->select != NULL) book_shelf->select(info); //选择文件回调
						book_shelf->display(DISABLE);
					}

				}
			}
			
		}
			break;
	}
}

BookShelf_t* lv_bookshelf_create(lv_obj_t* parent)
{
	lv_obj_t* btn;
	lv_obj_t* label;
	
	book_shelf = mymalloc(SRAMCCM, sizeof(BookShelf_t));
	info = mymalloc(SRAMCCM, sizeof(BookInfo_t));
	memset(book_shelf, 0, sizeof(BookShelf_t));
	memset(info, 0, sizeof(BookInfo_t));
	
	book_shelf->display = shelf_display;
	book_shelf->destroy = shelf_destroy;
	book_shelf->save = shelf_save;
	
	book_shelf->shelf = lv_obj_create(parent);
	lv_obj_set_size(book_shelf->shelf, LV_PCT(95), 200);
	lv_obj_set_style_pad_all(book_shelf->shelf, 5, 0);
	lv_obj_align(book_shelf->shelf, LV_ALIGN_TOP_MID, 0, 35);
	
	//书架
	cont_shelf = lv_obj_create(book_shelf->shelf);
	lv_obj_set_size(cont_shelf, LV_PCT(100), LV_PCT(85));
	lv_obj_set_style_pad_all(cont_shelf, 0, 0);
	lv_obj_set_style_border_opa(cont_shelf, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_flex_flow(cont_shelf, LV_FLEX_FLOW_ROW_WRAP);
	
	lv_gridnav_add(cont_shelf, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), cont_shelf);
	lv_obj_add_event_cb(cont_shelf, event_key_handler, LV_EVENT_KEY, NULL);
	
	//书籍名称
	label_name = lv_label_create(book_shelf->shelf);
	lv_obj_set_size(label_name, LV_PCT(60), LV_SIZE_CONTENT);
	lv_obj_add_style(label_name, &font_zh_full_16, 0); //中文样式
	lv_obj_set_style_text_color(label_name, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_text_align(label_name, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
	lv_label_set_long_mode(label_name, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(label_name, "");
	lv_obj_align(label_name, LV_ALIGN_BOTTOM_MID, 0, 0);
	
	//删除(KEY_HOME)
	label = lv_label_create(book_shelf->shelf);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
	lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(label, "Del");
	lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 5, 0);
	
	//打开(KEY_END)
	label = lv_label_create(book_shelf->shelf);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
	lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_label_set_text(label, "Open");
	lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, -5, 0);
	
	//从EEPROM读取书籍记录初始化
	shelf_init();
	shelf_item_switch(cont_shelf);
	
	return book_shelf;
}
