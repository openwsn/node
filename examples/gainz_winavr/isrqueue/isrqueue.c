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

/******************************************************************************* 
 * isrqueue
 * This program demonstrates the TiSysQueue between interrupt service routines(ISR)
 * and main program running in non-interrupt mode. The ISR will generate events
 * and push them into the system queue, and the main program will pick them out
 * from the queue and process it. 
 * 
 * @modified by zhangwei on 2010.05.25
 *  - upgraded to WinAVR Portable 2009.03.13 and AVR Studio 4.15.
 ******************************************************************************/

#define CONFIG_DEBUG

#define  CONFIG_AUTO_STOP
#undef  CONFIG_AUTO_STOP

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/osx/osx_queue.h"
#include "../../common/openwsn/rtl/rtl_lightqueue.h"

#define SYSQUE_CAPACITY 32
#define SYSQUEUE_SIZE (LIGHTQUEUE_HOPESIZE(sizeof(TiEvent), SYSQUE_CAPACITY))
#define TIMER_ID 0
#define UART_ID 0

/* g_sysque                  System Queue Object used to holding the events
 * g_timer                   Timer Adapter object. In this demo program, the timer
 *                           will generate events and put them into the system queue
 *                           for later processing.
 */
char m_sysque_buffer[SYSQUEUE_SIZE];
TiTimerAdapter m_timer;
//TiSysQueue * g_sysque;
TiOsxQueue * g_sysque;
TiTimerAdapter * g_timer;
uint16 g_count;

static void asv_evolve( void * objptr, TiEvent * e );
static void timer_expired_listener( void * object, TiEvent * e );

int main(void)
{
    TiEvent * e;

    /* initialize target board */
	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );
	dbo_open( UART_ID, 38400 );

	// g_sysque = sysque_construct( &(m_sysque_buffer[0]), SYSQUEUE_SIZE, sizeof(TiEvent) );
    g_sysque = osx_queue_open( &(m_sysque_buffer[0]), SYSQUEUE_SIZE, sizeof(TiEvent) );
	g_timer = timer_construct( (void *)&m_timer, sizeof(TiTimerAdapter) );
	g_count = 0;
	/* @attention: The timer_setinterval() cannot accept large duration values because
	 * the hardware may not support it */

	// Q: what's the maximum value of timer_setinterval for each hardware timer?
	// A: 1~8

	timer_open( g_timer, TIMER_ID, timer_expired_listener, g_sysque, 0x01 );
	timer_setinterval( g_timer, 5, 1 );
	timer_start( g_timer );

	hal_enable_interrupts();

	
	/* The main program will repeated check the system queue and process the arriving 
	 * events. These events are generated inside the timer expired listener. 
	 * attention in this demo, the event itself has a handler function, so we can 
	 * simply call this handler function to process the event 
	 */
	while (1)
	{	
		// e = (TiEvent *)sysque_front( g_sysque );
        e = (TiEvent *)osx_queue_front( g_sysque );
		if (e != NULL)
		{
            e->handler( NULL, e );
		}
		hal_delay(10);
	}
}

/* Application service evolve function. It functions as the default event processor. */
void asv_evolve( void * objptr, TiEvent * e )
{
	if (e != NULL)
	{
		switch (e->id)
		{
		case 0x99:
			dbo_n8toa( e->id );
			led_toggle( LED_GREEN );
			// sysque_popfront( g_sysque );
            osx_queue_popfront( g_sysque );
			break;
		default:
			dbo_n8toa( 0 );
			led_toggle( LED_YELLOW );
			// sysque_popfront( g_sysque );
            osx_queue_popfront( g_sysque );
			break;
		}
	}
}

/* This is the timer's listener function. Everytime the timer expired, this function
 * will be called. So we can generate the event and put it into the system in this 
 * function.
 */
void timer_expired_listener( void * object, TiEvent * e )
{
	TiEvent newe;

	g_count ++;
	if ((g_count % 15) == 0)
	{
	    led_toggle( LED_RED );
		newe.id = 0x99;
		newe.handler = asv_evolve;
		newe.objectfrom = NULL;
		newe.objectto = NULL;
		//sysque_pushback( (TiSysQueue *)object, (void *)(&newe) );
        osx_queue_pushback( (TiOsxQueue *)object, (void *)(&newe) );
	}
	
	#ifdef CONFIG_AUTO_STOP
	/* If you want to stop the timer in the future, you can uncomment the following 
	 * section of source code.  
	 */
	if (g_count == 151)
    {
        timer_close( &g_timer );
        timer_destroy( &g_timer );
		g_count = 0;
		led_off(LED_RED);
    } 
	#endif
}

