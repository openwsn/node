#ifndef _TASK1_H_4687_
#define _TASK1_H_4687_

/* appsvc1.h
 * This is the interface file of Task1. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"
#include "../../../common/openwsn/osx/osx_taskpool.h"
#include "../../../common/openwsn/osx/osx_taskheap.h"
#include "../../../common/openwsn/osx/osx_tlsche.h"
#include "appsvc2.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
	void * sche;
}TiAppService1;

extern TiAppService1 g_task1data;

TiAppService1 * asv1_open( TiAppService1 * taskdata, uint16 interval, void *sche);
void asv1_close( TiAppService1 * taskdata );
void asv1_evolve( TiAppService1 * taskdata, TiEvent * e );

#endif
