/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/08/08
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 

/* @Note
 * Compatibility HID Example:
 * This program provides examples of the pass-through of USB-HID data and serial port
 *  data based on compatibility HID device. And the data returned by Get_Report request is
 *  the data sent by the last Set_Report request.Speed of UART1/2 is 115200bps.
 *
 * Interrupt Transfers:
 *   UART2_RX   ---> Endpoint1
 *   Endpoint2  ---> UART2_TX
 *
 *   Note that the first byte is the valid data length and the remaining bytes are
 *   the transmission data for interrupt Transfers.
 *
 * Control Transfers:
 *   Set_Report ---> UART1_TX
 *   Get_Report <--- last Set_Report packet
 *
 *  */
 
#include "debug.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "usbd_compatibility_hid.h"
#include "EasyCon_API.h"
#include "led.h"
#include "uart.h"
#include "ch32f10x_usbfs_device.h"
/* Global define */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	Delay_Init(); 
	USB_Port_Set(DISABLE, DISABLE);
	USBHD_Device_Init( DISABLE );
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  USART_Printf_Init(115200);
  EasyCon_script_init();
	/* USB20 device init */
	UART1_ParaInit(1);
	USBHD_RCC_Init( );
	USBHD_Device_Init( ENABLE );
  
	/* Timer init */
  TIM2_Init();
  ledb_test();
  /* USBD init */
	Set_USBConfig(); 
  USB_Init();	    
 	USB_Interrupts_Config();    

  HIDInit();

	EasyCon_script_start();
	
	while(1)
	{
		// Process local script instructions.
		EasyCon_script_task();
		
		// send report
		HIDTask();
		
		UART1_DataTx_Deal();
	}
}







