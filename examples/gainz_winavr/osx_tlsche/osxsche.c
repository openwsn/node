/*******************************************************************************
 * osx_tlsche
 * osx is an component based, lightweight operating system kernel. 
 * this program demostrates how to develop an complete program and separate runnable services.
 *
 * @status
 *	- release
 *
 * @author zhangwei(TongJi University) on 20090706
 *	- first created
 * @modified by zhangwei(TongJi University) on 20091006
 *	- finished osx kernel, this demo program and compile them passed
 * @modified by yan shixing(TongJi University) on 20091112
 *	- tested success.
 ******************************************************************************/

/**
 * osxsche.c
 * This file is used to demonstrate how to using the osx kernel's Time Line Scheduler.
 */

#include "task_configall.h"
#include <string.h>
#include <avr/sleep.h>
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
#include "../../../common/openwsn/hal/hal_interrupt.h"
#include "../../../common/openwsn/hal/hal_targetboard.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
#include "../../../common/openwsn/hal/hal_timer.h"
#include "../../../common/openwsn/hal/hal_rtc.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/osx/osx_taskheap.h"
#include "../../../common/openwsn/osx/osx_taskpool.h"
#include "../../../common/openwsn/osx/osx_tlsche.h"
#include "../../../common/openwsn/osx/osx_kernel.h"
#include "task_foundation.h"
#include "task_measure.h"
#include "task_siocomm.h"
#include "task_wlscomm.h"

#define CONFIG_UART_ID              0
#define CONFIG_TIMER_ID             1
             
#define TEST_INTERRUPT_DRIVEN
#undef  TEST_INTERRUPT_DRIVEN

static TiRtc m_rtc;
static TiOsxTimeLineScheduler m_sche;

void _on_timer_expired( void * object, TiEvent * e );
void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx );
void _osx_taskheap_dump( TiOsxTaskHeap * heap );

/*************************************************************************/

int main()
{
	TiMeasureTask * task1data;
	TiWioTask * task2data;
	TiSioTask * task3data;
    //TiTimerAdapter * evt_timer;
	TiRtcAdapter * rtc;
     TiOsxTimeLineScheduler * sche;
	char * msg = "welcome to osx timeline scheduler test...";

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );

	// dbo_open( CONFIG_UART_ID, 38400 );
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );
	dbc_putchar( 0xF0 );

	/* interrupt must keep disabled during the osx initializing process. It will 
	 * enabled until osx_execute(). */	
	// osx_init();


	/* @attention: The timer_setinterval() cannot accept large duration values because
	 * the hardware may not support it 
	 * 
	 * Q: what's the maximum value of timer_setinterval for each hardware timer?
	 * A: 1~8 (???)  */
	 
	/*evt_timer = timer_construct( (void *)&m_timer, sizeof(TiTimerAdapter) );
	timer_open( evt_timer, CONFIG_TIMER_ID, on_timer_expired, (void*)g_osx, 0x01 );
	timer_setinterval( evt_timer, 5, 1 );
	timer_start( evt_timer );*/

    // construct an timer for osx kernel scheduler
	rtc = rtc_construct( (void *)&m_rtc, sizeof(m_rtc) );

#ifdef TEST_INTERRUPT_DRIVEN
	rtc_open( rtc, (TiFunEventHandler)_on_timer_expired, NULL, 0x01 );
#else
	rtc_open( rtc, NULL, NULL, 0x01 );
#endif

    sche = osx_tlsche_open( &m_sche, rtc);

    task1data = measure_task_open( &g_task1data, sizeof(TiMeasureTask) );
	task2data = wio_task_open( &g_task2data, sizeof(TiWioTask) );
	task3data = sio_task_open( &g_task3data, sizeof(TiSioTask) );

	task1data->sche = (void*)sche;
	task2data->sche = (void*)sche;
	task3data->sche = (void*)sche;

	osx_tlsche_taskspawn( sche, (TiOsxTask)measure_task_evolve, task1data, 0, 0, 0x00 );
	//osx_tlsche_taskspawn( sche, (TiOsxTask)wio_task_evolve, task2data, 0, 1, 0x00 );


#ifdef TEST_INTERRUPT_DRIVEN
    // during execution of this while loop, the osx_tlsche_evolve() function will be
    // called by the timer directly. all the task execution operations are in this
    // evolve function.
    //
	rtc_setinterval( sche->timer, 0, 2, 0x01); //定时周期为一秒 
	hal_enable_interrupts();
	rtc_start( sche->timer );
    while (1) 
    {
        //set_sleep_mode(SLEEP_MODE_IDLE);
	    //sleep_enable();
	    //sleep_cpu();
	    //sleep_disable();
    }
    
#endif

#ifndef TEST_INTERRUPT_DRIVEN
    // execute function will check the task queue/task heap. if it finds nothing for
    // execution, then it will go into the sleep mode and wait for the timer's interrupt
    // to wakeup the CPU.
    osx_tlsche_execute( sche );
#endif

    measure_task_close( task1data );
    wio_task_close( task2data );
    sio_task_close( task3data );
}

void _on_timer_expired( void * object, TiEvent * e )
{   
    //led_toggle(LED_GREEN);
    osx_tlsche_evolve( &m_sche, NULL );
	
	//_osx_taskheap_dump(&m_sche.taskheap);//显示堆中各项的timeline
	
}

void _osx_taskheap_dump( TiOsxTaskHeap * heap )
{
	int i;
	for (i=0; i<osx_taskheap_capacity(heap); i++)
	{
		dbc_putchar(0xF4);
		_osx_taskheap_item_dump( heap, i );
	}
}

void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx )
{
	TiOsxTaskHeapItem * item;

	if (idx >= 0)
	{
		item = osx_taskheap_items(heap)[idx];
		dbc_putchar(item->timeline);
			
	}
}
