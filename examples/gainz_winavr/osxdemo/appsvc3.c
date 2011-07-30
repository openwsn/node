
/* appsvc3.c
 * This is the implementation file of TiAppService3. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc3.h"

TiAppService3 * asv3_open( TiAppService3 * svc, uint16 interval )
{
	memset( (void*)svc, 0x00, sizeof(TiAppService3) );
	svc->state = 0;
	svc->interval = interval;
    return svc;
}

void asv3_close( TiAppService3 * svc )
{
	return;
}

void asv3_evolve( void * svcptr, TiEvent * e )
{
	TiAppService3 * svc = (TiAppService3 *)svcptr;


	switch (svc->state)
	{
	case 0:
		led_on( LED_YELLOW );
		svc->state = 1;
		break;
	case 1:
		led_off( LED_YELLOW );
		svc->state = 0;
	}
}

