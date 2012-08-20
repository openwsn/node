
/* appsvc1.c
 * This is the implementation file of TiAppService1. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc1.h"
#include "../../../common/openwsn/osx/osx_kernel_new.h"

TiAppService1 * asv1_open( TiAppService1 * svc, uint16 interval )
{
	memset( (void*)svc, 0x00, sizeof(TiAppService1) );
	svc->state = 0;
	svc->interval = interval;
    return svc;
}

void asv1_close( TiAppService1 * svc )
{
	return;
}

void asv1_evolve( void * svcptr, TiEvent * e )
{
	TiAppService1 * svc = (TiAppService1 *)svcptr;

//	switch (svc->state)
//	{
//	case 0:
//		led_on( LED_RED );
//		svc->state = 1;
//		break;
//	case 1:
//		led_off( LED_RED );
//		svc->state = 0;
//	}
	dbc_putchar(0xa1);

	//osx_postx(1,asv1_evolve,svc,svc);	//JOE
	osx_taskspawn(asv1_evolve, svc, 1, 0, 0 );
	led_toggle(LED_RED);
}

