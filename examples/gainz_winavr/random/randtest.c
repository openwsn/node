/*******************************************************************************
 * randtest
 * test random functions
 *
 * @status
 *	tested ok.
 *
 * @author sunqiang(TongJi University), zhangwei(TongJi University) on 20091031
 *
 * @history
 * @modified by Sun Qiang(TongJi University) on 20091110
 * -revision
 * @modified by Sun Qiang(Tongji University) on 20091112
 *
 ******************************************************************************/

#include "../common/configall.h"
#include <stdlib.h>
#include <string.h>
#include "../common/foundation.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_debugio.h"
#include "../common/rtl/rtl_random.h"
#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_target.h"
#include <string.h>

int main(void)
{
	uint16 seed=0x3212;
	int i;
	uint16 n;
	
	target_init();

    dbo_open( 38400 );
	led_open();
	led_on( LED_RED );
	hal_delay( 1000 );
	led_off( LED_ALL );

	//timer_construct( (void *)(&tm), sizeof(tm) );
	//timer = timer_open( &g_timer, );

	rand_open( seed );
	for (i=0; i<100; i++)
	{
		n = rand_read();
		dbo_uint16( n );	
	}
	rand_close();

	return 0;
}

