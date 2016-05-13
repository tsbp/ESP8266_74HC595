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
//==============================================================================
typedef union
{
  uint32 byte[11];
  struct
  {
    uint32 interval;
    uint32 delta;
    uint32 swapSens;
    uint32 day[7];
    uint32 DEFAULT_AP;
    uint8 SSID[32];
    uint8 SSID_PASS[64];
  };
}u_NASTROYKI;
//==============================================================================
#define PERIODS_CNT     (6)
//==============================================================================
typedef union
{
    uint32 byte[2];
    struct
    {
       uint32 hmStart;
       uint32 temperature;
    };
}s_PCONFIG;
//==============================================================================
#define CONF_ARRAY_LENGTH (PERIODS_CNT * sizeof(s_PCONFIG))
//==============================================================================
typedef union
{
  uint32 byte[1 + CONF_ARRAY_LENGTH];
  struct
  {
    uint32 periodsCnt;
    s_PCONFIG pConfig[PERIODS_CNT];
  };
}u_CONFIG_u;
//==============================================================================
typedef struct
{
	u_CONFIG_u cfg[2];
	u_NASTROYKI nastr;
}u_CONFIG;
//extern u_CONFIG configs;
//==============================================================================
typedef union
{
  unsigned char byte[sizeof(u_CONFIG) + 5];
  struct
  {
    unsigned char  msgHeader;
    unsigned char  msgNumber;
    unsigned char  partsCount;
    s_PCONFIG config;
    unsigned char _0a;
    unsigned char _0d;
  };
}u_CONFIG_TX_BUFFER;
//extern u_CONFIG_TX_BUFFER configTXBuffer;

//extern u_NASTROYKI *nastroyki;
//==============================================================================
extern u_CONFIG configs;
extern u_CONFIG *cPtrH, *cPtrW;
//==============================================================================
uint32 getSetTemperature(void);
unsigned char cmpTemperature (unsigned char *aT, signed int arcTemper);
  
