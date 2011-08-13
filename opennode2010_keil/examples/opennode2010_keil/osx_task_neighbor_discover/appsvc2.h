#ifndef _TASK2_H_4687_
#define _TASK2_H_4687_

/* appsvc2.h
 * This is the interface file of Task2. This file also demonstrates how to 
 * develope an interface file.
 */

#include "asv_foundation.h"
#include "../../../common/openwsn/osx/osx_taskpool.h"
#include "../../../common/openwsn/osx/osx_taskheap.h"

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define TEST_ACK_REQUEST

typedef struct{
	uint8 state;
	uint16 interval;
    TiFunEventHandler listener;
    void * lisowner;
}TiAppService2;

TiAppService2 * asv2_open( TiAppService2 * svc, uint16 interval );
void asv2_close( TiAppService2 * svc );
void asv2_setlistener( TiAppService2 * svc, TiFunEventHandler listener, void * lisowner );
void asv2_evolve( void * svcptr, TiOsxTaskHeapItem *item );
void send(void);

#endif
