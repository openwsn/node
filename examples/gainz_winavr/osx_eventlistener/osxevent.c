/******************************************************************************
 * osxevent
 * this module tests the events queue in "osx". the timer hardware and UART's receiveing 
 * part will periodically generate events and put them into the queue. attention
 * the generating process is inside the interrupt service routine(ISR), while the 
 * event processing is outside of the ISR.
 *
 * @author by zhangwei on 20070910
 * @modified by zhangwei on 20090704
 *  - revision
 * @modified by zhangwei on 2010.05.30
 *  - compiled successfully. upgraded from Portable WinAVR 2008 to 2009 version.
 *
 *****************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/osx/osx_kernel.h"

TiTimerAdapter		g_timer;
TiUartAdapter		g_uart;

static void _osx_event_listener(void * object, TiEvent * e);

int main(void)
{
	// hal_init( _hal_listener, NULL );
    hal_init( NULL, NULL );
	target_init();

    osx_init();

	// add an event listener to the hal layer. if there's a event raised by the hal 
	// layer, then it will be routed to the osx kernel and drive the kernel to do something.
    // @modified by zhangwei on 2010.05.30
    // - due to the macro problems, we cannot use osx_post() here. we must use _osx_post,
    // through osx_post is only an macro and defined as _osx_post
	//
	hal_setlistener( (TiFunEventHandler)_osx_post, g_osx );

	// set the map between event id and event listener
    // todo: seems error here
	//
	osx_attach( 0, _osx_event_listener, NULL );

	hal_enable_interrupts();
	while (1) {};
	return 0;
}

void _osx_event_listener(void * object, TiEvent * e)
{
	led_toggle( LED_ALL );
}

