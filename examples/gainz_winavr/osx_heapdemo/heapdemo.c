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
TiOsxTaskHeap m_taskheap;

void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx );
void _osx_taskheap_dump( TiOsxTaskHeap * heap );
void _osx_priority_queue_popfront_test( TiOsxTaskHeap * heap );

/***************************************************************/

int main()
{
   	TiOsxTaskPool * tpl;
    TiOsxTaskHeap * heap;
	TiOsxTaskHeapItem item, *newitem;
	TiOsxTaskHeapItem **heaptable;
	int8 i, idx;
	char * msg = "welcome tp heapdemo...\r\n";

	tpl = osx_taskpool_construct( (void *)&m_taskpool, sizeof(TiOsxTaskPool) );
	heap = osx_taskheap_open( &m_taskheap, tpl );

	osx_assert( tpl != NULL );
	osx_assert( heap != NULL );	

	target_init();

    led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL ); 

	dbo_open( 0, 38400 );
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );
	dbo_putchar(0xF0 );

	dbo_putchar( osx_taskheap_capacity(heap) );
	

	for (i=0; i<osx_taskheap_capacity(heap); i+=2 )
	{
		memset( &item, 0x00, sizeof(item) );
		item.taskfunction = NULL;
		item.taskdata = NULL;
		item.timeline = 10 - i;
		item.priority = 10 - i;

		idx = osx_taskheap_insert( heap, &item );
        
		dbo_putchar(0xF1 );
		dbo_putchar(idx );


		if (idx >= 0)
		{
			dbo_putchar( 0xF2 ); // indicate heap insert success
			_osx_taskheap_item_dump( heap, idx );
			newitem = osx_taskheap_items(heap)[idx];
		}
		else{
					dbo_putchar( 0xF3 ); // indicate heap insert failure
		}
	}


	for (i=1; i<osx_taskheap_capacity(heap); i+=2 )
	{
		memset( &item, 0x00, sizeof(item) );
		item.taskfunction = NULL;
		item.taskdata = NULL;
		item.timeline = i;
		item.priority = i;

		idx = osx_taskheap_insert( heap, &item );
		
		dbo_putchar(0xF1 );
		dbo_putchar(idx );

		if (idx >= 0)
		{
			dbo_putchar( 0xF2 ); // indicate heap insert success
			_osx_taskheap_item_dump( heap, idx );
			newitem = osx_taskheap_items(heap)[idx];
		}
		else{
					dbo_putchar( 0xF3 ); // indicate heap insert failure
		}
	}


    _osx_taskheap_dump( heap );

	_osx_priority_queue_popfront_test( heap );

    while(1){};
}

void _osx_taskheap_dump( TiOsxTaskHeap * heap )
{
	int i;
	//heaptable = osx_taskheap_items(heap);
	for (i=0; i<osx_taskheap_capacity(heap); i++)
	{
		dbo_putchar(0xF4);
		_osx_taskheap_item_dump( heap, i );
	}
}

void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx )
{
	TiOsxTaskHeapItem * item;
	
	if (idx >= 0)
	{
		item = osx_taskheap_items(heap)[idx];
		dbo_putchar(item->timeline);
		dbo_putchar( heap->count );
		dbo_putchar( idx ); 
		//dbo_n16toa( newitem->timeline );
		dbo_putchar( item->timeline );
		dbo_putchar( item->priority );
		dbo_putchar( item->heapindex );	
	}
}


void _osx_priority_queue_popfront_test( TiOsxTaskHeap * heap )
{
	TiOsxTaskHeapItem * item;
	while (!osx_taskheap_empty(heap))
	{
		item = osx_taskheap_root(heap);
		rtl_assert( item->heapindex == 0 );
		// _osx_taskheap_item_dump( heap, item->heapindex );
		dbc_uint16( item->timeline );

		osx_taskheap_deleteroot( heap );
		

	}
}



