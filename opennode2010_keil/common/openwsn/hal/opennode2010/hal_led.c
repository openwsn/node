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

#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_targetboard.h"
#include "../hal_cpu.h"
#include "../hal_led.h"
#include "../hal_mcu.h"

static uint8 m_ledstate = 0x00;

void led_open()
{
	m_ledstate = 0x00;

	// @attention: You should avoid conflictions if you call the following PORTA initialization 
	// multiple times.
	//  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    led_off( LED_ALL );
}

void led_close()
{
    led_off( LED_ALL );
}

void led( uint8 id, bool state )
{
	if (state)
		led_off( id );
	else
		led_on( id );
}

void led_off( uint8 id )
{
	if (id & LED_RED)
	{
		GPIO_SetBits( GPIOA, GPIO_Pin_8);
	}

//	if (id & LED_RED)
//	{
//     	PORTA|=_BV(PA2);
//		m_ledstate |= LED_RED;		
//	}
//	if (id & LED_GREEN)
//	{
//     	PORTA|=_BV(PA1);
//		m_ledstate |= LED_GREEN;		
//	}
//	if (id & LED_YELLOW)
//	{
//        PORTA|=_BV(PA0);
//		m_ledstate |= LED_YELLOW;		
//	}
}

void led_on( uint8 id )
{
	if (id & LED_RED)
	{
		GPIO_ResetBits( GPIOA, GPIO_Pin_8);
	}
//	if (id & LED_RED)
//	{
//      	PORTA&=~_BV(PA2);
//		m_ledstate &= ~LED_RED;		
//	}
//	if (id & LED_GREEN)
//	{
//      	PORTA&=~_BV(PA1);
//		m_ledstate &= ~LED_GREEN;		
//	}
//	if (id & LED_YELLOW)
//	{
//      	PORTA&=~_BV(PA0);
//		m_ledstate &= ~LED_YELLOW;		
//	}
}

void led_toggle( uint8 id )
{
	if (id & LED_RED)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));
	}
//	if (id & LED_GREEN)
//	{
//		if (m_ledstate & LED_GREEN)
//		{
//      		PORTA&=~_BV(PA1);
//			m_ledstate &= ~LED_GREEN;		
//		}
//		else{
//      		PORTA|=_BV(PA1);
//			m_ledstate |= LED_GREEN;		
//		}
//	}
//	if (id & LED_YELLOW)
//	{
//		if (m_ledstate & LED_YELLOW)
//		{
//      		PORTA&=~_BV(PA0);
//			m_ledstate &= ~LED_YELLOW;		
//		}
//		else{
//      		PORTA|=_BV(PA0);
//			m_ledstate |= LED_YELLOW;		
//		}
//	}
}

void led_twinkle( uint8 id , uint16 interval, uintx count )
{
    if (count == 0)
        count = ~count;

    led_off( LED_ALL );
	while (count > 0)
	{
		led_toggle( id );
//		hal_delay( interval );
        count --;
	}
}

void led_showstate( uint8 state )
{
	(state & 0x04) ? led_on(LED1) : led_off(LED1);
	(state & 0x02) ? led_on(LED2) : led_off(LED2);
	(state & 0x01) ? led_on(LED3) : led_off(LED3);
}
