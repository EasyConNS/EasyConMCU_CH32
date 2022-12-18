/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.00
* Date               : 2022/08/20
* Description        : uart serial port related initialization and processing
*******************************************************************************/

#include "UART.h"

/*******************************************************************************/
/* Variable Definition */
/* Global */

/* The following are serial port transmit and receive related variables and buffers */
volatile UART_CTL Uart;

__attribute__ ((aligned(4))) uint8_t  UART1_Tx_Buf[ DEF_UARTx_TX_BUF_LEN ];  /* Serial port 1 transmit data buffer */
__attribute__ ((aligned(4))) uint8_t  UART1_Rx_Buf[ DEF_UARTx_RX_BUF_LEN ];  /* Serial port 1 receive data buffer */
volatile uint32_t UARTx_Rx_DMACurCount;                       /* Serial port 1 receive dma current counter */
volatile uint32_t UARTx_Rx_DMALastCount;                      /* Serial port 1 receive dma last value counter  */

/*********************************************************************
 * @fn      RCC_Configuration
 *
 * @brief   Configures the different system clocks.
 *
 * @return  none
 */
uint8_t RCC_Configuration( void )
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    return 0;
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   100us Timer
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    TIM_DeInit( TIM2 );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 100;
    TIM_TimeBaseStructure.TIM_Prescaler = 72;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    /* Clear TIM2 update pending flag */
    TIM_ClearFlag( TIM2, TIM_FLAG_Update );

    /* TIM IT enable */
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

    /* Enable Interrupt */
    NVIC_EnableIRQ( TIM2_IRQn );

    /* TIM2 enable counter */
    TIM_Cmd( TIM2, ENABLE );
}

/*********************************************************************
 * @fn      UART1_CfgInit
 *
 * @brief   Serial port 1 configuration initialization
 *
 * @return  none
 */
void UART1_CfgInit( uint32_t baudrate, uint8_t stopbits, uint8_t parity )
{
	USART_InitTypeDef USART_InitStructure = {0};
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	uint16_t dat = dat;

    /* First set the serial port introduction to output high then close the TE and RE of CTLR1 register (note that USART1->CTLR1 register setting 9 bits has a limit) */
    /* Note: This operation must be performed, the TX pin otherwise the level will be pulled low */
    GPIO_SetBits( GPIOA, GPIO_Pin_9 );
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

	/* clear te/re */
	USART1->CTLR1 &= ~0x0C;

	/* USART1 Hard configured: */
    /* Configure USART1 Rx (PA10) as input floating */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* Configure USART1 Tx (PA9) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* Test IO */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

  	/* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  	*/
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

	/* Number of stop bits (0: 1 stop bit; 1: 1.5 stop bits; 2: 2 stop bits). */
	if( stopbits == 1 )
	{
		USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	}
	else if( stopbits == 2 )
	{
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
	}
	else
	{
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
	}

	/* Check digit (0: None; 1: Odd; 2: Even; 3: Mark; 4: Space); */
	if( parity == 1 )
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	}
	else if( parity == 2 )
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;
	}
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &USART_InitStructure );
	USART_ClearFlag( USART1, USART_FLAG_TC );

	/* Enable USART1 */
	USART_Cmd( USART1, ENABLE ); 
}

/*********************************************************************
 * @fn      UART1_CfgInit
 *
 * @brief   Serial port 1 configuration initialization
 *
 * @return  none
 */
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
    UARTx_Rx_DMACurCount = 0x00;
    UARTx_Rx_DMALastCount = 0x00;

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
 * @fn      UART1_DMAInit
 *
 * @brief   Serial port 1 DMA configuration initialization
 *
 * @return  none
 */
void UART1_DMAInit( uint8_t type, uint8_t *pbuf, uint32_t len )
{
	DMA_InitTypeDef DMA_InitStructure = {0};
	DMA_Channel_TypeDef *DMAy_Channelx;

    if( type == 0x00 )
    {
        DMAy_Channelx = DMA1_Channel4;
    }
    else
    {
        DMAy_Channelx = DMA1_Channel5;
    }

    if( type == 0x00 )
    {
        /* UART1 Tx-DMA configuration */
        DMA_DeInit( DMAy_Channelx );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DATAR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DMAy_Channelx, &DMA_InitStructure );

        DMA_Cmd( DMAy_Channelx, ENABLE );
    }
    else
    {
        /* UART1 Rx-DMA configuration */
        DMA_DeInit( DMAy_Channelx );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DATAR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DMAy_Channelx, &DMA_InitStructure );

        DMA_Cmd( DMAy_Channelx, ENABLE );
    }
}

/*********************************************************************
 * @fn      UART1_Init
 *
 * @brief   Serial port 1 total initialization
 *
 * @return  none
 */
void UART1_Init( uint8_t mode, uint32_t baudrate, uint8_t stopbits, uint8_t parity )
{
    USART_DMACmd( USART1, USART_DMAReq_Rx, DISABLE );
    DMA_Cmd( DMA1_Channel4, DISABLE );
    DMA_Cmd( DMA1_Channel5, DISABLE );
    UART1_CfgInit( baudrate, stopbits, parity );
    UART1_DMAInit( 0, &UART1_Tx_Buf[ 0 ], 0 );
    UART1_DMAInit( 1, &UART1_Rx_Buf[ 0 ], DEF_UARTx_RX_BUF_LEN );
    USART_DMACmd( USART1, USART_DMAReq_Rx, ENABLE );

    UART1_ParaInit( mode );
}

/*********************************************************************
 * @fn      UART1_USB_Init
 *
 * @brief   Serial port 1 initialization in usb interrupt
 *
 * @return  none
 */
void UART1_USB_Init( void )
{
    uint32_t baudrate;
    uint8_t  stopbits;
    uint8_t  parity;

    baudrate = ( uint32_t )( Uart.Com_Cfg[ 3 ] << 24 ) + ( uint32_t )( Uart.Com_Cfg[ 2 ] << 16 );
    baudrate += ( uint32_t )( Uart.Com_Cfg[ 1 ] << 8 ) + ( uint32_t )( Uart.Com_Cfg[ 0 ] );
    stopbits = Uart.Com_Cfg[ 4 ];
    parity = Uart.Com_Cfg[ 5 ];

    UART1_Init( 0, baudrate, stopbits, parity );
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
        /* Query whether the DMA transmission of the serial port is completed */
        if( USART1->STATR & USART_FLAG_TC )
        {
            USART1->STATR = (uint16_t)( ~USART_FLAG_TC );
            USART1->CTLR3 &= ( ~USART_DMAReq_Tx );

            Uart.Tx_Flag = 0x00;

            NVIC_DisableIRQ( USBHD_IRQn );
            NVIC_DisableIRQ( USBHD_IRQn );

            /* Calculate the variables of interest */
            count = Uart.Tx_CurPackLen - DEF_UART1_TX_DMA_CH->CNTR;
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

            /* Configure DMA and send */
            USART1->STATR = (uint16_t)( ~USART_FLAG_TC );
            DMA_Cmd( DEF_UART1_TX_DMA_CH, DISABLE );
            DEF_UART1_TX_DMA_CH->MADDR = (uint32_t)&UART1_Tx_Buf[ Uart.Tx_CurPackPtr ];
            DEF_UART1_TX_DMA_CH->CNTR = Uart.Tx_CurPackLen;
            DMA_Cmd( DEF_UART1_TX_DMA_CH, ENABLE );
            USART1->CTLR3 |= USART_DMAReq_Tx;

            Uart.Tx_Flag = 0x01;
        }
    }
}

/*********************************************************************
 * @fn      UART1_DataRx_Deal
 *
 * @brief   Serial port 1 data receiving processing
 *
 * @return  none
 */
void UART1_DataRx_Deal( void )
{
    uint16_t temp16;
    uint32_t remain_len;
    uint16_t packlen;

    /* Serial port 1 data DMA receive processing */
    NVIC_DisableIRQ( USBHD_IRQn );
    UARTx_Rx_DMACurCount = DEF_UART1_RX_DMA_CH->CNTR;
    if( UARTx_Rx_DMALastCount != UARTx_Rx_DMACurCount )
    {
        if( UARTx_Rx_DMALastCount > UARTx_Rx_DMACurCount )
        {
            temp16 = UARTx_Rx_DMALastCount - UARTx_Rx_DMACurCount;
        }
        else
        {
            temp16 = DEF_UARTx_RX_BUF_LEN - UARTx_Rx_DMACurCount;
            temp16 += UARTx_Rx_DMALastCount;
        }
        UARTx_Rx_DMALastCount = UARTx_Rx_DMACurCount;
        if( ( Uart.Rx_RemainLen + temp16 ) > DEF_UARTx_RX_BUF_LEN )
        {
            /* Overflow handling */
            /* Save frame error status */
            DUG_PRINTF("U0_O:%08lx\n",(uint32_t)Uart.Rx_RemainLen);
        }
        else
        {
            Uart.Rx_RemainLen += temp16;
        }

        /* Setting reception status */
        Uart.Rx_TimeOut = 0x00;
    }
    NVIC_EnableIRQ( USBHD_IRQn );

    /*****************************************************************/
    /* Serial port 1 data processing via USB upload and reception */
    if( Uart.Rx_RemainLen )
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            /* Calculate the length of this upload */
            remain_len = Uart.Rx_RemainLen;
            packlen = 0x00;
            if( remain_len >= DEF_USBD_FS_PACK_SIZE )
            {
                packlen = DEF_USBD_FS_PACK_SIZE;
            }
            else
            {
                if( Uart.Rx_TimeOut >= Uart.Rx_TimeOutMax )
                {
                    packlen = remain_len;
                }
            }
            if( packlen > ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr ) )
            {
                packlen = ( DEF_UARTx_RX_BUF_LEN - Uart.Rx_DealPtr );
            }
            /* Upload serial data via usb */
            if( packlen )
            {
                NVIC_DisableIRQ( USBHD_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;
                USBHD_Endp_DataUp( DEF_UEP2, &UART1_Rx_Buf[ Uart.Rx_DealPtr ], packlen, DEF_UEP_CPY_LOAD );
                NVIC_EnableIRQ( USBHD_IRQn );

                /* Calculate the variables of interest */
                Uart.Rx_RemainLen -= packlen;
                Uart.Rx_DealPtr += packlen;
                if( Uart.Rx_DealPtr >= DEF_UARTx_RX_BUF_LEN )
                {
                    Uart.Rx_DealPtr = 0x00;
                }

                /* Start 0-length packet timeout timer */
                if( packlen == DEF_USBD_FS_PACK_SIZE )
                {
                    Uart.USB_Up_Pack0_Flag = 0x01;
                }
            }
        }
        else
        {
            /* Set the upload success flag directly if the upload is not successful after the timeout */
            if( Uart.USB_Up_TimeOut >= DEF_UARTx_USB_UP_TIMEOUT )
            {
                Uart.USB_Up_IngFlag = 0x00;
                USBHD_Endp_Busy[ DEF_UEP3 ] = 0;
            }
        }
    }

    /*****************************************************************/
    /* Determine if a 0-length packet needs to be uploaded (required for CDC mode) */
    if( Uart.USB_Up_Pack0_Flag )
    {
        if( Uart.USB_Up_IngFlag == 0 )
        {
            if( Uart.USB_Up_TimeOut >= ( DEF_UARTx_RX_TIMEOUT * 20 ) )
            {
                NVIC_DisableIRQ( USBHD_IRQn );
                NVIC_DisableIRQ( USBHD_IRQn );
                Uart.USB_Up_IngFlag = 0x01;
                Uart.USB_Up_TimeOut = 0x00;
                USBHD_Endp_DataUp( DEF_UEP2, &UART1_Rx_Buf[ Uart.Rx_DealPtr ], 0, DEF_UEP_CPY_LOAD );
                Uart.USB_Up_IngFlag = 0;
                NVIC_EnableIRQ( USBHD_IRQn );
                Uart.USB_Up_Pack0_Flag = 0x00;
            }
        }
    }
}
