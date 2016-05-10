//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/N2730LCD.h"
#include "driver/DS18B20_PR.h"
#include "driver/Configs.h"
#include "driver/UDP_Source.h"
#include "driver/wifi.h"
#include "driver/gpio16.h"
#include "driver/LCD_GRAPHIC.h"
//==============================================================================
extern int ets_uart_printf(const char *fmt, ...);
int (*console_printf)(const char *fmt, ...) = ets_uart_printf;

#define LOOP_PERIOD		(1000) // in msec
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

static volatile os_timer_t loop_timer;
static void  loop_timer_cb(os_event_t *events);

static char temperature[2][POINTS_CNT][4];

//==============================================================================
void ICACHE_FLASH_ATTR loop_timer_cb(os_event_t *events)
{
	char temp[2][4];
	ds18b20(0, temp[0]);
	ds18b20_Convert();


	//=========== show temperature ===================
		if (temp[0][0] == '+')
			printDigit_16x24(20 , 120 , YELLOW, BLUE, 12);
		else
			printDigit_16x24(20 , 120 , YELLOW, BLUE, 13);

		printDigit_16x24(36 , 120 , YELLOW, BLUE, temp[0][1] - '0');
		printDigit_16x24(52 , 120 , YELLOW, BLUE, temp[0][2] - '0');
		printDigit_16x24(68 , 120 , YELLOW, BLUE, 14);
		printDigit_16x24(84 , 120 , YELLOW, BLUE, temp[0][3] - '0');
		//printDigit_16x24(100 , 120 , YELLOW, BLUE, 11);
		addValueToArray(temp[0], temperature[0], NON_ROTATE);

	//================================================
		static int cntr = 60;
		if (cntr)		cntr--;
		else
		{
			cntr = 60;
			ets_uart_printf("T1 = %s, T2 = \r\n", temp[0]);
			addValueToArray(temp[0], temperature[0], ROTATE);
			//addValueToArray(temp[1], temperature[1], ROTATE);
			//mergeAnswerWith(temperature);
		}
		mergeAnswerWith(temperature);

	//================================================
	timeIncrement();
	printTime();
	//printDate();
}
//==============================================================================
void ICACHE_FLASH_ATTR setup(void)
{

	// HSPI init
	hspi_init();
	LCD_wakeup();
	lcd_clear(BLUE);
	tft_fillRoundRect(10, 100, 105, 60, 20, YELLOW);
	tft_fillRoundRect(15, 105, 95, 50, 15, BLUE);
	tft_fillRoundRect(125, 100, 105, 60, 20, YELLOW);
	tft_fillRoundRect(130, 105, 95, 50, 15, BLUE);


	setup_wifi_ap_mode();
	UDP_Init();

	ets_uart_printf("DS init start\r\n");
	ds18b20_init();
	temperArrInit(temperature);

	// Start loop timer
	os_timer_disarm(&loop_timer);
	os_timer_setfn(&loop_timer, (os_timer_func_t *) loop_timer_cb, NULL);
	os_timer_arm(&loop_timer, LOOP_PERIOD, true);
}
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{

	UARTInit(BIT_RATE_115200);
	console_printf("\r\n");

	wifi_station_disconnect();
	wifi_station_set_auto_connect(0);


	// Start setup timer
		os_timer_disarm(&loop_timer);
		os_timer_setfn(&loop_timer, (os_timer_func_t *) setup, NULL);
		os_timer_arm(&loop_timer, 500, false);

}
