//============================================================================================================================
#include "ets_sys.h"
#include "osapi.h"
#include "c_types.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "driver/DS18B20_PR.h"
//============================================================================================================================
//uint8_t ow_addr[8];
int SignBit, Whole, Fract;

//============================================================================================================================
void ICACHE_FLASH_ATTR addValueToArray(char * tPtr, char arPtr[POINTS_CNT][4], char aRot)
{
	int i,j;
	if(aRot == ROTATE)
	{
		for(i = 0; i < POINTS_CNT-1; i++)
				for(j = 0; j<4; j++)
					arPtr[i][j] = arPtr[i+1][j];
		//ets_uart_printf("rotated\r\n");

	}
	//else ets_uart_printf("non rotated\r\n");
	for(i = 0; i<4; i++)
		arPtr[POINTS_CNT-1][i] = tPtr[i];

	//uart0_tx_buffer(arPtr[0], 24*4);
	//ets_uart_printf("temp: %s\r\n", temperature);
}
//============================================================================================================================
void ICACHE_FLASH_ATTR temperArrInit(char arPtr[2][POINTS_CNT][4] )
{

  int i, k;
  for(k = 0; k < 2; k++)
    for(i = 0; i < POINTS_CNT; i++)
    {
    	arPtr[k][i][0] = '+';
    	arPtr[k][i][1] = '2';
    	arPtr[k][i][2] = '2';
    	arPtr[k][i][3] = '0';
    }
  mergeAnswerWith(arPtr);
}
//============================================================================================================================
void  ICACHE_FLASH_ATTR ds18b20_init()
{
	int r;
	ds_init();
	OWFind();

//		r = ds_search(ow_addr);
//		if(r)
//		{
//			ets_uart_printf("Found Device @ %02x %02x\r\n", ow_addr[0], ow_addr[1], ow_addr[2], ow_addr[3], ow_addr[4], ow_addr[5], ow_addr[6], ow_addr[7]);
//			if(crc8(ow_addr, 7) != ow_addr[7])
//				ets_uart_printf( "CRC mismatch, crc=%xd, addr[7]=%xd\r\n", crc8(ow_addr, 7), ow_addr[7]);
//		}
//		else ets_uart_printf("No DS18B20 detected, sorry.\r\n");

}
//============================================================================================================================
void ICACHE_FLASH_ATTR ds18b20(int aDevNumb, char * tPtr)
{
	int i;
	uint8_t  data[12];
	int HighByte, LowByte, TReading,Tc_100;

	//os_delay_us(1000000);

	//============ read temperature ===========================
			//console_printf("Scratchpad: ");
			reset();
			select(device[aDevNumb].data);
			write(DS1820_READ_SCRATCHPAD, 0); // read scratchpad

			for(i = 0; i < 9; i++) data[i] = read();

			LowByte = data[0];
			HighByte = data[1];
			TReading = (HighByte << 8) + LowByte;
			SignBit = TReading & 0x8000;  // test most sig bit
			if (SignBit) // negative
				TReading = (TReading ^ 0xffff) + 1; // 2's comp

			Whole = TReading >> 4;  // separate off the whole and fractional portions
			Fract = (TReading & 0xf) * 10 / 16;

			tPtr[0] = SignBit ? '-' : '+';
			tPtr[1] = Whole/10 + '0';
			tPtr[2] = Whole%10 + '0';
			tPtr[3] = Fract + '0';
}
void ICACHE_FLASH_ATTR ds18b20_Convert(void)
{
//============ perform the conversion ===========================
			reset();
			//select(device[0].data);
			write(DS1820_SKIP_ROM, 1);
			write(DS1820_CONVERT_T, 1);
}


