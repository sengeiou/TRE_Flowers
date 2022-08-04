#include "arduboy.h"
#include "glcdfont.h"
#include "ab_logo.h"

uint8_t frameRate;
uint16_t frameCount;
uint8_t eachFrameMillis;
long lastFrameStart;
long nextFrameStart;
bool post_render;
uint8_t lastFrameDurationMs;

unsigned char sBuffer[(HEIGHT*WIDTH)/8];
int16_t cursor_x;
int16_t cursor_y;
uint8_t textsize;
bool wrap;

bool pressed(uint8_t buttons);
bool notPressed(uint8_t buttons);
void begin();
void beginNoLogo();
void bootLogo();
void clear();
void display();
void drawPixel(int x, int y, uint8_t color);
uint8_t getPixel(uint8_t x, uint8_t y);
void drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
void drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, uint8_t color);
void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color);
void fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta, uint8_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
void drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color);
void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color);
void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
void fillScreen(uint8_t color);
void drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
void fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void fillTriangle (int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
void drawSlowXYBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);
void setCursor(int16_t x, int16_t y);
void setTextSize(uint8_t s);
void setTextWrap(boolean w);
unsigned char* getBuffer();
void initRandomSeed();
void print(const char* str);
void swap(int16_t* a, int16_t* b);
void setFrameRate(uint8_t rate);
bool nextFrame();
bool everyXFrames(uint8_t frames);
int cpuLoad();
uint16_t rawADC(byte adc_bits);

static size_t write(uint8_t c)
{
  if (c == '\n')
  {
    cursor_y += textsize*8;
    cursor_x = 0;
  }
  else if (c == '\r')
  {
    // skip em
  }
  else
  {
    drawChar(cursor_x, cursor_y, c, 1, 0, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (WIDTH - textsize*6)))
    {
      // calling ourselves recursively for 'newline' is 
      // 12 bytes smaller than doing the same math here
      write('\n');
    }
  }
}

bool pressed(uint8_t buttons)
{
  return (buttonsState() & buttons) == buttons;
}

bool notPressed(uint8_t buttons)
{
  return (buttonsState() & buttons) == 0;
}

void begin()
{
  // frame management
  setFrameRate(60);
  frameCount = 0;
  nextFrameStart = 0;
  post_render = false;
  // init not necessary, will be reset after first use
  // lastFrameStart
  // lastFrameDurationMs

  // font rendering  
  cursor_x = 0;
  cursor_y = 0;
  textsize = 1;
	
}

void beginNoLogo()
{
	//
}

void bootLogo()
{
  // setRGBled(10,0,0);
  for(int8_t y = -18; y<=24; y++) {
//    setRGBled(24-y, 0, 0);

    clear();
    drawBitmap(20,y, arduboy_logo, 88, 16, WHITE);
    display();
    delay(27);
    // longer delay post boot, we put it inside the loop to
    // save the flash calling clear/delay again outside the loop
    if (y==-16) {
      delay(250);
    }
  }

  delay(750);
//  setRGBled(0,0,0);
}

void setFrameRate(uint8_t rate)
{
	frameRate = rate;
  eachFrameMillis = 1000/rate;
}

bool everyXFrames(uint8_t frames)
{
  return frameCount % frames == 0;
}

bool nextFrame()
{
  long now = millis();
  uint8_t remaining;

  // post render
  if (post_render) {
    lastFrameDurationMs = now - lastFrameStart;
    frameCount++;
    post_render = false;
  }

  // if it's not time for the next frame yet
  if (now < nextFrameStart) {
    remaining = nextFrameStart - now;
    // if we have more than 1ms to spare, lets sleep
    // we should be woken up by timer0 every 1ms, so this should be ok
//    if (remaining > 1)
//      idle();
    return false;
  }

  // pre-render

  // technically next frame should be last frame + each frame but if we're
  // running a slow render we would constnatly be behind the clock
  // keep an eye on this and see how it works.  If it works well the
  // lastFrameStart variable could be eliminated completely
  nextFrameStart = now + eachFrameMillis;
  lastFrameStart = now;
  post_render = true;
  return post_render;
}

int cpuLoad()
{
  return lastFrameDurationMs*100 / eachFrameMillis;
}

void initRandomSeed()
{

}

uint16_t rawADC(byte adc_bits)
{

  return 0;
}

void clear()
{
  fillScreen(BLACK);
}

void drawPixel(int x, int y, uint8_t color)
{
  #ifdef PIXEL_SAFE_MODE
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1))
  {
    return;
  }
  #endif

  uint8_t row = (uint8_t)y / 8;
  if (color)
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  }
  else
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
  }
}

uint8_t getPixel(uint8_t x, uint8_t y)
{
  uint8_t row = y / 8;
  uint8_t bit_position = y % 8;
  return (sBuffer[(row*WIDTH) + x] & _BV(bit_position)) >> bit_position;
}

void drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void drawCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x4)
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8)
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1)
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color)
{
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void fillCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta,
 uint8_t color)
{
  // used to do circles and roundrects!
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }

    if (cornername & 0x2)
    {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void drawLine
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
  // bresenham's algorithm - thx wikpedia
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      drawPixel(y0, x0, color);
    }
    else
    {
      drawPixel(x0, y0, color);
    }

    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void drawFastVLine
(int16_t x, int16_t y, uint8_t h, uint8_t color)
{
  int end = y+h;
  for (int a = max(0,y); a < min(end,HEIGHT); a++)
  {
    drawPixel(x,a,color);
  }
}

void drawFastHLine
(int16_t x, int16_t y, uint8_t w, uint8_t color)
{
  int end = x+w;
  for (int a = max(0,x); a < min(end,WIDTH); a++)
  {
    drawPixel(a,y,color);
  }
}

void fillRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++)
  {
    drawFastVLine(i, y, h, color);
  }
}

void fillScreen(uint8_t color)
{
  // C version : 
  if (color) color = 0xFF;  //change any nonzero argument to b11111111 and insert into screen array.
//  for(int16_t i=0; i<1024; i++)  { sBuffer[i] = color; }  //sBuffer = (128*64) = 8192/8 = 1024 bytes. 
	
	memset(sBuffer, color, sizeof(sBuffer));
  
//  asm volatile
//  (
//    // load color value into r27
//    "mov r27, %1 \n\t"
//    // if value is zero, skip assigning to 0xff
//    "cpse r27, __zero_reg__ \n\t"
//    "ldi r27, 0xff \n\t"
//    // load sBuffer pointer into Z
//    "movw  r30, %0\n\t"
//    // counter = 0
//    "clr __tmp_reg__ \n\t"
//    "loopto:   \n\t"
//    // (4x) push zero into screen buffer,
//    // then increment buffer position
//    "st Z+, r27 \n\t"
//    "st Z+, r27 \n\t"
//    "st Z+, r27 \n\t"
//    "st Z+, r27 \n\t"
//    // increase counter
//    "inc __tmp_reg__ \n\t"
//    // repeat for 256 loops
//    // (until counter rolls over back to 0)
//    "brne loopto \n\t"
//    // input: sBuffer, color
//    // modified: Z (r30, r31), r27
//    :
//    : "r" (sBuffer), "r" (color)
//    : "r30", "r31", "r27"
//  );
}

void drawRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  drawFastHLine(x+r, y, w-2*r, color); // Top
  drawFastHLine(x+r, y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x, y+r, h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r, h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}

void fillRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

void drawTriangle
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void fillTriangle
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{

  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    swap(&y0, &y1); swap(&x0, &x1);
  }
  if (y1 > y2)
  {
    swap(&y2, &y1); swap(&x2, &x1);
  }
  if (y0 > y1)
  {
    swap(&y0, &y1); swap(&x0, &x1);
  }

  if(y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)
    {
      a = x1;
    }
    else if(x1 > b)
    {
      b = x1;
    }
    if(x2 < a)
    {
      a = x2;
    }
    else if(x2 > b)
    {
      b = x2;
    }
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t dx01 = x1 - x0,
      dy01 = y1 - y0,
      dx02 = x2 - x0,
      dy02 = y2 - y0,
      dx12 = x2 - x1,
      dy12 = y2 - y1,
      sa = 0,
      sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
  {
    last = y1;   // Include y1 scanline
  }
  else
  {
    last = y1-1; // Skip it
  }


  for(y = y0; y <= last; y++)
  {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b)
    {
      swap(&a,&b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);

  for(; y <= y2; y++)
  {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b)
    {
      swap(&a,&b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }
}

void drawBitmap
(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, 
 uint8_t color)
{
  // no need to dar at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h/8;
  if (h%8!=0) rows++;
  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++) {
        if (iCol + x > (WIDTH-1)) break;
        if (iCol + x >= 0) {
          if (bRow >= 0) {
            if      (color == WHITE) sBuffer[ (bRow*WIDTH) + x + iCol ] |= *(bitmap+(a*w)+iCol) << yOffset;
            else if (color == BLACK) sBuffer[ (bRow*WIDTH) + x + iCol ] &= ~(*(bitmap+(a*w)+iCol) << yOffset);
            else                     sBuffer[ (bRow*WIDTH) + x + iCol ] ^= *(bitmap+(a*w)+iCol) << yOffset;
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
            if      (color == WHITE) sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] |= *(bitmap+(a*w)+iCol) >> (8-yOffset);
            else if (color == BLACK) sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] &= ~(*(bitmap+(a*w)+iCol) >> (8-yOffset));
            else                     sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] ^= *(bitmap+(a*w)+iCol) >> (8-yOffset);
          }
        }
      }
    }
  }
}


void drawSlowXYBitmap
(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
  // no need to dar at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;

  int16_t xi, yi, byteWidth = (w + 7) / 8;
  for(yi = 0; yi < h; yi++) {
    for(xi = 0; xi < w; xi++ ) {
      if(*(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7))) {
        drawPixel(x + xi, y + yi, color);
      }
    }
  }
}


void drawChar
(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  boolean draw_background = bg != color;

  if ((x >= WIDTH) ||         // Clip right
    (y >= HEIGHT) ||        // Clip bottom
    ((x + 5 * size - 1) < 0) ||   // Clip left
    ((y + 8 * size - 1) < 0)    // Clip top
  )
  {
    return;
  }

  for (int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if (i == 5)
    {
      line = 0x0;
    }
    else
    {
      line = *(font+(c*5)+i);
    }

    for (int8_t j = 0; j<8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            drawPixel(x + (i * size) + a, y + (j * size) + b, draw_color);
          }
        }
      }
      line >>= 1;
    }
  }
}

void setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

void setTextSize(uint8_t s)
{
  // textsize must always be 1 or higher
  textsize = max(1,s); 
}

void setTextWrap(boolean w)
{
  wrap = w;
}

void print(const char* str)
{
	if(str == NULL) return;
	while(*str != '\0')
	{
		write(*(str++));
	}
}

void display()
{
	if(arduboy.paintHandler)
	{
		arduboy.paintHandler(sBuffer);
	}else
	{
		paintScreen(sBuffer);
	}
}

unsigned char* getBuffer()
{
  return sBuffer;
}

void swap(int16_t* a, int16_t* b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}


Arduboy arduboy = 
{
	.width = width,
	.height = height,
	.pressed = pressed,
	.notPressed = notPressed,
	.begin = begin,
	.beginNoLogo = beginNoLogo,
	.bootLogo = bootLogo,
	.clear = clear,
	.display = display,
	.drawPixel = drawPixel,
	.getPixel = getPixel,
	.drawCircle = drawCircle,
	.drawCircleHelper = drawCircleHelper,
	.fillCircle = fillCircle,
	.fillCircleHelper = fillCircleHelper,
	.drawLine = drawLine,
	.drawRect = drawRect,
	.drawFastVLine = drawFastVLine,
	.drawFastHLine = drawFastHLine,
	.fillRect = fillRect,
	.fillScreen = fillScreen,
	.drawRoundRect = drawRoundRect,
	.fillRoundRect = fillRoundRect,
	.drawTriangle = drawTriangle,
	.fillTriangle = fillTriangle,
	.drawBitmap = drawBitmap,
	.drawSlowXYBitmap = drawSlowXYBitmap,
	.drawChar = drawChar,
	.setCursor = setCursor,
	.setTextSize = setTextSize,
	.setTextWrap = setTextWrap,
	.getBuffer = getBuffer,
	.print = print,
	.initRandomSeed = initRandomSeed,
	.swap = swap,
	.setFrameRate = setFrameRate,
	.nextFrame = nextFrame,
	.everyXFrames = everyXFrames,
	.cpuLoad = cpuLoad
};







