#include "os_task.h"
#include "string.h"
#include "malloc.h"

#define TASK_QUEUE_MAX 6

list_t *os_tasks;
static QueueHandle_t task_queue;

#define TASK_DETECT_PRIO				1
#define TASK_DETECT_STACK_SIZE	512
static TaskHandle_t TaskDetect_Handler;
static void detect_task(void *pvParameters);

static list_node_t* find_ots(OTS_t* ots)
{
	OTS_t *p_ots = NULL;
	list_node_t* p_node = NULL;
	if(os_tasks == NULL) return p_node;
	
	list_iterator_t *it = list_iterator_new(os_tasks, LIST_HEAD);
	while ((p_node = list_iterator_next(it))) {
		p_ots = p_node->val;
		if(!strcmp(p_ots->taskName, ots->taskName))
		{
			Debug("find %s\n", p_ots->taskName);
			break;
		}
		else
		{
			p_node = NULL;
		}
	}
	
	myfree(SRAMIN, it);
	return p_node;
}

void detect_task(void *pvParameters)
{	
	BaseType_t x_status;
	BaseType_t xReturn;
	eTaskState task_state;
	list_node_t *p = NULL;
	for(;;)
	{
		OTS_t *p_ots_temp = mymalloc(SRAMIN, sizeof(OTS_t));
		x_status = xQueueReceive(task_queue, p_ots_temp, portMAX_DELAY);
		if(x_status == pdPASS && find_ots(p_ots_temp) == NULL)
		{
			taskENTER_CRITICAL();
			
			list_rpush(os_tasks, list_node_new(p_ots_temp));
			xReturn = xTaskCreate((TaskFunction_t )p_ots_temp->taskCode,     
								(const char*    )p_ots_temp->taskName,   
								(uint16_t       )p_ots_temp->taskStackSize,
								(void*          )p_ots_temp->taskParameters,
								(UBaseType_t    )p_ots_temp->taskPriority,
								(TaskHandle_t*  )p_ots_temp->taskHandler);
								
			if(xReturn == pdPASS)
			{
				Debug("os task create\n");
			}
			
			taskEXIT_CRITICAL();
		}
		else
		{
			Debug("os task created\n");
		}
	
		vTaskDelay(5);
	}

}

void os_task_init()
{
	BaseType_t xReturn;
	os_tasks = list_new();
	
	//创建任务队列
	task_queue = xQueueCreate(TASK_QUEUE_MAX, sizeof(OTS_t));
	
	//创建添加检测任务
	xReturn = xTaskCreate((TaskFunction_t )detect_task,     
					(const char*    )"task_detect",   
					(uint16_t       )TASK_DETECT_STACK_SIZE,
					(void*          )NULL,
					(UBaseType_t    )TASK_DETECT_PRIO,
					(TaskHandle_t*  )TaskDetect_Handler);
					
	if(xReturn == pdPASS)
	{
		Debug("detect task create ok\n");
	}
} 

void os_task_add(OTS_t ots)
{
	//发送到待创建的任务队列
	if(xQueueSend(task_queue, &ots, 10) != pdPASS)
	{
		Debug("task send failed\n");
	}
	else
	{
		Debug("task send success\n");
	}
}

void os_task_start(OTS_t* ots)
{
	if(os_tasks == NULL) return;
	eTaskState task_state;
	list_node_t* p_node = find_ots(ots);
	if(p_node == NULL)
	{
		os_task_add(*ots);
		return;
	}
	
	task_state = eTaskGetState(*(ots->taskHandler));
	if(task_state == eSuspended || task_state == eReady)
	{
		taskENTER_CRITICAL();
		Debug("start: %s\n", ots->taskName);
		vTaskResume(*(ots->taskHandler));
		taskEXIT_CRITICAL();
	}
	else if(task_state == eDeleted)
	{
		myfree(SRAMIN, p_node->val);
		list_remove(os_tasks, p_node);
		Debug("deleted: %s\n", ots->taskName);
		
		os_task_add(*ots); //重新添加任务
	}
	
}

void os_task_stop(OTS_t* ots)
{

	eTaskState task_state;
	if(os_tasks == NULL || find_ots(ots) == NULL) return;
	
	task_state = eTaskGetState(*(ots->taskHandler));
	
	Debug("stop: %s\n", ots->taskName);
	vTaskSuspend(*(ots->taskHandler));
}







