//==============================================================================
#include "driver/plot.h"
#include "driver/N2730LCD.h"
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
void ICACHE_FLASH_ATTR  showGraphic(signed int *aBuf, unsigned int aY)
{  
	int i;
  static float cena = 0;
  static signed int tmax, tmin;
#define BGCOL   (0xa0a050)
  
  rectangle (0, aY, AREA_WIDTH, aY + AREA_HEIGH, BGCOL);
  lineH(LEFT_OFFSET, aY + TOP_OFFSET, PLOT_WIDTH,  0x5f5f5f);
  for(i = 0; i < VGRID_CNT+1;  i++) lineH(LEFT_OFFSET,  aY + PLOT_HEIGH -  i*VGRID_SPACING + TOP_OFFSET, PLOT_WIDTH,  0x5f5f5f);
  for(i = 0; i < POINTS_CNT; i++) lineV(i*HGRID_SPACING + LEFT_OFFSET, aY + TOP_OFFSET, PLOT_HEIGH,  0x5f5f5f);
  
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
              (i +1) *HGRID_SPACING + LEFT_OFFSET, aY + PLOT_HEIGH + TOP_OFFSET - (unsigned int)((aBuf[i+1] - tmin)*cena),  RED);
}
////==============================================================================
//void showGraphic(signed int *aBuf)
//{  
//  static float cena = 0;
//  static signed int tmax, tmin;
//#define BGCOL   (0x505050)
//  
//  rectangle (0, 0,AREA_WIDTH, AREA_HEIGH, BGCOL);
//  lineH(LEFT_OFFSET, TOP_OFFSET, PLOT_WIDTH,  0x3f3f3f);
//  for(int i = 0; i < VGRID_CNT+1;  i++) lineH(LEFT_OFFSET,  PLOT_HEIGH -  i*VGRID_SPACING + TOP_OFFSET, PLOT_WIDTH,  0x3f3f3f);
//  for(int i = 0; i < POINTS_CNT; i++) lineV(i*HGRID_SPACING + LEFT_OFFSET,  TOP_OFFSET, PLOT_HEIGH,  0x3f3f3f);
//  
//  tmax = tBuffer[0], tmin = aBuf[0];
//  for(int i = 1; i < POINTS_CNT; i++) if (tmax < tBuffer[i]) tmax = aBuf[i]; // tmax
//  for(int i = 1; i < POINTS_CNT; i++) if (tmin > tBuffer[i]) tmin = aBuf[i]; // tmin
//  
//  tmax /= 10; tmax *= 10; tmax += 10;
//  tmin /= 10; tmin *= 10; tmin -= 10;
//  
//  float delta = tmax - tmin;
//  
//  cena = PLOT_HEIGH / delta;
//  int tmp  = tmax;
//  if (tmp < 0) { char_6x8_s(0, TOP_OFFSET - 4, YELLOW, BGCOL, '-');  tmp *= (-1);}
//  else         { char_6x8_s(0, TOP_OFFSET - 4, YELLOW, BGCOL, '+');}
//  char_6x8_s(6,  TOP_OFFSET - 4, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
//  char_6x8_s(12, TOP_OFFSET - 4, YELLOW, BGCOL, (tmp /10) + '0'); 
//  char_6x8_s(18, TOP_OFFSET - 4, YELLOW, BGCOL, ','); 
//  char_6x8_s(22, TOP_OFFSET - 4, YELLOW, BGCOL, (tmp %10) + '0'); 
//  
//  tmp  = tmin;
//  if (tmp < 0) { char_6x8_s(0, AREA_HEIGH-9, YELLOW, BGCOL, '-');  tmp *= (-1);}
//  else         { char_6x8_s(0, AREA_HEIGH-9, YELLOW, BGCOL, '+');}
//  char_6x8_s(6,  AREA_HEIGH-9, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
//  char_6x8_s(12, AREA_HEIGH-9, YELLOW, BGCOL, (tmp /10) + '0'); 
//  char_6x8_s(18, AREA_HEIGH-9, YELLOW, BGCOL, ','); 
//  char_6x8_s(22, AREA_HEIGH-9, YELLOW, BGCOL, (tmp %10) + '0');     
//  
//  tmp  = aBuf[POINTS_CNT-1];
//  if (tmp < 0) { char_6x8_s(110, TOP_OFFSET + 10, YELLOW, BGCOL, '-');  tmp *= (-1);}
//  else         { char_6x8_s(110, TOP_OFFSET + 10, YELLOW, BGCOL, '+');}
//  char_6x8_s(116, TOP_OFFSET + 10, YELLOW, BGCOL, (tmp /100)+ '0'); tmp %= 100;
//  char_6x8_s(122, TOP_OFFSET + 10, YELLOW, BGCOL, (tmp /10) + '0'); 
//  char_6x8_s(128, TOP_OFFSET + 10, YELLOW, BGCOL, ','); 
//  char_6x8_s(132, TOP_OFFSET + 10, YELLOW, BGCOL, (tmp %10) + '0'); 
//
//  
//  for(int i = 0; i < POINTS_CNT-1; i++)      
//        line (i*HGRID_SPACING + LEFT_OFFSET,       PLOT_HEIGH + TOP_OFFSET - (unsigned int)((aBuf[i] -tmin)*cena), 
//              (i +1) *HGRID_SPACING + LEFT_OFFSET, PLOT_HEIGH + TOP_OFFSET  - (unsigned int)((aBuf[i+1]   -tmin)*cena),  RED);
//}
