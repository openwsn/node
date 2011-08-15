/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

/* 
 * @attention
 *	- you should call hal_init() before calling some functions in this module. 
 *  - you may encounter the following message when build the whole application:
 *          undefined reference to 'm_int2handler'
 *    this is due to you haven't add hal_foundation.c into your project. the above
 *    variable is declared in hal_foundation.c
 */

#include "../hal_configall.h" 
#include "../hal_foundation.h" 
#include "../hal_interrupt.h"
#include "../hal_cpu.h"
#include "../hal_led.h"
#include "../hal_assert.h"

#ifdef CONFIG_TARGETBOARD_OPENNODE2010
// #include "./cm3/device/stm32f10x/stm32f10x.h"
#endif

#ifdef CONFIG_TARGETBOARD_GAINZ
#include <avr/interrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * interrupt number - object's handler mapping table  (abbr. as iht)
 * this table records the relationships between interrupt number and related object's
 * event handler. it's usually used to find the related object by interrupt number
 * only in interrupt service routines (isr).
 *
 * 	{interrupt number, objects handler, object}
 *
 *****************************************************************************/

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_attachhandler( uint8 num, TiFunEventHandler handler, void * owner )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	m_int2handler[idx].num = num;
	m_int2handler[idx].handler = handler;
	m_int2handler[idx].owner = owner;

	return idx;
}
#endif

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_detachhandler( uint8 num )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	m_int2handler[idx].num = 0;
	m_int2handler[idx].handler = NULL;
	m_int2handler[idx].owner = NULL;

	return idx;
}
#endif

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** powner )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	*phandler = m_int2handler[idx].handler;
	*powner = m_int2handler[idx].owner;

	return (m_int2handler[idx].handler == NULL) ? -1 : idx;
}
#endif

/* transfer the call to an interrupt as the call to an object handler. this function 
 * is usually called inside an interrupt routine 
 */
inline void hal_invokehandler( uint8 num, TiEvent * e )
{
	_TiIntHandlerItem * item;
	item = &(m_int2handler[num % CONFIG_INT2HANDLER_CAPACITY]);
	if (item->handler != NULL)
	{
		item->handler( item->owner, e );
	}
}

/******************************************************************************
 * default interrupt handlers
 * this section provides an default implementation of interrupt service routines. 
 * these routines help to call the object handler when combined with interrupt 
 * handler table (iht) 
 *****************************************************************************/

/**
 * Q: where can I find the interrupt macros and number defined in WinAVR?
 * R: For STM32F10x firmware library and CM3, please search:
 *      %cm3%\device\stm32f10x\stm32f10x.h
 *      %cm3%\device\stm32f10x\startup_stm32f10x_hd.s
 * In earlier version firmware library, there's a stm32f10x_it.c. 
 * 
 * The current versioin startup file recognize the following interrupt service routines
 * (You can find them in startup_stm32f10x_hd.s):
 */

/*
WWDG_IRQHandler
PVD_IRQHandler
TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
ADC1_2_IRQHandler
USB_HP_CAN1_TX_IRQHandler
USB_LP_CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
USBWakeUp_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
ADC3_IRQHandler
FSMC_IRQHandler
SDIO_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_IRQHandler
TIM7_IRQHandler
DMA2_Channel1_IRQHandler
DMA2_Channel2_IRQHandler
DMA2_Channel3_IRQHandler
DMA2_Channel4_5_IRQHandler
*/
  
/*
void WWDG_IRQHandler(void)
{
	hal_invokehandler( INTNUM_WATCHDOG, NULL );
}

void PVD_IRQHandler(void)
{
}

void TAMPER_IRQHandler(void)
{
}
*/
void RTC_IRQHandler(void)
{
	hal_invokehandler( INTNUM_RTC, NULL );
}
/*
void FLASH_IRQHandler(void)
{
}
*/
// RCC_IRQHandler
void EXTI0_IRQHandler(void)
{
	hal_invokehandler( INTNUM_FRAME_ACCEPTED, NULL );
}
/*
void EXTI1_IRQHandler(void)
{
}

void EXTI2_IRQHandler(void)
{
}

void EXTI3_IRQHandler(void)
{
}

void EXTI4_IRQHandler(void)
{
}

// DMA1_Channel1_IRQHandler
// DMA1_Channel2_IRQHandler
// DMA1_Channel3_IRQHandler
// DMA1_Channel4_IRQHandler
// DMA1_Channel5_IRQHandler
// DMA1_Channel6_IRQHandler
// DMA1_Channel7_IRQHandler
// ADC1_2_IRQHandler

void USB_HP_CAN1_TX_IRQHandler(void)
{
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
}

void CAN1_RX1_IRQHandler(void)
{
}

void CAN1_SCE_IRQHandler(void)
{
}

// EXTI9_5_IRQHandler
// TIM1_BRK_IRQHandler
// TIM1_UP_IRQHandler
// TIM1_TRG_COM_IRQHandler
// TIM1_CC_IRQHandler
*/
void TIM2_IRQHandler(void)
{
	hal_invokehandler( INTNUM_TIMER2, NULL );
}

void TIM3_IRQHandler(void)
{
	hal_invokehandler( INTNUM_TIMER3, NULL );
}

void TIM4_IRQHandler(void)
{
	hal_invokehandler( INTNUM_TIMER4, NULL );
}

// I2C1_EV_IRQHandler
// I2C1_ER_IRQHandler
// I2C2_EV_IRQHandler
// I2C2_ER_IRQHandler
// SPI1_IRQHandler
// SPI2_IRQHandler
// USART1_IRQHandler

void USART2_IRQHandler(void)
{
	hal_invokehandler( INTNUM_USART, NULL );
}

// USART3_IRQHandler
// EXTI15_10_IRQHandler

void RTCAlarm_IRQHandler(void)
{
	hal_invokehandler( INTNUM_RTCALARM, NULL );
}

// USBWakeUp_IRQHandler
// TIM8_BRK_IRQHandler
// TIM8_UP_IRQHandler
// TIM8_TRG_COM_IRQHandler
// TIM8_CC_IRQHandler
// ADC3_IRQHandler
// FSMC_IRQHandler
// SDIO_IRQHandler
// void TIM5_IRQHandler(void)
// SPI3_IRQHandler
// UART4_IRQHandler
// UART5_IRQHandler
// TIM6_IRQHandler
// TIM7_IRQHandler
// DMA2_Channel1_IRQHandler
// DMA2_Channel2_IRQHandler
// DMA2_Channel3_IRQHandler
// DMA2_Channel4_5_IRQHandler

#ifdef __cplusplus
}
#endif
