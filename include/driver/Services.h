/*
 * Services.h
 *
 *  Created on: 16 трав. 2016
 *      Author: Voodoo
 */

#ifndef INCLUDE_DRIVER_SERVICES_H_
#define INCLUDE_DRIVER_SERVICES_H_

//==============================================================================

void ICACHE_FLASH_ATTR service_timer_start (void);
void ICACHE_FLASH_ATTR service_timer_stop (void);
void ICACHE_FLASH_ATTR button_intr_callback(unsigned pin, unsigned level);
void ICACHE_FLASH_ATTR button_init(void);
//==============================================================================
#define MODE_NORMAL		(0)
#define MODE_SW_RESET	(1)
#define MODE_BTN_RESET	(2)
//==============================================================================
extern uint8	serviceMode;
extern char tData[2][4];
//==============================================================================
typedef union __packed
{
	uint8 byte[12];
	struct
	{
		uint8 head[4];
		uint8 sData[2][4];
	};
}u_REMOTE_TEMP;
extern u_REMOTE_TEMP remoteTemp;
//==============================================================================
#endif /* INCLUDE_DRIVER_SERVICES_H_ */
