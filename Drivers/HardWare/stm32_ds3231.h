/*
 * stm32_ds3231.h
 *
 *  Created on: 2019. 3. 17.
 *      Author: Jihoon Lee
 */

#ifndef STM32_DS3231_H_
#define STM32_DS3231_H_

#include "main.h"
#include <stdlib.h>
#include <stdbool.h>

#define DS3231_REG_TIME         0x00
#define DS3231_REG_ALARM1       0x07
#define DS3231_REG_ALARM2       0x0B
#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F
#define DS3231_REG_TEMP         0x11

#define DS3231_CON_EOSC         0x80
#define DS3231_CON_BBSQW        0x40
#define DS3231_CON_CONV         0x20
#define DS3231_CON_RS2          0x10
#define DS3231_CON_RS1          0x08
#define DS3231_CON_INTCN        0x04
#define DS3231_CON_A2IE         0x02
#define DS3231_CON_A1IE         0x01

#define DS3231_STA_OSF          0x80
#define DS3231_STA_32KHZ        0x08
#define DS3231_STA_BSY          0x04
#define DS3231_STA_A2F          0x02
#define DS3231_STA_A1F          0x01

typedef enum
{
  ALARM_MODE_ALL_MATCHED = 0,
  ALARM_MODE_HOUR_MIN_SEC_MATCHED,
  ALARM_MODE_MIN_SEC_MATCHED,
  ALARM_MODE_SEC_MATCHED,
  ALARM_MODE_ONCE_PER_SECOND
} AlarmMode;

typedef enum
{
  SUNDAY = 1,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
} DaysOfWeek;

typedef struct
{
  uint8_t Year;
  uint8_t Month;
  uint8_t Date;
  uint8_t DaysOfWeek;
  uint8_t Hour;
  uint8_t Min;
  uint8_t Sec;
} _RTC;

extern const char* DS3231_weeks[];

void DS3231_Init(I2C_HandleTypeDef *handle);
bool DS3231_GetTime(_RTC *rtc);
bool DS3231_SetTime(_RTC *rtc);
bool DS3231_ReadTemperature(float *temp);
bool DS3231_SetAlarm1(AlarmMode mode, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec);
bool DS3231_ClearAlarm1();
bool ReadRegister(uint8_t regAddr, uint8_t *value);
bool WriteRegister(uint8_t regAddr, uint8_t value);

#endif /* STM32_DS3231_H_ */
