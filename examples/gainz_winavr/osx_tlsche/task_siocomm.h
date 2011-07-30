#ifndef _TASK_SIOCOMM_H_6778_
#define _TASK_SIOCOMM_H_6778_

/* task for serial communication
 * This is the interface file of TiSioTask. This file also demonstrates how to 
 * develope an interface file.
 */

#include "task_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
	void * sche;
}TiSioTask;


extern TiSioTask g_task3data;

#ifdef __cplusplus
extern "C" {
#endif

TiSioTask * sio_task_open( TiSioTask * taskdata, uint16 interval );
void sio_task_close( TiSioTask * taskdata );
void sio_task_evolve( TiSioTask * taskdata, TiEvent * e );

#ifdef __cplusplus
}
#endif

#endif
