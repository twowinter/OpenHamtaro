

#ifndef _ZIGBEE_HA_H
#define _ZIGBEE_HA_H

#include <stdio.h>
#include <string.h>

// Use USART1 for ZB Moudule
#define ZB_IRQ_FUN  USART1_IRQHandler

#define MAX_ZB_BUFFER_LEN  256  //环形缓冲区最大长度
#define ZB_UART_CMD_APP_REQ     0x2900


#define ZCL_Cluster_ID_OnOff        0x0006
#define ZCL_Cluster_ID_LevelControl 0x0008
#define ZCL_Cluster_ID_ColorControl 0x0300


typedef union {
    struct {
        uint8_t onoff;
    } cmd_onoff;
    
    struct {
        uint8_t level;
    } cmd_levelctrl;
    
    struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } cmd_colorctrl;
    
} ZB_LIGHT_ATTR_UNION;

typedef struct {
    uint16_t clusterId;
    ZB_LIGHT_ATTR_UNION attr;
} ZB_LIGHT_CMD_T;

void ZB_HA_Handle(void);
void ZB_HA_LightControl(ZB_LIGHT_CMD_T *cmd);
void ZB_HA_Init(void);

#endif
