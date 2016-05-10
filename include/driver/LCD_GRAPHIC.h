//==============================================================================
#define  WIDTH     (240)
#define  HEIGHT    (320)
//==============================================================================
void tft_fillScreen(unsigned long color);
void tft_fillRect(unsigned int x, unsigned int y, 
                  unsigned int w, unsigned int h, 
                  unsigned long color);
void tft_drawRoundRect(unsigned int x, unsigned int y, 
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) ;
void tft_fillRoundRect(unsigned int x, unsigned int y, 
                       unsigned int w, unsigned int h, 
                       unsigned int r, unsigned long color) ;
void tft_drawCircle(unsigned int x0, unsigned int y0, 
                    unsigned int r,    unsigned long color);
void tft_drawFastVLine(unsigned int x, unsigned int y, 
                       unsigned int h, unsigned long color);
void tft_drawFastHLine(unsigned int x, unsigned int y, 
                       unsigned int w, unsigned long color);