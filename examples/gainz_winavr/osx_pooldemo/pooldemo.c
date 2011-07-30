#include "../../common/openwsn/osx/osx_configall.h"
#include <string.h>
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/osx/osx_taskpool.h"
#include "../../common/openwsn/osx/osx_taskheap.h"

TiOsxTaskPool m_taskpool;

void _osx_taskpool_dump( TiOsxTaskPool * tpl );

/*******************************************************/

int main()
{
   	int8 idx=-1;
	
	TiOsxTaskPool * tpl;

    char * msg = "welcome to pooldemo...\r\n";

	target_init();

    led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL ); 

	dbo_open( 0, 38400 );
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );

	tpl = osx_taskpool_construct( (void *)&m_taskpool, sizeof(TiOsxTaskPool) );

	dbc_write( msg, strlen(msg) );
	dbc_putchar( 0xF0 );


	for(int m=0; m<osx_taskpool_capacity(tpl); m++)
	{
		idx=osx_taskpool_apply( tpl );
		if(idx >= 0)
		{

	        dbo_putchar(0xF1);  // indicate apply success
			dbo_putchar(tpl->count);
			dbo_putchar(tpl->items[m].taskid);
			dbo_putchar(tpl->emptylist);
			dbo_putchar(idx);
			idx=-1;
		}
		else
			dbo_putchar(0xF2); // indicate apply failure
	}

	//osx_taskpool_destroy( tpl );
	dbo_putchar(0xF5);
	//dbo_putchar(tpl->count);
	_osx_taskpool_dump(tpl);

	while(1)
	{}

}

void _osx_taskpool_dump( TiOsxTaskPool * tpl )
{
	putchar( '\r' );
	putchar( '\n' );

	
	for(int m=0; m<osx_taskpool_capacity(tpl); m++)
	{
	    TiOsxTaskPoolItem * item;
		item = osx_taskpool_gettaskdesc( tpl, m );

		dbc_putchar( 0xF0 );		
		dbc_putchar( 0xF0 );	
			
		// dbc_string( "task->state = " );
		dbc_putchar( item->state);
		dbc_putchar( item->taskid);
		dbc_uint16( item->timeline );
		dbc_putchar( item->priority );
		dbc_putchar( item->heapindex );
		dbc_putchar(item->itemnext );

	    putchar( '\r' );
	    putchar( '\n' );
	}
}


