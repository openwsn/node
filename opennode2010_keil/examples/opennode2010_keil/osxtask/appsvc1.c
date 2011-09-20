
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

#define USART_Send
 //TiAdcAdapter  g_adc;
 //TiLightSensor g_light;
 //TiUartAdapter g_uart;
 uint16 lightvalue;

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

uint16 getlightvalue(void)
{
	//TiAdcAdapter * adc;
	//TiLightSensor * light;
	TiUartAdapter * uart;
	char * welcome = "sensor adc_light";
	uint16 val;

	//adc = adc_construct( (void *)&g_adc, sizeof(TiAdcAdapter) );
	//light = light_construct( (void *)&g_light, sizeof(TiLightSensor) );
	//uart = uart_construct( (void *)&g_uart, sizeof(TiUartAdapter) );

	// xiao-yuezhang changed the second parameter from 5 to 0
	//adc_open( adc, 0, NULL, NULL, 0 );
	//adc_open( adc, 0, NULL, NULL, 0 );
	//light_open( light, 0, adc );
	//uart_open( uart, 2, 9600, 8, 1, 0 );
	//uart_write( uart, welcome, strlen(welcome), 0x00 );

    dbo_putchar( '>' );
	//val = light_value( light );
	dbo_n16toa( val );

	//light_close( light );
	//adc_close( adc );
	uart_close( uart );


	return val;
}

void createtask(int8 id,TiOsxTaskHeap *heap,int16 timeline)
{
	int8 idx;
	TiOsxTaskHeapItem item;
	switch(id)
	{
	case 1:{	
		memset( &item, 0x00, sizeof(item) );
		item.taskfunction =asv1_evolve;
		item.taskdata = NULL;
		item.timeline = timeline;
		item.priority = 1;
		break;}
	case 2:{	
		memset( &item, 0x00, sizeof(item) );
		item.taskfunction =asv2_evolve;
		item.taskdata = NULL;
		item.timeline = timeline;
		item.priority = 1;
		break;}
	default:return;
	}


	idx = osx_taskheap_insert( heap, &item );
}

void asv1_evolve( void * svcptr, TiOsxTaskHeapItem *item )
 {  
	 USART_Send(0xf2);//led_toggle(LED_RED);
	 createtask(1,heap,3);
	 createtask(2,heap,2);
	
	//lightvalue=getlightvalue();

}


