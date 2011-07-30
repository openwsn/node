/******************************************************************************
 * TiFifoScheduler (ssche)
 * this is a simple fifo scheduler. it supports non-preamble FIFO events processing
 * only.
 *****************************************************************************/

#include "osx_configall.h"
#include <stdlib.h>
#include <string.h>
#include "osx_foundation.h"
#include "../rtl/rtl_lightqueue.h"
#include "osx_fifosche.h"

TiFifoScheduler * fifosche_construct(void * buf, uintx size, uintx que_capacity,
	TiFunEventHandler listener, void * listenowner )
{
	TiFifoScheduler * sche;
	uintx quesize;

	quesize = LIGHTQUEUE_HOPESIZE( sizeof(TiEvent), que_capacity );

	memset( buf, 0x00, size );
	sche = (TiFifoScheduler *)buf;
	sche->eventqueue = lwque_construct((char*)buf+sizeof(TiFifoScheduler), quesize, sizeof(TiEvent));
	sche->listener = listener;
	sche->listenowner = listenowner;

	return sche;
}

void fifosche_destroy(TiFifoScheduler * sche)
{
	return;
}

/* @modified by zhangwei in 200905
 * 	you need go into sleep mode when lwque_empty(sche->eventqueue) returns true.
 * the sleep/wakeup control is management by SLEEP_REQUEST/WAKEUP_REQUEST events.
 */
/*
void fifosche_execute(TiFifoScheduler * sche)
{
}
*/

void fifosche_setlistener( TiFifoScheduler * sche, TiFunEventHandler listener )
{
	sche->listener = listener;
}

/* this function will be called by sche_execute() or by hardware timer ticks */
void fifosche_evolve(TiFifoScheduler * sche, TiEvent * e)
{
	bool pop = false;

	if (e == NULL)
	{
		e = lwque_front(sche->eventqueue);
		pop = true;
	}

	if (e != NULL)
	{
		if (e->handler == NULL)
			sche->listener( sche->listenowner, e );
		else
			e->handler( e->objectto, e );
	}

	if (pop)
		lwque_popfront(sche->eventqueue);
}

void fifosche_post(TiFifoScheduler * sche, TiEvent * e)
{
	lwque_pushback( sche->eventqueue, e );
}

void fifosche_trigger(TiFifoScheduler * sche, TiEvent * e)
{
	lwque_pushback( sche->eventqueue, e );

	while (!lwque_empty(sche->eventqueue))
	{
		e = lwque_front(sche->eventqueue);
		if (e != NULL)
		{
			sche->listener( sche->listenowner, e );
			lwque_popfront(sche->eventqueue);
		}
	}
}

void fifosche_asyninvoke(TiFifoScheduler * sche, TiFunEventHandler handler, uintx evtid,
	void * objectfrom, void * objectto )
{
	TiEvent e;
	e.id = evtid;
	e.handler = handler;
	e.objectfrom = objectfrom;
	e.objectto = objectto;
	fifosche_post(sche, &e);
}

#ifdef FIFOSCHE_TEST
void fifosche_test()
{
	create scheduler
	create notifier
	...
	create app
	app->attach(sche);
	sche_configure
	sche_push(first event to start the application)
	sche_attach( app_evolve );
	sche_execute();

	destroy app
	destroy notifier
	destroy scheduler
}
#endif


