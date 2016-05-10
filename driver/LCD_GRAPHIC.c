//==============================================================================
#include "driver/LCD_GRAPHIC.h"
#include "driver/N2730LCD.h"
//==============================================================================
unsigned int _width    = WIDTH;
unsigned int _height   = HEIGHT;
//==============================================================================
void tft_fillRect(unsigned int x, unsigned int y, 
                  unsigned int w, unsigned int h, 
                  unsigned long color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	// TODO: this can result in a very long transaction time
	// should break this into multiple transactions, even though
	// it'll cost more overhead, so we don't stall other SPI libs
	//SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	LCD_setAddr(x, y, x+w-1, y+h-1);
	lcdWrite(COMMAND, RAMWR);
	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			setPixel(color);
		}
		//setPixel(color);
//		if (y > 1 && (y & 1)) {
//			SPI.endTransaction();
//			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
//		}
	}
//	SPI.endTransaction();
}
//==============================================================================
void tft_fillScreen(unsigned long color)
{
	tft_fillRect(0, 0, _width, _height, color);
}
//==============================================================================
void tft_drawFastVLine(unsigned int x, unsigned int y, unsigned int h, unsigned long color)
{
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((y+h-1) >= _height) h = _height-y;
	//SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	LCD_setAddr(x, y, x, y+h-1);
	lcdWrite(COMMAND, RAMWR);
	while (h-- > 0) {
		setPixel(color);//writedata16_cont(color);
	}
//	writedata16_last(color);
//	SPI.endTransaction();
}
//==============================================================================
void tft_drawFastHLine(unsigned int x, unsigned int y, unsigned int w, unsigned long color)
{
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
	//SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	LCD_setAddr(x, y, x+w-1, y);
	lcdWrite(COMMAND, RAMWR);//writecommand_cont(ILI9341_RAMWR);
	while (w-- > 0) {
		setPixel(color);//writedata16_cont(color);
	}
//	writedata16_last(color);
//	SPI.endTransaction();
}
//==============================================================================
void drawPixel( int x,  int y,  long color) {

	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

	//SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	LCD_setAddr(x, y, x, y);
	lcdWrite(COMMAND, RAMWR);//writecommand_cont(ILI9341_RAMWR);
	setPixel(color);//writedata16_last(color);
	//SPI.endTransaction();
}
//==============================================================================
void drawCircleHelper( unsigned int x0, unsigned int y0,
                       unsigned int r, unsigned int cornername, 
                       unsigned long color) 
{
  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}
//==============================================================================
// Draw a circle outline
//==============================================================================
void tft_drawCircle(unsigned int x0, unsigned int y0, 
                    unsigned int r,    unsigned long color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
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


//==============================================================================
// Draw a rounded rectangle
//==============================================================================
void tft_drawRoundRect(unsigned int x, unsigned int y, 
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) 
{
  // smarter version
  tft_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  tft_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  tft_drawFastVLine(x    , y+r  , h-2*r, color); // Left
  tft_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}
//==============================================================================
// Used to do circles and roundrects
//==============================================================================
void fillCircleHelper(unsigned int x0, unsigned int y0, 
                      unsigned int r,    unsigned int cornername, 
                      unsigned int delta, unsigned long color) {

  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      tft_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      tft_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      tft_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      tft_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
//==============================================================================
// Fill a rounded rectangle
//==============================================================================
void tft_fillRoundRect(unsigned int x, unsigned int y, 
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) 
{
  // smarter version
  tft_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}
//==============================================================================
