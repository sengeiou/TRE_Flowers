#include "at24cxx.h"
#include "i2c.h"

#if _EEPROM_USE_IWDG
	#include "iwdg.h"
#endif

#if (_EEPROM_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define at24_delay(x)   osDelay(x)
#else
#define at24_delay(x)   HAL_Delay(x)
#endif

#if (_EEPROM_SIZE_KBIT == 1) || (_EEPROM_SIZE_KBIT == 2)
#define _EEPROM_PSIZE     8
#elif (_EEPROM_SIZE_KBIT == 4) || (_EEPROM_SIZE_KBIT == 8) || (_EEPROM_SIZE_KBIT == 16)
#define _EEPROM_PSIZE     16
#else
#define _EEPROM_PSIZE     32
#endif

static uint8_t at24_lock = 0;

/**
  * @brief  Checks if memory device is ready for communication.
  * @param  none
  * @retval bool status
  */
bool at24_isConnected(void)
{
  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
  #endif
  if (HAL_I2C_IsDeviceReady(&_EEPROM_I2C, _EEPROM_ADDRESS, 2, 100) == HAL_OK)
    return true;
  else
    return false;	
}

/**
  * @brief  Write an amount of data in blocking mode to a specific memory address
  * @param  address Internal memory address
  * @param  data Pointer to data buffer
  * @param  len Amount of data to be sent
  * @param  timeout Timeout duration
  * @retval bool status
  */
bool at24_write(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
  if (at24_lock == 1)
    return false;
	
  at24_lock = 1; 
  uint16_t w;
  uint32_t startTime = HAL_GetTick();
	
  #if	(_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN,GPIO_PIN_RESET);
  #endif
	
  while (1)
  {
#if _EEPROM_USE_IWDG
		HAL_IWDG_Refresh(&_EEPROM_IWDG);
#endif
    w = _EEPROM_PSIZE - (address  % _EEPROM_PSIZE);
    if (w > len)
      w = len;        
    #if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==4)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==8)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==16)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)		
    #else
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, w, 100) == HAL_OK)
    #endif
    {
      at24_delay(10);
      len -= w;
      data += w;
      address += w;
      if (len == 0)
      {
        #if (_EEPROM_USE_WP_PIN==1)
        HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
        #endif
        at24_lock = 0;
        return true;
      }
      if (HAL_GetTick() - startTime >= timeout) 
      {
        at24_lock = 0;
        return false;
      }
    }
    else
    {
      #if (_EEPROM_USE_WP_PIN==1)
      HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
      #endif
      at24_lock = 0;
      return false;
    }
  }
}

/**
  * @brief  Read an amount of data in blocking mode to a specific memory address
  * @param  address Internal memory address
  * @param  data Pointer to data buffer
  * @param  len Amount of data to be sent
  * @param  timeout Timeout duration
  * @retval bool status
  */
bool at24_read(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
#if _EEPROM_USE_IWDG
	HAL_IWDG_Refresh(&_EEPROM_IWDG);
#endif
  if (at24_lock == 1)
    return false;
  at24_lock = 1;
  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
  #endif
  #if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT == 4)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT == 8)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT==16)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #else
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, len, timeout) == HAL_OK)
  #endif
  {
    at24_lock = 0;
    return true;
  }
  else
  {
    at24_lock = 0;
    return false;	
  }    
}

/**
  * @brief  Erase memory.
  * @param  none
  * @retval bool status
  */
bool at24_eraseChip(void)
{
  const uint8_t eraseData[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF\
    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint32_t bytes = 0;
  while ( bytes < (_EEPROM_SIZE_KBIT * 128))
  {
    if (at24_write(bytes, (uint8_t*)eraseData, sizeof(eraseData), 100) == false)
      return false;
    bytes += sizeof(eraseData);           
  }
  return true;  
}

/* EOF */
