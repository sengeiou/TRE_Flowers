#ifndef _OS_TASK_H
#define _OS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list_c.h"
#include "stdio.h"
#include "semphr.h"
#include "sys.h"

typedef struct OsTaskStruct
{
	TaskFunction_t	taskCode;
	const char*			taskName;
	uint16_t				taskStackSize;
	void*						taskParameters;
	UBaseType_t			taskPriority;
	TaskHandle_t*		taskHandler;
}OTS_t;

void os_task_init();
void os_task_add(OTS_t ots);
void os_task_start(OTS_t* ots);
void os_task_stop(OTS_t* ots);

#endif
