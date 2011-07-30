#ifndef _FIFOSCHEDULER_H_8245_
#define _FIFOSCHEDULER_H_8245_

/******************************************************************************
 * TiFifoScheduler (ssche)
 * this is a simple fifo(first in first out) scheduler. it supports non-preamble
 * FIFO events processing.
 *****************************************************************************/

#include "osx_configall.h"
#include "../rtl/rtl_lightqueue.h"
#include "osx_foundation.h"

#define FIFOSCHE_HOPESIZE(lwque_capacity) sizeof(TiFifoScheduler) \
	+LIGHTQUEUE_HOPESIZE(sizeof(TiEvent), lwque_capacity)

#ifdef __cplusplus
extern "C" {
#endif

/* TiFifoScheduler
 * 	eventqueue: the light queue data structure containing pending events
 * 	defaulthandler: event processing handler
 * 	defaulthdlowner: default handler's owner object.
 * {defaulthdlowner, defaulthandler} determins the object and its handler uniquely.
 */
typedef struct{ 
	TiLightQueue * eventqueue;
	TiFunEventHandler listener;
	void * listenowner;
}TiFifoScheduler;

TiFifoScheduler * fifosche_construct(void * buf, uintx size, uintx que_capacity, TiFunEventHandler handler, void * listener_owner );
void fifosche_destroy(TiFifoScheduler * sche);
//void fifosche_execute(TiFifoScheduler * sche);
void fifosche_setlistener( TiFifoScheduler * sche, TiFunEventHandler handler );
void fifosche_evolve(TiFifoScheduler * sche, TiEvent * e);

void fifosche_post(TiFifoScheduler * sche, TiEvent * e);
void fifosche_trigger(TiFifoScheduler * sche, TiEvent * e);
void fifosche_asyninvoke(TiFifoScheduler * sche, TiFunEventHandler handler, uintx evtid,
	void * objectfrom, void * objectto );

#ifdef FIFOSCHE_TEST
void fifosche_test()
#endif


#ifdef __cplusplus
}
#endif

#endif
