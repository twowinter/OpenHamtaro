



#include <stm32f10x.h>
#include "Hal_Usart/hal_uart.h"
#include "dal_rb.h"
#include "zigbee_ha.h"
#include "header.h"
#include "math.h"

/* Start-of-frame delimiter for UART transport */
#define ZB_UART_SOF                     0xFE

/* State values for ZTool protocal */
#define SOP_STATE      0x00
#define CMD_STATE1     0x01
#define CMD_STATE2     0x02
#define LEN_STATE      0x03
#define DATA_STATE     0x04
#define FCS_STATE      0x05

typedef union {
	uint16_t   hword;
#ifdef __BIG_ENDIAN
	struct {
		uint8_t  high;
		uint8_t  low;
	} bytes;
#else
	struct {
		uint8_t  low;
		uint8_t  high;
	} bytes;
#endif
} HWORD_UNION;

typedef struct
{
    uint8_t     sop;
    uint8_t     len;
    HWORD_UNION cmd;
    uint8_t     data[200];
    uint8_t     chksum;
} zbFrame_T;

typedef struct {
    uint8_t   rxState;
    uint8_t   rxLen;
    zbFrame_T rxFrame;
    uint8_t   txFrame[256];
    uint8_t   txSeq;
} ZB_HA_T;

// Ñ­»·»º³å
static dal_rb_t zbRb;
static char zbBuf[MAX_ZB_BUFFER_LEN];
static ZB_HA_T s_zcb;

static uint8_t UartCalcFCS(uint8_t *msg_ptr, uint8_t len)
{
  uint8_t x;
  uint8_t xorResult;

  xorResult = 0;

  for ( x = 0; x < len; x++, msg_ptr++ )
    xorResult = xorResult ^ *msg_ptr;

  return ( xorResult );
}

static int8_t GetDataFromBuf(void)
{
    uint8_t  ch;
    
    while (DalRbCanRead(&zbRb)) {
        DalRbRead(&zbRb, &ch, 1);

        switch (s_zcb.rxState) {
            case SOP_STATE:
                if (ch == ZB_UART_SOF) {
                    s_zcb.rxFrame.sop = ZB_UART_SOF;
                    s_zcb.rxState = LEN_STATE;
                    s_zcb.rxLen = 0;
                }
                break;

            case LEN_STATE:
                s_zcb.rxFrame.len = ch;
                s_zcb.rxFrame.chksum = ch;
                s_zcb.rxState = CMD_STATE1;
                break;

            case CMD_STATE1:
                s_zcb.rxFrame.cmd.bytes.high = ch;
                s_zcb.rxState = CMD_STATE2;
                s_zcb.rxFrame.chksum ^= ch;
                break;

            case CMD_STATE2:
                s_zcb.rxFrame.cmd.bytes.low = ch;
                s_zcb.rxFrame.chksum ^= ch;
                /* If there is no data, skip to FCS state */
                if (s_zcb.rxFrame.len) {
                  s_zcb.rxState = DATA_STATE;
                } else {
                  s_zcb.rxState = FCS_STATE;
                }
                break;

            case DATA_STATE:
                s_zcb.rxFrame.data[s_zcb.rxLen++] = ch;
                s_zcb.rxFrame.chksum ^= ch;
                if ( s_zcb.rxFrame.len == s_zcb.rxLen) {
                    s_zcb.rxState = FCS_STATE;
                }
                break;

            case FCS_STATE:
                /* Reset the state, send or discard the buffers at this point */
                s_zcb.rxState = SOP_STATE;
                if (s_zcb.rxFrame.chksum != ch) {
                    #if EN_DEBUG > 0
                    printf("uart chk sum err\r\n");
                    #endif
                } else {
                    #if EN_DEBUG > 0
                    printf("uart chk OK\r\n");
                    #endif
                    return 1;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}


static int8_t PutDataIntoBuf(uint8_t *buf, uint32_t len)
{
    uint8_t count = 0;

    if (NULL == buf) {
        #if EN_DEBUG > 0
        printf("ERROR: buf is empty \r\n");
        #endif
        return -1;
    }

    count = DalRbWrite(&zbRb, buf, len);
    if (count != len) {
        #if EN_DEBUG > 0
        printf("ERROR: Failed to rbWrite \r\n");
        #endif
        return -1;
    }

    return count;
}


void ZB_IRQ_FUN(void)
{
    uint8_t value = 0;
    
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { 		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);

		value = USART_ReceiveData(USART1);
		PutDataIntoBuf(&value, 1);
	}
}


void ZB_HA_Handle(void)
{
    int8_t ret;

    ret = GetDataFromBuf();
    if (ret == 1) {
        switch (s_zcb.rxFrame.cmd.hword) {
            default :
                break;
        }
    }
}


static int8_t UartSendPack(uint16_t cmd, uint8_t *pData, uint8_t len)
{
	uint8_t i, sendLen = 0;
	
	if (pData==NULL) len=0;
	
	s_zcb.txFrame[sendLen++] = ZB_UART_SOF;
	s_zcb.txFrame[sendLen++] = len;
	s_zcb.txFrame[sendLen++] = cmd >> 8;
	s_zcb.txFrame[sendLen++] = cmd;
	
	if (len > 0) {
		memcpy(&s_zcb.txFrame[sendLen], pData, len);
		sendLen += len;
	}

    s_zcb.txFrame[sendLen++] = UartCalcFCS(&s_zcb.txFrame[2], len + 3);
    
    for (i = 0; i < sendLen; i++) {
        uart1SendData(s_zcb.txFrame[i]);
    }
	return sendLen;
}

static void Conver_RGB_To_xyY(uint8_t R, uint8_t G, uint8_t B, uint16_t *XX, uint16_t *YY)
{
    //rgb -> XYZ
    float var_R = (R / 255.0);
    float var_G = (G / 255.0);
    float var_B = (B / 255.0);

    if (var_R > 0.04045)
        var_R = pow((var_R + 0.055) / 1.055,2.4);
    else
        var_R = var_R / 12.92;
    if (var_G > 0.04045)
        var_G = pow((var_G + 0.055) / 1.055,2.4);
    else
        var_G = var_G / 12.92;
    if (var_B > 0.04045)
        var_B = pow((var_B + 0.055) / 1.055,2.4);
    else
        var_B = var_B / 12.92;

    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    //Observer. = 2¡ã, Illuminant = D65
    float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    float x = X/(X+Y+Z);
    float y = Y/(X+Y+Z);

    *XX = (uint16_t) (x*65536);
    *YY = (uint16_t) (y*65536);
}


/*

On:     FE 0B 29 00 08 FF FF 08 06 00 04 02 01 00 01 29 
Off:    FE 0B 29 00 08 FF FF 08 06 00 04 02 01 01 00 00 
Red:    FE 0B 29 00 08 FF FF 08 06 00 04 02 01 02 01 2B 
Yellow: FE 11 29 00 08 FF FF 08 00 03 0A 02 01 03 07 58 6B 58 81 00 00 CD 
Pink:   FE 11 29 00 08 FF FF 08 00 03 0A 02 01 04 07 2D 9E 74 54 00 00 7E 
Purple: FE 11 29 00 08 FF FF 08 00 03 0A 02 01 05 07 E1 96 52 4D 00 00 37 

*/

void ZB_HA_LightControl(ZB_LIGHT_CMD_T *cmd)
{
    uint8_t data[200] = {0x08, 0xFF, 0xFF, 0x08};
    uint8_t cmdLen = 4;
    uint16_t colorX, colorY;
    
    data[cmdLen++] = cmd->clusterId;
    data[cmdLen++] = cmd->clusterId >> 8;
    switch (cmd->clusterId) {
        case ZCL_Cluster_ID_OnOff:
            data[cmdLen++] = 4;
            data[cmdLen++] = 0x02;
            data[cmdLen++] = 0x01;
            data[cmdLen++] = s_zcb.txSeq++;
            data[cmdLen++] = cmd->attr.cmd_onoff.onoff;//OnOff
            break;
        case ZCL_Cluster_ID_LevelControl:
            data[cmdLen++] = 7;
            data[cmdLen++] = 0x02;
            data[cmdLen++] = 0x01;
            data[cmdLen++] = s_zcb.txSeq++;
            data[cmdLen++] = 0x00;// Move to level
            data[cmdLen++] = cmd->attr.cmd_levelctrl.level;
            data[cmdLen++] = 0x00;
            data[cmdLen++] = 0x00;
            break;
        case ZCL_Cluster_ID_ColorControl:
            data[cmdLen++] = 10;
            data[cmdLen++] = 0x02;
            data[cmdLen++] = 0x01;
            data[cmdLen++] = s_zcb.txSeq++;
            data[cmdLen++] = 0x07;// Move to color
            Conver_RGB_To_xyY(cmd->attr.cmd_colorctrl.red, cmd->attr.cmd_colorctrl.green, cmd->attr.cmd_colorctrl.blue, &colorX, &colorY);
            data[cmdLen++] = colorX;
            data[cmdLen++] = colorX >> 8;
            data[cmdLen++] = colorY;
            data[cmdLen++] = colorY >> 8;
            data[cmdLen++] = 0x00;
            data[cmdLen++] = 0x00;
            break;
        default :
            break;
    }
    
    UartSendPack(ZB_UART_CMD_APP_REQ, data, cmdLen);
}

void ZB_HA_Init(void)
{    
    zbRb.rbCapacity = MAX_ZB_BUFFER_LEN;
    zbRb.rbBuff = zbBuf;
    DalRbCreate(&zbRb);
}

