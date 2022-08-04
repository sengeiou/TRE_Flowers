/*
 * stm32_ds3231.c
 *
 *  Created on: 2019. 3. 17.
 *      Author: kiki
 */
#include "stm32_ds3231.h"

#define DS3231_ADDR  (0x68 << 1)

I2C_HandleTypeDef *i2c;

static uint8_t B2D(uint8_t bcd);
static uint8_t D2B(uint8_t decimal);

const char* DS3231_weeks[] = {
	"SUN.","MON.","TUE.","WED.","THU.","FRI.","SAT."
};

void DS3231_Init(I2C_HandleTypeDef *handle)
{
  i2c = handle;
}

bool DS3231_GetTime(_RTC *rtc)
{
  uint8_t startAddr = DS3231_REG_TIME;
  uint8_t buffer[7] = {0,};

  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK) return false;
  if(HAL_I2C_Master_Receive(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;

  rtc->Sec = B2D(buffer[0] & 0x7F);
  rtc->Min = B2D(buffer[1] & 0x7F);
  rtc->Hour = B2D(buffer[2] & 0x3F);
  rtc->DaysOfWeek = buffer[3] & 0x07;
  rtc->Date = B2D(buffer[4] & 0x3F);
  rtc->Month = B2D(buffer[5] & 0x1F);
  rtc->Year = B2D(buffer[6]);

  return true;
}

bool DS3231_SetTime(_RTC *rtc)
{
  uint8_t startAddr = DS3231_REG_TIME;
  uint8_t buffer[8] = {startAddr, D2B(rtc->Sec), D2B(rtc->Min), D2B(rtc->Hour), rtc->DaysOfWeek, D2B(rtc->Date), D2B(rtc->Month), D2B(rtc->Year)};
  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;

  return true;
}

bool DS3231_ReadTemperature(float *temp)
{
  uint8_t startAddr = DS3231_REG_TEMP;
  uint8_t buffer[2] = {0,};

  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK) return false;
  if(HAL_I2C_Master_Receive(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;

  int16_t value = (buffer[0] << 8) | (buffer[1]);
  value = (value >> 6);

  *temp = value / 4.0f;
  return true;
}

bool DS3231_SetAlarm1(AlarmMode mode, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec)
{
  uint8_t alarmSecond = D2B(sec);
  uint8_t alarmMinute = D2B(min);
  uint8_t alarmHour = D2B(hour);
  uint8_t alarmDate = D2B(date);

  switch(mode)
  {
  case ALARM_MODE_ALL_MATCHED:
    break;
  case ALARM_MODE_HOUR_MIN_SEC_MATCHED:
    alarmDate |= 0x80;
    break;
  case ALARM_MODE_MIN_SEC_MATCHED:
    alarmDate |= 0x80;
    alarmHour |= 0x80;
    break;
  case ALARM_MODE_SEC_MATCHED:
    alarmDate |= 0x80;
    alarmHour |= 0x80;
    alarmMinute |= 0x80;
    break;
  case ALARM_MODE_ONCE_PER_SECOND:
    alarmDate |= 0x80;
    alarmHour |= 0x80;
    alarmMinute |= 0x80;
    alarmSecond |= 0x80;
    break;
  default:
    break;
  }

  /* Write Alarm Registers */
  uint8_t startAddr = DS3231_REG_ALARM1;
  uint8_t buffer[5] = {startAddr, alarmSecond, alarmMinute, alarmHour, alarmDate};
  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;

  /* Enable Alarm1 at Control Register */
  uint8_t ctrlReg = 0x00;
  ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
  ctrlReg |= DS3231_CON_A1IE;
  ctrlReg |= DS3231_CON_INTCN;
  WriteRegister(DS3231_REG_CONTROL, ctrlReg);

  return true;
}

bool DS3231_ClearAlarm1()
{
  uint8_t ctrlReg;
  uint8_t statusReg;

  /* Clear Control Register */
  ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
  ctrlReg &= ~DS3231_CON_A1IE;
  WriteRegister(DS3231_REG_CONTROL, ctrlReg);

  /* Clear Status Register */
  ReadRegister(DS3231_REG_STATUS, &statusReg);
  statusReg &= ~DS3231_STA_A1F;
  WriteRegister(DS3231_REG_STATUS, statusReg);

  return true;
}

bool ReadRegister(uint8_t regAddr, uint8_t *value)
{
  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, &regAddr, 1, HAL_MAX_DELAY) != HAL_OK) return false;
  if(HAL_I2C_Master_Receive(i2c, DS3231_ADDR, value, 1, HAL_MAX_DELAY) != HAL_OK) return false;

  return true;
}

bool WriteRegister(uint8_t regAddr, uint8_t value)
{
  uint8_t buffer[2] = {regAddr, value};
  if(HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK) return false;

  return true;
}

static uint8_t B2D(uint8_t bcd)
{
  return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t D2B(uint8_t decimal)
{
  return (((decimal / 10) << 4) | (decimal % 10));
}
