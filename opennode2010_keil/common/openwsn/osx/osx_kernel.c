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
 * Q: how to port the microkernel to a new MCU?
 * This module uses the others:
 *	- TiCPU: provide global interrupt enable/disable functions
 *	- TiTimerAdapter: scheduler will use one hardware timer to generate periodic
 *		to drive the whole application to run.
 *	- TiUartAdapter: used for debug only. It's not a mandatory part in the kernel.
 * and the following data structures in runtime library (RTL)
 *	- TiVector
 *	- TiRingQueue
 *	- TiNotifier
 *	- TiDispatcher
 *	- TiEventScheduler
 * so if you want to port the microkernel to another MCU with different architecture,
 * you only need to provide the following four HAL objects: TiCPU, TiVIC, TiTimerAdapter
 * and TiUartAdapter
 */

#include "osx_configall.h"
#include <string.h>
#ifdef CONFIG_OSX_DYNAMIC_MEMORY
  #include <stdlib.h>
#endif

#include "osx_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_debugio.h"
#include "../rtl/rtl_dispatcher.h"
#include "osx_kernel.h"
#include "osx_ticker.h"
#include "../hal/hal_event.h"	

#define OSX_STATE_INITIAL		0x00
#define OSX_STATE_RUNNING_BIT   0x01
#define OSX_STATE_SLEEPING_BIT  0x02
#define OSX_STATE_LOOPRUN_BIT   0x03

#ifndef CONFIG_OSX_DYNAMIC_MEMORY
static char m_osxmem[ OSX_SIZE ];
#endif

TiOSX * g_osx = NULL;

/******************************************************************************
 * osx_construct
 * osx_destroy
 * osx_open
 * osx_close
 *****************************************************************************/


#ifdef CONFIG_OSX_DYNAMIC_MEMORY
TiOSX * _osx_create( uint16 quesize, uint16 dpasize )
{
	uint16 size = OSX_HOPESIZE( quesize, dpasize );
	TiOSX * osx = (TiOSX *)malloc( size );
	return _osx_open( osx, size, quesize, dpasize );
}
#endif

#ifdef CONFIG_OSX_DYNAMIC_MEMORY
void _osx_free( TiOSX * osx )
{
	/* stop timer, clear used UART */
	_osx_close( osx );
	free( osx );
}
#endif

TiOSX * _osx_open( void * buf, uint16 size, uint16 quesize, uint16 dpasize )
{
	char * ptr;
	#ifdef CONFIG_OSX_TLSCHE_ENABLE
	char * ptrticker;
	#endif
	
	TiOSX * osx = (TiOSX *)buf;
    uint16 blksize;
	memset( buf, 0x00, size );
	osx->state = OSX_STATE_INITIAL;

	blksize = OSX_QUEUE_HOPESIZE( sizeof(TiEvent), quesize );

	osx->eventqueue = osx_queue_open((char*)buf+sizeof(TiOSX), blksize, sizeof(TiEvent));
	osx->listener = NULL;
	osx->listenowner = NULL;
	
	ptr = (char *)buf + sizeof(TiOSX) + blksize;
	osx->dispatcher = dispa_construct( ptr, DISPA_HOPESIZE(dpasize), dpasize );

	#ifdef CONFIG_OSX_TLSCHE_ENABLE
	ptr += DISPA_HOPESIZE(dpasize);
	ptrticker = ptr + sizeof(TiOsxTimeLineScheduler);	

	osx->ticker = osx_ticker_construct( (void *)ptrticker , sizeof(TiOsxTicker)); 			
	osx->ticker = osx_ticker_open( osx->ticker );						  	
	osx->scheduler = osx_tlsche_open( (TiOsxTimeLineScheduler *)ptr,osx->ticker);	
	
	osx_ticker_setlistener(osx->ticker, osx_ticker_listener, osx->scheduler); 
	
	#endif
	return osx;
}

void _osx_close( TiOSX * osx )
{
    hal_disable_interrupts();
	osx_queue_close( osx->eventqueue );
	dispa_destroy( osx->dispatcher );
	#ifdef CONFIG_OSX_TLSCHE_ENABLE
	osx_tlsche_close( osx->scheduler );
	#endif
	return;
}

/******************************************************************************
 * event related functions 
 * event post, rtpost, futurepost, trigger, and event listerner
 *****************************************************************************/

/* this function will return immediately after push the event into the event
 * queue. this function is usually called inside interrupt service routines.
 * 
 * @attention
 *	this function can also be the event listener of hal layer or other components.
 *  reference: hal/hal/hal_foundation.c
 */
void _osx_post( TiOSX * osx, TiEvent * e )
{
	osx_queue_pushback( osx->eventqueue, e );
}

void _osx_postx( TiOSX * osx, uint8 evtid, TiFunEventHandler handler, void * objectfrom, void * objectto )
{
	TiEvent e;
	e.id = evtid;
	e.handler = handler;
	e.objectfrom = objectfrom;
	e.objectto = objectto;
	osx_queue_pushback( osx->eventqueue, &e );
}

void _osx_rtpost( TiOSX * osx, TiEvent * e, uint16 deadline )
{
	hal_assert(false);
	//osx_queue_pushback( osx->rtqueue, e, deadline );
}

void _osx_futurepost( TiOSX * osx, TiEvent * e, uint16 future )
{
	hal_assert(false);
	//osx_queue_pushback( osx->futqueue, e, future );
}

/* this function will push the event into the event queue and wait it to be
 * finished. this function is usually called outside of the interrupt service
 * routines.
 */
void _osx_trigger(TiOSX * osx, TiEvent * e)
{
	osx_queue_pushback( osx->eventqueue, e );

	while (!osx_queue_empty(osx->eventqueue))
	{
		_osx_evolve( osx, NULL );
	}
}

void _osx_asyninvoke( TiOSX * osx, TiFunEventHandler handler, uint16 evtid,
	void * objectfrom, void * objectto )
{
	TiEvent e;
	e.id = evtid;
	e.handler = handler;
	e.objectfrom = objectfrom;
	e.objectto = objectto;
	osx_queue_pushback( osx->eventqueue, &e );
}

bool _osx_attach( TiOSX * osx, uint8 eid, TiFunEventHandler handler, void * object )
{
	return dispa_attach(osx->dispatcher, eid, handler, object);
}

bool _osx_detach( TiOSX * osx, uint8 eid )
{
	dispa_detach(osx->dispatcher, eid );
	return true;
}

/*******************************************************************************
 * osx_evolve() and osx_execute()
 ******************************************************************************/

/* _osx_evolve()
 * Everytime this function is called, one event is processed. 
 */
void _osx_evolve( void * osxptr, TiEvent * e )
{
	bool pop = false;
    TiOSX * osx = (TiOSX *)osxptr;

	if (e == NULL)
	{
		e = osx_queue_front( osx->eventqueue );
		pop = true;
	}

	if (e != NULL)
	{
		if (e->handler == NULL)
		{	
			hal_assert(e->id != 0);
			dispa_send( osx->dispatcher, e );
		}
		else
		{
			e->handler( e->objectto, e );
		}
	}

	if (pop)
		osx_queue_popfront(osx->eventqueue);	
}

void _osx_execute( TiOSX * osx )
{
	/*  register default handler for dispatching later */

	//dispa_attach( osx->dispatcher, EVENT_SLEEP, _osx_target_sleep );
	//dispa_attach( osx->dispatcher, EVENT_WAKEUP, _osx_target_wakeup );
	//dispa_attach( osx->dispatcher, EVENT_REBOOT, _osx_target_handler );
	//dispa_attach( sche->dispatcher, EVENT_SHUTDOWN, _osx_target_shutdown );

	hal_setlistener( (TiFunEventHandler)_osx_post, osx ); 	//hal_layer can use this listener to post the event into osx

    hal_assert( g_osx != NULL );

	hal_enable_interrupts();
	#ifdef CONFIG_OSX_TLSCHE_ENABLE
	osx_ticker_start(osx->ticker);
	// osx_postx(1,osx_tlsche_evolve,osx->scheduler,osx->scheduler);	 	//way 1 to deal with the osx_tlsche_evolve: 
																			//we should also modify the osx_tlsche.c line 90
	#endif																			
	while (1)
	{
		_osx_evolve( osx, NULL );
		#ifdef CONFIG_OSX_TLSCHE_ENABLE
		osx_tlsche_evolve( osx->scheduler, NULL );	  //way 2 to deal with the osx_tlsche_evolve 
		#endif
	}
	_osx_close( g_osx );
}

/* hardevolve is called by the system timer hardware. The entire calling process 
 * flow is: 
 *
 *   interrupt call          call                       send event to
 * timer => _osx_hardevolve() => event handler or dispatcher => runnable services
 * 
 *                  event
 * runnable services--|---> osx_queue:  
 *   hal services ----|
 *
 * _osx_hardevolve() will read events from osx internal queue. 
 * 
 * @attention
 *	since this function is called by the interrupt service routine, then you should
 * attention the special interrupt manipulations before exiting an interrupt.
 *	you may need to port this function to a new hardware platform. 
 */

void _osx_hardevolve( TiOSX * osx, TiEvent * e )
{
	//hal_atomic_begin();
	// to do:
	
	/*
	everytime this function was called, it will switch the CPU to another
	object's evolution.
	in the standard scheduler mode, there's only one object active at the same time.
	however, you can keep a list of all the active objects and using
	this function to switch among them
	*/
	/* todo
	if already in evolve then set flag and exit
	*/
	_osx_evolve( osx, NULL );
	
	//hal_atomic_end();
}

void _osx_hardexecute( TiOSX * osx )
{
	/*  register default handler for dispatching later */

	//dispa_attach( osx->dispatcher, EVENT_SLEEP, _osx_target_sleep );
	//dispa_attach( osx->dispatcher, EVENT_WAKEUP, _osx_target_wakeup );
	//dispa_attach( osx->dispatcher, EVENT_REBOOT, _osx_target_handler );
	//dispa_attach( sche->dispatcher, EVENT_SHUTDOWN, _osx_target_shutdown );
/* todo
	_osx_ticker_construct( &osx->systimer, sizeof(TiOsxTicker) );
	osx->ticker = _osx_timer_open( &osx->ticker, CONFIG_OSX_TIMER_INTERVAL, _osx_hardevolve, osx );
	hal_assert( osx->ticker != NULL );
	_osx_ticker_start( osx );

	hal_enable_interrupts();
	//hal_setlistener( (TiFunEventHandler)_osx_post, osx );

	// wait for interrupt happen to call osx_hardevolve() and drive the system to run.
	while (1) 
	{
		if (osx_queue_empty(osx->eventqueue))
		{
			_osx_cpu_sleep();
			_osx_cpu_nop();
		}
	};
	_osx_free( g_osx );
    */
}


/******************************************************************************
 * sleep() and wakeup() functions 
 * for lower power management
 *****************************************************************************/

/* this function will place an EVENT_SLEEP event into the queue. this indicates
 * the osx kernel to go into sleep mode after finished processing all previous
 * events.
 */
void _osx_sleep_request( TiOSX * osx )
{
	_osx_postx( osx, EVENT_SLEEP, NULL, osx, osx );
}

/* this function will place an EVENT_WAKEUP event into the queue. this indicates
 * the osx kernel to do some post processings after hardware wakeup request. the
 * hardware wakeup request is usually raised by external hardware interrupts.
 */
void _osx_on_wakeup( TiOSX * osx )
{
	_osx_postx( osx, EVENT_WAKEUP, NULL, osx, osx );
}

void _osx_sleep(TiOSX * osx, uint16 sleep_time)
{
	#ifdef CONFIG_OSX_TLSCHE_ENABLE
	//step 1:reset rtc to alarm
	osx_ticker_stop(osx->ticker);
	osx->ticker = osx_ticker_alarm_open(osx->ticker);
	osx_ticker_setlistener(osx->ticker, NULL, NULL); 
	osx_ticker_start(osx->ticker);
	//step 2:sleep
	osx_setalarm_count(osx->ticker,sleep_time,0);
   	osx_enter_stop_mode();
	//step 3:step_forward the rtc
	osx_tlsche_stepforward(osx->scheduler,sleep_time);
	//step 4:reset alarm to rtc
	osx_ticker_stop(osx->ticker);
	osx->ticker = osx_ticker_open(osx->ticker);
	osx_ticker_setlistener(osx->ticker, osx_ticker_listener, osx->scheduler); 
	osx_ticker_start(osx->ticker);	
	#endif
}

void _osx_wakeup(TiOSX * osx)
{

}

/******************************************************************************
 * osx_init() and osx_execute()
 *****************************************************************************/

void osx_init( void )
{
	// osx_startup();
	
	#ifdef CONFIG_OSX_DYNAMIC_MEMORY
	g_osx = _osx_create( CONFIG_OSX_QUEUE_CAPACITY, CONFIG_OSX_DISPATCHER_CAPACITY );
	#endif

	#ifndef CONFIG_OSX_DYNAMIC_MEMORY
	g_osx = _osx_open( (void*)(&m_osxmem[0]), sizeof(m_osxmem), CONFIG_OSX_QUEUE_CAPACITY, CONFIG_OSX_DISPATCHER_CAPACITY );
	#endif
}

void osx_execute( void )
{
/*
	#ifndef CONFIG_OSX_TIMER_ENABLE
	_osx_execute( g_osx );
	#else
	_osx_hardexecute( g_osx );
	#endif
    */
	_osx_execute( g_osx );
}









