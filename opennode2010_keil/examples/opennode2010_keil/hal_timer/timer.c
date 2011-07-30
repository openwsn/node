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
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_debugio.h"

static TiTimerAdapter g_timer;
static volatile uint8 g_count = 0;

void timer_query_driven_test( uint8 id );
void timer_interrupt_driven_test( uint8 id );
void on_timer_expired( void * object, TiEvent * e );

int main(void)
{
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
    
	//hal_init( NULL, NULL );

	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open( 38400 );

	// the parameter id can be 0-3 denoting timer hardware 0 to 3
     	timer_interrupt_driven_test( 0 );
    // 	timer_query_driven_test( 2 );
  		while(1){}
}

void timer_query_driven_test( uint8 id )
{
	TiTimerAdapter *timer;
	timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, id, NULL, NULL, 0x00 ); 
	timer_setinterval( timer,5, 1 );  //(timer,interval,repeat)  the range of the interval  1~8
	timer_start( timer );
//next is the function for the query model
	int i;

	while (1)
	{

        i=0;
        while(i<100)
        {
            // todo: maybe mistake here. shall we restart the timer manualy here?
		    if(timer_expired(timer))
		        i++;
		}
		led_toggle( LED_RED );
		led_toggle(LED_GREEN);
	}

  //  timer_close( timer );
}

void timer_interrupt_driven_test( uint8 id )
{ 
	TiTimerAdapter *timer;

	hal_enable_interrupts();
    timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, id, on_timer_expired, NULL, 0x01 ); 
    timer_setinterval( timer, 1, 0 );  //(timer,interval,repeat)  the range of the interval  1~8
	timer_start( timer );
	
    while (1) {};
}

void on_timer_expired( void * object, TiEvent * e )
{   	
    g_count ++;
	if (g_count == 100)
	{
	    led_toggle( LED_RED );
		g_count = 0;
	}
    
}

