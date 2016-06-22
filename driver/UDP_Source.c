#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "espconn.h"
#include "at_custom.h"
#include "mem.h"
#include "driver/UDP_Source.h"
#include "driver/DS18B20_PR.h"
#include "driver/services.h"
#include "driver/configs.h"
//=========================================================================================
extern u_CONFIG configs;
struct espconn *UDP_P;
struct espconn *UDP_PC;

uint8 channelFree = 1;

sint16 plotData[2][24];
//============================================================================================================================
void ICACHE_FLASH_ATTR UDP_Init() {


	/*struct espconn* */UDP_P = (struct espconn *) os_zalloc(
			sizeof(struct espconn));
	UDP_P->proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));
	UDP_P->state = ESPCONN_NONE;
	UDP_P->type = ESPCONN_UDP;

	UDP_P->proto.udp->local_port = 7777; //The port on which we want the esp to serve
	UDP_P->proto.udp->remote_port = 7777; //The port on which we want the esp to serve

	//Set The call back functions
	espconn_regist_recvcb(UDP_P, UDP_Recieved);
	espconn_create(UDP_P);
}
//============================================================================================================================
void ICACHE_FLASH_ATTR UDP_Init_client()
{

	UDP_PC = (struct espconn *) os_zalloc(
			sizeof(struct espconn));
	UDP_PC->proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));
	UDP_PC->state = ESPCONN_NONE;
	UDP_PC->type = ESPCONN_UDP;

	UDP_PC->proto.udp->local_port = 7777; //The port on which we want the esp to serve
	UDP_PC->proto.udp->remote_port = 7777; //The port on which we want the esp to serve

	//Set The call back functions
	espconn_regist_recvcb(UDP_PC, UDP_Recieved);
	espconn_create(UDP_PC);
}
//============================================================================================================================
void ICACHE_FLASH_ATTR addValueToArray(char * tPtr, sint16 *arPtr, char aRot)
{
	int i,j;
	if(aRot == ROTATE)
	{
		for(i = 0; i < POINTS_CNT-1; i++)
			arPtr[i]= arPtr[i+1];
	}

    sint16 e = (tPtr[1]- '0') * 100 + (tPtr[2]- '0') * 10 + (tPtr[3] - '0');

    if((tPtr[0] - '0') == '-')  e *= (-1);

		arPtr[POINTS_CNT-1] = e;
}
//=========================================================================================
typedef union __packed
{
	uint8 bytes[49];
	struct __packed
	{
	   uint8 command;
	   sint16 plotData[24]; // 0 - in, 1 - out
	};
}s_PLOT_DATA_PACK;


s_PLOT_DATA_PACK plotDataPack = {
		.command = PLOT_DATA_ANS};
//=========================================================================================
void ICACHE_FLASH_ATTR sendUDPbroadcast(uint8* abuf, uint16 aLen)
{
	if(channelFree)
	{
		UDP_PC->proto.udp->remote_port = (int)7777;
		UDP_PC->proto.udp->remote_ip[0] = 255;
		UDP_PC->proto.udp->remote_ip[1] = 255;
		UDP_PC->proto.udp->remote_ip[2] = 255;
		UDP_PC->proto.udp->remote_ip[3] = 255;
		espconn_sent(UDP_PC, abuf, aLen);
		//ets_uart_printf("UDP end ro port %d\r\n", UDP_PC->proto.udp->remote_port);
	}
}
//=========================================================================================
void UDP_Recieved(void *arg, char *pusrdata, unsigned short length)
{
	int a;
	ets_uart_printf("recv udp data: ");
	for (a = 0; a < length; a++)
		ets_uart_printf("%02x ", pusrdata[a]);
	ets_uart_printf("\r\n");

	struct espconn *pesp_conn = arg;
	uint8 flashWriteBit = 0;
	remot_info *premot = NULL;

	if (espconn_get_connection_info(pesp_conn, &premot, 0) == ESPCONN_OK) {
		pesp_conn->proto.udp->remote_port = 7777;
		pesp_conn->proto.udp->remote_ip[0] = premot->remote_ip[0];
		pesp_conn->proto.udp->remote_ip[1] = premot->remote_ip[1];
		pesp_conn->proto.udp->remote_ip[2] = premot->remote_ip[2];
		pesp_conn->proto.udp->remote_ip[3] = premot->remote_ip[3];

//		ets_uart_printf("recv udp ip: %d.%d.%d.%d\r\n", premot->remote_ip[0] ,premot->remote_ip[1], premot->remote_ip[2], premot->remote_ip[3]);
//		ets_uart_printf("recv udp port: %d\r\n", premot->remote_port);

		switch(pusrdata[0])
		{
			case BROADCAST_DATA:
								{
									int i, j, e;

									for (i = 0; i < 2; i++)
										if (configs.hwSettings.sensor[i].mode == SENSOR_MODE_REMOTE)
											for (j = 0; j < 4; j++)
												tData[i][j] = pusrdata[1 + i * 4 + j];

									if(configs.hwSettings.deviceMode == DEVICE_MODE_SLAVE)
									{
										date_time.TIME.sec = pusrdata[9];
										date_time.TIME.min = pusrdata[10];
										date_time.TIME.hour = pusrdata[11];
										date_time.DATE.day = pusrdata[12];
										date_time.DATE.month = pusrdata[13];
										date_time.DATE.year = pusrdata[14] + 2000;

								sendUDPbroadcast(remoteTemp.byte, (uint16)sizeof(remoteTemp));
									}
								}
								break;

			case HARDWARE_CFG:
								{
									int i, j;
									os_memset(configs.hwSettings.wifi.SSID, 0,sizeof(configs.hwSettings.wifi.SSID));
									os_memset(configs.hwSettings.wifi.SSID_PASS, 0,	sizeof(configs.hwSettings.wifi.SSID_PASS));

									for (i = 1; i < length; i++)
									{
										if (pusrdata[i] == '$')	break;
										else	configs.hwSettings.byte[i - 1] = pusrdata[i];
									}

									j = i + 1;
									for (i = j; i < length; i++) configs.hwSettings.wifi.SSID_PASS[i - j] = pusrdata[i];

									serviceMode = MODE_SW_RESET;
									service_timer_start();
									flashWriteBit = 1;



									char ans[] = {OK_ANS};
									espconn_sent(pesp_conn, ans, 1);
								}
								break;
		}


        //============================================================================================================================
		if(configs.hwSettings.deviceMode == DEVICE_MODE_MASTER)
		{
			if(serviceMode != MODE_SW_RESET)
			{
				channelFree = 0;
				serviceMode = MODE_REMOTE_CONTROL;
				service_timer_start();
			}

			switch(pusrdata[0])
			{
				case PLOT_DATA:
								{
									timeUpdate(pusrdata);
									int i;
									for( i = 0; i < 24; i++)
											plotDataPack.plotData[i] = plotData[pusrdata[1] >> 7][i];
									espconn_sent(pesp_conn, plotDataPack.bytes, sizeof(s_PLOT_DATA_PACK));
								}
								break;

				case READ_WEEK_CONFIGS:
								{
									unsigned char weekTxBuf[8];
									weekTxBuf[0] = READ_WEEK_CONFIGS_ANS;
									int i;
									for (i = 0; i < 7; i++) weekTxBuf[i + 1] = (char) configs.nastr.day[i];
									espconn_sent(pesp_conn, weekTxBuf, 11);
								}
								break;

				case SAVE_WEEK_CONFIGS:

								{
									char ans[] = {OK_ANS};
									int i;
									for (i = 0; i < 7; i++)
										configs.nastr.day[i] = (uint32) pusrdata[i + 1];
									//=== write flash =====
									flashWriteBit = 1;
									espconn_sent(pesp_conn, ans, 1);
								}
								break;

				case READ_DAY_CONFIGS:
								{
									int i, a  = (pusrdata[1] >> 7) ? 1 : 0;
									char cBuf[7];

									cBuf[0] = READ_DAY_CONFIGS_ANS;
									cBuf[1] = (pusrdata[1] & 0x0f);
									cBuf[2] = (char) (configs.cfg[a].periodsCnt & 0x0f);
									cBuf[3] = (char) (configs.cfg[a].pConfig[(pusrdata[1] & 0x0f) - 1].hStart);
									cBuf[4] = (char) (configs.cfg[a].pConfig[(pusrdata[1] & 0x0f) - 1].mStart);
									cBuf[5] = (char) (configs.cfg[a].pConfig[(pusrdata[1] & 0x0f) - 1].temperature & 0xff);
									cBuf[6] = (char)((configs.cfg[a].pConfig[(pusrdata[1] & 0x0f) - 1].temperature >> 8) & 0xff);

									espconn_sent(pesp_conn, cBuf, 7);
								}
								break;

				case SAVE_DAY_CONFIGS:
								{
									int i, a = (pusrdata[1] >> 7) ? 1 : 0;

									for(i = 0; i < 4; i++)
										configs.cfg[a].pConfig[(pusrdata[2]) - 1].byte[i] = pusrdata[i + 4];

									if (pusrdata[2] == pusrdata[3])
									{
										configs.cfg[a].periodsCnt =  pusrdata[3];
										flashWriteBit = 1;
									}

									char ans[] = {OK_ANS};
									espconn_sent(pesp_conn, ans, 1);
								}


					break;
			}

					//========= read ustanovki ===========================
					if (pusrdata[0] == 'G' && pusrdata[1] == 'U' && pusrdata[2] == 'S' 	&& pusrdata[3] == 'T')
					{
						char data[8] = { "UST" };
						data[3] = configs.nastr.delta / 10 + '0';
						data[4] = '.';
						data[5] = configs.nastr.delta % 10 + '0';
						data[6] = 'S';
						data[7] = configs.hwSettings.swapSens;

						espconn_sent(pesp_conn, data, 8);
					}
					//========= save ustanovki ===========================
					if (pusrdata[0] == 'S' && pusrdata[1] == 'U' && pusrdata[2] == 'S' && pusrdata[3] == 'T')
					{
						configs.nastr.delta = (pusrdata[4] - '0') * 10 + (pusrdata[6] - '0');
						configs.hwSettings.swapSens = pusrdata[8];
						flashWriteBit = 1;

					}
		}
		if (flashWriteBit == 1) saveConfigs();
	}
}



