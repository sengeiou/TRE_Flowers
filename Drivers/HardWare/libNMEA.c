/**
 * @file libNMEA.c
 * @author	Michał Pałka
 *
 *  Created on: Nov 23, 2020
 *
 *  Implementation of NMEA library.
 */


#include "libNMEA.h"
#include "sys.h"
#include "malloc.h"
/**
 * Pointer to UART handler structure using to communication with GPS module.
 */
UART_HandleTypeDef	*NMEA_huart;
/**
 * Pointer to DMA handler structure using to receive messages from GPS module.
 */
DMA_HandleTypeDef	*NMEA_DMA;

/**
 * NMEA_data instance declaration.
 */
NMEA_data nmea_data;

uint8_t NMEA_read_flag = 0;

/**
 * NMEA_UART_DMA_buffer is buffer to which incoming UART data are copied by DMA.\n
 * Size of this buffer has to be bigger then length of all incoming data from module in one shot.
 */
static uint8_t NMEA_UART_DMA_buffer[NMEA_UART_DMA_BUFFER_SIZE]={0};
/**
 * NMEA_UART_buffer is a circular buffer to which data are copied from NMEA_UART_DMA_buffer.\n
 * Size of this buffer should be at least twice NMEA_UART_DMA_BUFFER_SIZE.
 */
static uint8_t NMEA_UART_buffer[NMEA_UART_BUFFER_SIZE]={0};
/**
 * NMEA_working_buffer is buffer to which data from circular buffer are copied to be parse.\n
 * Size of this buffer has to be bigger than longest NMEA message.
 */
static uint8_t NMEA_working_buffer[NMEA_WORKING_BUFFER_SIZE]={0};
static int UART_buffer_head=0;		/**< Index of circular buffer head*/
static int UART_buffer_tail=0;		/**< Index of circular buffer tail*/		
static int UART_buffer_lines=0;	/**< Number of lines in circular buffer to read.*/
/**
 * speed_change_CB_fun_ptr is a pointer to function that will be triggered after delta of speed defined in speed_change_tolerance.
 */
static void (*speed_change_CB_fun_ptr)(void);
static float  speed_change_tolerance;	/**< Tolerance of speed change. If delta of speed is higher than speed_change_tolerance, speed_change_CB_fun_ptr() will be triggered.*/
/**
 * speed_raise_barrier_CB_fun_ptr is a pointer to function that will be triggered when speed will cross up speed_raise_barrier.
 */
static void (*speed_raise_barrier_CB_fun_ptr)(void);
static float speed_raise_barrier;	/**< Barrier of increasing speed*/
/**
 * speed_fall_barrier_CB_fun_ptr is a pointer to function that will be triggered when speed will cross down speed_fall_barrier.
 */
static void (*speed_fall_barrier_CB_fun_ptr)(void);
static float speed_fall_barrier;	/**< Barrier of decreasing speed*/
/**
 * default_CB() is a default function for all the CB pointers which do nothing.\n
 * The unregistering CB pointer is setting to pointer of this function.  
 */
static void default_CB(void){}

NMEA_status NMEA_CB_register(void (*CB_fun)(void),NMEA_CB_ID CB_id,float barrier){
	switch (CB_id) {
		case SPEED_CHANGE_CB:
			if (barrier < 0) return NMEA_WRONG_DATA;
			speed_change_CB_fun_ptr = CB_fun;
			speed_change_tolerance = barrier;
			return NMEA_OK;
		case SPEED_RISE_BARRIER_CB:
			if (barrier < 0) return NMEA_WRONG_DATA;
			speed_raise_barrier_CB_fun_ptr = CB_fun;
			speed_raise_barrier = barrier;
			return NMEA_OK;
		case SPEED_FALL_BARRIER_CB:
			if (barrier < 0) return NMEA_WRONG_DATA;
			speed_fall_barrier_CB_fun_ptr = CB_fun;
			speed_fall_barrier = barrier;
			return NMEA_OK;
		default:
			return NMEA_WRONG_CB_ID;
	}
	return NMEA_ERROR;
}

NMEA_status NMEA_CB_unregister(NMEA_CB_ID CB_id){
	switch (CB_id) {
		case SPEED_CHANGE_CB:
			speed_change_tolerance = 0;
			speed_change_CB_fun_ptr = default_CB;
			return NMEA_OK;
		case SPEED_RISE_BARRIER_CB:
			speed_raise_barrier = 0;
			speed_raise_barrier_CB_fun_ptr = default_CB;
			return NMEA_OK;
		case SPEED_FALL_BARRIER_CB:
			speed_fall_barrier = 0;
			speed_fall_barrier_CB_fun_ptr = default_CB;
			return NMEA_OK;
		default:
			return NMEA_WRONG_CB_ID;
	}
	return NMEA_ERROR;
}
/**
 * NMEA_parser is function which parses single correct NMEA message.\n
 * Inside this function all known types of NMEA message are recognized and nmea_data structure fields are set.\n
 * There is also implemented mechanism of recognizing specified events and calling corresponding to them callbacks.
 * @param[in]	message	pointer to buffer storing NMEA message.
 */
static void 	NMEA_parser(char *message){
	NMEA_data previous_data = nmea_data;
	//Debug("%s\n", message);
	
	uint8_t type = 0; //GPS
	uint8_t prn = 0;
	uint8_t gsv_len = 0;
	uint8_t gsv_index = 0;
	
	int num = 0;
	char *fields[32]={NULL};
	fields[num++]=message;
	while ((message = strchr(message, ','))) {
		*message++ = 0;
		fields[num++]=message;
	}

	if(strcmp(fields[0],"$GPGLL")==0 || strcmp(fields[0],"$GNGLL")==0){

		nmea_data.latitude = atof(fields[1]);
		nmea_data.latitude_direction = *fields[2];
		nmea_data.longitude = atof(fields[3]);
		nmea_data.longitude_direction = *fields[4];

	}else if(strcmp(fields[0],"$GPRMC")==0 || strcmp(fields[0],"$GNRMC")==0){

		nmea_data.UTC_time = atof(fields[1]);
		nmea_data.UT_date = atoi(fields[9]);

		nmea_data.latitude = atof(fields[3]);
		nmea_data.latitude_direction = *fields[4];
		nmea_data.longitude = atof(fields[5]);
		nmea_data.longitude_direction = *fields[6];
		
		nmea_data.speed_knots =  atof(fields[7]);
		nmea_data.speed_direction = atof(fields[8]);

	}else if(strcmp(fields[0],"$GPVTG")==0 || strcmp(fields[0],"$GNVTG")==0){

		nmea_data.speed_knots =  atof(fields[5]);
		nmea_data.speed_kmph =  atof(fields[7]);

	}else if(strcmp(fields[0],"$GPGGA")==0 || strcmp(fields[0],"$GNGGA")==0){
		nmea_data.UTC_time = atof(fields[1]);

		nmea_data.latitude = atof(fields[2]);
		nmea_data.latitude_direction = *fields[3];
		nmea_data.longitude = atof(fields[4]);
		nmea_data.longitude_direction = *fields[5];

		nmea_data.fix = atoi(fields[6]);
		nmea_data.sat_in_use = atoi(fields[7]);
		nmea_data.HDOP = atof(fields[8]);

		nmea_data.altitude = atof(fields[9]);
		nmea_data.geoidal_separation = atof(fields[11]);

	}else if(strcmp(fields[0],"$GPGSA")==0 || strcmp(fields[0],"$GNGSA")==0){
		nmea_data.fix_mode = atoi(fields[2]);

		nmea_data.PDOP = atof(fields[15]);
		nmea_data.HDOP = atof(fields[16]);
		nmea_data.VDOP = atof(fields[17]);

	}else if(strcmp(fields[0],"$GPGSV")==0 || strcmp(fields[0],"$BDGSV") == 0){ //gsv_info_len: [(num-4-1)/4]
		type = strcmp(fields[0],"$BDGSV") == 0;
		
		if(type == 0){
			nmea_data.sat_in_view_gp = atoi(fields[3]);
		}
		else{
			nmea_data.sat_in_view_bd = atoi(fields[3]);
		}
		
		//添加到列表
		gsv_len = (num-4-1) >> 2;
		for(int i=0;i<gsv_len;i++)
		{
			gsv_index = (i + 1) << 2;
			prn = atoi(fields[gsv_index]);
			
			if(prn <= 0) continue;
			
			GSV_data* gsv_data = mymalloc(SRAMIN, sizeof(GSV_data));
			gsv_data->prn = prn;
			gsv_data->elev = atoi(fields[gsv_index+1]);
			gsv_data->azi = atoi(fields[gsv_index+2]);
			gsv_data->snr = atoi(fields[gsv_index+3]);
			gsv_data->type = type;
			
			list_rpush(nmea_data.gsv_list, list_node_new(gsv_data));
		}
		
	}

	//abs()
	if (fabs(nmea_data.speed_kmph - previous_data.speed_kmph) > speed_change_tolerance){
		speed_change_CB_fun_ptr();
	}

	if (nmea_data.speed_kmph > speed_raise_barrier && previous_data.speed_kmph <= speed_raise_barrier){
		speed_raise_barrier_CB_fun_ptr();
	}

	if (nmea_data.speed_kmph < speed_raise_barrier && previous_data.speed_kmph >= speed_raise_barrier){
		speed_fall_barrier_CB_fun_ptr();
	}
}
/**
 * hx2int is function which converts hex number written using characters to corresponding integer.
 * @param[in]	n2		is older position ix hex code
 * @param[in]	n1		is younger position ix hex code
 * @param[out]	uint8_t	is integer corresponding to input hex
 */
static uint8_t hx2int(uint8_t n2, uint8_t n1){
	if (n2 <= '9') n2-='0';
	else n2=n2-'A'+10;

	if (n1 <= '9') n1-='0';
	else n1=n1-'A'+10;

	return n2*16+n1;

}
/**
 * NMEA_checksum_clc is function which calculates checksum of the message and compares it to checksum value given in NMEA message.\n
 * To convert given checksum it uses hx2int function.
 * @param[in]	message	pointer to buffer storing NMEA message.
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
static NMEA_status NMEA_checksum_clc(uint8_t * message){
	uint8_t index = 1;
	uint8_t checksum_clc =0;

	while (message[index]!='*' && index<NMEA_WORKING_BUFFER_SIZE-2){
		checksum_clc^=message[index++];
	}

	uint8_t checksum = hx2int(message[index+1],message[index+2]);
	if (checksum!=checksum_clc){
		return NMEA_CHECKSUM_ERROR;
	}
	return NMEA_OK;


}

/**
 * NMEA_read_line is function which reads one NMEA message line from NMEA_UART_buffer circular buffer to NMEA_working_buffer.
 */
static void NMEA_read_line(void){
	int index = 0;
	while (index < NMEA_WORKING_BUFFER_SIZE) NMEA_working_buffer[index++]=0;	// Clean up working buffer.

	index = 0;
	while(NMEA_UART_buffer[UART_buffer_tail]!= '\n' && index < NMEA_WORKING_BUFFER_SIZE-2){
		NMEA_working_buffer[index]=NMEA_UART_buffer[UART_buffer_tail];
		NMEA_UART_buffer[UART_buffer_tail] = 0;
		UART_buffer_tail = (UART_buffer_tail + 1)%NMEA_UART_BUFFER_SIZE;
		++index;
	}
	NMEA_working_buffer[index]=NMEA_UART_buffer[UART_buffer_tail];
	NMEA_UART_buffer[UART_buffer_tail] = 0;
	UART_buffer_tail = (UART_buffer_tail + 1)%NMEA_UART_BUFFER_SIZE;
	++index;
	--UART_buffer_lines;

}

void NMEA_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *DMA){
	delay_ms(10);
	NMEA_huart=huart;
	NMEA_DMA=DMA;
	__HAL_UART_ENABLE_IT(NMEA_huart,UART_IT_IDLE);
	HAL_UART_Receive_DMA(NMEA_huart, NMEA_UART_DMA_buffer, NMEA_UART_DMA_BUFFER_SIZE);


	speed_change_CB_fun_ptr = &default_CB;
	speed_raise_barrier_CB_fun_ptr = &default_CB;
	speed_fall_barrier_CB_fun_ptr = &default_CB;
	
	nmea_data.gsv_list = list_new();
}

/**
 * NMEA_UART_DMA_get_char is a function which takes character from NMEA_UART_DMA_buffer and places it inside NMEA_UART_buffer circular buffer.\n
 * If buffer overflowes, the oldest NMEA message will be deleted to make space for incoming messages.\n
 * If new line character is detected ('\ n'), the line counter (UART_buffer_lines) increases.
 * @param[in]	DMA_char	character from DMA buffer
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
static NMEA_status NMEA_UART_DMA_get_char(uint8_t DMA_char){
	int position = (UART_buffer_head + 1)%NMEA_UART_BUFFER_SIZE;
	NMEA_status stat=NMEA_OK;

	if (position == UART_buffer_tail){		//buffer overflowed! make space for new message
		while (NMEA_UART_buffer[UART_buffer_tail]!='\n' && NMEA_UART_buffer[UART_buffer_tail]!=0){
			NMEA_UART_buffer[UART_buffer_tail]=0;
			UART_buffer_tail=(UART_buffer_tail + 1)%NMEA_UART_BUFFER_SIZE;
		}
		NMEA_UART_buffer[UART_buffer_tail]=0;
		UART_buffer_tail=(UART_buffer_tail + 1)%NMEA_UART_BUFFER_SIZE;
		stat=NMEA_BUFFER_OVERFLOWED;
	}

	NMEA_UART_buffer[UART_buffer_head]=DMA_char;

	UART_buffer_head=position;

	if(DMA_char=='\n'){
		++UART_buffer_lines;	//increment lines counter
	}

	return stat;
}

/**
 * NMEA_UART_DMA_copy_buffer is a function which copies messages from DMA buffer to UART circular buffer.\n
 * To do so, it uses NMEA_UART_DMA_get_char function for every character in NMEA_UART_DMA_buffer from 0 to (NMEA_UART_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(NMEA_DMA)).
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
static NMEA_status NMEA_UART_DMA_copy_buffer(void){
	
	NMEA_status stat=NMEA_OK;

	int data_length = NMEA_UART_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(NMEA_DMA);

	for (int i = 0; i < data_length; i++){
		if (NMEA_UART_DMA_get_char(NMEA_UART_DMA_buffer[i])==NMEA_BUFFER_OVERFLOWED){
			stat=NMEA_BUFFER_OVERFLOWED;
		}
		NMEA_UART_DMA_buffer[i]=0;
	}

	HAL_UART_Receive_DMA(NMEA_huart, NMEA_UART_DMA_buffer, NMEA_UART_DMA_BUFFER_SIZE);
	return stat;
}

static void NMEA_gsv_clear(){
	list_node_t* p_node = NULL;
	if(nmea_data.gsv_list == NULL) return;
	
	list_iterator_t *it = list_iterator_new(nmea_data.gsv_list, LIST_HEAD);
	while ((p_node = list_iterator_next(it))) {
		myfree(SRAMIN, p_node->val);
		list_remove(nmea_data.gsv_list, p_node);
	}
	myfree(SRAMIN, it);
}

NMEA_status NMEA_process_task(void){
	NMEA_status stat = NMEA_OK;
	NMEA_gsv_clear(); //先清除gsv列表
	NMEA_read_flag = 0;
	while(UART_buffer_lines>0) {
		NMEA_read_line();
		if (NMEA_checksum_clc(NMEA_working_buffer) == NMEA_OK){
			NMEA_parser((char *)NMEA_working_buffer);
		}else stat = NMEA_CHECKSUM_ERROR;
	}
	return stat;
}

NMEA_status user_UART_IDLE_IT_handler(void){
	NMEA_status stat = NMEA_OK;
	if (__HAL_UART_GET_FLAG(NMEA_huart, UART_FLAG_IDLE) == SET) {
		__HAL_UART_CLEAR_FLAG(NMEA_huart,UART_FLAG_IDLE);
		
		HAL_UART_DMAStop(NMEA_huart);
		stat = NMEA_UART_DMA_copy_buffer();
		NMEA_read_flag = 1;
	}
	return stat;
}




