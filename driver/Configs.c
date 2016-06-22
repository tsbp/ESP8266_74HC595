//=============================================================================
#include <ets_sys.h>
#include "osapi.h"
#include "c_types.h"
#include "gpio.h"
#include "os_type.h"
#include <mem.h>
#include "user_config.h"
#include "user_interface.h"
#include "driver/N2730LCD.h"
#include "driver/Configs.h"
#include "driver/services.h"
//==============================================================================
s_DATE_TIME date_time = {.DATE.day   = 25,
                         .DATE.month = 1,
                         .DATE.year  = 2016,
                         .TIME.hour = 16,
                         .TIME.min = 24};
unsigned char daysInMonth[]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int  dayOfWeek;
//==============================================================================
void ICACHE_FLASH_ATTR printTime(void)
{
	int x = 12, y = 48;
	printDigitT_16x24(x, y, GREEN, 0x1f, date_time.TIME.hour/10); x += 16;
	printDigitT_16x24(x, y , GREEN, 0x1f,  date_time.TIME.hour%10);x += 16;
	printDigitT_16x24(x, y , GREEN, 0x1f,  10);x += 16;
	printDigitT_16x24(x, y , GREEN, 0x1f,  date_time.TIME.min/10);x += 16;
	printDigitT_16x24(x, y , GREEN, 0x1f,  date_time.TIME.min%10);x += 32;
//	printDigitT_16x24(x, y , YELLOW, BLUE,  10);x += 16;
//	printDigitT_16x24(x, y , YELLOW, BLUE,  date_time.TIME.sec/10);x += 17;
//	printDigitT_16x24(x, y , YELLOW, BLUE,  date_time.TIME.sec%10);x += 16;

	printDigitT_16x24(x, y , 0x7f7fff, 0x1f,  date_time.DATE.day/10);x += 16;
	printDigitT_16x24(x, y , 0x7f7fff, 0x1f,  date_time.DATE.day%10);x += 16;
	printDigitT_16x24(x, y , 0x7f7fff, 0x1f,  19);x += 16;
	printDigitT_16x24(x, y , 0x7f7fff, 0x1f,  (date_time.DATE.month+1)/10);x += 16;
	printDigitT_16x24(x, y , 0x7f7fff, 0x1f,  (date_time.DATE.month+1)%10);x += 20;


	switch(getDayOfWeek())
	{
		case 0: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  16);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  18);x += 16; break;
		case 1: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  17);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  16);x += 16; break;
		case 2: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  15);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  12);x += 16; break;
		case 3: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  17);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  11);x += 16; break;
		case 4: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  16);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  13);x += 16; break;
		case 5: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  14);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  11);x += 16; break;
		case 6: printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  15);x += 16; printDigitT_16x24(x, y , 0x7f7f00, 0x1f,  11);x += 16; break;
	}
}
//==============================================================================
int getDayOfWeek(void)
{
  int m, Y = date_time.DATE.year;
  if (date_time.DATE.month < 2) { Y = Y - 1; m = date_time.DATE.month + 13;}
  else                          {            m = date_time.DATE.month + 1;}

  int a = (date_time.DATE.day + (26*(m+1)/10) + Y + Y/4 + 6*Y/100 + Y/400) % 7;
   return a;
}
//==============================================================================
void ICACHE_FLASH_ATTR timeIncrement(void)
{
  date_time.TIME.sec++;
  if(date_time.TIME.sec >= 60)
  {
    date_time.TIME.sec = 0;
    date_time.TIME.min++;
    if(date_time.TIME.min >= 60)
    {
      date_time.TIME.min = 0;
      date_time.TIME.hour++;
      if(date_time.TIME.hour >= 24)
      {
        date_time.TIME.hour = 0;
        date_time.DATE.day++;
        if (date_time.DATE.day > daysInMonth[date_time.DATE.month])
        {
          date_time.DATE.day = 1;
          date_time.DATE.month ++;
          if(date_time.DATE.month >= 12)
          {
            date_time.DATE.month = 0;
            date_time.DATE.year++;
          }
        }
      }
    }
  }
        remoteTemp.timeData[0] = (uint8) date_time.TIME.sec;
  		remoteTemp.timeData[1] = (uint8) date_time.TIME.min;
  		remoteTemp.timeData[2] = (uint8) date_time.TIME.hour;
  		remoteTemp.timeData[3] = (uint8) date_time.DATE.day;
  		remoteTemp.timeData[4] = (uint8) date_time.DATE.month;
  		remoteTemp.timeData[5] = (uint8) (date_time.DATE.year - 2000);
}
//==============================================================================
void ICACHE_FLASH_ATTR timeUpdate(char *aPtr)
{
						  date_time.DATE.year  = aPtr[2] + 2000;
		                  date_time.DATE.month = aPtr[3];
		                  date_time.DATE.day   = aPtr[4];

		                  date_time.TIME.hour =  aPtr[5];
		                  date_time.TIME.min   = aPtr[6];
		                  date_time.TIME.sec   = aPtr[7];
}
//=============================================================================
u_CONFIG configs = {
		.cfg[0].periodsCnt = 1,
		.cfg[0].pConfig[0].hStart = 0,
		.cfg[0].pConfig[0].mStart = 0,
		.cfg[0].pConfig[0].temperature = 250,

		.cfg[1].periodsCnt = 1,
		.cfg[1].pConfig[0].hStart = 0,
		.cfg[1].pConfig[0].mStart = 0,
		.cfg[1].pConfig[0].temperature = 230,

	    .hwSettings.wifi.mode = STATION_MODE,
		.hwSettings.wifi.SSID = "voodoo",
        .hwSettings.wifi.SSID_PASS = "eminem82"};
//=============================================================================
void ICACHE_FLASH_ATTR saveConfigs(void) {
	int result = -1;
	os_delay_us(100000);
	result = spi_flash_erase_sector(PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE);
	result = -1;
	os_delay_us(100000);
	result = spi_flash_write(
			(PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
			(uint32 *) &configs, sizeof(u_CONFIG));

	ets_uart_printf("Write W = %d\r\n", result);
}

//=============================================================================
void ICACHE_FLASH_ATTR readConfigs(void) {
	int result = -1;
	result = spi_flash_read(
			(PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
			(uint32 *) &configs, sizeof(u_CONFIG));
}
//==============================================================================
uint16 ICACHE_FLASH_ATTR getSetTemperature()
{
	 
  unsigned int aTime = date_time.TIME.hour * 60 + date_time.TIME.min;

  int aDayNumber = getDayOfWeek();

  unsigned char aDay;
  u_CONFIG *aPtr;
  unsigned long col;
  
  if      (aDayNumber == 0)  aDay = configs.nastr.day[5];
  else if (aDayNumber == 1)  aDay = configs.nastr.day[6]; 
  else                       aDay = configs.nastr.day[aDayNumber - 2]; 
  
  u_CONFIG_u cPtr;// = (aDay == 'H') ? configs.cfg[1] : configs.cfg[0];

  if(aDay == 'H'){ cPtr = configs.cfg[1]; print_icon(8, 8, 0xff7f7f, 0x5f, 4);}
  else           { cPtr = configs.cfg[0]; print_icon(8, 8, 0x7f00, 0x5f, 5);}

  cPtr = (aDay == 'H') ? configs.cfg[1] : configs.cfg[0];
      
  uint32 curPeriod = 0;

  for(curPeriod = 0; curPeriod < (cPtr.periodsCnt - 1); curPeriod++)
  {
	//uint32 a = cPtr.pConfig[curPeriod + 1].hmStart;
	unsigned int end = cPtr.pConfig[curPeriod + 1].hStart * 60 + cPtr.pConfig[curPeriod + 1].mStart;;
//			(((a>>24) - '0') * 10 +   (((a>>16) & 0x00000ff) - '0')) * 60 +
//                       ((((a>>8) & 0x00000ff) - '0') * 10 +   ((a & 0x00000ff) - '0'));
    if(aTime < end)  break;     
  }

  // set temperature
  char_6x8(36 + 28 , 12 , GREEN, 0x5f, (char)(cPtr.pConfig[curPeriod].temperature / 100) + '0');
  char_6x8(48 + 28 , 12 , GREEN, 0x5f, (char)((cPtr.pConfig[curPeriod].temperature % 100) /10) + '0');
  char_6x8(60 + 28 , 12 , GREEN, 0x5f, ',');
  char_6x8(72 + 28 , 12 , GREEN, 0x5f, (char)(cPtr.pConfig[curPeriod].temperature % 10) + '0');

  // delta
  char_6x8(96 + 28 , 12 , 0x7f00, 0x5f, 0x7e);
  char_6x8(108 + 28 , 12 , 0x7f00, 0x5f, configs.nastr.delta/10 + '0');
  char_6x8(120 + 28 , 12 , 0x7f00, 0x5f, ',');
  char_6x8(132 + 28 , 12 , 0x7f00, 0x5f, configs.nastr.delta%10 + '0');


  return cPtr.pConfig[curPeriod].temperature;
}
//==============================================================================
unsigned char ICACHE_FLASH_ATTR cmpTemperature (uint16 aT, signed int arcTemper)
{  
  static unsigned char out = 0;

  ///int tmp = (aT[2] - '0') * 100 + (aT[1] - '0') * 10 + (aT[0] - '0');
  
  if      (arcTemper > aT + (configs.nastr.delta))
  {
	print_icon(34, 8, GREEN, 0x5f, 1);
    out = 0; 
  }
  else if (arcTemper < aT - (configs.nastr.delta))
  {
	  print_icon(34, 8, RED, 0x5f, 0);
    out = 1;
  }
  

 return out; 
}
//==============================================================================
void ICACHE_FLASH_ATTR showTemperature(uint8 aSwap, unsigned char *aBuf)
{
	if (aBuf[0] == '+')
				printDigit_16x32(20 + aSwap*115, 98, GREEN, 0x1f, 11);
			else
				printDigit_16x32(20 + aSwap*115, 98, GREEN, 0x1f, 12);

			printDigit_16x32(20+16 + aSwap*115, 98, GREEN, 0x1f, aBuf[1] - '0');
			printDigit_16x32(20+16*2 + aSwap*115, 98, GREEN, 0x1f, aBuf[2] - '0');
			printDigit_16x32(20+16*3 + aSwap*115, 98, GREEN, 0x1f, 10);
			printDigit_16x32(20+16*4 + aSwap*115, 98, GREEN, 0x1f, aBuf[3] - '0');

}
