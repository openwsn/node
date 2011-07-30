#ifndef _APPSVC2_H_4687_
#define _APPSVC2_H_4687_

/* appsvc2.h
 * This is the interface file of TiAppService2. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
}TiAppService2;

TiAppService2 * asv2_open( TiAppService2 * svc, uint16 interval );
void asv2_close( TiAppService2 * svc );
void asv2_setlistener( TiAppService2 * svc, TiFunEventHandler listener, void * lisowner );
void asv2_evolve( void * svcptr, TiEvent * e );

#endif
