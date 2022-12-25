/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.00
* Date               : 2022/08/20
* Description        : uart serial port related initialization and processing
*******************************************************************************/

#include "UART.h"
#include "EasyCon_API.h"

/*******************************************************************************/
/* Variable Definition */
/* Global */

/* The following are serial port transmit and receive related variables and buffers */
volatile UART_CTL Uart;

__attribute__ ((aligned(4))) uint8_t  UART1_Tx_Buf[ DEF_UARTx_TX_BUF_LEN ];  /* Serial port 1 transmit data buffer */

void UART1_ParaInit( uint8_t mode )
{	
    uint8_t i;

    Uart.Rx_LoadPtr = 0x00;
    Uart.Rx_DealPtr = 0x00;
    Uart.Rx_RemainLen = 0x00;
    Uart.Rx_TimeOut = 0x00;
    Uart.Rx_TimeOutMax = 30;

    Uart.Tx_LoadNum = 0x00;
    Uart.Tx_DealNum = 0x00;
    Uart.Tx_RemainNum = 0x00;
    for( i = 0; i < DEF_UARTx_TX_BUF_NUM_MAX; i++ )
    {
        Uart.Tx_PackLen[ i ] = 0x00;
    }
    Uart.Tx_Flag = 0x00;
    Uart.Tx_CurPackLen = 0x00;
    Uart.Tx_CurPackPtr = 0x00;

    Uart.USB_Up_IngFlag = 0x00;
    Uart.USB_Up_TimeOut = 0x00;
    Uart.USB_Up_Pack0_Flag = 0x00;
    Uart.USB_Down_StopFlag = 0x00;

    if( mode )
    {
        Uart.Com_Cfg[ 0 ] = (uint8_t)( DEF_UARTx_BAUDRATE );
        Uart.Com_Cfg[ 1 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 8 );
        Uart.Com_Cfg[ 2 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 16 );
        Uart.Com_Cfg[ 3 ] = (uint8_t)( DEF_UARTx_BAUDRATE >> 24 );
        Uart.Com_Cfg[ 4 ] = DEF_UARTx_STOPBIT;
        Uart.Com_Cfg[ 5 ] = DEF_UARTx_PARITY;
        Uart.Com_Cfg[ 6 ] = DEF_UARTx_DATABIT;
        Uart.Com_Cfg[ 7 ] = DEF_UARTx_RX_TIMEOUT;
    }
}

/*********************************************************************
 * @fn      UART1_USB_Init
 *
 * @brief   Serial port 1 initialization in usb interrupt
 *
 * @return  none
 */
void UART_USB_Init( void )
{
    uint32_t baudrate;
    uint8_t  stopbits;
    uint8_t  parity;

    baudrate = ( uint32_t )( Uart.Com_Cfg[ 3 ] << 24 ) + ( uint32_t )( Uart.Com_Cfg[ 2 ] << 16 );
    baudrate += ( uint32_t )( Uart.Com_Cfg[ 1 ] << 8 ) + ( uint32_t )( Uart.Com_Cfg[ 0 ] );
    stopbits = Uart.Com_Cfg[ 4 ];
    parity = Uart.Com_Cfg[ 5 ];
    printf( "baud %d\r\n", baudrate );

    /* restart usb receive  */
    R16_UEP2_DMA = (uint16_t)(uint32_t)(uint8_t *)&UART1_Tx_Buf[ 0 ];
    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
}

/*********************************************************************
 * @fn      UART1_DataTx_Deal
 *
 * @brief   Serial port 1 data transmission processing
 *
 * @return  none
 */
void UART1_DataTx_Deal( void )
{
    uint16_t  count;

    /* uart1 transmission processing */
    if( Uart.Tx_Flag )
    {

			Uart.Tx_Flag = 0x00;

			NVIC_DisableIRQ( USBHD_IRQn );
			NVIC_DisableIRQ( USBHD_IRQn );

			/* Calculate the variables of interest */
			count = Uart.Tx_CurPackLen;
			Uart.Tx_CurPackLen -= count;
			Uart.Tx_CurPackPtr += count;
			if( Uart.Tx_CurPackLen == 0x00 )
			{
					Uart.Tx_PackLen[ Uart.Tx_DealNum ] = 0x0000;
					Uart.Tx_DealNum++;
					if( Uart.Tx_DealNum >= DEF_UARTx_TX_BUF_NUM_MAX )
					{
							Uart.Tx_DealNum = 0x00;
					}
					Uart.Tx_RemainNum--;
			}

			/* If the current serial port has suspended the downlink, restart the driver downlink */
			if( ( Uart.USB_Down_StopFlag == 0x01 ) &&
					( Uart.Tx_RemainNum < ( DEF_UARTx_TX_BUF_NUM_MAX - 2 ) ) )
			{
					R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
					Uart.USB_Down_StopFlag = 0x00;
			}

			NVIC_EnableIRQ( USBHD_IRQn );

    }
    else
    {
        /* Load data from the serial port send buffer to send  */
        if( Uart.Tx_RemainNum )
        {
            /* Determine whether to load from the last unsent buffer or from a new buffer */
            if( Uart.Tx_CurPackLen == 0x00 )
            {
                Uart.Tx_CurPackLen = Uart.Tx_PackLen[ Uart.Tx_DealNum ];
                Uart.Tx_CurPackPtr = ( Uart.Tx_DealNum * DEF_USB_FS_PACK_LEN );
            }
						for(count=0;count<Uart.Tx_CurPackLen;count++)
						{
							EasyCon_serial_task(UART1_Tx_Buf[ Uart.Tx_CurPackPtr+count ]);
							//printf("%x",UART1_Tx_Buf[ Uart.Tx_CurPackPtr+count ]);
						}
							
						


            Uart.Tx_Flag = 0x01;
        }
    }
}
