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
//=========================================================================================
char ans[8][31] = { {"I146+234-254+274+204+214+234\n\r"},
					{"I246+234+254+274+204+214+234\n\r"},
					{"I346+234+254-275+204+214+234\n\r"},
					{"I446+234+254+274+204+214+234\n\r"},
					{"O146+234+254-274+204+214+234\n\r"},
					{"O246+234-254+274+204+214-234\n\r"},
					{"O346+234+254+274+204-214+234\n\r"},
					{"O446+234-254+274+204+214-234\n\r"}};


//=========================================================================================
void  mergeAnswerWith(char tPtr[2][24][4])
{
	int i,j,k;

	for(k = 0; k<4; k++)
		for(i = 0; i<6; i++)
			for(j = 0; j<4; j++)
					ans[k][i*4+j+4]= tPtr[0][k*6+i][j];

	for(k = 0; k<4; k++)
			for(i = 0; i<6; i++)
				for(j = 0; j<4; j++)
						ans[k+4][i*4+j+4]= tPtr[1][k*6+i][j];

}
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
		ets_uart_printf("UDP end ro port %d\r\n", UDP_PC->proto.udp->remote_port);
}
}
//=========================================================================================
void UDP_Recieved(void *arg, char *pusrdata, unsigned short length) {
	ets_uart_printf("recv udp data: %s\r\n", pusrdata);
	struct espconn *pesp_conn = arg;
	uint8 flashWriteBit = 0;

	remot_info *premot = NULL;
	//sint8 value = ESPCONN_OK;
	if (espconn_get_connection_info(pesp_conn, &premot, 0) == ESPCONN_OK) {
		pesp_conn->proto.udp->remote_port = 7777;
		pesp_conn->proto.udp->remote_ip[0] = premot->remote_ip[0];
		pesp_conn->proto.udp->remote_ip[1] = premot->remote_ip[1];
		pesp_conn->proto.udp->remote_ip[2] = premot->remote_ip[2];
		pesp_conn->proto.udp->remote_ip[3] = premot->remote_ip[3];

		ets_uart_printf("recv udp ip: %d.%d.%d.%d\r\n", premot->remote_ip[0] ,premot->remote_ip[1], premot->remote_ip[2], premot->remote_ip[3]);
		ets_uart_printf("recv udp port: %d\r\n", premot->remote_port);

		//========= Remote temperature ===========================
		if (pusrdata[0] == 'R' && pusrdata[1] == 'T' && pusrdata[2] == 'M'	&& pusrdata[3] == 'P')
		{
			int i, j, e;

			if (configs.hwSettings.sensor[0].mode == SENSOR_MODE_REMOTE &&
				configs.hwSettings.sensor[1].mode == SENSOR_MODE_REMOTE)
				e = DevicesCount;
			else e = 1;

			for (i = 0; i < e; i++)
				if (configs.hwSettings.sensor[i].mode == SENSOR_MODE_REMOTE)
					for (j = 0; j < 4; j++)
						tData[i][j] = pusrdata[4 + i * 4 + j];

			if(configs.hwSettings.deviceMode == DEVICE_MODE_SLAVE)
			{
					espconn_sent(pesp_conn,remoteTemp.byte, (uint16)sizeof(remoteTemp));
			}
		}
		//========= save hardware configs ===========================
		if (pusrdata[0] == 'H' && pusrdata[1] == 'W' && pusrdata[2] == 'C' && pusrdata[3] == 'F' && pusrdata[4] == 'G')
		{
			int i, j;
			os_memset(configs.hwSettings.wifi.SSID, 0,sizeof(configs.hwSettings.wifi.SSID));
			os_memset(configs.hwSettings.wifi.SSID_PASS, 0,	sizeof(configs.hwSettings.wifi.SSID_PASS));

			for (i = 5; i < length; i++)
			{
				if (pusrdata[i] == '$')	break;
				else	configs.hwSettings.byte[i - 5] = pusrdata[i];
			}

			j = i + 1;
			for (i = j; i < length; i++) configs.hwSettings.wifi.SSID_PASS[i - j] = pusrdata[i];

			serviceMode = MODE_SW_RESET;
			service_timer_start();
			flashWriteBit = 1;
			espconn_sent(pesp_conn, "SAVED", 5);
		}
        //============================================================================================================================
		if(configs.hwSettings.deviceMode == DEVICE_MODE_MASTER)
		{
				channelFree = 0;
				serviceMode = MODE_REMOTE_CONTROL;
				service_timer_start();

		int shift = 0xff;

		if (pusrdata[0] == 'O')			shift = 4;
		else if (pusrdata[0] == 'I')	shift = 0;

		if (shift != 0xff)
		{
			if (pusrdata[0] == 'I' && pusrdata[1] == '1')
			{
				ans[0][0] = 'I'; //zatychka
				timeUpdate(pusrdata);
			}

			espconn_sent(pesp_conn, ans[shift + (pusrdata[1] - '0') - 1], 30);
		}		

		//========= read day configs ===========================
		if (pusrdata[0] == 'C' && pusrdata[1] == 'O' && pusrdata[2] == 'N'	&& pusrdata[3] == 'F')
		{
			u_CONFIG_u ptr = (pusrdata[4] == 'H') ? configs.cfg[1] : configs.cfg[0];

			char cBuf[10];
			cBuf[0] = 'C';
			cBuf[1] = pusrdata[5];
			cBuf[2] = (char) (ptr.periodsCnt);
			cBuf[3] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].hmStart >> 24);
			cBuf[4] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].hmStart >> 16);
			cBuf[5] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].hmStart >> 8);
			cBuf[6] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].hmStart);
			cBuf[7] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].temperature >> 16);
			cBuf[8] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].temperature >> 8);
			cBuf[9] = (char) (ptr.pConfig[pusrdata[5] - '0' - 1].temperature);
			cBuf[10] = 0x0a;
			cBuf[11] = 0x0d;

			espconn_sent(pesp_conn, cBuf, 12);
		}
		//========= save day configs ===========================
		if (pusrdata[0] == 'C' && pusrdata[1] == 'S' && pusrdata[2] == 'A'	&& pusrdata[3] == 'V')
		{
			int a = (pusrdata[4] == 'H') ? 1 : 0;

			configs.cfg[a].pConfig[pusrdata[5] - '0' - 1].hmStart =
							  ((uint32) (pusrdata[7] << 24))
							| ((uint32) (pusrdata[8] << 16))
							| ((uint32) (pusrdata[9] << 8))
							| ((uint32) (pusrdata[10]));

			configs.cfg[a].pConfig[pusrdata[5] - '0' - 1].temperature =
							  ((uint32) (pusrdata[11] << 16))
							| ((uint32) (pusrdata[12] << 8))
							| ((uint32) (pusrdata[13]));

			if (pusrdata[5] == pusrdata[6])
			{
				configs.cfg[a].periodsCnt = (uint32) pusrdata[6];
				//=== write flash =====
				flashWriteBit = 1;
			}

			char okAnswer[3] = { 'O', 'K', pusrdata[5] };

			espconn_sent(pesp_conn, okAnswer, 3);
		}
		//========= read week configs ===========================
		if (pusrdata[0] == 'W' && pusrdata[1] == 'E' && pusrdata[2] == 'E' && pusrdata[3] == 'K')
		{
			unsigned char weekTxBuf[11];
			weekTxBuf[0] = 'W';
			weekTxBuf[1] = 'C';
			weekTxBuf[9] = 0x0a;
			weekTxBuf[10] = 0x0d;
			int i;
						for (i = 0; i < 7; i++) weekTxBuf[i + 2] = (char) configs.nastr.day[i];

			espconn_sent(pesp_conn, weekTxBuf, 11);
		}
		//========= save week configs ===========================
		if (pusrdata[0] == 'C' && pusrdata[1] == 'S' && pusrdata[2] == 'A' && pusrdata[3] == 'W')
		{
			int i;
			for (i = 0; i < 7; i++)
				configs.nastr.day[i] = (uint32) pusrdata[i + 4];
			//=== write flash =====
			flashWriteBit = 1;
			espconn_sent(pesp_conn, "OKW", 3);
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
		if (flashWriteBit == 1) saveConfigs();
	}

	}
}



