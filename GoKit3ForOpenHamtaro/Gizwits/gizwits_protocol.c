#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gizwits_product.h"
#include "gizwits_protocol.h"

#define min(a, b) (a)<(b)?(a):(b)

/** 环形缓冲区变量定义 **/
typedef struct {
    size_t rbCapacity;
    char  *rbHead;
    char  *rbTail;
    char  *rbBuff;
}rb_t;

rb_t pRb;

static char rbBuf[MAX_RINGBUFFER_LEN];

/** 协议全局变量 **/
gizwitsProtocol_t gizwitsProtocol;
extern volatile gizwitsReport_t reportData;//上报数据结构体

static void rbCreate(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return;
    }

    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
}

static void rbDelete(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return;
    }

    rb->rbBuff = NULL;
    rb->rbHead = NULL;
    rb->rbTail = NULL;
    rb->rbCapacity = 0;
}

static size_t rbCapacity(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return -1;
    }

    return rb->rbCapacity;
}

static size_t rbCanRead(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return -1;
    }

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);
}

static size_t rbCanWrite(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return -1;
    }

    return rbCapacity(rb) - rbCanRead(rb);
}

static size_t rbRead(rb_t *rb, void *data, size_t count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: input data is NULL\r\n");
        return -1;
    }

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, rbCanRead(rb));
        memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += rbRead(rb, (char*)data+copySz, count-copySz);
            return copySz;
        }
    }
}

static size_t rbWrite(rb_t *rb, const void *data, size_t count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: rb is empty \r\n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \r\n");
        return -1;
    }

    if (count >= rbCanWrite(rb))
    {
        GIZWITS_LOG("ERROR: no memory %d \r\n", rbCanWrite(rb));
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            return count;
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + rbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        return count;
    }
}

static uint8_t protocolSum( uint8_t *buf, int len )
{
    uint8_t     sum = 0;
    int         i = 0;

    if(buf == NULL || len <= 0)
    {
        GIZWITS_LOG("ERROR: buf is empty or len error %d \r\n", len);
        return 0;
    }

    for(i=2; i<len-1; i++)
    {
        sum += buf[i];
    }

    return sum;
}

uint16_t protocolExchangeBytes(uint16_t value)
{
    uint16_t    tmp_value;
    uint8_t     *index_1, *index_2;

    index_1 = (uint8_t *)&tmp_value;
    index_2 = (uint8_t *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}

/**
* @brief Data Type Conversions x : 转化为协议中的x值及实际通讯传输的值
*
* @param ratio : 修正系数k
* @param addition : 增量m
* @param preValue : 作为协议中的y值, 是App UI界面的显示值
*
* @return uint16_t aft_value : 作为协议中的x值, 是实际通讯传输的值
*/
uint32_t Y2X(uint32_t ratio, int32_t addition, int32_t preValue)
{
    uint32_t aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief Data Type Conversions y : 转化为协议中的y值及App UI界面的显示值
*
* @param ratio : 修正系数k
* @param addition : 增量m
* @param preValue : 作为协议中的x值, 是实际通讯传输的值
*
* @return int16_t : 作为协议中的y值, 是App UI界面的显示值
*/
int32_t X2Y(uint32_t ratio, int32_t addition, uint32_t preValue)
{
    int32_t aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

static int8_t protocolGetOnePacket(rb_t *rb, uint8_t *data, uint16_t *len)
{
    uint8_t ret = 0;
    uint8_t sum = 0;
    uint8_t i = 0;
    uint8_t tmpData;
    uint8_t tmpLen = 0;
    uint16_t tmpCount = 0;
    static uint8_t protocolFlag = 0;
    uint8_t *protocolBuff = data;
    static uint16_t protocolCount = 0;
    static uint8_t lastData = 0;
    static uint8_t debugCount = 0;
    protocolHead_t *head = NULL;

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return (-1);
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = protocolExchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = protocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            memcpy(data, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}


//custom
void dataPoint2Event(gizwitsIssued_t *issuedData, eventInfo_t *info)
{
    if((NULL != info) && (NULL != issuedData))
    {
        if(0x01 == issuedData->attrFlags.LED_OnOff)
        {
            info->event[info->num] = SetLED_OnOff;
            info->num++;
        }
        if(0x01 == issuedData->attrFlags.LED_Color)
        {
            info->event[info->num] = SetLED_Color;
            info->num++;
        }
        if(0x01 == issuedData->attrFlags.LED_R)
        {
            info->event[info->num] = SetLED_R;
            info->num++;
        }
        if(0x01 == issuedData->attrFlags.LED_G)
        {
            info->event[info->num] = SetLED_G;
            info->num++;
        }
        if(0x01 == issuedData->attrFlags.LED_B)
        {
            info->event[info->num] = SetLED_B;
            info->num++;
        }
        if(0x01 == issuedData->attrFlags.Motor_Speed)
        {
            info->event[info->num] = SetMotor;
            info->num++;
        }
    }
}

//custom
int8_t checkReport(gizwitsReport_t *cur, gizwitsReport_t *last)
{
    int8_t ret = 0;
    static uint32_t lastReportTime = 0;

    if(last->devStatus.LED_OnOff != cur->devStatus.LED_OnOff)
    {
        GIZWITS_LOG("LED_OnOff Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.LED_Color != cur->devStatus.LED_Color)
    {
        GIZWITS_LOG("LED_Color Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.LED_R != cur->devStatus.LED_R)
    {
        GIZWITS_LOG("LED_R Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.LED_G != cur->devStatus.LED_G)
    {
        GIZWITS_LOG("LED_G Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.LED_B != cur->devStatus.LED_B)
    {
        GIZWITS_LOG("LED_B Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.Motor_Speed != cur->devStatus.Motor_Speed)
    {
        GIZWITS_LOG("Motor_Speed Check Changed\n");
        ret = 1;
    }

#if EN_HAM == 0   
    if(last->devStatus.Infrared != cur->devStatus.Infrared)
    {
        GIZWITS_LOG("Infrared Check Changed\n");
        ret = 1;
    }
#endif

    if(last->devStatus.Alert_1 != cur->devStatus.Alert_1)
    {
        GIZWITS_LOG("Alert_1 Check Changed\n");
        ret = 1;
    }
    if(last->devStatus.Alert_2 != cur->devStatus.Alert_2)
    {
        GIZWITS_LOG("Alert_2 Check Changed\n");
        ret = 1;
    }
    if(last->devStatus.Fault_LED != cur->devStatus.Fault_LED)
    {
        GIZWITS_LOG("Fault_LED Check Changed\n");
        ret = 1;
    }
    if(last->devStatus.Fault_Motor != cur->devStatus.Fault_Motor)
    {
        GIZWITS_LOG("Fault_Motor Check Changed\n");
        ret = 1;
    }
#if EN_HAM == 0   
    if(last->devStatus.Fault_TemHum != cur->devStatus.Fault_TemHum)
    {
        GIZWITS_LOG("Fault_TemHum Check Changed\n");
        ret = 1;
    }
    if(last->devStatus.Fault_IR != cur->devStatus.Fault_IR)
    {
        GIZWITS_LOG("Fault_IR Check Changed\n");
        ret = 1;
    }
    
    if(last->devStatus.Temperature != cur->devStatus.Temperature)
    {
        if((gizwitsGetTimerCount()-lastReportTime) >= REPORT_TIME_MAX)
        {
            GIZWITS_LOG("Temperature  Check Changed\n");
            lastReportTime = gizwitsGetTimerCount();
            ret = 1;
        }
    }
    
    if(last->devStatus.Humidity != cur->devStatus.Humidity)
    {
        if((gizwitsGetTimerCount()-lastReportTime) >= REPORT_TIME_MAX)
        {
            GIZWITS_LOG("Humidity  Check Changed\n");
            lastReportTime = gizwitsGetTimerCount();
            ret = 1;
        }
    }
#endif      
    return ret;
}

void protocolHeadInit(protocolHead_t *head)
{
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: head is empty \r\n");
        return;
    }

    memset((uint8_t *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;
}

void protocolWaitAck(uint8_t *data, uint32_t len)
{
    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \r\n");
        return;
    }

    memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    memcpy((uint8_t *)gizwitsProtocol.waitAck.buf, data, len);
    gizwitsProtocol.waitAck.dataLen = (uint16_t)len;
    
    gizwitsProtocol.waitAck.flag = 1;
    gizwitsProtocol.waitAck.sendTime = gizwitsGetTimerCount();
}

void protocolResendData(void)
{
    uint8_t ret = 0;

    if(0 == gizwitsProtocol.waitAck.flag)
    {
        return;
    }

    GIZWITS_LOG("Warning: timeout, resend data \r\n");
    
    ret = uartWrite(gizwitsProtocol.waitAck.buf, gizwitsProtocol.waitAck.dataLen);
    if(ret != gizwitsProtocol.waitAck.dataLen)
    {
        GIZWITS_LOG("ERROR: resend data error\n");
    }

    gizwitsProtocol.waitAck.sendTime = gizwitsGetTimerCount();
}

void protocolWaitAckCheck(protocolHead_t *data)
{
    protocolHead_t *waitAckHead = (protocolHead_t *)gizwitsProtocol.waitAck.buf;

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \r\n");
        return;
    }

    if(waitAckHead->cmd+1 == data->cmd)
    {
        memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    }
}

int8_t protocolCommonAck(protocolHead_t *buf)
{
    int8_t ret = 0;
    protocolCommon_t ack;

    memcpy((uint8_t *)&ack, (uint8_t *)buf, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = protocolExchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = protocolSum((uint8_t *)&ack, sizeof(protocolCommon_t));

    ret = uartWrite((uint8_t *)&ack, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }

    return ret;
}

void protocolAckHandle(void)
{
    if(1 == gizwitsProtocol.waitAck.flag)
    {
        if(SEND_MAX_NUM >= gizwitsProtocol.waitAck.num)
        {
            //200ms未收到ACK重发
            if(SEND_MAX_TIME < (gizwitsGetTimerCount() - gizwitsProtocol.waitAck.sendTime))
            {
                GIZWITS_LOG("Warning:protocolResendData %d %d %d\r\n", gizwitsGetTimerCount(), gizwitsProtocol.waitAck.sendTime, gizwitsProtocol.waitAck.num);
                protocolResendData();
                gizwitsProtocol.waitAck.num++;
            }
        }
        else
        {
            memset((uint8_t *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
        }
    }
}

int8_t protocolGetDeviceInfo(protocolHead_t * head)
{
    int8_t ret = 0;
    protocolDeviceInfo_t deviceInfo;

    protocolHeadInit((protocolHead_t *)&deviceInfo);
    deviceInfo.head.cmd = ACK_GET_DEVICE_INFO;
    deviceInfo.head.sn = head->sn;
    memcpy((uint8_t *)deviceInfo.protocolVer, protocol_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.p0Ver, P0_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.softVer, SOFTWARE_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.hardVer, HARDWARE_VERSION, 8);
    memcpy((uint8_t *)deviceInfo.productKey, PRODUCT_KEY, 32);
    memcpy((uint8_t *)deviceInfo.devAttr, DEV_DEFAULT, 8);
    deviceInfo.ninableTime = 0;
    deviceInfo.head.len = protocolExchangeBytes(sizeof(protocolDeviceInfo_t)-4);
    deviceInfo.sum = protocolSum((uint8_t *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    ret = uartWrite((uint8_t *)&deviceInfo, sizeof(protocolDeviceInfo_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }
    
    return ret;
}

int8_t protocolIssuedProcess(uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen)
{
    protocolReport_t *protocolIssuedData = (protocolReport_t *)inData;
    uint8_t issuedAction = 0;
    issuedAction = protocolIssuedData->action;
    
    memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
    switch(issuedAction)
    {
        case ACTION_CONTROL_DEVICE:
            dataPoint2Event((gizwitsIssued_t *)(inData+sizeof(protocolP0Head_t)), &gizwitsProtocol.issuedProcessEvent);
            memcpy((uint8_t *)&gizwitsProtocol.issuedData, (uint8_t *)(inData+sizeof(protocolP0Head_t)), sizeof(gizwitsIssued_t)); 
            gizwitsProtocol.issuedFlag = 1;
            outData = NULL;
            *outLen = 0;
            break;
        
        case ACTION_READ_DEV_STATUS:
            memcpy(outData, (uint8_t *)&gizwitsProtocol.lastReportData, sizeof(gizwitsReport_t));
            *outLen = sizeof(gizwitsReport_t);
            break;
        
        case ACTION_W2D_TRANSPARENT_DATA:
            memcpy(gizwitsProtocol.transparentBuff, inData+sizeof(protocolP0Head_t), inLen-sizeof(protocolP0Head_t)-1);
            gizwitsProtocol.transparentLen = inLen - sizeof(protocolP0Head_t) - 1;
            gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = TRANSPARENT_DATA;
            gizwitsProtocol.issuedProcessEvent.num++;
            gizwitsProtocol.issuedFlag = 3;
            break;
        
        default:
            break;
    }

    return 0;
}

uint8_t protocolModuleStatus(protocolWifiStatus_t *status)
{
    static wifiStatus_t lastStatus;

    status->ststus.value = protocolExchangeBytes(status->ststus.value);
   
    //OnBoarding mode status
    if(lastStatus.types.onboarding != status->ststus.types.onboarding)
    {
        if(1 == status->ststus.types.onboarding)
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\r\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_AIRLINK;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: AirLink mode\r\n");
            }
        }
        else
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\r\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_STATION;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: Station mode\r\n");
            }
        }
    }

    //binding mode status
    if(lastStatus.types.binding != status->ststus.types.binding)
    {
        lastStatus.types.binding = status->ststus.types.binding;
        if(1 == status->ststus.types.binding)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in binding mode\r\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out binding mode\r\n");
        }
    }

    //router status
    if(lastStatus.types.con_route != status->ststus.types.con_route)
    {
        lastStatus.types.con_route = status->ststus.types.con_route;
        if(1 == status->ststus.types.con_route)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected router\r\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected router\r\n");
        }
    }

    //M2M server status
    if(lastStatus.types.con_m2m != status->ststus.types.con_m2m)
    {
        lastStatus.types.con_m2m = status->ststus.types.con_m2m;
        if(1 == status->ststus.types.con_m2m)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected m2m\r\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected m2m\r\n");
        }
    }

    //APP status
    if(lastStatus.types.app != status->ststus.types.app)
    {
        lastStatus.types.app = status->ststus.types.app;
        if(1 == status->ststus.types.app)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: app connect\r\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: no app connect\r\n");
        }
    }

    //test mode status
    if(lastStatus.types.test != status->ststus.types.test)
    {
        lastStatus.types.test = status->ststus.types.test;
        if(1 == status->ststus.types.test)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in test mode\r\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out test mode\r\n");
        }
    }

    gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_RSSI;
    gizwitsProtocol.wifiStatusEvent.num++;
    gizwitsProtocol.wifiStatusData.rssi = status->ststus.types.rssi;
    GIZWITS_LOG("RSSI is %d \r\n", gizwitsProtocol.wifiStatusData.rssi);

    gizwitsProtocol.issuedFlag = 2;

    return 0;
}

void gizwitsTimerMs(void)
{
    gizwitsProtocol.timerMsCount++;
}

uint32_t gizwitsGetTimerCount(void)
{
    return gizwitsProtocol.timerMsCount;
}

void gizwitsInit(void)
{
    timerInit();
    uartInit();
    
    pRb.rbCapacity = MAX_RINGBUFFER_LEN;
    pRb.rbBuff = rbBuf;
    rbCreate(&pRb);
    
    memset((uint8_t *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}

int8_t gizwitsPutData(uint8_t *buf, uint32_t len)
{
    uint8_t count = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("ERROR: buf is empty \r\n");
        return -1;
    }

    count = rbWrite(&pRb, buf, len);
    if(count != len)
    {
        GIZWITS_LOG("ERROR: Failed to rbWrite \r\n");
        return -1;
    }

    return count;
}

uint8_t gizwitsSetDefault(void)
{
    int8_t ret = 0;
    protocolCommon_t setDefault;

    protocolHeadInit((protocolHead_t *)&setDefault);
    setDefault.head.cmd = CMD_SET_DEFAULT;
    setDefault.head.sn = gizwitsProtocol.sn++;
    setDefault.head.len = protocolExchangeBytes(sizeof(protocolCommon_t)-4);
    setDefault.sum = protocolSum((uint8_t *)&setDefault, sizeof(protocolCommon_t));
    ret = uartWrite((uint8_t *)&setDefault, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }

    protocolWaitAck((uint8_t *)&setDefault, sizeof(protocolCommon_t));

    return ret;
}
 
uint8_t gizwitsSetMode(uint8_t mode)
{
    int8_t ret = 0;
    protocolCfgMode_t cfgMode;

    protocolHeadInit((protocolHead_t *)&cfgMode);
    cfgMode.head.cmd = CMD_WIFI_CONFIG;
    cfgMode.head.sn = gizwitsProtocol.sn++;
    cfgMode.cfgMode = mode;
    cfgMode.head.len = protocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
    cfgMode.sum = protocolSum((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));
    ret = uartWrite((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }

    protocolWaitAck((uint8_t *)&cfgMode, sizeof(protocolCfgMode_t));

    return ret;
}

uint8_t protocolIssuedDataAck(protocolHead_t *head, uint8_t *data, uint32_t len)
{
    int8_t ret = 0;
    uint8_t *ptrData = NULL;
    uint32_t dataLen = 0;
    protocolReport_t protocolReport;
    protocolCommon_t protocolCommon;
    protocolP0Head_t *p0Head = (protocolP0Head_t *)head;

    if(0 == len)
    {
        protocolHeadInit((protocolHead_t *)&protocolCommon);
        protocolCommon.head.cmd = head->cmd+1;
        protocolCommon.head.sn = head->sn;
        protocolCommon.head.len = protocolExchangeBytes(sizeof(protocolCommon_t)-4);
        protocolCommon.sum = protocolSum((uint8_t *)&protocolCommon, sizeof(protocolCommon_t));
        
        ptrData = (uint8_t *)&protocolCommon;
        dataLen = sizeof(protocolCommon_t);
    }
    else
    {
        protocolHeadInit((protocolHead_t *)&protocolReport);
        protocolReport.head.cmd = p0Head->head.cmd+1;
        protocolReport.head.sn = p0Head->head.sn;
        protocolReport.head.len = protocolExchangeBytes(sizeof(protocolP0Head_t)+len+1-4);
        protocolReport.action = p0Head->action+1;

        memcpy((uint8_t *)&protocolReport.reportData, data, len);
        protocolReport.sum = protocolSum((uint8_t *)&protocolReport, sizeof(protocolReport_t));
        
        ptrData = (uint8_t *)&protocolReport;
        dataLen = sizeof(protocolReport_t);
    }
    
    ret = uartWrite(ptrData, dataLen);
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: protocolIssuedDataAck uart write error %d dataLen %d \r\n", ret, dataLen);
    }

    return ret;
}

uint8_t gizwitsReportData(uint8_t action, uint8_t *data, uint32_t len)
{
    int8_t ret = 0;
    protocolReport_t protocolReport;

    protocolHeadInit((protocolHead_t *)&protocolReport);
    protocolReport.head.cmd = CMD_REPORT_P0;
    protocolReport.head.sn = gizwitsProtocol.sn++;
    protocolReport.action = action;
    protocolReport.head.len = protocolExchangeBytes(sizeof(protocolReport_t)-4);
    memcpy((gizwitsReport_t *)&protocolReport.reportData, (gizwitsReport_t *)data,len);
    protocolReport.sum = protocolSum((uint8_t *)&protocolReport, sizeof(protocolReport_t));
    
    ret = uartWrite((uint8_t *)&protocolReport, sizeof(protocolReport_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }

    protocolWaitAck((uint8_t *)&protocolReport, sizeof(protocolReport_t));
    gizwitsProtocol.lastReportTime = gizwitsGetTimerCount();

    return ret;
}

uint8_t protocolErrorCmd(protocolHead_t *head,errorPacketsType_t errno)
{
    int8_t ret = 0;
    protocolErrorType_t errorType;

    protocolHeadInit((protocolHead_t *)&errorType);
    errorType.head.cmd = ACK_ERROR_PACKAGE;
    errorType.head.sn = head->sn;
    
    errorType.head.len = protocolExchangeBytes(sizeof(protocolErrorType_t)-4);
    errorType.error = errno;
    errorType.sum = protocolSum((uint8_t *)&errorType, sizeof(protocolErrorType_t));
    
    ret = uartWrite((uint8_t *)&errorType, sizeof(protocolErrorType_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \r\n", ret);
    }

    return ret;
}

/*******************************************************************************
* Function Name  : protocolReboot
* Description    : WiFi模组请求复位设备MCU，MCU回复ACK，并执行设备复位
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void protocolReboot(void)
{
    uint32_t timeDelay = gizwitsGetTimerCount();
    
    /*Wait 600ms*/
    while((gizwitsGetTimerCount() - timeDelay) <= 60);

    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

void gizwitsHandle(gizwitsReport_t *reportData)
{
    int8_t ret = 0;
    uint8_t i = 0;
    uint8_t ackData[MAX_RINGBUFFER_LEN];
    uint16_t protocolLen = 0;
    uint32_t ackLen = 0;
    protocolHead_t *recvHead = NULL;
    
    /*重发机制*/
    protocolAckHandle();
    ret = protocolGetOnePacket(&pRb, gizwitsProtocol.protocolBuf, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get One Packet!\r\n");
        
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("WiFi2MCU[%4d:%4d]: ", gizwitsGetTimerCount(), protocolLen);
        for(i=0; i<protocolLen;i++)
        {
            GIZWITS_LOG("%02x ", gizwitsProtocol.protocolBuf[i]);
        }
        GIZWITS_LOG("\r\n");
#endif

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        switch (recvHead->cmd)
        {
            case CMD_GET_DEVICE_INTO:
                protocolGetDeviceInfo(recvHead);
                break;
            case CMD_ISSUED_P0:
                ret = protocolIssuedProcess(gizwitsProtocol.protocolBuf, protocolLen, ackData, &ackLen);
                if(0 == ret)
                {
                    protocolIssuedDataAck(recvHead, ackData, ackLen);
                }
                break;
            case CMD_HEARTBEAT:
                protocolCommonAck(recvHead);
                break;
            case CMD_WIFISTATUS:
                protocolCommonAck(recvHead);
                protocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case ACK_REPORT_P0:
            case ACK_WIFI_CONFIG:
            case ACK_SET_DEFAULT:
                protocolWaitAckCheck(recvHead);
                break;
            case CMD_MCU_REBOOT:
                protocolCommonAck(recvHead);
                GIZWITS_LOG("report:MCU reboot!\r\n");
                
                protocolReboot();
                break;
            case CMD_ERROR_PACKAGE:
                break;
            default:
                protocolErrorCmd(recvHead,ERROR_CMD);
                GIZWITS_LOG("ERROR: cmd code error!\r\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //校验失败，报告异常
        protocolErrorCmd(recvHead,ERROR_ACK_SUM);
        GIZWITS_LOG("ERROR: check sum error!\r\n");
        return;
    }
    
    if(1 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        eventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)&gizwitsProtocol.issuedData, sizeof(gizwitsIssued_t));
        memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent));//WORK_DONE
        
    }
    else if(2 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        eventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8_t *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
        memset((uint8_t *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));//WORK_DONE
    }
    else if(3 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        eventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
    }
    
    if((1 == checkReport(reportData, (gizwitsReport_t *)&gizwitsProtocol.lastReportData)))
    {
        memcpy((uint8_t *)&gizwitsProtocol.lastReportData, (uint8_t *)reportData, sizeof(gizwitsReport_t));
        GIZWITS_LOG("changed, report data\r\n");
        gizwitsReportData(ACTION_REPORT_DEV_STATUS, (uint8_t *)&gizwitsProtocol.lastReportData, sizeof(gizwitsReport_t));
    }
    
    if(1000*60*10 <= (gizwitsGetTimerCount() - gizwitsProtocol.lastReportTime))
    {
        GIZWITS_LOG("Info: 600S report data\r\n");
        gizwitsReportData(ACTION_REPORT_DEV_STATUS, (uint8_t *)&gizwitsProtocol.lastReportData, sizeof(gizwitsReport_t));
    }
}

