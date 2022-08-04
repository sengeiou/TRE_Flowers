#include "gui_app_nes.h"
#include "nes_main.h"
#include "player.h"

#define TASK_NES_PRIO				2
#define TASK_NES_STACK_SIZE	512
static TaskHandle_t TaskNES_Handler; 
static void nes_task(void *pvParameters);

static OTS_t ots_nes;
static SemaphoreHandle_t xSemaphoreNes = NULL;

char* nes_file_path;

static void nes_task(void *pvParameters)
{
	for(;;)
	{
		BaseType_t err = pdFALSE;
		err = xSemaphoreTake(xSemaphoreNes, portMAX_DELAY);
		
		if(err == pdTRUE)
		{
			Debug("Nes path: %s\n", nes_file_path);
			gui_task_state(DISABLE);
			uint8_t res = nes_load((uint8_t*)nes_file_path);
			if(res == 0) 
			{
				Debug("NES exit\n");
			}
			
			myfree(SRAMCCM, nes_file_path);
			nes_file_path = NULL;
			
			lv_obj_invalidate(page_self_root);
			gui_task_state(ENABLE);
			
			file_manager->display(ENABLE);
		}
		
		vTaskDelay(1);
	}

	vTaskDelete(*ots_nes.taskHandler);
}

static void status_bar_hidecb()
{
	page_call_handler((void*)PAGE_APPS, DEL);
}

static void file_select_cb(const char* name, const char* path)
{	
	nes_file_path = mymalloc(SRAMCCM, strlen(path));
	
	if(nes_file_path == NULL) return;
	strcpy(nes_file_path, path);
	
	xSemaphoreGive(xSemaphoreNes);
}

static void file_close_cb()
{
	status_bar->state_set(STATUS_BAR, DISABLE);
}

void gui_nes_init(lv_obj_t* root)
{
	page_self_root = root;
	
	//OS Task
	if(ots_nes.taskHandler == NULL)
	{
		xSemaphoreNes = xSemaphoreCreateBinary();
		
		ots_nes.taskCode = nes_task;
		ots_nes.taskName = "nes";
		ots_nes.taskStackSize = TASK_NES_STACK_SIZE;
		ots_nes.taskParameters = NULL;
		ots_nes.taskPriority = TASK_NES_PRIO;
		ots_nes.taskHandler = &TaskNES_Handler;
		
		os_task_start(&ots_nes);
	}
}

void gui_nes_focus(void)
{
	file_manager->select_cb = file_select_cb;
	file_manager->close_cb = file_close_cb;
	file_manager->display(ENABLE);
	file_manager->select_once = true;
	file_manager->pfilter = ".NES"; 
 	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = status_bar_hidecb;
	status_bar->title_set(app_names[PAGE_APP_NES - 3]);

}


