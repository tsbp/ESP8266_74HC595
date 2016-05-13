//==============================================================================
#include "driver/plot.h"
#include "driver/N2730LCD.h"
#include "driver/LCD_GRAPHIC.h"
//==============================================================================
signed int tBuffer[POINTS_CNT];// = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
signed int tBuffer2[POINTS_CNT];// = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int plotIntervalCntr;// = nastroyki -> interval;
//==============================================================================
void ICACHE_FLASH_ATTR  valueToBuffer(signed int aVal, signed int *aBuf)
{
	int i;
    for(i = 0; i < (POINTS_CNT - 1); i++) aBuf[i] = aBuf[i+1];
    aBuf[POINTS_CNT - 1] = aVal;  
}
//==============================================================================
void ICACHE_FLASH_ATTR  showGraphic(signed int *aBuf, unsigned int aY, uint32 BGCOL)
{  
	int i;
  static float cena = 0;
  static signed int tmax, tmin;
//#define BGCOL   (0xa0a050)
  
  tft_fillRect (0, aY, AREA_WIDTH, AREA_HEIGH, BGCOL);
  tft_drawFastHLine(LEFT_OFFSET, aY + TOP_OFFSET, PLOT_WIDTH,  0);
  for(i = 0; i < VGRID_CNT+1;  i++) tft_drawFastHLine(LEFT_OFFSET,  aY + PLOT_HEIGH -  i*VGRID_SPACING + TOP_OFFSET, PLOT_WIDTH,  0);
  for(i = 0; i < POINTS_CNT; i++) tft_drawFastVLine(i*HGRID_SPACING + LEFT_OFFSET, aY + TOP_OFFSET, PLOT_HEIGH,  0);
  
  tmax = aBuf[0], tmin = aBuf[0];
  for(i = 1; i < POINTS_CNT; i++) if (tmax < aBuf[i]) tmax = aBuf[i]; // tmax
  for(i = 1; i < POINTS_CNT; i++) if (tmin > aBuf[i]) tmin = aBuf[i]; // tmin
  
  tmax /= 10; tmax *= 10; tmax += 10;
  tmin /= 10; tmin *= 10; tmin -= 10;
  
  float delta = tmax - tmin;
  
  cena = PLOT_HEIGH / delta;
  int tmp  = tmax;
  if (tmp < 0) { char_6x8_s(0, aY + TOP_OFFSET - 4, YELLOW, BGCOL, '-');  tmp *= (-1);}
  else         { char_6x8_s(0, aY + TOP_OFFSET - 4, YELLOW, BGCOL, '+');}
  char_6x8_s(6,  aY + TOP_OFFSET - 4, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
  char_6x8_s(12, aY + TOP_OFFSET - 4, YELLOW, BGCOL, (tmp /10) + '0'); 
  char_6x8_s(18, aY + TOP_OFFSET - 4, YELLOW, BGCOL, ','); 
  char_6x8_s(22, aY + TOP_OFFSET - 4, YELLOW, BGCOL, (tmp %10) + '0'); 
  
  tmp  = tmin;
  if (tmp < 0) { char_6x8_s(0, aY + AREA_HEIGH-9, YELLOW, BGCOL, '-');  tmp *= (-1);}
  else         { char_6x8_s(0, aY + AREA_HEIGH-9, YELLOW, BGCOL, '+');}
  char_6x8_s(6,  aY + AREA_HEIGH-9, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
  char_6x8_s(12, aY + AREA_HEIGH-9, YELLOW, BGCOL, (tmp /10) + '0'); 
  char_6x8_s(18, aY + AREA_HEIGH-9, YELLOW, BGCOL, ','); 
  char_6x8_s(22, aY + AREA_HEIGH-9, YELLOW, BGCOL, (tmp %10) + '0');     
  
  tmp  = aBuf[POINTS_CNT-1];
  if (tmp < 0) { char_6x8_s(0, aY + AREA_HEIGH/2-9, YELLOW, BGCOL, '-');  tmp *= (-1);}
    else         { char_6x8_s(0, aY + AREA_HEIGH/2-9, YELLOW, BGCOL, '+');}
    char_6x8_s(6,  aY + AREA_HEIGH/2-9, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
    char_6x8_s(12, aY + AREA_HEIGH/2-9, YELLOW, BGCOL, (tmp /10) + '0');
    char_6x8_s(18, aY + AREA_HEIGH/2-9, YELLOW, BGCOL, ',');
    char_6x8_s(22, aY + AREA_HEIGH/2-9, YELLOW, BGCOL, (tmp %10) + '0');

  
  for(i = 0; i < POINTS_CNT-1; i++)
        line (i*HGRID_SPACING + LEFT_OFFSET,       aY + PLOT_HEIGH + TOP_OFFSET - (unsigned int)((aBuf[i]   - tmin)*cena), 
              (i +1) *HGRID_SPACING + LEFT_OFFSET, aY + PLOT_HEIGH + TOP_OFFSET - (unsigned int)((aBuf[i+1] - tmin)*cena),  YELLOW);
}
