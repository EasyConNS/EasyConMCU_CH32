/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.h
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022Äê7ÔÂ8ÈÕ
 * Description:
*******************************************************************************/

#ifndef USER_USBD_COMPATIBILITY_HID_H_
#define USER_USBD_COMPATIBILITY_HID_H_

#include "debug.h"
#include "string.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"


#define DEF_UART2_BUF_SIZE            2048
#define DEF_UART2_TOUT_TIME           30             // NOTE: the timeout time should be set according to the actual baud rate.

#define SET_REPORT_DEAL_OVER          0x00
#define SET_REPORT_WAIT_DEAL          0x01


extern uint8_t  HID_Report_Buffer[DEF_USBD_MAX_PACK_SIZE];               // HID Report Buffer
extern uint8_t  HIDTxBuffer[DEF_USBD_MAX_PACK_SIZE];

extern void UART2_Tx_Service( void );
extern void UART2_Rx_Service( void );
extern void UART2_Init( void );
extern void UART2_DMA_Init( void );
extern void TIM2_Init( void );
extern uint8_t USBD_ENDPx_DataUp( uint8_t endp, uint8_t *pbuf, uint16_t len );
extern uint8_t USBD_Endp2_Busy;

#endif /* USER_USBD_COMPATIBILITY_HID_H_ */
