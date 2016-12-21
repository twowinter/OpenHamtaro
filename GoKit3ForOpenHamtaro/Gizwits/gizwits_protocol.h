#ifndef _GIZ_PROTOCOL_H
#define _GIZ_PROTOCOL_H

#include <stdint.h>
#include "header.h"

#define DEBUG
#ifdef DEBUG
#define GIZWITS_LOG printf
#endif

//#define PROTOCOL_DEBUG

#define SoftAp_Mode    0x01
#define AirLink_Mode   0x02

#define MAX_P0_LEN      128              //p0数据最大长度
#define MAX_PACKAGE_LEN     (MAX_P0_LEN*2)   //数据缓冲区最大长度
#define MAX_RINGBUFFER_LEN  MAX_PACKAGE_LEN  //环形缓冲区最大长度
#define Max_UartBuf         MAX_PACKAGE_LEN  //串口数据缓冲区最大长度

#define SEND_MAX_TIME   3000 //3000ms
#define SEND_MAX_NUM    2

#define protocol_VERSION "00000004"
#define P0_VERSION  "00000002"
#define DEV_DEFAULT  "00000000"

#define DISPLAY2COMMUNICATION(ration, addition, value) ((value)-(addition))/(ration)
#define COMMUNICATION2DISPLAY(ration, addition, value) ((value)*(ration))+(addition)

#define EN_HAM  1

#if EN_HAM > 0
#define PRODUCT_KEY "efea64cc739c4c68bee4fc6826b1762e" // "your_product_key"
#else
//custom
#define PRODUCT_KEY "your_product_key"
#endif

#define LED_ONOFF_DEFAULT 0
#define LED_COLOR_DEFAULT 0

#define LED_R_RATIO 1
#define LED_R_ADDITION 0
#define LED_R_DEFAULT 0

#define LED_G_RATIO 1
#define LED_G_ADDITION 0
#define LED_G_DEFAULT 0

#define LED_B_RATIO 1
#define LED_B_ADDITION 0
#define LED_B_DEFAULT 0

#define MOTOR_SPEED_RATIO 1
#define MOTOR_SPEED_ADDITION (-5)
#define MOTOR_SPEED_DEFAULT 0

#define TEMPERATURE_RATIO 1
#define TEMPERATURE_ADDITION (-13)
#define TEMPERATURE_DEFAULT 0

#define HUMIDITY_RATIO 1
#define HUMIDITY_ADDITION 0
#define HUMIDITY_DEFAULT 0

//custom
__packed typedef enum
{
    WIFI_SOFTAP    = 0x00,
    WIFI_AIRLINK,
    WIFI_STATION,
    WIFI_OPEN_BINDING,
    WIFI_CLOSE_BINDING,
    WIFI_CON_ROUTER,
    WIFI_DISCON_ROUTER,
    WIFI_CON_M2M,
    WIFI_DISCON_M2M,
    WIFI_OPEN_TESTMODE,
    WIFI_CLOSE_TESTMODE,
    WIFI_CON_APP,
    WIFI_DISCON_APP,
    WIFI_RSSI,
    TRANSPARENT_DATA,
    //custom
    SetLED_OnOff,
    SetLED_Color,
    SetLED_R,
    SetLED_G,
    SetLED_B,
    SetMotor,
    EVENT_MAX
} EVENT_TYPE_T;

//custom
__packed typedef enum
{
    LED_OnOff         = 0x00,
    LED_OnOn          = 0x01,
    LED_Costom        = 0x00,
    LED_Yellow        = 0x01,
    LED_Purple        = 0x02,
    LED_Pink          = 0x03,
} LED_ColorTypeDef;

__packed typedef struct {
    uint8_t  LED_OnOff:1;
    uint8_t  LED_Color:1;
    uint8_t  LED_R:1;
    uint8_t  LED_G:1;
    uint8_t  LED_B:1;
    uint8_t  Motor_Speed:1;
}attrFlags_t;
    
__packed typedef struct {
    uint8_t  LED_OnOff:1;
    uint8_t  LED_Color:2;
    uint8_t  reserve:5;
    uint8_t  LED_R;
    uint8_t  LED_G;
    uint8_t  LED_B;
    uint16_t Motor_Speed;
}attrVals_t;

__packed typedef struct {
    uint8_t  LED_OnOff:1;
    uint8_t  LED_Color:2;
    uint8_t  reserve_0:5;
    uint8_t  LED_R;
    uint8_t  LED_G;  
    uint8_t  LED_B;
    uint16_t Motor_Speed;
#if EN_HAM == 0
    uint8_t  Infrared:1;
    uint8_t  reserve_1:7;
    uint8_t  Temperature;
    uint8_t  Humidity;
#endif
    uint8_t  Alert_1:1;
    uint8_t  Alert_2:1;
    uint8_t  reserve_2:6;
    uint8_t  Fault_LED:1;
    uint8_t  Fault_Motor:1;
#if EN_HAM == 0
    uint8_t  Fault_TemHum:1;
    uint8_t  Fault_IR:1;
    uint8_t  reserve_3:4;
#else
    uint8_t  reserve_3:6;
#endif
}devStatus_t;

//custom
__packed typedef struct {
    attrFlags_t attrFlags;
    attrVals_t  attrVals;
}gizwitsIssued_t;

__packed typedef struct {
    devStatus_t devStatus;
}gizwitsReport_t;

//custom
__packed typedef struct {
    uint8_t num;
    uint8_t event[EVENT_MAX];
}eventInfo_t;

__packed typedef struct {
    uint8_t rssi;
}moduleStatusInfo_t;

//protocol
/*命令码*/
__packed typedef enum
{
    CMD_GET_DEVICE_INTO             = 0x01,
    ACK_GET_DEVICE_INFO             = 0x02,

    CMD_ISSUED_P0                   = 0x03,
    ACK_ISSUED_P0                   = 0x04,

    CMD_REPORT_P0                   = 0x05,
    ACK_REPORT_P0                   = 0x06,

    CMD_HEARTBEAT                   = 0x07,
    ACK_HEARTBEAT                   = 0x08,

    CMD_WIFI_CONFIG                 = 0x09,
    ACK_WIFI_CONFIG                 = 0x0A,

    CMD_SET_DEFAULT                 = 0x0B,
    ACK_SET_DEFAULT                 = 0x0C,

    CMD_WIFISTATUS                  = 0x0D,
    ACK_WIFISTATUS                  = 0x0E,

    CMD_MCU_REBOOT                  = 0x0F,
    ACK_MCU_REBOOT                  = 0x10,

    CMD_ERROR_PACKAGE               = 0x11,
    ACK_ERROR_PACKAGE               = 0x12,

    /*3.10*/
} PROTOCOL_CMDTYPE;


/******************************************************
* 重发机制结构体
********************************************************/
__packed typedef struct {
    uint8_t                 num;//重发次数
    uint8_t                 flag;//1,表示有需要等待的ACK;0,表示无需要等待的ACK
    uint8_t                 buf[MAX_PACKAGE_LEN];//重发数据缓冲区
    uint16_t                dataLen;
    uint32_t                sendTime;//重发的系统时间
} protocolWaitAck_t;


/******************************************************
* 协议标准头
********************************************************/
__packed typedef struct
{
    uint8_t                 head[2];
    uint16_t                len;
    uint8_t                 cmd;
    uint8_t                 sn;
    uint8_t                 flags[2];
} protocolHead_t;

/******************************************************
* 4.1  WiFi模组请求设备信息
********************************************************/
__packed typedef struct
{
    protocolHead_t          head;
    uint8_t                 protocolVer[8];
    uint8_t                 p0Ver[8];
    uint8_t                 hardVer[8];
    uint8_t                 softVer[8];
    uint8_t                 productKey[32];
    uint16_t                ninableTime;
    uint8_t                 devAttr[8];
    uint8_t                 sum;
} protocolDeviceInfo_t;

/*****************************************************
* 协议通用数据帧(4.2、4.4、4.6、4.9、4.10)
******************************************************/
__packed typedef struct
{
    protocolHead_t          head;
    uint8_t                 sum;
} protocolCommon_t;


/******************************************************
* 4.3 设备MCU通知WiFi模组进入配置模式
********************************************************/
__packed typedef struct
{
    protocolHead_t          head;
    uint8_t                 cfgMode;
    uint8_t                 sum;
} protocolCfgMode_t;

/*****************************************************
* 4.5 WiFi模组向MCU通知WiFi模组工作状态的变化
******************************************************/
/*****************************************************
* WiFi模组工作状态
******************************************************/
__packed typedef union
{
    uint16_t                value;
    __packed  struct
    {
        uint16_t            softap:1;
        uint16_t            station:1;
        uint16_t            onboarding:1;
        uint16_t            binding:1;
        uint16_t            con_route:1;
        uint16_t            con_m2m:1;
        uint16_t            reserve1:2;
        uint16_t            rssi:3;
        uint16_t            app:1;
        uint16_t            test:1;
        uint16_t            reserve2:3;
    }types;
    
} wifiStatus_t;

__packed typedef struct
{
    protocolHead_t          head;
    wifiStatus_t            ststus;
    uint8_t                 sum;
} protocolWifiStatus_t;

/*****************************************************
* 非法报文类型
******************************************************/
__packed typedef enum
{
    ERROR_ACK_SUM = 0x01,      //校验错误
    ERROR_CMD     = 0x02,      //命令码错误
    ERROR_OTHER   = 0x03,      //其他
} errorPacketsType_t;


/*****************************************************
* 4.7 非法消息通知
******************************************************/
__packed    typedef struct
{
    protocolHead_t          head;
    errorPacketsType_t      error;
    uint8_t                 sum;
} protocolErrorType_t;


/*****************************************************
* P0 command 命令码
******************************************************/
__packed typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x01,
    ACTION_READ_DEV_STATUS      = 0x02,
    ACTION_READ_DEV_STATUS_ACK  = 0x03,
    ACTION_REPORT_DEV_STATUS    = 0X04,
    ACTION_W2D_TRANSPARENT_DATA = 0x05,
    ACTION_D2W_TRANSPARENT_DATA = 0x06,
} actionType_t;

__packed typedef struct
{
    protocolHead_t          head;
    actionType_t            action;
    gizwitsReport_t         reportData;
    uint8_t                 sum;
} protocolReport_t;

/******************************************************
* P0报文标准头
********************************************************/
__packed typedef struct
{
    protocolHead_t          head;
    actionType_t            action;
} protocolP0Head_t;

__packed typedef struct
{
    uint8_t issuedFlag;
    uint8_t protocolBuf[MAX_PACKAGE_LEN];
    uint8_t transparentBuff[MAX_PACKAGE_LEN];
    uint32_t transparentLen;
    
    uint32_t sn;
    uint32_t timerMsCount;
    uint32_t lastReportTime;
    protocolWaitAck_t waitAck;
    
    eventInfo_t issuedProcessEvent;                 //控制事件
    eventInfo_t wifiStatusEvent;                    //WIFI状态 事件

    volatile gizwitsReport_t lastReportData;
    gizwitsIssued_t issuedData;                     //云端下发控制报文数据
    moduleStatusInfo_t wifiStatusData;              //WIFI 状态信息(信号强度)
}gizwitsProtocol_t;

uint16_t protocolExchangeBytes(uint16_t value);
void gizwitsInit(void);
void gizwitsHandle(gizwitsReport_t *reportData);
void gizwitsTimerMs(void);
uint8_t gizwitsSetDefault(void);
uint8_t gizwitsSetMode(uint8_t mode);
uint8_t gizwitsReportData(uint8_t action, uint8_t *data, uint32_t len);
int8_t gizwitsPutData(uint8_t *buf, uint32_t len);
int8_t eventProcess(eventInfo_t *info, uint8_t *data, uint32_t len);

uint32_t gizwitsGetTimerCount(void);

#endif

