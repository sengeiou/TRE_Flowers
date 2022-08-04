#ifndef _FM_H
#define _FM_H

#include "sys.h"
#include "tea5767.h"

#define FM_I2C hi2c1

void FM_init();
void FM_Search_Manual(int dir);
void FM_Search_Auto(int dir);
void FM_State_Set(int state);
void FM_test(void);

#endif