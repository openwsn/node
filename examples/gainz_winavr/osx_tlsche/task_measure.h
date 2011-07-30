#ifndef _APPSVC1_H_4687_
#define _APPSVC1_H_4687_

/* appsvc1.h
 * This is the interface file of TiMeasureTask. This file also demonstrates how to 
 * develope an interface file.
 */

#include "task_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
	void * sche;
}TiMeasureTask;


extern TiMeasureTask g_task1data;

TiMeasureTask * measure_task_open( TiMeasureTask * taskdata, uint16 interval );
void measure_task_close( TiMeasureTask * taskdata );
void measure_task_evolve( TiMeasureTask * taskdata, TiEvent * e );

#endif
