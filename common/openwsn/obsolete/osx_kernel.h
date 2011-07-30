#ifndef _OSX_MICROKERNEL_H_7482_
#define _OSX_MICROKERNEL_H_7482_
/******************************************************************************
 * osx_microkernel
 * this module implement an lightweight OS kernel to help developing embedded 
 * applications. the kernel may includes:
 *
 * 	TiEvent (event or evt, defined in "hal_foundation.h")
 * 	TiEventHandler (evh or handler, defined in "hal_foundation.h")
 *	TiDispatcher (dpa, implemented in "rtl_dispatcher")
 *	TiEventScheduler (sche, may support different schedulers)
 *	TiEventQueue (que, implemented in "rtl_lightqueue")
 * 	TiNotifier (notif) (not used now)
 *	TiIoFilters (iof or filters or flt)
 *
 * @author zhangwei on 200610
 * @modified by zhangwei on 20090520
 *	- revision
 *****************************************************************************/

/* the default osx configuration is:
 * 		using simple fifo event scheduler instead of real time scheduler and
 *			future event scheduler
 *		enable debug agent
 *      disable osx system timer
 * so the developer should call osx_evolve() or osx_execute() manualy somewhere
 * to drive the kernel to run.
 */
#define CONFIG_OSX_DBA_ENABLE
#define CONFIG_OSX_FIFOSCHE_ENABLE
#undef  CONFIG_OSX_RTSCHE_ENABLE
#undef  CONFIG_OSX_FUTURESCHE_ENABLE
#define CONFIG_OSX_TIMER_ENABLE

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../rtl/rtl_dispatcher.h"

#ifdef CONFIG_OSX_TIMER_ENABLE
#include "../hal/hal_timer.h"
#endif

#ifdef CONFIG_OSX_FIFOSCHE_ENABLE
#include "osx_fifosche.h"
#endif

#ifdef CONFIG_OSX_DBA_ENABLE
#include "osx_dba.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Q: how to use kernel in your system?
 * A: the key idea to use this kernel is how to drive the kernel evolution.
 * there're three methods to drive the kernel to run now:
 *   - connect a hardware timer and let the hardware timer call osx_hardevolve()
 *	   periodically
 *   - if you already have an RTOS running, you can call osx_evolve() or
 *     osx_execute() inside a separate thread to drive the kernel evolution.
 *   - if your system havn't RTOS, and you don't want the hardware to drive the kernel,
 *     then you can call osx_evolve() repeatly or call osx_execute() in your own
 *     applicaton.
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

#ifdef CONFIG_OSX_FIFOSCHE_ENABLE
  #define TiScheduler TiFifoScheduler
  #define OSX_HOPESIZE(quesize,dpasize) (FIFOSCHE_HOPESIZE(quesize) + DISPA_HOPESIZE(dpasize))
  #define OSX_SIZE (OSX_HOPESIZE(CONFIG_OSX_QUEUE_CAPACITY,CONFIG_OSX_DISPATCHER_CAPACITY))
#endif

#ifdef CONFIG_OSX_RTSCHE_ENABLE
  #define TiScheduler Undefined
  #define OSX_HOPESIZE(quesize,dpasize) 0
  #define OSX_SIZE 0
#endif

#ifdef CONFIG_OSX_FUTURESCHE_ENABLE
  #define TiScheduler Undefined
  #define OSX_HOPESIZE(quesize,dpasize) 0
  #define OSX_SIZE 0
#endif

typedef void (* TiFunScheEvolve)(void * sche, TiEvent * e);
typedef void (* TiFunSchePost)(void * sche, TiEvent * e);
typedef void (* TiFunScheTrigger)(void * sche, TiEvent * e);
typedef void (* TiFunScheRealtimePost)(void * sche, TiEvent * e, uintx deadline);
typedef void (* TiFunScheFuturePost)(void * sche, TiEvent * e, uintx future);

typedef struct{
	uint8               state;
	TiScheduler *		sche;
	TiDispatcher *      dispatcher;

	#ifdef CONFIG_OSX_DBA_ENABLE
	TiDebugAgent *      dba;
	#endif

	#ifdef CONFIG_OSX_TIMER_ENABLE
	TiTimerAdapter *    timer;
	#endif

	TiFunScheEvolve     evolve;
	TiFunSchePost       post;
	TiFunScheTrigger    trigger;
	TiFunScheRealtimePost rtpost;
	TiFunScheFuturePost futurepost;
}TiOSX;

extern TiOSX *          g_osx;


#define osx_create(quesize,dpasize)         _osx_create(quesize,dpasize)
#define osx_free(osx)                       _osx_free((g_osx))

#define osx_construct(buf,size,quesize,dpasize) _osx_construct((buf),(size),(quesize),(dpasize))
#define osx_destroy()                       _osx_destroy(g_osx);
#define osx_setscheduler(sche,evolve,post,rtpost,futurepost) _osx_setscheduler(g_osx,sche,evolve,post,rtpost,futurepost)

#define osx_post(e)                         _osx_post(g_osx,e)
#define osx_postx(eid,handler,objectfrom,objectto) _osx_postx(g_osx,eid,handler,objectfrom,objectto)
#define osx_rtpost(e,deadline)              _osx_rtpost(g_osx,e,deadline)
#define osx_futurepost(e,future)            _osx_futurepost(g_osx,e,future)
#define osx_trigger(e)                      _osx_trigger(g_osx,e)
#define osx_asyninvoke(handler,object,eid)  _osx_postx(eid,handler,NULL,object)
#define osx_eventlistener(osx,e)			_osx_post(g_osx,e)

#define osx_attach(eid,handler,object)      _osx_attach(g_osx,eid,handler,object)
#define osx_detach(eid)                     _osx_detach(g_osx,eid)

#define osx_evolve(e)                       _osx_evolve(g_osx,e)
#define osx_hardevolve(e)                   _osx_hardevolve(g_osx,e)
#define osx_sleep()                         _osx_sleep(g_osx)
#define osx_wakeup()                        _osx_wakeup(g_osx)


/******************************************************************************
 * osx_init()
 * initialize osx. after initialization, you can call osx_post() functions and 
 * enjoy the scheduler. 
 *
 * osx_execute()
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
 *****************************************************************************/

void osx_init( void );
void osx_execute( void );


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

TiOSX * _osx_create( uintx quesize, uintx dpasize );
void _osx_free( TiOSX * osx );
TiOSX * _osx_construct( void * buf, uintx size, uintx quesize, uintx dpasize );
void _osx_destroy();
void _osx_setscheduler( TiOSX * osx, TiScheduler * sche, TiFunScheEvolve evolve, TiFunSchePost post, 
	TiFunScheRealtimePost rtpost, TiFunScheFuturePost futurepost );



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
void _osx_postx( TiOSX * osx, uintx evtid, TiFunEventHandler handler, void * objectfrom, void * objectto );
void _osx_rtpost( TiOSX * osx, TiEvent * e, uintx deadline );
void _osx_futurepost( TiOSX * osx, TiEvent * e, uintx future );
void _osx_trigger( TiOSX * osx, TiEvent * e );

bool _osx_attach( TiOSX * osx, uintx eid, TiFunEventHandler handler, void * object );
bool _osx_detach( TiOSX * osx, uintx eid );


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

void _osx_evolve( TiOSX * osx, TiEvent * e );
void _osx_execute( TiOSX * osx );

void _osx_hardevolve( TiOSX * osx, uintx period );
void _osx_hardexec( TiOSX * osx );


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


#ifdef __cplusplus
}
#endif

#endif /* OSX_MICROKERNEL_H_*/

