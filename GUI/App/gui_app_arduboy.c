#include "gui_app_arduboy.h"
#include "sys.h"
#include "arduboy.h"
#include "ArduboyButtons.h"
#include "SnakePort.h"
#include "FlappyBall.h"
#include "Dino.h"
#include "player.h"

const char* game_names[] = {
    "ButtonTest","Snake","FlappyBall","Dino"
};

lv_obj_t* list_game;
static uint16_t *arduboy_img;
static uint8_t id_game;
static OTS_t ots_arduboy;

#define TASK_ARDUBOY_PRIO				2
#define TASK_ARDUBOY_STACK_SIZE	256
static TaskHandle_t TaskArduboy_Handler; 
static void arduboy_task(void *pvParameters);

static bool exit_flag = false;

static SemaphoreHandle_t xSemaphoreArduboy = NULL;

/*************************************************************************/
static void arduboy_update(const unsigned char *image);
static void bg_set_state(uint8_t state);

/*************************************************************************/
static void anim_ready_cb(lv_anim_t* a)
{
	lv_anim_del(a->var, NULL);
	if(exit_flag)
		page_call_handler((void*)PAGE_APPS, DEL);
	else
		xSemaphoreGive(xSemaphoreArduboy);
}

static void bg_set_state(uint8_t state)
{
	lv_anim_t anim;
	
	if(state == ENABLE)
	{
		//游戏退出,显示控件
		status_bar->state_set(STATUS_BAR, ENABLE);
		lv_obj_set_style_bg_color(page_self_root, lv_color_white(), LV_PART_MAIN);
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, NULL, 300, 0, 100, list_game, GUI_HEIGHT, 0, 0);
	}
	else
	{
		//游戏运行，隐藏控件
		status_bar->state_set(STATUS_BAR, DISABLE);
		if(!exit_flag) lv_obj_set_style_bg_color(page_self_root, lv_color_black(), LV_PART_MAIN);
		
		ANIM_LINE_ADD(&anim, anim_y_cb, lv_anim_path_overshoot, anim_ready_cb, 300, 0, 100, list_game, 0, GUI_HEIGHT, 0);
	
	}
	lv_anim_start(&anim);
}

static void arduboy_task(void *pvParameters)
{
	for(;;)
	{
		BaseType_t err = pdFALSE;
		err = xSemaphoreTake(xSemaphoreArduboy, portMAX_DELAY);
		
		if(err == pdTRUE)
		{				
			arduboy_img = mymalloc(SRAMEX,(256*128*2));
			if(arduboy_img) 
				memset(arduboy_img, 0, 65536);
			
			gui_task_state(DISABLE);
			
			arduboy.begin();
			arduboy.setFrameRate(15);
			arduboy.bootLogo();
			Debug("Arduboy init!\n");
			
			switch(id_game)
			{
				case 0:
					arduboy_btn_init();
					break;
				case 1:
					snake_init();
					break;
				case 2:
					flappyball_init();
					break;
				case 3:
					dino_init();
					break;
			}
			
			Debug("Arduboy run!\n");
			
			for(;;)
			{
				if(KEY_EXIT_VAL == KEY_EXIT_EN) break;
				switch(id_game)
				{
					case 0:
						arduboy_btn_loop();
						break;
					case 1:
						snake_loop();
						break;
					case 2:
						flappyball_loop();
						break;
					case 3:
						dino_loop();
						break;
				}
				
				vTaskDelay(1);
			}
			
			Debug("Arduboy exit!\n");
			lv_obj_invalidate(page_self_root);
			
			gui_task_state(ENABLE);
			bg_set_state(ENABLE);
			
			if(arduboy_img) myfree(SRAMEX, arduboy_img);
		}
		
		vTaskDelay(5);
	}

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
				id_game = lv_obj_get_index(obj);
				bg_set_state(DISABLE);
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				exit_flag = true;
				bg_set_state(DISABLE);
			}
			else if(key == LV_KEY_HOME)
			{

			}
			else if(key == LV_KEY_END)
			{
				
			}
			
		}	
			break;
	}
}

void gui_arduboy_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	list_game = lv_list_create(page_self_root);
	lv_gridnav_add(list_game, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), list_game);
	lv_obj_set_size(list_game, lv_pct(90), lv_pct(70));
	lv_obj_align(list_game, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_bg_color(list_game, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);

	uint32_t i;
	uint8_t len = sizeof(game_names) / sizeof(game_names[0]);
	for (i = 0; i < len; i++) {
		lv_obj_t* item = lv_list_add_btn(list_game, LV_SYMBOL_SHUFFLE, game_names[i]);
		lv_obj_add_event_cb(item, event_key_handler, LV_EVENT_KEY, NULL);
		lv_obj_set_style_bg_opa(item, 0, 0);
		lv_group_remove_obj(item);   
	}
	
	//Arduboy Init
	arduboy.delayHandler = vTaskDelay;
	arduboy.paintHandler = arduboy_update;
	
	//OS Task
	if(ots_arduboy.taskHandler == NULL)
	{
		xSemaphoreArduboy = xSemaphoreCreateBinary();
		
		ots_arduboy.taskCode = arduboy_task;
		ots_arduboy.taskName = "arduboy";
		ots_arduboy.taskStackSize = TASK_ARDUBOY_STACK_SIZE;
		ots_arduboy.taskParameters = NULL;
		ots_arduboy.taskPriority = TASK_ARDUBOY_PRIO;
		ots_arduboy.taskHandler = &TaskArduboy_Handler;
		
		os_task_start(&ots_arduboy);
	}

}

void gui_arduboy_focus(void)
{
	lv_group_focus_obj(list_game);
	exit_flag = false;
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->title_set(app_names[PAGE_APP_ARDUBOY - 3]);
	
	bg_set_state(ENABLE);
	
}

/**
 *@name: arduboy_update
 *@desc: arduboy 数据填充 
 *@parem: image 帧缓冲区
 *@return: 
 */
static void arduboy_update(const unsigned char *image)
{
	static BaseType_t err = pdFALSE;
	
	//放大2倍
	//256x128
	uint16_t len = arduboy.width() * arduboy.height() / 8;
	uint16_t row_index = 0;
	uint16_t index = 0;
	memset(arduboy_img, 0x00, 65536);
	
	for(int i=0;i<len;i+=arduboy.width())
	{
		for(int col=0;col<arduboy.width();col++)
		{
			uint16_t index = i+col;
			uint8_t tmp = *(image + index);
			uint8_t bit = 0;
			for(int row=0;row<8;row++)
			{
				bit = (tmp & (1 << row)) >> row;

				index = ((row_index + (row<<1)) * (arduboy.width()<<1) + (col<<1) + (row<<1) + row_index);
				index = (row_index >= 64) ? (index - 64) : index;
				
				arduboy_img[index] = bit == 0 ? FLIPW(BLACK) : FLIPW(GREEN);
				arduboy_img[index+1] = arduboy_img[index];
			}
		}
		row_index+=16;
	}
	
	//1/2帧发送
	LCD_DisplayWindows(30,56,286,119,arduboy_img);
	vTaskDelay(10);
	LCD_DisplayWindows(30,120,286,184,arduboy_img+16384);
}




