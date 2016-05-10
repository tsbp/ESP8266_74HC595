//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
//==============================================================================
#define  WIDTH     (240)
#define  HEIGHT    (320)
//==============================================================================
void ICACHE_FLASH_ATTR tft_fillScreen(unsigned long color);
void ICACHE_FLASH_ATTR tft_fillRect(unsigned int x, unsigned int y,
                  unsigned int w, unsigned int h, 
                  unsigned long color);
void ICACHE_FLASH_ATTR tft_drawRoundRect(unsigned int x, unsigned int y,
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) ;
void ICACHE_FLASH_ATTR tft_fillRoundRect(unsigned int x, unsigned int y,
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) ;
void ICACHE_FLASH_ATTR tft_drawCircle(unsigned int x0, unsigned int y0,
                    unsigned int r,    unsigned long color);
void ICACHE_FLASH_ATTR tft_drawFastVLine(unsigned int x, unsigned int y,
                       unsigned int h, unsigned long color);
void ICACHE_FLASH_ATTR tft_drawFastHLine(unsigned int x, unsigned int y,
                       unsigned int w, unsigned long color);
