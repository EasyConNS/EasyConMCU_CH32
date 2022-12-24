/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.c
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022Äê7ÔÂ8ÈÕ
 * Description:
*******************************************************************************/
#include "debug.h"
#include "string.h"
#include "usbd_compatibility_hid.h"

__attribute__ ((aligned(4))) uint8_t HIDTxBuffer[DEF_USBD_MAX_PACK_SIZE];         // HID Tx Buffer

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Timer2 1000us initialisation
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_Prescaler =1000;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   TIM2 IRQ handler
 *
 * @return  none
 */
extern void EasyCon_tick(void);
void TIM2_IRQHandler(void)
{
		EasyCon_tick();
    //UART2_TimeOut++;
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}