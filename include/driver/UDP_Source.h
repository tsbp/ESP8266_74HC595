

void ICACHE_FLASH_ATTR UDP_Init();
void ICACHE_FLASH_ATTR UDP_Init_client();
void UDP_Recieved(void *arg, char *pusrdata, unsigned short length);
void   mergeAnswerWith(char tPtr[2][24][4]);
void ICACHE_FLASH_ATTR sendUDPbroadcast(uint8* abuf, uint16 aLen);


extern uint8 channelFree;

