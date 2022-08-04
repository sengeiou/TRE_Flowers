#include "key.h"

static delay_handle dhandler;
FC_GamePad_TypeDef fcpad;

void key_delay_set(delay_handle handler)
{
	dhandler = handler;
}

void KEYBRD_FCPAD_Decode(uint8_t *fcbuf,uint8_t mode)
{
	fcpad.b.a=!KEY_ENTER_VAL;
	fcpad.b.b=!KEY_BACK_VAL;
	fcpad.b.select=!KEY_HOME_VAL;
 	fcpad.b.start=!KEY_MENU_VAL;
 	fcpad.b.up=!KEY_UP_VAL;
 	fcpad.b.down=!KEY_DOWN_VAL;
 	fcpad.b.left=!KEY_LEFT_VAL;
 	fcpad.b.right=!KEY_RIGHT_VAL;
	
}

uint8_t key_read(uint8_t mode)
{
	static u8 key=1;
	if(key==1 && (KEY_ENTER_VAL == 0 || KEY_BACK_VAL == 0 || KEY_HOME_VAL == 0 || \
								KEY_MENU_VAL == 0 || KEY_UP_VAL == 0 || KEY_DOWN_VAL == 0 || KEY_LEFT_VAL == 0 || KEY_RIGHT_VAL == 0))
	{
		buzzer_on();
		if(dhandler != NULL) 
			dhandler(30);
		else
			delay_ms(30);
		buzzer_off();
		key = 0;
		
		if(KEY_ENTER_VAL == 0)
			return KEY_ENTER_DEF;
		else if(KEY_BACK_VAL == 0)
			return KEY_BACK_DEF;
		else if(KEY_HOME_VAL == 0)
			return KEY_HOME_DEF;
		else if(KEY_MENU_VAL == 0)
			return KEY_MENU_DEF;
		else if(KEY_UP_VAL == 0)
			return KEY_UP_DEF;
		else if(KEY_DOWN_VAL == 0)
			return KEY_DOWN_DEF;
		else if(KEY_LEFT_VAL == 0)
			return KEY_LEFT_DEF;
		else if(KEY_RIGHT_VAL == 0)
			return KEY_RIGHT_DEF;
	}
	else if(KEY_ENTER_VAL == 1 && KEY_BACK_VAL == 1 && KEY_HOME_VAL == 1 && \
					KEY_MENU_VAL == 1 && KEY_UP_VAL == 1 && KEY_DOWN_VAL == 1 && KEY_LEFT_VAL == 1 && KEY_RIGHT_VAL == 1)
	{
		key = 1;
	}
	
	if(mode == 1) 
		key = 1;
	
	return 0;
}