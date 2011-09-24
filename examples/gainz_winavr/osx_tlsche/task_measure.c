
/* measure_task1.c
 * This is the implementation file of TiMeasureTask. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "task_configall.h"
#include <string.h>
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/osx/osx_tlsche.h"
#include "task_foundation.h"
#include "task_measure.h"
#include "task_wlscomm.h"
#include "task_siocomm.h"

TiMeasureTask g_task1data;

TiMeasureTask * measure_task_open( TiMeasureTask * taskdata, uint16 interval )
{
	memset( (void*)taskdata, 0x00, sizeof(TiMeasureTask) );
	taskdata->state = 0;
	taskdata->interval = interval;
    return taskdata;
}

void measure_task_close( TiMeasureTask * taskdata )
{
	return;
}

void measure_task_evolve( TiMeasureTask * taskdata, TiEvent * e )
{
	//TiMeasureTask * taskdata = (TiMeasureTask *)svcptr;

	led_on(LED_RED);
    hal_delay(1000);
	led_off(LED_RED);


	osx_tlsche_taskspawn( (TiOsxTimeLineScheduler *)(g_task1data.sche), (TiOsxTask)measure_task_evolve, &g_task1data, 3000, 0, 0x00 );
	osx_tlsche_taskspawn( (TiOsxTimeLineScheduler *)(g_task2data.sche), (TiOsxTask)wio_task_evolve, &g_task2data, 0, 1, 0x00 );

	/*switch (taskdata->state)
	{
	case 0:
		led_on( LED_RED );
		taskdata->state = 1;
		break;
	case 1:
		led_off( LED_RED );
		taskdata->state = 0;
	}*/
}

