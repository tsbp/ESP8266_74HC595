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
#include "driver/plot.h"
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
	ds18b20(1, temp[1]);
	ds18b20_Convert();


	//=========== show temperature ===================
	showTemperature(20,  105, temp[0]);
	showTemperature(132, 105, temp[1]);
	addValueToArray(temp[0], temperature[0], NON_ROTATE);
	addValueToArray(temp[1], temperature[1], NON_ROTATE);
	//================================================
	signed int a = (temp[0][3] - '0') + (temp[0][2] - '0') * 10	+ (temp[0][1] - '0') * 100;
	signed int b = (temp[1][3] - '0') + (temp[1][2] - '0') * 10	+ (temp[1][1] - '0') * 100;

	static int cntr = 5;
	if (cntr)
	{
		cntr--;
		uint32 t = getSetTemperature();
		cmpTemperature ((unsigned char *)(&t), a);
	}
	else
	{
		cntr = 60;
		ets_uart_printf("T1 = %s, T2 = \r\n", temp[0]);
		addValueToArray(temp[0], temperature[0], ROTATE);
		addValueToArray(temp[1], temperature[1], ROTATE);
		//mergeAnswerWith(temperature);

		//===== graphic ========

		if (temp[0][0] == '-')
			a = a * (-1);
		if (temp[1][0] == '-')
			b = b * (-1);
		valueToBuffer(a, tBuffer);
		valueToBuffer(b, tBuffer2);
		showGraphic(tBuffer, 160, 0x0000a0);
		showGraphic(tBuffer2, 240, 0x5b5b00);
	}
	mergeAnswerWith(temperature);

	//================================================
	timeIncrement();
	printTime();
	//sendUDPbroadcast();




}
//==============================================================================
void ICACHE_FLASH_ATTR setup(void)
{

	// HSPI init
	hspi_init();
	LCD_wakeup();
	//====== Draw screen =======
	tft_fillRect(0, 0, 240, 40, 0x5f);
	tft_fillRect(0, 40, 240, 40, 0x1f);
	tft_fillRect(0, 80, 240, 80, BLUE);

	//tft_fillRect(34, 8, 54, 24, 0xff00ff);

	tft_fillRoundRect(10, 90, 105, 60, 20, YELLOW);
	tft_fillRoundRect(15, 95, 95, 50, 15, BLUE);
	tft_fillRoundRect(125, 90, 105, 60, 20, GREEN);
	tft_fillRoundRect(130, 95, 95, 50, 15, BLUE);

	tft_drawRoundRect(62, 8, 54, 24, 5, GREEN);

	readConfigs();
	ets_uart_printf("configs.nastr.DEFAULT_AP = %d\r\n", configs.nastr.DEFAULT_AP);
	//configs.nastr.DEFAULT_AP = 0;
	if(configs.nastr.DEFAULT_AP == 0)
			 setup_wifi_ap_mode();
		else setup_wifi_st_mode();

	ets_uart_printf("configs.nastr.SSID = %s\r\n", configs.nastr.SSID);
	ets_uart_printf("configs.nastr.SSID_PASS = %s\r\n", configs.nastr.SSID_PASS);

	UDP_Init();


	ds18b20_init();
	temperArrInit(temperature);

	//saveConfigs();

	print_icon(208, 8, BLUE, 0x5f, 2);

	// Start loop timer
	os_timer_disarm(&loop_timer);
	os_timer_setfn(&loop_timer, (os_timer_func_t *) loop_timer_cb, NULL);
	os_timer_arm(&loop_timer, LOOP_PERIOD, true);
}
//======================= GPIO interrupt callback =======================================================
extern uint8_t pin_num[GPIO_PIN_NUM];
//=======================
void ICACHE_FLASH_ATTR button_intr_callback(unsigned pin, unsigned level)
{
	ets_uart_printf("INTERRUPT: Set default AP\r\n");
	if(configs.nastr.DEFAULT_AP != 0)
	{
		configs.nastr.DEFAULT_AP = 0;
		saveConfigs();
	}
}
//======================= GPIO init function ============================================================
void ICACHE_FLASH_ATTR button_init(void)
{
	// Pin number 3 = GPIO0
	GPIO_INT_TYPE gpio_type;
	uint8_t gpio_pin = 1;

	gpio_type = GPIO_PIN_INTR_NEGEDGE;
	if (set_gpio_mode(gpio_pin, GPIO_FLOAT, GPIO_INT)) {
		ets_uart_printf("GPIO%d set interrupt mode\r\n", pin_num[gpio_pin]);
		if (gpio_intr_init(gpio_pin, gpio_type)) {
			ets_uart_printf("GPIO%d enable %s mode\r\n", pin_num[gpio_pin], "NEG EDGE");
			gpio_intr_attach(button_intr_callback);
		} else {
			ets_uart_printf("Error: GPIO%d not enable %s mode\r\n", pin_num[gpio_pin], "NEG EDGE");
		}
	} else {
		ets_uart_printf("Error: GPIO%d not set interrupt mode\r\n",  pin_num[gpio_pin]);
	}
}
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{

	UARTInit(BIT_RATE_115200);
	console_printf("\r\n");

	wifi_station_disconnect();
	wifi_station_set_auto_connect(0);


	button_init();
	// Start setup timer
		os_timer_disarm(&loop_timer);
		os_timer_setfn(&loop_timer, (os_timer_func_t *) setup, NULL);
		os_timer_arm(&loop_timer, 500, false);

}
