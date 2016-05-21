//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/DS18B20_PR.h"
#include "driver/Configs.h"
#include "driver/UDP_Source.h"
#include "driver/wifi.h"
#include "driver/gpio16.h"
#include "driver/services.h"
//==============================================================================
static volatile os_timer_t service_timer;
static void  service_timer_cb(os_event_t *events);
uint8_t factory_reset_pin = 2;
uint8	serviceMode = MODE_NORMAL;

char tData[2][4];

u_REMOTE_TEMP remoteTemp = {.head = "RTMP",
                            .sData[0] = "0000",
							.sData[1] = "0000"};
//==============================================================================
void ICACHE_FLASH_ATTR service_timer_start (void)
{
	 //SELECT command timer
		os_timer_disarm(&service_timer);
		os_timer_setfn(&service_timer, (os_timer_func_t *) service_timer_cb, NULL);
		os_timer_arm(&service_timer, 1000, true);
}
//==============================================================================
void ICACHE_FLASH_ATTR service_timer_stop (void)
{
	 //SELECT command timer
		os_timer_disarm(&service_timer);
//		os_timer_setfn(&select_timer, (os_timer_func_t *) select_loop, NULL);
//		os_timer_arm(&select_timer, 500, true);
}
//======================= GPIO interrupt callback =======================================================
//extern uint8_t pin_num[GPIO_PIN_NUM];
int resetCntr = 0;
//=======================
void ICACHE_FLASH_ATTR button_intr_callback(unsigned pin, unsigned level)
{
	//ets_uart_printf("RESET BUTTON PRESSED!!!\r\n");
	serviceMode = MODE_BTN_RESET;
		resetCntr = 0;
		service_timer_start();
}
//======================= GPIO init function ============================================================
void ICACHE_FLASH_ATTR button_init(void)
{
	GPIO_INT_TYPE gpio_type;
	gpio_type = GPIO_PIN_INTR_NEGEDGE;
	if (set_gpio_mode(factory_reset_pin, GPIO_FLOAT, GPIO_INT))
		if (gpio_intr_init(factory_reset_pin, gpio_type))  gpio_intr_attach(button_intr_callback);
}
//==============================================================================
void ICACHE_FLASH_ATTR getTemperature(void)
{
	int i,j,e;
	if (configs.hwSettings.sensor[0].mode == SENSOR_MODE_LOCAL && configs.hwSettings.sensor[1].mode == SENSOR_MODE_LOCAL)
		e = DevicesCount;
	else e = 1;

	for(i = 0; i < e; i++)
		{

		  if(configs.hwSettings.sensor[i].mode == SENSOR_MODE_LOCAL)
		  {
			 ds18b20(i, tData[i]);
			 for(j = 0; j < 4; j++) remoteTemp.sData[i][j] = tData[i][j];
		}
	}
	ds18b20_Convert();
}
//==============================================================================
static void ICACHE_FLASH_ATTR service_timer_cb(os_event_t *events) {


	switch (serviceMode)
	{
	    case MODE_REMOTE_CONTROL:
	    	resetCntr++;
	    	if (resetCntr > 5)
	    	{
	    		service_timer_stop();
	    		resetCntr = 0;
	    		serviceMode = MODE_NORMAL;
	    		channelFree = 1;
	    	}
	    	break;

		case MODE_BTN_RESET:
				if (gpio_read(factory_reset_pin) == 0)
				{
					resetCntr++;
					if (resetCntr >= 10)
					{
						os_printf("do reset \r\n");
						configs.hwSettings.wifi.mode = SOFTAP_MODE;
						configs.hwSettings.wifi.auth = AUTH_OPEN;						
						saveConfigs();
						system_restart();					
					}
				} else {
					service_timer_stop();
					resetCntr = 0;
					serviceMode = MODE_NORMAL;
				}

				break;

		case MODE_SW_RESET:
			resetCntr++;
			if (resetCntr >= 2)
				system_restart();
			break;
	}

}
