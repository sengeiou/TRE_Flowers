/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_HOME_Pin GPIO_PIN_2
#define KEY_HOME_GPIO_Port GPIOE
#define KEY_MENU_Pin GPIO_PIN_3
#define KEY_MENU_GPIO_Port GPIOE
#define KEY_BACK_Pin GPIO_PIN_4
#define KEY_BACK_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF
#define XCS_Pin GPIO_PIN_0
#define XCS_GPIO_Port GPIOC
#define XDCS_Pin GPIO_PIN_1
#define XDCS_GPIO_Port GPIOC
#define DREQ_Pin GPIO_PIN_1
#define DREQ_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_MOSI_Pin GPIO_PIN_7
#define LCD_MOSI_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_5
#define LCD_RS_GPIO_Port GPIOC
#define XRESET_Pin GPIO_PIN_11
#define XRESET_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOB
#define W25QX_CS_Pin GPIO_PIN_14
#define W25QX_CS_GPIO_Port GPIOB
#define KEY_ENTER_Pin GPIO_PIN_6
#define KEY_ENTER_GPIO_Port GPIOG
#define KEY_RIGHT_Pin GPIO_PIN_7
#define KEY_RIGHT_GPIO_Port GPIOG
#define KEY_LEFT_Pin GPIO_PIN_8
#define KEY_LEFT_GPIO_Port GPIOG
#define KEY_UP_Pin GPIO_PIN_11
#define KEY_UP_GPIO_Port GPIOG
#define KEY_DOWN_Pin GPIO_PIN_13
#define KEY_DOWN_GPIO_Port GPIOG
#define FLASH_CS_Pin GPIO_PIN_14
#define FLASH_CS_GPIO_Port GPIOG
#define KEY_EXIT_Pin GPIO_PIN_8
#define KEY_EXIT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
