#include "atgm336h.h"
#include <stdbool.h>

#define TASK_GPS_PRIO				2
#define TASK_GPS_STACK_SIZE	1024
static TaskHandle_t TaskGPS_Handler;
static void gps_task(void *pvParameters);

static OTS_t ots_gps;
static SemaphoreHandle_t xSemaphoreGPS = NULL;

//NMEA
static int time;
static NMEA_status status;
static NMEA_data *gps_data;
bool gps_run_flag = false;

void stop_measure_speed_CB(void){
	time = nmea_data.UTC_time - time;
	NMEA_CB_unregister(SPEED_RISE_BARRIER_CB);
}

void start_measure_speed_CB(void){
	time = nmea_data.UTC_time;
	NMEA_CB_register(&stop_measure_speed_CB, SPEED_RISE_BARRIER_CB, 60);
}

void atgm336_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *dma)
{
#if CONFIG_ENABLE_GPS
	ots_gps.taskCode = gps_task;
	ots_gps.taskName = "gps_task";
	ots_gps.taskStackSize = TASK_GPS_STACK_SIZE;
	ots_gps.taskParameters = NULL;
	ots_gps.taskPriority = TASK_GPS_PRIO;
	ots_gps.taskHandler = &TaskGPS_Handler;
	
	uint8_t time_out;
	NMEA_init(huart, dma);
	NMEA_CB_register(&start_measure_speed_CB, SPEED_RISE_BARRIER_CB, 10);
	
	gps_data = &nmea_data;
	xSemaphoreGPS = xSemaphoreCreateBinary();
	os_task_start(&ots_gps);
#endif
}

void atgm336_start()
{
#if CONFIG_ENABLE_GPS
	if(gps_data == NULL) return;
	
	gps_run_flag = true;
	xSemaphoreGive(xSemaphoreGPS);
#endif
}

void atgm336_stop()
{
#if CONFIG_ENABLE_GPS
	if(gps_data == NULL) return;
	gps_run_flag = false;
#endif
}

NMEA_data* get_gps_data()
{
	return gps_data;
}

static void gps_task(void *pvParameters)
{
	for(;;)
	{
		BaseType_t err = pdFALSE;
		err = xSemaphoreTake(xSemaphoreGPS, portMAX_DELAY);
		
		if(err != pdTRUE) continue;
		
		while(gps_run_flag)
		{
			status = NMEA_process_task();
			if(status == NMEA_OK)
			{	
				while(!NMEA_read_flag)
				{
					vTaskDelay(10);
				}
			}
			
			vTaskDelay(500);
		}

	}
}
