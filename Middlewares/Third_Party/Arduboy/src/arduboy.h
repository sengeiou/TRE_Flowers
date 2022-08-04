#ifndef _ARDUBOY_H
#define _ARDUBOY_H

#include "arduboy_core.h"
#include "stdio.h"
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <time.h>

typedef void(*delayHandle)(uint32_t t);
typedef void(*paintHandle)(const unsigned char *image);

typedef struct ArduboyStruct
{
	delayHandle delayHandler;
	paintHandle paintHandler;
	uint8_t(*width)();
	uint8_t(*height)();
	bool(*pressed)(uint8_t buttons);
	bool(*notPressed)(uint8_t buttons);
	void(*begin)();
	void(*beginNoLogo)();
	void(*bootLogo)();
	void(*clear)();
	void(*display)();
	void(*drawPixel)(int x, int y, uint8_t color);
	uint8_t(*getPixel)(uint8_t x, uint8_t y);
	void(*drawCircle)(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
	void(*drawCircleHelper)(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, uint8_t color);
	void(*fillCircle)(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
	void(*fillCircleHelper)(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta, uint8_t color);
	void(*drawLine)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
	void(*drawRect)(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
	void(*drawFastVLine)(int16_t x, int16_t y, uint8_t h, uint8_t color);
	void(*drawFastHLine)(int16_t x, int16_t y, uint8_t w, uint8_t color);
	void(*fillRect)(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
	void(*fillScreen)(uint8_t color);
	void(*drawRoundRect)(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
	void(*fillRoundRect)(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
	void(*drawTriangle)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
	void(*fillTriangle)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
	void(*drawBitmap)(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
	void(*drawSlowXYBitmap)(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
	void(*drawChar)(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);
	void(*setCursor)(int16_t x, int16_t y);
	void(*setTextSize)(uint8_t s);
	void(*setTextWrap)(boolean w);
	unsigned char* (*getBuffer)();
	size_t(*write)(uint8_t);
	void(*print)(const char* str);
	void(*initRandomSeed)();
	void(*swap)(int16_t* a, int16_t* b);
	void(*setFrameRate)(uint8_t rate);
	bool(*nextFrame)();
	bool(*everyXFrames)(uint8_t frames);
	int(*cpuLoad)();
}Arduboy;

extern uint8_t frameRate;
extern uint16_t frameCount;
extern uint8_t eachFrameMillis;
extern long lastFrameStart;
extern long nextFrameStart;
extern bool post_render;
extern uint8_t lastFrameDurationMs;

extern Arduboy arduboy ;

#define delay(ms) arduboy.delayHandler(ms)
#define millis() HAL_GetTick()

#endif