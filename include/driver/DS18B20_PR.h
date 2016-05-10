#include "driver/ds18b20.h"


#define POINTS_CNT (24)

#define ROTATE	(1)
#define NON_ROTATE	(0)


void  ICACHE_FLASH_ATTR ds18b20_init();
void ICACHE_FLASH_ATTR temperArrInit(char arPtr[2][POINTS_CNT][4] );
void ICACHE_FLASH_ATTR addValueToArray(char * tPtr, char arPtr[POINTS_CNT][4], char aRot);


