
/* appsvc1.c
 * This is the implementation file of TiAppService1. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc1.h"

TiAppService1 g_task1data;

TiAppService1 * asv1_open( TiAppService1 * taskdata, uint16 interval ,void *sche)
{
	memset( (void*)taskdata, 0x00, sizeof(TiAppService1) );
	taskdata->state = 0;
	taskdata->interval = interval;
	taskdata->sche=sche;
    return taskdata;
}

void asv1_close( TiAppService1 * taskdata )
{
	return;
}


void asv1_evolve( TiAppService1 * taskdata, TiEvent * e )
{  
	 USART_Send(0xA1);
	 osx_tlsche_taskspawn( (TiOsxTimeLineScheduler *)(g_task1data.sche), (TiOsxTask)asv1_evolve, &g_task1data, 3, 0, 0x00 );
	 osx_tlsche_taskspawn( (TiOsxTimeLineScheduler *)(g_task2data.sche), (TiOsxTask)asv2_evolve, &g_task2data, 2, 0, 0x00 );
}
	 



