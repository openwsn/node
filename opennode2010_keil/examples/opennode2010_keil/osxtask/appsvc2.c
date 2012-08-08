
/* appsvc2.c
 * This is the implementation file of TiAppService2. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc2.h"

TiAppService2 g_task2data;

TiAppService2 * asv2_open( TiAppService2 * taskdata, uint16 interval, void *sche )
{
	memset( (void*)taskdata, 0x00, sizeof(TiAppService2) );
	taskdata->state = 0;
	taskdata->interval = interval;
	taskdata->sche=sche;
    return taskdata;
}

void asv2_close( TiAppService2 * svc )
{
	return;
}


void asv2_evolve( TiAppService2 * taskdata, TiEvent * e )
{
	USART_Send(0xA2);
}


