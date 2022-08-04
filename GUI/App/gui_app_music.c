#include "gui_app_music.h"
#include "player.h"
#include "fatfs.h"
#include "task.h"

#define TASK_MP3_HANDLE_PRIO				1
#define TASK_MP3_HANDLE_STACK_SIZE	256
static TaskHandle_t TaskMp3_Handler;
static void mp3_task(void *pvParameters);

static OTS_t ots_music;
SemaphoreHandle_t xSemaphoreMusic = NULL; 

static void time_task(lv_timer_t *timer);

//记录控件ID
static struct OBJS
{
	uint16_t label_time:2;
	uint16_t btn_file:2;
	uint16_t btn_exit:2;
	uint16_t btn_prev:3;
	uint16_t btn_next:3;
	uint16_t btn_play:4;
	uint16_t label_file:3;
	uint16_t btn_volume:4;
	uint16_t btn_mode:4;
}objs;

static lv_obj_t* img_ico;
static lv_anim_t anim_ico; 

static lv_timer_t* lv_timer;

static PlayState_t* play_state;

/************************************************************/
static lv_obj_t* find_obj(uint8_t id);
static void play_state_set(uint8_t state);

/************************************************************/
static void player_end()
{
	lv_timer_pause(lv_timer);
	play_state_set(DISABLE);
}

static void play_state_init()
{

}

static void play_state_set(uint8_t state)
{
	lv_obj_t* obj;
	if(page_self_root == NULL) return;
	
	obj = find_obj(objs.btn_play);
	obj = lv_obj_get_child(obj, 0);
	
	if(state)
	{	
		MP3_Resume();
		
		lv_label_set_text(obj, LV_SYMBOL_PAUSE); 
		if(play_state && play_state->file_name) lv_label_set_text(find_obj(objs.label_file), play_state->file_name);
		lv_anim_start(&anim_ico);
	}
	else
	{
		MP3_Pause();
		lv_label_set_text(obj, LV_SYMBOL_PLAY); 
		lv_anim_del(img_ico, NULL);
	}
}

static void statusbar_hide_cb()
{
	if(isPlaying) MP3_Stop();
	
	page_self_root = NULL;
	//停止播放任务
	play_state_set(DISABLE);
	//释放内存
	myfree(SRAMCCM, play_state);
	//删除定时任务
	lv_timer_del(lv_timer);
	page_call_handler((void*)PAGE_APPS, DEL); 
}

static void file_select_cb(const char* name, const char* path)
{
	lv_obj_t* obj;
	obj = find_obj(objs.label_file);
	
	lv_label_set_text(obj, name);
	mp3Path = path;
	play_state->file_name = name;
	
	
	if(isFileOpen) MP3_Stop();
	xSemaphoreGive(xSemaphoreMusic);
}

static void file_close_cb()
{

}

static lv_obj_t* find_obj(uint8_t id)
{
	return lv_obj_get_child(page_self_root, id);
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
				if(obj == find_obj(objs.btn_file))
				{
					file_manager->display(ENABLE); //打开文件浏览弹窗
				}
				else if(obj == find_obj(objs.btn_exit))
				{	
					status_bar->state_set(STATUS_BAR, DISABLE); 
				}
				else if(obj == find_obj(objs.btn_play))
				{	

					if(isPlaying)
					{//Pause
						lv_timer_pause(lv_timer);
						play_state_set(DISABLE);
					}
					else
					{//Play
						lv_timer_resume(lv_timer);
						play_state_set(ENABLE);
					}
					
					if(!isFileOpen)
					{
						xSemaphoreGive(xSemaphoreMusic);
					}

				}
				else if(obj == find_obj(objs.btn_volume))
				{
				
				}
				else if(obj == find_obj(objs.btn_mode))
				{
					
				}
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				if(obj == page_self_root)
					status_bar->state_set(STATUS_BAR, DISABLE); 
			}
			else if(key == LV_KEY_HOME || key == LV_KEY_LEFT)
			{
				
			}
			else if(key == LV_KEY_END || key == LV_KEY_RIGHT)
			{
				
			}
			
		}	
			break;
	}
}

static void mp3_task(void *pvParameters)
{
	BaseType_t err = pdFALSE;
	
	for(;;)
	{
		err = xSemaphoreTake(xSemaphoreMusic, portMAX_DELAY);
		
		if(err == pdTRUE)
		{	
			Debug("play begin\n");
			
			if(MP3_Play(mp3Path))
			{
				if(play_state) memset(play_state, 0, sizeof(PlayState_t)); //重置
				lv_timer_resume(lv_timer); //开启定时任务
				
				play_state_set(ENABLE);
			}
			
			while(isFileOpen){
				if(!isPlaying) continue;
				while(HAL_GPIO_ReadPin(VS1053_DREQ_PORT, VS1053_DREQ_PIN) == GPIO_PIN_RESET) vTaskDelay(10);
				MP3_Feeder();
			}
			player_end();
			Debug("play end\n");
		}

		vTaskDelay(5);
	}

	vTaskDelete(NULL);
}

static void time_task(lv_timer_t *timer)
{
	if(!isPlaying) return;
	
	if(play_state->kbps == 0)
	{
		play_state->kbps = VS_Get_HeadInfo();
		Debug("%d\n", play_state->kbps);
	}
	else
	{
		//play_state->sec++;
		VS1053_GetDecodeTime(&(play_state->sec));
	}
	lv_label_set_text_fmt(find_obj(objs.label_time), "%02d:%02d/%d Kbps", play_state->sec/60, play_state->sec%60, play_state->kbps);
}


void gui_music_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_gridnav_add(page_self_root, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	lv_obj_t* btn;
	lv_obj_t* label;

	//音乐图标//0
	lv_obj_t* cont_img = lv_obj_create(page_self_root);
	lv_obj_set_size(cont_img, 80, 80);
	lv_obj_set_style_radius(cont_img, LV_PCT(50), LV_PART_MAIN);
	lv_obj_clear_flag(cont_img, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_align(cont_img, LV_ALIGN_TOP_LEFT, 15, 40);

	LV_IMG_DECLARE(ico_music);
	
	img_ico = lv_img_create(cont_img);
	lv_img_set_src(img_ico, &ico_music);
	//lv_img_set_pivot(img1, 0, 0);
	lv_obj_align(img_ico, LV_ALIGN_CENTER, 0, 0);
	
	lv_anim_init(&anim_ico); //图标旋转动画
	lv_anim_set_var(&anim_ico, img_ico);
	lv_anim_set_exec_cb(&anim_ico, set_img_angle);
	lv_anim_set_values(&anim_ico, 0, 3600);
	lv_anim_set_time(&anim_ico, 5000);
	lv_anim_set_repeat_count(&anim_ico, LV_ANIM_REPEAT_INFINITE);

	//时间//1
	label = lv_label_create(page_self_root);
	objs.label_time = lv_obj_get_index(label);
	lv_obj_set_size(label, LV_PCT(60), LV_SIZE_CONTENT);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_22, LV_PART_MAIN);
	lv_label_set_text(label, "00:00/0Kbps");
	lv_obj_align_to(label, cont_img, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

	//文件按钮//2
	btn = lv_btn_create(page_self_root);
	objs.btn_file = lv_obj_get_index(btn);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
		
	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_LIST);

	//退出按钮//3
	btn = lv_btn_create(page_self_root);
	objs.btn_exit = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
	
	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_CLOSE);

	//上一首//4
	btn = lv_btn_create(page_self_root);
	objs.btn_prev = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, -80, -45);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_PREV);

	//下一首//5
	btn = lv_btn_create(page_self_root);
	objs.btn_next = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 80, -45);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_NEXT);

	//播放暂停//6
	btn = lv_btn_create(page_self_root);
	objs.btn_play = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -45);
	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_PLAY);

	//文件名称//7
	label = lv_label_create(page_self_root);
	objs.label_file = lv_obj_get_index(label);
	lv_obj_set_size(label, LV_PCT(80), LV_SIZE_CONTENT);
	lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_style(label, &font_zh_full_16, 0);
	lv_label_set_text(label, "No file");
	lv_obj_align_to(label, btn, LV_ALIGN_OUT_TOP_MID, 0, -15);

	//声音//9
	btn = lv_btn_create(page_self_root);
	objs.btn_volume = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, -40, -5);
//	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_VOLUME_MAX);

	//模式//10
	btn = lv_btn_create(page_self_root);
	objs.btn_mode = lv_obj_get_index(btn);
	lv_obj_add_event_cb(btn, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 40, -5);
//	lv_obj_set_style_shadow_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_white(), LV_PART_MAIN);
	lv_group_remove_obj(btn);

	label = lv_label_create(btn);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_LOOP);
	
	//LVGL定时器
	lv_timer = lv_timer_create(time_task, 1000, NULL);
	lv_timer_pause(lv_timer);
		
	
	//状态记录结构体
	play_state = mymalloc(SRAMCCM, sizeof(PlayState_t)); //申请内存，用于记录文件名
	
	//OS任务
	if(ots_music.taskHandler == NULL)
	{
		xSemaphoreMusic = xSemaphoreCreateBinary();
		
		ots_music.taskCode = mp3_task;
		ots_music.taskName = "mp3";
		ots_music.taskStackSize = TASK_MP3_HANDLE_STACK_SIZE;
		ots_music.taskParameters = NULL;
		ots_music.taskPriority = TASK_MP3_HANDLE_PRIO;
		ots_music.taskHandler = &TaskMp3_Handler;

		os_task_start(&ots_music);
	}
}

void gui_music_focus(void)
{
	lv_group_focus_obj(page_self_root);
	lv_gridnav_set_focused(page_self_root, find_obj(objs.btn_play), LV_ANIM_ON);
	
	file_manager->select_cb = file_select_cb;
	file_manager->close_cb = file_close_cb;
	file_manager->select_once = true;
	file_manager->re_focus_obj = page_self_root;
	file_manager->pfilter = ".MP3"; 
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = statusbar_hide_cb;
	
}

