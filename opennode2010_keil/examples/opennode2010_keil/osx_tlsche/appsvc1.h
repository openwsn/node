#ifndef _APPSVC1_H_4687_
#define _APPSVC1_H_4687_

/* appsvc1.h
 * This is the interface file of TiAppService1. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
}TiAppService1;

TiAppService1 * asv1_open( TiAppService1 * svc, uint16 interval );
void asv1_close( TiAppService1 * svc );
void asv1_evolve( void * svcptr, TiEvent * e );

#endif
