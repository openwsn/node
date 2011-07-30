
/* appsvc2.c
 * This is the implementation file of TiWioTask. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "task_configall.h"
#include <string.h>
#include "../../common/openwsn/hal/hal_led.h"
#include"../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/osx/osx_tlsche.h"
#include "task_foundation.h"
#include "task_wlscomm.h"
#include "task_siocomm.h"

TiWioTask g_task2data;

TiWioTask * wio_task_open( TiWioTask * taskdata, uint16 interval )
{
	memset( (void*)taskdata, 0x00, sizeof(TiWioTask) );
	taskdata->state = 0;
	taskdata->listener = NULL;
	taskdata->lisowner = NULL;
	taskdata->interval = interval;
    return taskdata;
}

void wio_task_close( TiWioTask * taskdata )
{
	return;
}

void wio_task_setlistener( TiWioTask * taskdata, TiFunEventHandler listener, void * lisowner )
{
	taskdata->listener = listener;
	taskdata->lisowner = lisowner;
}

void wio_task_evolve( TiWioTask * taskdata, TiEvent * e )
{
	//TiWioTask * taskdata = (TiWioTask *)svcptr;
    
	led_on(LED_YELLOW);
	hal_delay(300);
	led_off(LED_YELLOW);


	osx_tlsche_taskspawn( (TiOsxTimeLineScheduler *)(g_task3data.sche), (TiOsxTask)sio_task_evolve, &g_task3data, 0, 1, 0x00 );


	/*switch (taskdata->state)
	{
	case 0:
		led_on( LED_GREEN );
		taskdata->state = 1;
		break;
	case 1:
		led_off( LED_GREEN );
		taskdata->state = 0;
	}

	if (taskdata->listener != NULL)
	{
		taskdata->listener( taskdata->lisowner, e );
	}*/
}

