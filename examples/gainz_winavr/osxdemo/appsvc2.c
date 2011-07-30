
/* appsvc2.c
 * This is the implementation file of TiAppService2. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "../../common/openwsn/hal/hal_led.h"
#include"../../common/openwsn/hal/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc2.h"

TiAppService2 * asv2_open( TiAppService2 * svc, uint16 interval )
{
	memset( (void*)svc, 0x00, sizeof(TiAppService2) );
	svc->state = 0;
	svc->listener = NULL;
	svc->lisowner = NULL;
	svc->interval = interval;
    return svc;
}

void asv2_close( TiAppService2 * svc )
{
	return;
}

void asv2_setlistener( TiAppService2 * svc, TiFunEventHandler listener, void * lisowner )
{
	svc->listener = listener;
	svc->lisowner = lisowner;
}

void asv2_evolve( void * svcptr, TiEvent * e )
{
	TiAppService2 * svc = (TiAppService2 *)svcptr;

	switch (svc->state)
	{
	case 0:
		led_on( LED_GREEN );
		svc->state = 1;
		break;
	case 1:
		led_off( LED_GREEN );
		svc->state = 0;
	}

	if (svc->listener != NULL)
	{
		svc->listener( svc->lisowner, e );
	}
}

