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
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 *
 * @author zhangwei(TongJi University) on 20070423
 * 	- revision today.
 * @modified by yan-shixing(TongJi University) on 20091105
 *  - revision. 
 * @modified by Zhang Wei(TongJi University) in 2011.04
 *  - Correct bugs during initialization. The timer_open() call is missed since 
 *    last revision. 
 ******************************************************************************/ 

#include "../../common/openwsn/svc/svc_configall.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_timer.h"

#define VTM_RESOLUTION 5

static TiTimerAdapter g_timeradapter;
static TiTimerManager g_vtm;

void vti_listener1( void * vtmptr, TiEvent * e );
void vti_listener2( void * vtmptr, TiEvent * e );

int main(void)
{
	char * msg = "welcome to csma sendnode...";
	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti1;
	TiTimer * vti2;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );

	timeradapter = timer_construct( (void *)(&g_timeradapter), sizeof(g_timeradapter) );
	vtm = vtm_construct( (void*)&g_vtm, sizeof(g_vtm) );

	// We need timer_open() call to set the timer id. The vtm_open() will still 
	// call timer_open() inside to set vtm_inputevent() callback listener.
	
	//timeradapter = timer_open( timeradapter, 0, vtm_inputevent, NULL, 0x00 );
	timeradapter = timer_open( timeradapter, 0, NULL, NULL, 0x00 );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );

	// vti_setinterval( vti, interval, repeat )
	
	vti1 = vtm_apply( vtm );
	hal_assert( vti1 != NULL );
	vti_open( vti1, vti_listener1, vti1 );
	vti_setscale( vti1, 1 );
	vti_setinterval( vti1, 1000, 0x01 );
	vti_start( vti1 );

	vti2 = vtm_apply( vtm );
	hal_assert( vti2 != NULL );
	vti_open( vti2, vti_listener2, vti2 );
	vti_setscale( vti2, 1 );
	vti_setinterval( vti2, 500, 0x01 );
	vti_start( vti2 );

	hal_enable_interrupts();
	while(1){}

	return 0;
}

/* This function is called by the virtual timer(vti) object */
void vti_listener1( void * vtmptr, TiEvent * e )
{
	led_toggle( LED_RED );
}

/* This function is called by the virtual timer(vti) object */
void vti_listener2( void * vtmptr, TiEvent * e )
{
	led_toggle( LED_YELLOW );
}


