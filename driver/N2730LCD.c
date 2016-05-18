//==============================================================================
#include "driver/N2730LCD.h"
//#include "driver/hspi.h"
#include "driver/fonts.h"
//==============================================================================
uLCD_WORD lcdWord;
//==============================================================================
void ICACHE_FLASH_ATTR serialSendData(void)
{
	//os_printf("%04x\r\n", lcdWord.word);
	hspi_send_uint16((uint16)lcdWord.word);
	hspi_wait_ready();
}
//==============================================================================
void ICACHE_FLASH_ATTR lcdWrite(unsigned char aType, unsigned char aByte)
{

  LCD_DCX(aType);
  serialSendData();
  lcdWord.data = aByte;
  serialSendData();
  LCD_WRX(0);
  serialSendData();
  LCD_WRX(1);
  serialSendData();
}
//==============================================================================
void ICACHE_FLASH_ATTR LCD_Init(void)
{



}
//==============================================================================
void ICACHE_FLASH_ATTR LCD_setAddr(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
{
   lcdWrite(COMMAND, CASET);
   lcdWrite(DATA, (unsigned char)((x0) >> 8));
   lcdWrite(DATA, (unsigned char) (x0));
   lcdWrite(DATA, (unsigned char)((x1) >> 8));
   lcdWrite(DATA, (unsigned char) (x1));

   lcdWrite(COMMAND, RASET);

   lcdWrite(DATA, (unsigned char)((y0) >> 8));
   lcdWrite(DATA, (unsigned char) (y0));
   lcdWrite(DATA, (unsigned char)((y1) >> 8));
   lcdWrite(DATA, (unsigned char) (y1));
}
//==============================================================================
void ICACHE_FLASH_ATTR LCD_wakeup(void)
{
  LCD_RDX(1); serialSendData();
  LCD_CSX(1);   serialSendData();          //RES = 0;
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_CSX(0);   serialSendData();          //RES = 1;
  LCD_WRX(1); serialSendData();             //WR = 1;
  LCD_RESX(0);serialSendData();//LCD_RESX(0);             //RES = 0;
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_RESX(1); serialSendData();//LCD_RESX(1);             //RES = 1;
  lcdWrite(COMMAND, SWRESET); //SendCom(0x01);       // Выход дисплея из спячки
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(5);
  lcdWrite(COMMAND, SLPOUT); //SendCom(0x11);       // Выход дисплея из спячки
  lcdWrite(COMMAND, DISPON); //SendCom(0x29);       // Включение дисплея
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  lcdWrite(COMMAND, MADCTR); //SendCom(0x36);
  lcdWrite(DATA,    0xc0); //SendDat(0xC0);       // Книжная ориентация дисплея
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  os_delay_us(100000); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
//  lcdWrite(COMMAND, COLMOD); //SendCom(0x3A);
//  lcdWrite(DATA,    7); //SendDat(0b00000111); // 24 битныйрежим

}
//==============================================================================
void ICACHE_FLASH_ATTR setPixel(unsigned long aCol)
{
  lcdWrite(DATA, (unsigned char)(aCol >> 16));
  lcdWrite(DATA, (unsigned char)(aCol >> 8));
  lcdWrite(DATA, (unsigned char)aCol);
}
//==============================================================================
void ICACHE_FLASH_ATTR lcd_clear(unsigned long aCol)
{
	int i, j;
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((0) >> 8));
   lcdWrite(DATA, (unsigned char)(0));
   lcdWrite(DATA, (unsigned char)((239) >> 8));
   lcdWrite(DATA, (unsigned char)(239));

   lcdWrite(COMMAND, RASET);

   lcdWrite(DATA, (unsigned char)(0 >> 8));
   lcdWrite(DATA, (unsigned char)(0));
   lcdWrite(DATA, (unsigned char)((319) >> 8));
   lcdWrite(DATA, (unsigned char)(319));

  lcdWrite(COMMAND, RAMWR);

  for(i = 0; i < 240; i++)
      for(j = 0; j < 240; j++) setPixel(aCol);

}
//==============================================================================
void ICACHE_FLASH_ATTR char_6x8 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{

   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+11) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+11));

   lcdWrite(COMMAND, RASET);

   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+16) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+16));



   lcdWrite(COMMAND, RAMWR);
   int i, j;

   for(i = 0; i < 8; i++)
   {
      for(j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
      for(j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
   }
}
//==============================================================================
void ICACHE_FLASH_ATTR char_6x8_s (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+5) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+5));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+7) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+7));

   lcdWrite(COMMAND, RAMWR);

   int i, j;
   for(i = 0; i < 8; i++)
   {
      for(j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL);}
        else                               { setPixel(aBGCOL);}
      }

   }
}
//==============================================================================
#define SYM_HEIGHT  (24/8) // количество байт в высоту
#define SYM_WIDGHT  (16)
//==============================================================================
void ICACHE_FLASH_ATTR printDigit_16x24 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+15) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+15));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+23));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 3; i++)
	   for(j = 0; j < 8; j++)
		   for(k = 0; k < 16; k++)
		   {
			   if (Times_16x24[16*3*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL);}
			   else                               { setPixel(aBGCOL);}
		   }
}
//==============================================================================
void ICACHE_FLASH_ATTR printDigitT_16x24 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+15) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+15));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+23));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 3; i++)
	   for(j = 0; j < 8; j++)
		   for(k = 0; k < 16; k++)
		   {
			   if (digit_16x24[16*3*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL);}
			   else                               { setPixel(aBGCOL);}
		   }
}
//==============================================================================
void ICACHE_FLASH_ATTR print_icon (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+23));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+23));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 3; i++)
	   for(j = 0; j < 8; j++)
		   for(k = 0; k < 24; k++)
		   {
			   if (icons[24*3*aChar + 24*i + k] & (1 << j)) { setPixel(aCOL);}
			   else                               { setPixel(aBGCOL);}
		   }
}
//==============================================================================
void ICACHE_FLASH_ATTR printDigit_16x32 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+15) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+15));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+31) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+31));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 4; i++)
	   for(j = 0; j < 8; j++)
		   for(k = 0; k < 16; k++)
		   {
			   if (GOST_16x32[16*4*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL);}
			   else                               { setPixel(aBGCOL);}
		   }
}
//==============================================================================
void ICACHE_FLASH_ATTR printStringS (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char *aStr)
{
  while (*aStr)
  {
    char_6x8_s(aX, aY, aCOL, aBGCOL, *aStr++);
    aX += 6;
  }
}
//==============================================================================
void ICACHE_FLASH_ATTR printString (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char *aStr)
{
  while (*aStr)
  {
    char_6x8(aX, aY, aCOL, aBGCOL, *aStr++);
    aX += 12;
  }
}
