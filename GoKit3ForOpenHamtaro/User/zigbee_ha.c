


#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>
#include "Hal_Usart/hal_uart.h"
#include "dal_rb.h"
#include "zigbee_ha.h"
#include "header.h"

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
    uint8_t     state;
    uint8_t     len;
    HWORD_UNION cmd;
    uint8_t     data[200];
    uint8_t     chksum;
    uint8_t     curLen;
} zbFrame_T;

// Ñ­»·»º³å
static dal_rb_t zbRb;
static char zbBuf[MAX_ZB_BUFFER_LEN];


static zbFrame_T s_zbFrame;


static int8_t GetDataFromBuf(void)
{
    uint8_t  ch;
    
    while (DalRbCanRead(&zbRb)) {
        DalRbRead(&zbRb, &ch, 1);

        switch (s_zbFrame.state) {
            case SOP_STATE:
                if (ch == ZB_UART_SOF) {
                    s_zbFrame.state = LEN_STATE;
                    s_zbFrame.curLen = 0;
                }
                break;

            case LEN_STATE:
                s_zbFrame.len = ch;
                s_zbFrame.chksum = ch;
                s_zbFrame.state = CMD_STATE1;
                break;

            case CMD_STATE1:
                s_zbFrame.cmd.bytes.high = ch;
                s_zbFrame.state = CMD_STATE2;
                s_zbFrame.chksum ^= ch;
                break;

            case CMD_STATE2:
                s_zbFrame.cmd.bytes.low = ch;
                s_zbFrame.chksum ^= ch;
                /* If there is no data, skip to FCS state */
                if (s_zbFrame.len) {
                  s_zbFrame.state = DATA_STATE;
                } else {
                  s_zbFrame.state = FCS_STATE;
                }
                break;

            case DATA_STATE:
                s_zbFrame.data[s_zbFrame.curLen++] = ch;
                s_zbFrame.chksum ^= ch;
                if ( s_zbFrame.len == s_zbFrame.curLen) {
                    s_zbFrame.state = FCS_STATE;
                }
                break;

            case FCS_STATE:
                /* Reset the state, send or discard the buffers at this point */
                s_zbFrame.state = SOP_STATE;
                if (s_zbFrame.chksum != ch) {
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
        switch (s_zbFrame.cmd.hword) {
            default :
                break;
        }
    }
}


static int8_t UartSendPack(uint16_t cmd, uint8_t *pData, uint8_t len)
{
	uint8_t sendData[256];
	uint8_t sendLen = 0;
	uint8_t i;
	
	if (pData==NULL) len=0;
	
	sendData[sendLen++] = ZB_UART_SOF;
	sendData[sendLen++] = len;
	sendData[sendLen++] = cmd>>8;
	sendData[sendLen++] = cmd;
	
	if (len > 0) {
		memcpy(sendData + sendLen, pData, len);
		sendLen += len;
	}

	sendData[sendLen] = 0;
	for (i=0; i < len + 3; i++) {
		sendData[sendLen] ^= sendData[i+1];
	}

	sendLen++;

	#if EN_DEBUG > 0
	printf("UartSendPack:%d\r\n",sendLen);
	#endif
	
    for (i = 0; i < sendLen; i++) {
        uart1SendData(sendData[i]);
    }
	return sendLen;
}


void ZB_HA_LightControl(HA_EVENT_TYPE_T )
{
    ;
}

void ZB_HA_Init(void)
{    
    zbRb.rbCapacity = MAX_ZB_BUFFER_LEN;
    zbRb.rbBuff = zbBuf;
    DalRbCreate(&zbRb);
}

