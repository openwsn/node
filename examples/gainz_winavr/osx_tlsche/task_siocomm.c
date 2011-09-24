
/* apptaskdata3.c
 * This is the implementation file of TiSioTask. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "task_configall.h"
#include <string.h>
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "task_foundation.h"
#include "task_siocomm.h"

TiSioTask g_task3data;

TiSioTask * sio_task_open( TiSioTask * taskdata, uint16 interval )
{
	memset( (void*)taskdata, 0x00, sizeof(TiSioTask) );
	taskdata->state = 0;
	taskdata->interval = interval;
    return taskdata;
}

void sio_task_close( TiSioTask * taskdata )
{
	return;
}

void sio_task_evolve( TiSioTask * taskdata, TiEvent * e )
{
	//TiSioTask * taskdata = (TiSioTask *)taskdataptr;

    led_on(LED_GREEN);
	hal_delay(700);
	led_off(LED_GREEN);
	/*switch (taskdata->state)
	{
	case 0:
		led_on( LED_YELLOW );
		taskdata->state = 1;
		break;
	case 1:
		led_off( LED_YELLOW );
		taskdata->state = 0;
	}*/
}

