#ifndef _APPSVC2_H_4687_
#define _APPSVC2_H_4687_

/* appsvc2.h
 * This is the interface file of TiWioTask. This file also demonstrates how to 
 * develope an interface file.
 */

#include "task_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
	void * sche;
}TiWioTask;

extern TiWioTask g_task2data;

TiWioTask * wio_task_open( TiWioTask * taskdata, uint16 interval );
void wio_task_close( TiWioTask * taskdata );
void wio_task_setlistener( TiWioTask * taskdata, TiFunEventHandler listener, void * lisowner );
void wio_task_evolve( TiWioTask * taskdata, TiEvent * e );

#endif
