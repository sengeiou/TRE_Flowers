#include "gui_port.h"
#include "gui_page.c"
#include "key.h"
#include "malloc.h"

#define TASK_GUI_HANDLE_PRIO				3
#define TASK_GUI_HANDLE_STACK_SIZE	1024
static TaskHandle_t TaskGui_Handler;
void gui_handle_task(void *pvParameters);

#define TASK_GUI_TICK_PRIO				3
#define TASK_GUI_TICK_STACK_SIZE	1024
static TaskHandle_t TaskGuiTick_Handler; 
void gui_tick_task(void *pvParameters);

static OTS_t ots_gui_handle;
static OTS_t ots_gui_tick;

static list_t* page_list;

SemaphoreHandle_t lvgl_mutex = NULL;
page_call_handle page_call_handler;

FileExplorer_t* file_manager;
StatusBar_t* status_bar;

lv_style_t font_zh_full_16;

static void lvgl_init()
{
	lv_init(); 							//lvgl 系统初始化
	lv_port_disp_init();		//lvgl 显示接口初始化
	lv_port_indev_init();		//lvgl 输入接口初始化，此处仅使用按键
	lv_group_set_default(group); //设置默认组, lv_port_indev.c
	
	dma_spi1_handler = disp_flush_lvglcb; //DMA传输中断
	key_delay_set(gui_delay);
}

static list_node_t* page_find(uint8_t id)
{
	PageStruct_t* p_page = NULL;
	list_node_t* p_node = NULL;
	list_iterator_t *it = list_iterator_new(page_list, LIST_HEAD);
	while ((p_node = list_iterator_next(it))) {
		p_page = p_node->val;
		if(id == p_page->page_id) break;
	}
	myfree(SRAMIN, it);
	
	return p_node;
}

static void page_remove(uint8_t id)
{
	if(page_list == NULL) return;
	list_node_t* node = page_find(id);
	//myfree(SRAMIN, node->val);
	list_remove(page_list, node);
}

static void page_add(PageStruct_t* page)
{
	if(page_list == NULL) return;
	list_node_t* node = page_find(page->page_id);
	if(node != NULL) 
	{
		Debug("page is added!\n");
		return;
	}
	
	node = list_rpush(page_list, list_node_new(page));
	if(node != NULL)
		Debug("page add!\n");
}

static PageStruct_t* page_create(uint8_t id)
{
	PageStruct_t* page = NULL;
	list_node_t* node = page_find(id);
	if(node != NULL)
	{
		page = node->val;
		
		page->root = lv_obj_create(NULL);
		lv_obj_set_size(page->root, GUI_WIDTH, GUI_HEIGHT);
		lv_obj_set_style_bg_color(page->root, lv_color_white(), LV_PART_MAIN);
		//lv_obj_set_style_radius(page->root, LV_RADIUS_CIRCLE, 0);
		lv_obj_clear_flag(page->root, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_center(page->root);
		
		page->init_handler(page->root);
	}
	
	return page;
}

/**
 *@页面切换回调
 *@param:arg 页面ID
 *@param:del 页面内存释放标志
 */
static void page_callback(void* arg, bool del)
{
	static uint8_t last_id = 0;
	uint8_t id = (uint8_t)arg;
	PageStruct_t *page;
	lv_obj_t *root;
	
	Debug("old:%d, new: %d\n", last_id, id);
	page = page_find(last_id)->val;
	
	if(page == NULL) return;
	if(del && page->root != NULL)
	{
		lv_anim_del_all();
		lv_obj_del_async(page->root);
		page->root = NULL;
	}
	
	page = page_find(id)->val;
	if(page == NULL) return;
	
	if(page->root == NULL)
	{
		Debug("page create\n");
		page_create(id);
	}
	root = page->root;
	if(root == NULL) return;
	
	//设置父控件
	status_bar->parent_set(root);
	file_manager->parent_set(root);
	
	lv_scr_load_anim_t load_dir = LV_SCR_LOAD_ANIM_MOVE_LEFT;
	
	if(id == PAGE_MAIN)
	{
		load_dir = LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
	}
	else if(id == PAGE_APPS)
	{
		load_dir = LV_SCR_LOAD_ANIM_MOVE_LEFT;
	}
	else
	{
		load_dir = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
	}
	
	if(root) lv_scr_load_anim(root, load_dir, 150, 50, false);
	
	page->focus_handler();
	last_id = page->page_id;
}

static void page_init()
{
	page_list = list_new();
	if(page_list == NULL) GUI_ASSERT
		
	//注册页面回调
	page_call_handler = page_callback;
	
	//样式初始化
	lv_style_init(&font_zh_full_16);
	if(device_cfg.font_zh_flag) lv_style_set_text_font(&font_zh_full_16, &songti_zh_16);
	
	//组件初始化
	file_manager = file_explorer_instance();
	status_bar = status_bar_instance();

	//添加页面
	page_add(&page_logo);
	page_add(&page_main);
	page_add(&page_apps);
	page_add(&page_about);
	page_add(&page_book);
	page_add(&page_arduboy);
	page_add(&page_nes);
	page_add(&page_gps);
	page_add(&page_setting);
	page_add(&page_calc);
	page_add(&page_calendar);
	
	//主页和LOGO页面创建
	PageStruct_t* page = page_create(PAGE_MAIN);
	if(page == NULL) Debug("page main create failed\n");
	page = page_create(PAGE_LOGO);
	if(page != NULL)
	{
		lv_scr_load(page->root);
		page->focus_handler();
	}
	else
	{
		Debug("page logo create failed\n");
	}
}

static void gui_memlog_task(lv_timer_t *timer)
{
	lv_read_mem(NULL);
}

void gui_port_init()
{
#if CONFIG_ENABLE_GUI
	lvgl_init();
	page_init();
	
	lvgl_mutex = xSemaphoreCreateMutex();
	
	ots_gui_handle.taskCode = gui_handle_task;
	ots_gui_handle.taskName = "gui_handle";
	ots_gui_handle.taskStackSize = TASK_GUI_HANDLE_STACK_SIZE;
	ots_gui_handle.taskParameters = NULL;
	ots_gui_handle.taskPriority = TASK_GUI_HANDLE_PRIO;
	ots_gui_handle.taskHandler = &TaskGui_Handler;
	
	ots_gui_tick.taskCode = gui_tick_task;
	ots_gui_tick.taskName = "gui_tick";
	ots_gui_tick.taskStackSize = TASK_GUI_TICK_STACK_SIZE;
	ots_gui_tick.taskParameters = NULL;
	ots_gui_tick.taskPriority = TASK_GUI_TICK_PRIO;
	ots_gui_tick.taskHandler = &TaskGuiTick_Handler;
	
	gui_task_state(ENABLE);
	
#if CONFIG_LOG_GUI_MEM
	lv_timer_t* lv_timer = lv_timer_create(gui_memlog_task, 1000, NULL);
#endif

#endif

}

void gui_handle_task(void *pvParameters)
{
	for(;;)
	{
//		xSemaphoreTake( lvgl_mutex, portMAX_DELAY );
		lv_task_handler();
//		xSemaphoreGive( lvgl_mutex );
		vTaskDelay(5);
	}
}

void gui_tick_task(void *pvParameters)
{
	for(;;)
	{
//		xSemaphoreTake( lvgl_mutex, portMAX_DELAY );
		lv_tick_inc(2);
//		xSemaphoreGive( lvgl_mutex );
		vTaskDelay(2);
	}
}

void gui_task_state(uint8_t state)
{
	if(state == ENABLE)
	{
		os_task_start(&ots_gui_handle);
		os_task_start(&ots_gui_tick);
		
	}
	else
	{
		os_task_stop(&ots_gui_tick);
		os_task_stop(&ots_gui_handle);
	}
}
