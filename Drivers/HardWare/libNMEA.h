/*!
 * @file libNMEA.h
 * @author	Michał Pałka
 *
 *  Created on: Nov 23, 2020
 *
 * 	This is the header file of the library created to handling communication with GPS devices by NMEA protocol.
 * 	It is created for STM32 MCUs family.
 * 	It should be included in "main.h" file
 */

#ifndef _LIB_NMEA_H
#define _LIB_NMEA_H

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list_c.h"
/**
 * UART DMA buffer size definition
 */
#define NMEA_UART_DMA_BUFFER_SIZE	1024
/**
 * Circular buffer size definition
 */
#define NMEA_UART_BUFFER_SIZE		1024
/**
 * Working buffer size definition
 */
#define NMEA_WORKING_BUFFER_SIZE	256

/*!
 * Definition of ERROR codes (NMEA_status).
 */
typedef enum {
	NMEA_OK=0,		/**< operation success*/
	NMEA_BUFFER_OVERFLOWED,	/**< circular buffer overflowed*/
	NMEA_CHECKSUM_ERROR,	/**< invalid data received*/
	NMEA_WRONG_DATA,	/**< wrong data passed by user (of the library)*/
	NMEA_WRONG_CB_ID,	/**< invalid CB ID*/
	NMEA_ERROR		/**< something went wrong*/

}NMEA_status;

/*
 * GSV_data(GPGSV&&BDGSV)
 */
typedef struct{
	uint16_t prn:8; 	//PRN码编号(1·255)
	uint16_t elev:7; 	//仰角(0~90)
	uint16_t type:1;	//卫星类型(0:GPS;1:BD)
	uint16_t azi:9;		//方位角(0~359)
	uint16_t snr:7;		//信噪比(0~99)
}GSV_data;

/*!
 * NMEA_data is a structure caring all useful received data.
 */
typedef struct{
	/*!
	 * <pre>	
	 * time of fix
	 * format: 
	 * hhmmss		for fix rate = 1Hz or less
	 * hhmmss.ss	for fix rate > 1Hz 
	 * </pre>
	 */
	float		UTC_time; 
	/*!	 
	 * <pre>
	 * date of fix
	 * format:	MMDDRR 
	 * </pre>
	 */
	int			UT_date;
	/*!	 
	 * <pre>
	 * latitude of position
	 * format:	DDDMM.MMMM 
	 * </pre>
	 */
	float 		latitude;
	/*!	 
	 * <pre>
	 * latitude direction
	 * N or S
	 * </pre>
	 */
	char 		latitude_direction;
	/*!	 
	 * <pre>
	 * longitude of position
	 * format:	DDDMM.MMMM 
	 * </pre>
	 */
	float 		longitude;
	/*!	 
	 * <pre>
	 * longitude direction
	 * E or W
	 * </pre>
	 */
	char 		longitude_direction;
	/*!	 
	 * <pre>
	 * Antenna altitude above mean-sea-level
	 * units of antenna altitude:	meters
	 * </pre>
	 */
	float 		altitude;
	/*!	 
	 * <pre>
	 * Geoidal separation
	 * units of geoidal separation:	meters
	 * </pre>
	 */
	float		geoidal_separation;
	/*!	 
	 * <pre>
	 * Speed over ground
	 * units of speed:	kilometers/hour
	 * </pre>
	 */
	float		speed_kmph;
	/*!	 
	 * <pre>
	 * Speed over ground
	 * units of speed:	knots
	 * </pre>
	 */
	float		speed_knots;
	/*!	 
	 * <pre>
	 * Direction over ground
	 * </pre>
	 */
	float		speed_direction;
	/*!	 
	 * <pre>
	 * Number of satellites in view [GPGSV!]
	 * </pre>
	 */
	uint8_t		sat_in_view_gp;
	/*!	 
	 * <pre>
	 * Number of satellites in view [BDGSV!]
	 * </pre>
	 */
	uint8_t		sat_in_view_bd;
	/*!	 
	 * <pre>
	 * Number of satellites in use
	 * </pre>
	 */
	uint8_t		sat_in_use;
	/*!	 
	 * <pre>
	 * Fix flag 
	 * 0 - Invalid
	 * 1 - GPS fix
	 * </pre>
	 */
	uint8_t		fix;
	/*!	 
	 * <pre>
	 * Fix mode 
	 * 1 - Fix not available
	 * 2 - 2D mode
	 * 3 - 3D mode
	 * </pre>
	 */
	uint8_t		fix_mode;
	/*!	 
	 * <pre>
	 * Position dilution of precision (3D)
	 * units:	meters
	 * </pre>
	 */
	float		PDOP;
	/*!	 
	 * <pre>
	 * Horizontal dilution of precision
	 * units:	meters
	 * </pre>
	 */
	float		HDOP;
	/*!	 
	 * <pre>
	 * Vertical dilution of precision 
	 * units:	meters
	 * </pre>
	 */
	float		VDOP;
	/*!	 
	 * <pre>
	 * list of GSV_data
	 * </pre>
	 */
	list_t* gsv_list;

}NMEA_data;

extern NMEA_data nmea_data;
extern uint8_t NMEA_read_flag;

/**
 * NMEA_CB_ID's are id's using during event cb registration.
 */
typedef enum {
	SPEED_CHANGE_CB=0,	/**< Id of callback to change of speed event.*/
	SPEED_RISE_BARRIER_CB,	/**< Id of callback to cross the rise speed barrier event.*/
	SPEED_FALL_BARRIER_CB	/**< Id of callback to cross the fall speed barrier event.*/
}NMEA_CB_ID;

/**
 * NMEA_init is a function to initialize library.\n
 * You should place it before while() loop. 
 * @param[in] 	huart 	is a pointer to UART_HandleTypeDef used to comunicate with GPS module.
 * @param[in]	DMA		is a pointer to DMA_HandleTypeDef used to receive data from module.
 */
void NMEA_init(UART_HandleTypeDef *huart,DMA_HandleTypeDef	*DMA);

/**
 * NMEA_process_task is a function that handles the processing data from module.\n
 * You should place it inside while() loop. 
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
NMEA_status NMEA_process_task(void);
/**
 * NMEA_CB_register is a function used to register callback functions.\n
 * Thanks to it you can customize program behavior for incoming events and realize measurements, monitoring parameters etc.
 * @param[in]	CB_fun	is a pointer to callback function which will be triggered after specified event. 
 * @param[in]	CB_id	is a NMEA_CB_ID which specifies event.
 * @param[in]	barrier	is a float value determining if the cb should be triggered or not (speed barrier, delta of speed etc).
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
NMEA_status NMEA_CB_register(void (*CB_fun)(void),NMEA_CB_ID CB_id,float barier);
/**
 * NMEA_CB_unregister is a function used to unregister specified callback function.\n
 * Thanks to it you delete selected callback. 
 * @param[in]	CB_id	is a NMEA_CB_ID which specifies event.
 * @param[out]	NMEA_status 	is a status code. It should be NMEA_OK. For more information check out NMEA_status documentation.
 */
NMEA_status NMEA_CB_unregister(NMEA_CB_ID CB_id);
/**
 * user_IDLE_IT_handler is a function that detects UART idle IT and handles it.\n
 * It should be used in void "UARTX_IRQHandler(void)" from "stm32XXxx_it.c" file like this:
 *@code
void UART4_IRQHandler(void)
{
	//USER CODE BEGIN UART4_IRQn 0
	
	user_UART_IDLE_IT_handler();
	
	//USER CODE END UART4_IRQn 0

  	...
 *@endcode
 */
NMEA_status user_UART_IDLE_IT_handler(void);

#endif













