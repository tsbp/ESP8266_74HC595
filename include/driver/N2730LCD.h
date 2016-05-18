/*
 * N2730LCD.h
 *
 *  Created on: 29 квіт. 2016 р.
 *      Author: notebook
 */
//==============================================================================
#ifndef INCLUDE_DRIVER_N2730LCD_H_
#define INCLUDE_DRIVER_N2730LCD_H_
//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
//==============================================================================
typedef union
{
	uint16 word;
	struct
	{
		uint8 control;
		uint8 data;

	};
}uLCD_WORD;
extern uLCD_WORD lcdWord;
//==============================================================================
//#define LCD_DATA        (P1OUT)
//#define LCD_DATA_IN     (P1IN)

#define LCD_WRX_BIT                     (1<<2)//(BIT2)
#define LCD_CSX_BIT                     (1<<6)//(BIT6)
#define LCD_RESX_BIT                    (1<<4)//(BIT4)
#define LCD_TE_BIT                      (1<<5)//(BIT5)
#define LCD_DCX_BIT                     (1<<3)//(BIT3)
#define LCD_RDX_BIT                     (1<<1)//(BIT1)

#define LCD_WRX(n) {if (n) lcdWord.control |= LCD_WRX_BIT;   else lcdWord.control &= ~LCD_WRX_BIT;}
#define LCD_CSX(n) {if (n) lcdWord.control |= LCD_CSX_BIT;   else lcdWord.control &= ~LCD_CSX_BIT;}
#define LCD_RESX(n){if (n) lcdWord.control |= LCD_RESX_BIT;  else lcdWord.control &= ~LCD_RESX_BIT;}
#define LCD_TE(n)  {if (n) lcdWord.control |= LCD_TE_BIT;    else lcdWord.control &= ~LCD_TE_BIT;}
#define LCD_DCX(n) {if (n) lcdWord.control |= LCD_DCX_BIT;   else lcdWord.control &= ~LCD_DCX_BIT;}
#define LCD_RDX(n) {if (n) lcdWord.control |= LCD_RDX_BIT;   else lcdWord.control &= ~LCD_RDX_BIT;}
//
//// direction
//#define _IN              (0)
//#define _OUT            (1)
//==============================================================================
#define COMMAND         (0)
#define DATA            (1)
//==============================================================================
#define SWRESET	        (0x01) // Software reset
#define RDDID 	        (0x04) // Read Display ID
#define RDDST	        (0x0A) // Read Display Power Mode
#define RDDPM 	        (0x0B) // Read Display MADCTR
#define RDDMADCTR 	(0x0C) // Read Display Pixel Format
#define RDDCOLMOD	(0x0D) // Read Display Image Mode
#define RDDIM 	        (0x0E) // Read Display Signal Mode
#define RDDSM	        (0x0F) // Read Display Self-diagnostic result
#define RDDSDR	        (0x10) // Sleep in & booster off
#define SLPOUT	        (0x11) // Sleep out & booster on
#define PTLON	        (0x12) // Partial mode on
#define NORON	        (0x13) // Partial off (Normal)
#define INVOFF	        (0x20) // Display inversion off (normal)
#define INVON	        (0x21) // Display inversion on
#define GAMSET	        (0x26) // Gamma curve select
#define DISPOFF	        (0x28) // Display off
#define DISPON	        (0x29) // Display on
#define CASET	        (0x2A) // Column address set
#define RASET	        (0x2B) // Row address set
#define RAMWR           (0x2C) // Memory write
#define RAMRD	        (0x2E) // Memory read
#define PTLAR0	        (0x30) // Partial start/end address set
#define TEOFF 	        (0x34) // Tearing effect line off
#define TEON	        (0x35) // Tearing effect mode set & on
#define MADCTR	        (0x36) // Memory data access control
#define IDMOFF	        (0x38) // Idle mode off
#define IDMON	        (0x39) // Idle mode on
#define COLMOD	        (0x3A) // Interface pixel format
#define WRDISBV	        (0x51) // Write Display Brightness
#define RDDISBV0	(0x52) // Read Display Brightness value
#define WRCTRLD 	(0x53) // Write Control Display
#define RDCTRLD0	(0x54) // Read Control Display
#define WRCABC	        (0x55) // Wrtie Content Adaptive Brightness
#define RDCABC	        (0x56) // Read Content Adaptive Brightness
#define RDID1	        (0xDA) // Read ID1
#define RDID20	        (0xDB) // Read ID2
#define RDID3	        (0xDC) // Read ID3
#define IFMODE	        (0xB0) // Set display interface mode
#define DISCLK	        (0xB1) // Display clock set
#define INVCTR	        (0xB2) // Display inversion control
#define REGCTR	        (0xC0) // Regulator control
#define VCOMCTR0	(0xC1) // VCOML/VCOMH voltage control
#define GAMCTR1 	(0xC8) // Set gamma correction characteristics
#define GAMCTR2 	(0xC9) // Set gamma correction characteristics
#define GAMCTR3 	(0xCA) // Set gamma correction characteristics
#define GAMCTR4	        (0xCB) // Set gamma correction characteristics
#define EPPGMDB	        (0xD0) // Write ID2,VCOM Offset for EEPROM program
#define EPERASE	        (0xD1) // EEPROM erase
#define EPPROG	        (0xD2) // EEPROM program
#define EPRDVRF	        (0xD3) // EEPROM read, verify register set
#define RDVCOF	        (0xD9) // VCOM offset bits read
#define LEDCTRL	        (0xEF) // Write LED control value
//==============================================================================
#define RED       (0xff0000)
#define GREEN     (0x00ff00)
#define BLUE      (0x0000ff)
#define YELLOW    (0xffff00)
#define WHITE     (0xffffff)
#define BLACK     (0x000000)
//==============================================================================
#define FLASH_DIGIT       (0)
#define FLASH_MAIN_SCR    (100)
#define FLASH_MENU_SCR    (600)
#define FLASH_NFOUND      (800)
//==============================================================================
void ICACHE_FLASH_ATTR serialSendData(void);
void ICACHE_FLASH_ATTR lcdWrite(unsigned char aType, unsigned char aByte);
void ICACHE_FLASH_ATTR LCD_Init(void);
void ICACHE_FLASH_ATTR LCD_setAddr(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
void ICACHE_FLASH_ATTR LCD_wakeup(void);
void ICACHE_FLASH_ATTR setPixel(unsigned long aCol);
void ICACHE_FLASH_ATTR lcd_clear(unsigned long aCol);
void ICACHE_FLASH_ATTR char_6x8 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);
void ICACHE_FLASH_ATTR char_6x8_s (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);
void ICACHE_FLASH_ATTR printDigit_16x24 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);
void ICACHE_FLASH_ATTR printDigitT_16x24 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);
void ICACHE_FLASH_ATTR printDigit_16x32 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);
void ICACHE_FLASH_ATTR printString (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char *aStr);
void ICACHE_FLASH_ATTR printStringS (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char *aStr);
void ICACHE_FLASH_ATTR print_icon (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar);

#endif /* INCLUDE_DRIVER_N2730LCD_H_ */
