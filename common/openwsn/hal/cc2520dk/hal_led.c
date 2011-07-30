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
#include "hpl_led.h"	
#include "hpl_types.h"
#include "hpl_led.h"


static uint8 m_ledstate = 0x00;

void led_open()
{
	m_ledstate = 0x00;
    
	// HAL_MAKE_RED_LED_OUTPUT();
    // HAL_MAKE_YELLOW_LED_OUTPUT();
    // HAL_MAKE_GREEN_LED_OUTPUT();
    // HAL_SET_RED_LED_PIN();
    // HAL_SET_YELLOW_LED_PIN();
    // HAL_SET_GREEN_LED_PIN();
    
	// for ICT GAINS/GAINZ platform
	// set PA0, PA1, and PA2 bit of PORTA as output pin. the initial state of these
	// pins are LEDs on. 

	DDRA |= (_BV(PA0));
	DDRA |= (_BV(PA1));
	DDRA |= (_BV(PA2));  

	// PORTA &= ~(_BV(PA0)); 
	// PORTA &= ~(_BV(PA1)); 
	// PORTA &= ~(_BV(PA2));

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
    /*
	if (id & LED_RED)
	{
     	PORTA|=_BV(PA2);
		m_ledstate |= LED_RED;		
	}
	if (id & LED_GREEN)
	{
     	PORTA|=_BV(PA1);
		m_ledstate |= LED_GREEN;		
	}
	if (id & LED_YELLOW)
	{
        PORTA|=_BV(PA0);
		m_ledstate |= LED_YELLOW;		
	}
    */
    switch (id)
    {
    case 1: HAL_LED_CLR_1(); break;
    case 2: HAL_LED_CLR_2(); break;
    case 3: HAL_LED_CLR_3(); break;
    case 4: HAL_LED_CLR_4(); break;
    default: break;
    }

}

void led_on( uint8 id )
{
    /*
	if (id & LED_RED)
	{
      	PORTA&=~_BV(PA2);
		m_ledstate &= ~LED_RED;		
	}
	if (id & LED_GREEN)
	{
      	PORTA&=~_BV(PA1);
		m_ledstate &= ~LED_GREEN;		
	}
	if (id & LED_YELLOW)
	{
      	PORTA&=~_BV(PA0);
		m_ledstate &= ~LED_YELLOW;		
	}
    */
    switch (id)
    {
    case 1: HAL_LED_SET_1(); break;
    case 2: HAL_LED_SET_2(); break;
    case 3: HAL_LED_SET_3(); break;
    case 4: HAL_LED_SET_4(); break;
    default: break;
    }

}

void led_toggle( uint8 id )
{
    /*
	if (id & LED_RED)
	{
		if (m_ledstate & LED_RED)
		{
      		PORTA&=~_BV(PA2);
			m_ledstate &= ~LED_RED;		
		}
		else{
      		PORTA|=_BV(PA2);
			m_ledstate |= LED_RED;		
		}
	}
	if (id & LED_GREEN)
	{
		if (m_ledstate & LED_GREEN)
		{
      		PORTA&=~_BV(PA1);
			m_ledstate &= ~LED_GREEN;		
		}
		else{
      		PORTA|=_BV(PA1);
			m_ledstate |= LED_GREEN;		
		}
	}
	if (id & LED_YELLOW)
	{
		if (m_ledstate & LED_YELLOW)
		{
      		PORTA&=~_BV(PA0);
			m_ledstate &= ~LED_YELLOW;		
		}
		else{
      		PORTA|=_BV(PA0);
			m_ledstate |= LED_YELLOW;		
		}
	}
    */
    switch (id)
    {
    case 1: HAL_LED_TGL_1(); break;
    case 2: HAL_LED_TGL_2(); break;
    case 3: HAL_LED_TGL_3(); break;
    case 4: HAL_LED_TGL_4(); break;
    default: break;
    }

}

void led_twinkle( uint8 id , uint16 interval, uintx count )
{
    if (count == 0)
        count = ~count;

    led_off( LED_ALL );
	while (count > 0)
	{
		led_toggle( id );
		hal_delay( interval );
        count --;
	}
}

