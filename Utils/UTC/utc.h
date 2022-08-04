#ifndef _NMEA_UTC_H
#define _NMEA_UTC_H

#include <stdint.h>

typedef struct
{
	uint16_t year;	//年份
	uint8_t month;	//月份
	uint8_t day;	//日期
	uint8_t hour; 	//小时
	uint8_t min; 	//分钟
	uint8_t sec; 	//秒钟
}NmeaTime_t;

NmeaTime_t BeijingToUTC(NmeaTime_t time);
NmeaTime_t UTCToBeijing(NmeaTime_t time);


#endif


