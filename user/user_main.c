//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "gpio.h"
#include <gpio.h>
#include "driver/uart.h"
#include "driver/N2730LCD.h"
#include "driver/DS18B20_PR.h"
#include "driver/Configs.h"
#include "driver/UDP_Source.h"
#include "driver/wifi.h"
#include "driver/gpio16.h"
#include "driver/LCD_GRAPHIC.h"
#include "driver/plot.h"
#include "driver/services.h"
//==============================================================================
extern int ets_uart_printf(const char *fmt, ...);
int (*console_printf)(const char *fmt, ...) = ets_uart_printf;

#define LOOP_PERIOD		(1000) // in msec
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

static volatile os_timer_t loop_timer;
static void  loop_timer_cb(os_event_t *events);

static char temperature[2][POINTS_CNT][4];

uint8 swap = 0;

//==============================================================================
void ICACHE_FLASH_ATTR loop_timer_cb(os_event_t *events)
{
	//=========== get temperature ===================
	getTemperature();
	//=========== show temperature ===================
	swap ^= 1;
	showTemperature(swap, tData[swap]);
	addValueToArray(tData[swap], temperature[swap], NON_ROTATE);
	//================================================
	signed int a = (tData[0][3] - '0') + (tData[0][2] - '0') * 10	+ (tData[0][1] - '0') * 100;
	signed int b = (tData[1][3] - '0') + (tData[1][2] - '0') * 10	+ (tData[1][1] - '0') * 100;

	static int cntr = 5;
	if (cntr)
	{
		cntr--;
		if(cntr == 0) 			showGraphic(tBuffer[0], 160, 0x0000a0);
		else if (cntr == 1) 	showGraphic(tBuffer[1], 240, 0x5b5b00);
		else if(configs.hwSettings.deviceMode == DEVICE_MODE_MASTER)
		{
			uint32 t = getSetTemperature();
			gpio_write(1, cmpTemperature ((unsigned char *)(&t), a));
		}
	}
	else
	{
		cntr = PLOT_INTERVAL;
		ets_uart_printf("T1 = %s, T2 = \r\n", tData[0]);
		addValueToArray(tData[0], temperature[0], ROTATE);
		addValueToArray(tData[1], temperature[1], ROTATE);
		//mergeAnswerWith(temperature);

		//===== graphic ========

		if (tData[0][0] == '-')
			a = a * (-1);
		if (tData[1][0] == '-')
			b = b * (-1);
		valueToBuffer(a, tBuffer[0]);
		valueToBuffer(b, tBuffer[1]);
	}
	mergeAnswerWith(temperature);

	//==========================================================================
	if(configs.hwSettings.deviceMode == DEVICE_MODE_MASTER)
	{
		timeIncrement();
		//============= sendUDPbroadcast in DEVICE_MODE_MASTER =================
		if(configs.hwSettings.wifi.mode == STATION_MODE)
			if(wifi_station_get_connect_status() == STATION_GOT_IP)
				sendUDPbroadcast(remoteTemp.byte, (uint16)sizeof(remoteTemp));
		if(configs.hwSettings.wifi.mode == SOFTAP_MODE)
			if(wifi_softap_get_station_num() > 0)
				sendUDPbroadcast(remoteTemp.byte, (uint16)sizeof(remoteTemp));
	}
	printTime();
}
//==============================================================================
void ICACHE_FLASH_ATTR setup(void)
{

	hspi_init();
	LCD_wakeup();
	//saveConfigs();
	readConfigs();

	printString (10, 240, BLACK, WHITE, configs.hwSettings.wifi.SSID);
	printString (10, 256, BLACK, WHITE, configs.hwSettings.wifi.SSID_PASS);


	//====== Draw screen =======
	tft_fillRect(0, 0, 240, 40, 0x5f);
	tft_fillRect(0, 40, 240, 120, 0x1f);
	//tft_fillRect(0, 80, 240, 80, BLUE);

	//tft_fillRect(34, 8, 54, 24, 0xff00ff);

	tft_fillRoundRect(10, 84, 105, 60, 20, BLUE);
	tft_fillRoundRect(15, 89, 95, 50, 15, 0x1f);
	tft_fillRoundRect(125, 84, 105, 60, 20, YELLOW);
	tft_fillRoundRect(130, 89, 95, 50, 15, 0x1f);

	if		(configs.hwSettings.sensor[0].mode == SENSOR_MODE_REMOTE) printStringS(50, 147, GREEN, 0x1f, "REMOTE");
	else if	(configs.hwSettings.sensor[0].mode == SENSOR_MODE_LOCAL)  printStringS(50, 147, GREEN, 0x1f, "LOCAL");

	if		(configs.hwSettings.sensor[1].mode == SENSOR_MODE_REMOTE) printStringS(165, 147, GREEN, 0x1f, "REMOTE");
	else if	(configs.hwSettings.sensor[1].mode == SENSOR_MODE_LOCAL)  printStringS(165, 147, GREEN, 0x1f, "LOCAL");




	ets_uart_printf("configs.hwSettings.wifi.mode = %d\r\n", configs.hwSettings.wifi.mode);
	if(configs.hwSettings.wifi.mode == SOFTAP_MODE)
	{
		print_icon(182, 8, BLUE|GREEN, 0x5f, 6);
		setup_wifi_ap_mode();
	}
	else if(configs.hwSettings.wifi.mode == STATION_MODE)
	{
		print_icon(182, 8, BLUE|GREEN, 0x5f, 7);
		setup_wifi_st_mode();
	}

	ets_uart_printf("configs.nastr.SSID = %s\r\n", configs.hwSettings.wifi.SSID);
	ets_uart_printf("configs.nastr.SSID_PASS = %s\r\n", configs.hwSettings.wifi.SSID_PASS);

	UDP_Init_client();


	ds18b20_init();
	temperArrInit(temperature);

	//saveConfigs();

	if      (configs.hwSettings.deviceMode == DEVICE_MODE_MASTER) { tft_drawRoundRect(62, 8, 54, 24, 5, GREEN); print_icon(208, 8, BLUE, 0x5f, 2);}
	else if (configs.hwSettings.deviceMode == DEVICE_MODE_SLAVE)  print_icon(208, 8, BLUE, 0x5f, 3);

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

	//system_update_cpu_freq(160);
	button_init();

	set_gpio_mode(1,GPIO_PULLUP, GPIO_OUTPUT);

	// Start setup timer
		os_timer_disarm(&loop_timer);
		os_timer_setfn(&loop_timer, (os_timer_func_t *) setup, NULL);
		os_timer_arm(&loop_timer, 500, false);

}
