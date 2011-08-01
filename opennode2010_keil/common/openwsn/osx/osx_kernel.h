#ifndef _OSX_MICROKERNEL_H_7482_
#define _OSX_MICROKERNEL_H_7482_
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
 * osx_kernel
 * This module implements an lightweight OS kernel to help developing embedded 
 * applications. the kernel includes:
 * 
 * - event queue as buffer between hardware abstraction layer("hal") and osx event 
 *   dispatcher 
 * - event scheduler;
 * - event dispatcher;
 * 
 * The other modules such as the event notifier, iofilters are implenmented in 
 * other modules (rtl_notifier and svc_iofilters).
 *
 * @author zhangwei(TongJi University) on 200610
 * @modified by zhangwei on 20090520
 *	- revision
 * @modified by zhangwei on 20091105
 *	- simplified the implementation. The current version supports FIFO schedule only.
 *    the real time schedule and future schedule are not mature. so I remove them
 *    from current version.
 * @modified by zhangwei on 20100529
 *  - revision. upgraded from Portable WinAVR 2008 to 2009 version
 * @modified by openwsn on 2010.12.12
 *	- revision
 ******************************************************************************/

/* modified by zhangwei(openwsn@gmail.com) on 20091106
 *	In the past, the kernel supports changing the scheduler (FIFO, Realtime, and Future)
 * dynamically by switching the scheduler's interface. 
 *	In the current version, we simplified the above mechanism by MACROs. We use 
 * configuration MACROs to decide which schedule algorithm to use. Though we cannot 
 * configure the scheduling algorithm runtime, but the efficiency is high and codesize
 * is small now, 
 * 
 * the default osx configuration is:
 * 		using simple fifo event scheduler instead of real time scheduler and
 *			future event scheduler
 *		enable debug agent
 *      disable osx system timer
 * so the developer should call osx_evolve() or osx_execute() manualy somewhere
 * to drive the kernel to run.
 */
#define CONFIG_OSX_DYNAMIC_MEMORY
#undef  CONFIG_OSX_DYNAMIC_MEMORY

#define CONFIG_OSX_DBA_ENABLE
#define CONFIG_OSX_FIFOSCHE_ENABLE
#undef  CONFIG_OSX_RTSCHE_ENABLE
#undef  CONFIG_OSX_FUTURESCHE_ENABLE
#undef  CONFIG_OSX_TIMER_ENABLE

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../rtl/rtl_dispatcher.h"
#include "osx_ticker.h"
#include "osx_queue.h"

#ifdef CONFIG_OSX_DBA_ENABLE
#include "osx_dba.h"
#endif

#define OSX_HOPESIZE(quesize,dpasize) (sizeof(TiOSX) + OSX_QUEUE_HOPESIZE(sizeof(TiEvent), quesize) + DISPA_HOPESIZE(dpasize))
#define OSX_SIZE (OSX_HOPESIZE(CONFIG_OSX_QUEUE_CAPACITY,CONFIG_OSX_DISPATCHER_CAPACITY))

/*
typedef void (* TiFunScheEvolve)(void * sche, TiEvent * e);
typedef void (* TiFunSchePost)(void * sche, TiEvent * e);
typedef void (* TiFunScheTrigger)(void * sche, TiEvent * e);
typedef void (* TiFunScheRealtimePost)(void * sche, TiEvent * e, uintx deadline);
typedef void (* TiFunScheFuturePost)(void * sche, TiEvent * e, uintx future);
*/

/******************************************************************************
 * Q: how to use kernel in your system?
 * A: the key idea to use this kernel is how to drive the kernel evolution.
 * there're three methods to drive the kernel to run now:
 *   - connect a hardware timer and let the hardware timer call osx_hardevolve()
 *	   periodically. The hardware time is implemented in "hal_foundation.h".
 *   - if you already have an RTOS running, you can call osx_evolve() or
 *     osx_execute() inside a separate thread to drive the kernel evolution.
 *   - if your system havn't RTOS, and you don't want the hardware to drive the kernel,
 *     then you can call osx_evolve() repeatly or call osx_execute() at the end of 
 *     your applicaton.
 *
 * Q: what kind of schedulers will be support by the kernel?
 * A: the kernel supports different schedulers:
 *  - fifosche/ssche: simple scheduler. there's only one FIFO event queue.
 *  - rtsche: realtime simple scheduler. there's two FIFO event queue. one for normal
 *		events and one for urgent events
 *  - sfsche: simple + future scheduler. there's two FIFO event queue. one for normal
 *		events and one for future scheduling. future events should be regarded as
 *		urgent events too. they usually required to be processed in a fixed time.
 *	- rtsfsche: simple + realtime + future. there's three FIFO event queue. one for
 *		normal events, one for urgent events and one for future events.
 * the current version only support "ssche" now.
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8                   state;
	TiOsxQueue *            eventqueue;
	TiFunEventHandler       listener;
	void *                  listenowner;
	TiDispatcher *          dispatcher;
	#ifdef CONFIG_OSX_DBA_ENABLE
	TiDebugAgent *          dba;
	#endif
	#ifdef CONFIG_OSX_TIMER_ENABLE
	TiOsxTicker *            ticker;
	#endif
}TiOSX;

// todo: i think it's better for the g_osx to be volatile 
//extern volatile TiOSX *              g_osx;
extern TiOSX *              g_osx;

#ifdef CONFIG_OSX_DYNAMIC_MEMORY
#define osx_create(quesize,dpasize)         _osx_create(quesize,dpasize)
#define osx_free()                          _osx_free((g_osx))
#endif   

#define osx_open(buf,size,quesize,dpasize)  _osx_open((buf),(size),(quesize),(dpasize))
#define osx_close()                         _osx_close(g_osx);

#define osx_post(e)                         _osx_post((g_osx),(e))
#define osx_postx(eid,handler,objectfrom,objectto) _osx_postx(g_osx,eid,handler,objectfrom,objectto)
#define osx_rtpost(e,deadline)              _osx_rtpost(g_osx,e,deadline)
#define osx_futurepost(e,future)            _osx_futurepost(g_osx,e,future)
#define osx_trigger(e)                      _osx_trigger(g_osx,e)
#define osx_asyninvoke(handler,object,eid)  _osx_postx(eid,handler,NULL,object)

#define osx_attach(eid,handler,object)      _osx_attach(g_osx,eid,handler,object)
#define osx_detach(eid)                     _osx_detach(g_osx,eid)

#define osx_eventlistener(osx,e)            _osx_post(g_osx,e)
#define osx_evolve(e)                       _osx_evolve(g_osx,e)
//#define osx_execute()                       _osx_execute(g_osx)
#define osx_hardevolve(e)                   _osx_hardevolve(g_osx,e)
#define osx_hardexecute()                   _osx_hardexecute(g_osx)
#define osx_sleep()                         _osx_sleep(g_osx)
#define osx_wakeup()                        _osx_wakeup(g_osx)


/******************************************************************************
 * construct() and destroy()
 *
 * osx_construct()
 * initialize kernel memory, including objects init and connections
 *
 * osx_destroy()
 * destroy the kernel.
 *
 * attention you can call osx_construct() and osx_destroy() any time when you want
 * to use the kernel. there's no hardware manipulations inside construct() and
 * destroy().
 *****************************************************************************/

#ifdef CONFIG_OSX_DYNAMIC_MEMORY
TiOSX * _osx_create( uint16 quesize, uint16 dpasize );
void _osx_free( TiOSX * osx );
#endif

TiOSX * _osx_open( void * buf, uint16 size, uint16 quesize, uint16 dpasize );
void _osx_close();


/******************************************************************************
 * osx_post(e)
 * post an event into the default system queue. this funcation can be called
 * in interrupt service routines
 *
 * osx_rtpost(e,deadline)
 * post an event into the default system queue. the event will be processed as
 * early as possible.
 *
 * osx_futurepost(e,future)
 * post an event into the default system queue. the event will be processed at
 * the time specified by the "future" parameter.
 *
 * osx_trigger(e)
 * put an event into the default system queue, and process all the event including
 * the one just put immediately.
 * you should not call this function in interrupt service routines.
 *****************************************************************************/

void _osx_post( TiOSX * osx, TiEvent * e );
void _osx_postx( TiOSX * osx, uint8 evtid, TiFunEventHandler handler, void * objectfrom, void * objectto );
void _osx_rtpost( TiOSX * osx, TiEvent * e, uint16 deadline );
void _osx_futurepost( TiOSX * osx, TiEvent * e, uint16 future );
void _osx_trigger( TiOSX * osx, TiEvent * e );


/* TiFunEventHandler is defined as the following:
 *  typedef void (* TiFunEventHandler)(void * object, TiEvent * e);
 * It's an function pointer type. It's defined in rtl_foundation.h
 */
bool _osx_attach( TiOSX * osx, uint8 eid, TiFunEventHandler handler, void * object );
bool _osx_detach( TiOSX * osx, uint8 eid );


/******************************************************************************
 * there're two methods to start the kernel.
 * - create an infinite deadloop can call osx_evolve() repeatedly.  osx_execute()
 * contains such an infinite loop and you can call it directly. attention this
 * function will never return. so it should be your last call in your main() function.

 * osx_evolve()
 * osx_execute()
 * every time osx_evolve() called, 0 to 1 event is processed.
 * if you what to use the kernel inside another RTOS, then you can use this method
 * and call osx_evolve()/osx_execute() inside a independent thread.
 *
 * - drive the kernel by the periodical interrupts of the hardware timer
 * osx_hardinit() will start the hardware timer
 * osx_hardevolve() is called by the interrupt service routine of the hardware timer.
 *****************************************************************************/

void _osx_evolve( void * osxptr, TiEvent * e );
void _osx_execute( TiOSX * osx );

void _osx_hardevolve( TiOSX * osx, TiEvent * e );
void _osx_hardexecute( TiOSX * osx );


/******************************************************************************
 * power management
 * osx_sleep()
 * will put an system sleep event into the default system queue. by default, the
 * event will trigger the call to target_sleep() function. attention the kernel
 * scheduler is still need to finish processing all other events pending inside
 * the system queue before the sleep request event.
 *
 * osx_wakeup
 * similar to osx_sleep(), this function will place an wakeup request event into
 * the default system queue. this will trigger the call to target_wakeup().
 *****************************************************************************/

void _osx_sleep( TiOSX * osx );
void _osx_wakeup( TiOSX * osx );

/******************************************************************************
 * osx_init()
 * osx_execute()
 *****************************************************************************/

/**
 * Initialize the osx kernel. Generally, you should post the tasks(is also the event
 * handler) or create event-handler mapping after osx_init().
 *
 * initialize osx. after initialization, you can call osx_post() functions and 
 * enjoy the scheduler. 
 *
 */
void osx_init( void );

/**
 * Start the osx kernel. The kernel will wait for incoming event and dispatch the
 * events to their related handlers. 
 * run the osx kernel. this function has an internal infinite loop to check events
 * in the system event queue. it will never return. but it can support sleep()/wakeup()
 * 
 * Q: how to use osx kernel?
 * R: example:
 *
 *   hal_init()  
 *   target_init()
 *   osx_init()
 *   osx_attach( id, handler, ... )
 *   osx_attach( id, handler, ... )
 *   osx_execute()
 * 
 */
void osx_execute( void );

#ifdef __cplusplus
}
#endif

#endif /* _OSX_MICROKERNEL_H_7482_*/

