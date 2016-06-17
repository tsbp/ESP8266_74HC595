//==============================================================================
#define PRIV_PARAM_START_SEC		0x3C
#define PRIV_PARAM_SAVE     0
#define SPI_FLASH_SEC_SIZE      4096
//==============================================================================
typedef struct
{
  union
  {
    unsigned int byte[3];
    struct
    {
      unsigned int day;
      unsigned int month;
      unsigned int year;
    };
  }DATE;
  union
  {
    unsigned char byte[3];
    struct
    {
      unsigned char sec;
      unsigned char min;
      unsigned char hour;
    };
  }TIME;
}s_DATE_TIME;
extern s_DATE_TIME date_time;
//==============================================================================
typedef union __packed
{
  uint32 byte[11];
  struct __packed
  {
	uint8 day[7];
    uint16 interval;
    uint8 delta;
  };
}u_NASTROYKI;
//==============================================================================
#define SENSOR_MODE_LOCAL			(0x00)
#define SENSOR_MODE_REMOTE			(0x01)

#define DEVICE_MODE_MASTER			(0x00)
#define DEVICE_MODE_SLAVE			(0x01)
//==============================================================================
typedef struct __attribute__ ((__packed__))
{
	uint8 mode;
}s_SENS_MODE;
//==============================================================================
typedef struct __attribute__ ((__packed__))
{
	uint8 mode;
    uint8 auth;
	uint8 SSID[32];
	uint8 SSID_PASS[64];
}s_WIFI_CFG;
//==============================================================================
typedef union __attribute__ ((__packed__))
{
	uint8 byte[102];
	struct __attribute__ ((__packed__))
	{
		uint8 deviceMode;
		s_SENS_MODE sensor[2];
		uint8 swapSens;
		s_WIFI_CFG wifi;
	};
}u_HARDWARE_SETTINGS;
//==============================================================================
#define MAX_PERIODS_CNT     (10)
//==============================================================================
typedef union __packed
{
    uint8 byte[4];
    struct __packed
    {
       uint8 hStart;
       uint8 mStart;
       uint16 temperature;
    };
}s_PCONFIG;
//==============================================================================
#define CONF_ARRAY_LENGTH (MAX_PERIODS_CNT * sizeof(s_PCONFIG))
//==============================================================================
typedef union __packed
{
  uint8 byte[1 + CONF_ARRAY_LENGTH];
  struct __packed
  {
    uint8 periodsCnt;
    s_PCONFIG pConfig[MAX_PERIODS_CNT];
  };
}u_CONFIG_u;
//==============================================================================
typedef struct __packed
{
	u_CONFIG_u cfg[2];
	u_NASTROYKI nastr;
	u_HARDWARE_SETTINGS hwSettings;
}u_CONFIG;
//extern u_CONFIG configs;
//==============================================================================
extern u_CONFIG configs;
extern u_CONFIG *cPtrH, *cPtrW;
//==============================================================================
uint16 ICACHE_FLASH_ATTR getSetTemperature();
unsigned char ICACHE_FLASH_ATTR cmpTemperature (uint16 aT, signed int arcTemper);
void ICACHE_FLASH_ATTR showTemperature(uint8 aSwap, unsigned char *aBuf);
  
