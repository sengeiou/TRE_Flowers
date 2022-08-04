#include "ArduboyButtons.h"
#include "stdio.h"

static char text[] = "Press Buttons!";
byte x;
byte y;

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define NUM_CHARS (sizeof(text) - 1)
#define X_MAX (WIDTH - (NUM_CHARS * CHAR_WIDTH) + 1)

#define Y_MAX (HEIGHT - CHAR_HEIGHT)

void arduboy_btn_init()
{
  arduboy.begin();
  arduboy.setFrameRate(30);

  x = (arduboy.width() / 2) - (NUM_CHARS * CHAR_WIDTH / 2);
  y = (arduboy.height() / 2) - (CHAR_HEIGHT / 2);
}
void arduboy_btn_loop()
{
  if (!(arduboy.nextFrame()))
    return;
  if(arduboy.pressed(RIGHT_BUTTON) && (x < X_MAX)) {
    x++;
  }
  if(arduboy.pressed(LEFT_BUTTON) && (x > 0)) {
    x--;
  }
  if((arduboy.pressed(UP_BUTTON) || arduboy.pressed(B_BUTTON)) && (y > 0)) {
    y--;
  }
  if((arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(A_BUTTON)) && (y < Y_MAX)) {
    y++;
  }
	if(arduboy.pressed(A_BUTTON)){
		Debug("A button pressed\n");
	}
	if(arduboy.pressed(B_BUTTON)){
		Debug("B button pressed\n");
	}
  arduboy.clear();
  arduboy.setCursor(x, y);
  arduboy.print(text);
  arduboy.display();
}