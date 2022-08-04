#include "arduboy_core.h"

void paintScreen(const unsigned char *image)
{
	//Paint
	printf("arduboy paint\n");
}

uint8_t buttonsState()
{
	uint8_t buttons = 0;
	// down, up, left right
	buttons |= (PIN_UP_BUTTON << 7);
	buttons |= (PIN_RIGHT_BUTTON << 6 );
	buttons |= (PIN_LEFT_BUTTON << 5);
	buttons |= (PIN_DOWN_BUTTON << 4);
  // A (left)
	buttons |= (PIN_A_BUTTON << 3);
  // B (right)
	buttons |= (PIN_B_BUTTON << 2);
	return buttons;
}

uint8_t width() { return WIDTH; }

uint8_t height() { return HEIGHT; }

int random(int a, int b)
{
	uint32_t random = 0;
	HAL_RNG_GenerateRandomNumber(&hrng,&random);
	srand(random);
	return (rand()%(b- a + 1)+a);
}

