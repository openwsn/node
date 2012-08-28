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
	void * sche;
}TiAppService2;

extern TiAppService2 g_task2data;

TiAppService2 * asv2_open( TiAppService2 * taskdata, uint16 interval, void *sche );
void asv2_close( TiAppService2 * taskdata );
void asv2_evolve( TiAppService2 * taskdata, TiEvent * e );

#endif
