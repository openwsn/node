#ifndef _TASK1_H_4687_
#define _TASK1_H_4687_

/* appsvc1.h
 * This is the interface file of Task1. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"
#include "openwsn/hal/hal_luminance.h"
#include "openwsn/osx/osx_taskpool.h"
#include "openwsn/osx/osx_taskheap.h"
#include "appsvc2.h"

extern TiOsxTaskHeap * heap;

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
}TiAppService1;

TiAppService1 * asv1_open( TiAppService1 * svc, uint16 interval );
void asv1_close( TiAppService1 * svc );
void asv1_evolve( void * svcptr, TiOsxTaskHeapItem *item );
uint16 getlightvalue(void);
void createtask(int8 id,TiOsxTaskHeap *heap,int16 timeline);

#endif
