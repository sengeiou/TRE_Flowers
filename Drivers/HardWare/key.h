#ifndef _KEY_H
#define _KEY_H

#include "sys.h"
#include "main.h"
#include "buzzer.h"

#define KEY_EXIT_VAL HAL_GPIO_ReadPin(KEY_EXIT_GPIO_Port, KEY_EXIT_Pin)

#define KEY_HOME_VAL HAL_GPIO_ReadPin(KEY_HOME_GPIO_Port, KEY_HOME_Pin)
#define KEY_MENU_VAL HAL_GPIO_ReadPin(KEY_MENU_GPIO_Port, KEY_MENU_Pin)

#define KEY_UP_VAL HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)
#define KEY_DOWN_VAL HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin)
#define KEY_LEFT_VAL HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin)
#define KEY_RIGHT_VAL HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin)

#define KEY_ENTER_VAL HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port, KEY_ENTER_Pin)
#define KEY_BACK_VAL HAL_GPIO_ReadPin(KEY_BACK_GPIO_Port, KEY_BACK_Pin)

#define KEY_UP_DEF 1
#define KEY_DOWN_DEF 2
#define KEY_LEFT_DEF 3
#define KEY_RIGHT_DEF 4
#define KEY_ENTER_DEF 5
#define KEY_BACK_DEF 6
#define KEY_HOME_DEF 7
#define KEY_MENU_DEF 8

enum{
	KEY_EXIT_EN,
	KEY_EXIT_NOT
};

typedef void(*delay_handle)(int t);

//FC游戏手柄数据格式定义
//1,表示没有按下,0表示按下.
typedef union _FC_GamePad_TypeDef 
{
	u8 ctrlval;
	struct
	{
		u8 a:1;		//A键
		u8 b:1;		//B键
		u8 select:1;//Select键
		u8 start:1; //Start键
		u8 up:1;	//上
		u8 down:1;	//下
		u8 left:1;	//左
		u8 right:1; //右
	}b;
}FC_GamePad_TypeDef ; 

void key_delay_set(delay_handle handler);
uint8_t key_read(uint8_t mode);

extern FC_GamePad_TypeDef fcpad;

#endif

