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

void led_open(uint16 id)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	if (id & LED_RED)
	{
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	}
	if(id & LED_GREEN)
	{
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	}
	if(id & LED_YELLOW)
	{	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	if(id & LED_BLUE)
	{	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	}
    led_off( id );
}

void led_close()
{
    led_off( LED_ALL );
}

void led( uint16 id, bool state )
{
	if (state)
		led_off( id );
	else
		led_on( id );
}

void led_off( uint16 id )
{
	if (id & LED_RED)
	{
		GPIO_SetBits( GPIOA, GPIO_Pin_8);
	}
	if(id & LED_GREEN)
	{
		GPIO_ResetBits( GPIOA, GPIO_Pin_1);
	}
	if(id & LED_YELLOW)
	{	
		GPIO_ResetBits( GPIOA, GPIO_Pin_2);
	}
	if(id & LED_BLUE)
	{	
		GPIO_ResetBits( GPIOA, GPIO_Pin_3);
	}
}

void led_on( uint16 id )
{
	if (id & LED_RED)
	{
		GPIO_ResetBits( GPIOA, GPIO_Pin_8);
	}
	if(id & LED_GREEN)
	{
		GPIO_SetBits( GPIOA, GPIO_Pin_1);
	}
	if(id & LED_YELLOW)
	{	
		GPIO_SetBits( GPIOA, GPIO_Pin_2);
	}
	if(id & LED_BLUE)
	{	
		GPIO_SetBits( GPIOA, GPIO_Pin_3);
	}
}

void led_toggle( uint16 id )
{
	if (id & LED_RED)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));
	}
	if(id & LED_GREEN)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1)));
	}
	if(id & LED_YELLOW)
	{	
		GPIO_WriteBit(GPIOA, GPIO_Pin_2, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2)));
	}
	if(id & LED_BLUE)
	{	
		GPIO_WriteBit(GPIOA, GPIO_Pin_3, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3)));
	}
}

void led_twinkle( uint16 id , uint16 interval, uintx count )
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

void led_showstate( uint16 state )
{
	(state & 0x04) ? led_on(LED1) : led_off(LED1);
	(state & 0x02) ? led_on(LED2) : led_off(LED2);
	(state & 0x01) ? led_on(LED3) : led_off(LED3);
}
