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
#include "osx_kernel.h"
#include "../rtl/rtl_dispatcher.h"


#define OSX_STATE_INITIAL		0x00
#define OSX_STATE_RUNNING_BIT   0x01
#define OSX_STATE_SLEEPING_BIT  0x02
#define OSX_STATE_LOOPRUN_BIT   0x03

#ifndef CONFIG_OSX_DYNAMIC_MEMORY
//volatile char m_osxmem[ OSX_SIZE ];
char m_osxmem[ OSX_SIZE ];
#endif

TiOSX * g_osx;

/******************************************************************************
 * osx_init() and osx_execute()
 *****************************************************************************/

void osx_init( void )
{
	#ifndef CONFIG_OSX_DYNAMIC_MEMORY
	volatile char * buf;
	uintx size;
	#endif

	#ifdef CONFIG_OSX_DYNAMIC_MEMORY
	uintx size;
	g_osx = _osx_create( CONFIG_OSX_QUEUE_CAPACITY, CONFIG_OSX_DISPATCHER_CAPACITY );
	#endif

	#ifndef CONFIG_OSX_DYNAMIC_MEMORY
	buf = &m_osxmem[0];
	g_osx = _osx_construct( (void*)buf, OSX_SIZE, CONFIG_OSX_QUEUE_CAPACITY, CONFIG_OSX_DISPATCHER_CAPACITY );
	#endif

	// assert( g_osx != NULL );

	#ifdef CONFIG_OSX_FIFOSCHE_ENABLE
	buf += OSX_SIZE;
	size = FIFOSCHE_HOPESIZE( CONFIG_OSX_QUEUE_CAPACITY );
	g_osx->sche = fifosche_construct( (void*)buf, size, CONFIG_OSX_QUEUE_CAPACITY, (TiFunEventHandler)dispa_send, g_osx->dispatcher);
	#endif

	g_osx->dba = NULL;
	g_osx->timer = NULL;
}

void osx_execute( void )
{
	#ifndef CONFIG_OSX_TIMER_ENABLE
	// g_osx_timer = timer_construct()
	// g_systimer->setlistener( _osx_hardevolve, g_osx ) 
	// timer_setlistener( ( g_osx->timer, NULL );
	// while (1) {};
	_osx_execute( g_osx );
	#endif

	#ifdef CONFIG_OSX_TIMER_ENABLE
	// g_osx_timer = timer_construct()
	// g_systimer->setlistener( _osx_post ) 
	// while (1) {};
	#endif


	#ifdef CONFIG_OSX_FIFOSCHE_ENABLE
	fifosche_destroy( g_osx->sche );
	#endif

	#ifdef CONFIG_OSX_DYNAMIC_MEMORY
	_osx_free( g_osx );
	#endif

	#ifndef CONFIG_OSX_DYNAMIC_MEMORY
	_osx_destroy( g_osx );
	#endif
}


/******************************************************************************
 * construct() and destroy()
 *****************************************************************************/

#ifdef CONFIG_OSX_DYNAMIC_MEMORY
TiOSX * _osx_create( uintx quesize, uintx dpasize )
{
	uintx size = OSX_HOPESIZE( quesize, dpasize );
	TiOSX * osx = (TiOSX *)malloc( size );
	return _osx_construct( osx, size, quesize, dpasize );
}
#endif

#ifdef CONFIG_OSX_DYNAMIC_MEMORY
void _osx_free( TiOSX * osx )
{
	/* stop timer, clear used UART */
	_osx_destroy( osx );
	free( osx );
}
#endif

TiOSX * _osx_construct( void * buf, uintx size, uintx quesize, uintx dpasize )
{
	char * ptr;
	TiOSX * osx = (TiOSX *)buf;

	memset( buf, 0x00, size );
	osx->state = OSX_STATE_INITIAL;

	ptr = (char *)buf + sizeof(TiOSX);
	osx->dispatcher = dispa_construct( ptr, DISPA_HOPESIZE(dpasize), dpasize );

/*
	len = FIFOSCHE_HOPESIZE( CONFIG_OSX_QUEUE_CAPACITY );
	g_sche = (TiScheduler *)&(m_osx_mem[0]);
	fifosche_construct( g_sche, len, CONFIG_OSX_QUEUE_CAPACITY, (TiFunEventHandler)(dispa_send), g_dispatcher );
	*/
	return osx;
}

void _osx_destroy()
{
	return;
}

void _osx_setscheduler( TiOSX * osx, TiScheduler * sche, TiFunScheEvolve evolve, TiFunSchePost post, 
	TiFunScheRealtimePost rtpost, TiFunScheFuturePost futurepost )
{
	osx->sche = sche;
	osx->evolve = evolve;
	osx->post = post;
	osx->rtpost= rtpost;
	osx->futurepost = futurepost;
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
 */
void _osx_post( TiOSX * osx, TiEvent * e )
{
	osx->post( osx->sche, e );
}

void _osx_postx( TiOSX * osx, uintx evtid, TiFunEventHandler handler, void * objectfrom, void * objectto )
{
	TiEvent e;
	e.id = evtid;
	e.handler = handler;
	e.objectfrom = objectfrom;
	e.objectto = objectto;

	osx->post( osx->sche, &e );
}

void _osx_rtpost( TiOSX * osx, TiEvent * e, uintx deadline )
{
	hal_assert(false);
	osx->rtpost( osx->sche, e, deadline );
}

void _osx_futurepost( TiOSX * osx, TiEvent * e, uintx future )
{
	hal_assert(false);
	osx->futurepost( osx->sche, e, future );
}

/* this function will push the event into the event queue and wait it to be
 * finished. this function is usually called outside of the interrupt service
 * routines.
 */
void _osx_trigger(TiOSX * osx, TiEvent * e)
{
	osx->post( osx->sche, e );

	/* 
	while (!sche_empty(osx->sche))
	{
		sche_evolve( osx->sche, NULL );
	}
	*/
}

bool _osx_attach( TiOSX * osx, uintx eid, TiFunEventHandler handler, void * object )
{
	return dispa_attach(osx->dispatcher, eid, handler, object);
}

bool _osx_detach( TiOSX * osx, uintx eid )
{
	dispa_detach(osx->dispatcher, eid );
	return true;
}

/******************************************************************************
 * evolve() and execute() functions
 * running control
 *****************************************************************************/

void _osx_evolve( TiOSX * osx, TiEvent * e )
{
	osx->evolve( osx->sche, e );
}

void _osx_execute( TiOSX * osx )
{
/*  register default handler for dispatching later

	dispa_attach( sche->dispatcher, EVENT_SLEEP, target_sleep );
	dispa_attach( sche->dispatcher, EVENT_WAKEUP, target_wakeup );
	dispa_attach( sche->dispatcher, EVENT_REBOOT, target_handler );
	dispa_attach( sche->dispatcher, EVENT_SHUTDOWN, target_shutdown );
*/
	hal_enable_interrupts();
	hal_setlistener( (TiFunEventHandler)_osx_post, osx );
	hal_setlistener( osx_post, sysqueue );


	while (1)
	{
		osx->evolve( osx->sche, NULL );
		//if (return false which means scheudler is empty)
		//{
		//	NULL;
		//}
	}
}

/* hardevolve is called by the timer hardware. it's actually a preemble
 * scheduler like the RTOS's scheduler.
 *
 * timer->scheduler->dispatcher->notifier->runnable objects
 * 
 * @attention
 *	since this function is called by the interrupt service routine, then you should
 * attention the special interrupt manipulations before exiting an interrupt.
 *	you may need to port this function to a new hardware platform. 
 */
void _osx_hardevolve( TiOSX * osx, uintx period )
{
	// hal_atomic_begin();
	
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
	osx->evolve( osx->sche, NULL );
	
	// hal_atomic_end();
}

void _osx_hardexec( TiOSX * osx )
{

/*  register default handler for dispatching later

	dispa_attach( sche->dispatcher, EVENT_SLEEP, target_sleep );
	dispa_attach( sche->dispatcher, EVENT_WAKEUP, target_wakeup );
	dispa_attach( sche->dispatcher, EVENT_REBOOT, target_handler );
	dispa_attach( sche->dispatcher, EVENT_SHUTDOWN, target_shutdown );
*/
	hal_setlistener( (TiFunEventHandler)_osx_post, (void*)osx );

	/*
	timer_construct( g_systimer );
	timer_setlistener( osx_hardevolve );
	timer_restart( g_systimer, period );
	*/
	while (1) {};
}



/******************************************************************************
 * sleep() and wakeup() functions 
 * for lower power management
 *****************************************************************************/

/* this function will place an EVENT_SLEEP event into the queue. this indicates
 * the osx kernel to go into sleep mode after finished processing all previous
 * events.
 */
void _osx_sleep( TiOSX * osx )
{
	_osx_postx( osx, EVENT_SLEEP, NULL, osx->sche, osx->sche );
}

/* this function will place an EVENT_WAKEUP event into the queue. this indicates
 * the osx kernel to do some post processings after hardware wakeup request. the
 * hardware wakeup request is usually raised by external hardware interrupts.
 */
void _osx_wakeup( TiOSX * osx )
{
	_osx_postx( osx, EVENT_WAKEUP, NULL, osx->sche, osx->sche );
}

