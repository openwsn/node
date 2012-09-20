#ifndef _APPSVC3_H_4687_
#define _APPSVC3_H_4687_

/* appsvc3.h
 * This is the interface file of TiAppService3. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
}TiAppService3;

#ifdef __cplusplus
extern "C" {
#endif

TiAppService3 * asv3_open( TiAppService3 * svc, uint16 interval );
void asv3_close( TiAppService3 * svc );
void asv3_evolve( void * svcptr, TiEvent * e );

#ifdef __cplusplus
}
#endif

#endif
