#ifndef _ARDUBOY_CORE_H
#define _ARDUBOY_CORE_H

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "key.h"
#include "rng.h"

#define SAFE_MODE

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define _BV(bit)  (1 << (bit))

#define PIN_LEFT_BUTTON !KEY_LEFT_VAL
#define PIN_RIGHT_BUTTON !KEY_RIGHT_VAL
#define PIN_UP_BUTTON !KEY_UP_VAL
#define PIN_DOWN_BUTTON !KEY_DOWN_VAL
#define PIN_A_BUTTON !KEY_ENTER_VAL
#define PIN_B_BUTTON !KEY_BACK_VAL

#define LEFT_BUTTON _BV(5)
#define RIGHT_BUTTON _BV(6)
#define UP_BUTTON _BV(7)
#define DOWN_BUTTON _BV(4)
#define A_BUTTON _BV(3)
#define B_BUTTON _BV(2)

#define WIDTH 128
#define HEIGHT 64

#define COLUMN_ADDRESS_END (WIDTH - 1) & 0x7F   // 128 pixels wide
#define PAGE_ADDRESS_END ((HEIGHT/8)-1) & 0x07  // 8 pages high

#define INVERT 2 //< lit/unlit pixel
#define WHITE 1 //< lit pixel
#define BLACK 0 //< unlit pixel

typedef unsigned char boolean;
typedef unsigned char byte;

uint8_t buttonsState(void);
void paintScreen(const unsigned char *image);
uint8_t width();
uint8_t height();
int random(int a, int b);

#endif