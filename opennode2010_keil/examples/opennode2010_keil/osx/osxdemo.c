/*******************************************************************************
 * osxdemo
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

#include "asv_configall.h"
#include <string.h>
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
#include "../../../common/openwsn/hal/hal_interrupt.h"
#include "../../../common/openwsn/hal/hal_targetboard.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
#include "../../../common/openwsn/hal/hal_timer.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/osx/osx_kernel.h"
#include "asv_foundation.h"
#include "appsvc1.h"
#include "appsvc2.h"
#include "appsvc3.h"

/******************************************************************************* 
 * CONFIG_AUTO_STOP 
 * The timer will send event to the osx kernel. This macro decides whether the timer 
 * stops after some time.
 *
 * CONFIG_TIMER_DRIVE
 * Let the hardware timer to drive the kernel to run. If this macro is undefined, 
 * then the kernel is drived by an infinite loop.
 *
 * CONFIG_DISPATCHER_TEST_ENABLE
 * This macro is defined by default. If this macro is undefined, then the events 
 * generated are with NULL event handler. This will cause the osx kernel to searching 
 * for appropriate handler to process it. This is done by the dispatcher object 
 * inside the kernel.
 ******************************************************************************/

#define CONFIG_AUTO_STOP
#undef  CONFIG_AUTO_STOP

#undef  CONFIG_TIMER_DRIVE
//#define CONFIG_TIMER_DRIVE

#define CONFIG_DISPATCHER_TEST_ENABLE

#define CONFIG_UART_ID              0
#define CONFIG_TIMER_ID             1

TiAppService1                       m_svcmem1;
TiAppService2                       m_svcmem2;
TiAppService3                       m_svcmem3;
TiTimerAdapter                      m_timer;
uint16                              g_count=0;

void on_timer_expired( void * object, TiEvent * e );

/******************************************************************************* 
 * main()
 ******************************************************************************/

int main()
{
	TiAppService1 * asv1;
	TiAppService2 * asv2;
	TiAppService3 * asv3;
    TiTimerAdapter * evt_timer;
	char * msg = "welcome to osxdemo...";

	target_init();

	led_open(LED_ALL);
	led_on( LED_RED );
	hal_delayms( 1000 );
	led_off( LED_RED );

	//dbo_open( CONFIG_UART_ID, 38400 );
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbio_putchar(NULL,0xF4);

	//dbc_write( msg, strlen(msg) );

	g_count = 0;

	/* interrupt must keep disabled during the osx initializing process. It will 
	 * enabled until osx_execute(). */
	
	osx_init();

	/* @attention: The timer_setinterval() cannot accept large duration values because
	 * the hardware may not support it 
	 * 
	 * Q: what's the maximum value of timer_setinterval for each hardware timer?
	 * A: 1~8 (???)  */
	 
	evt_timer = timer_construct( (void *)&m_timer, sizeof(TiTimerAdapter) );
	timer_open( evt_timer, CONFIG_TIMER_ID, on_timer_expired, (void*)g_osx, 0x01 );
	timer_setinterval( evt_timer, 5, 1 );
	timer_start( evt_timer );
	dbc_putchar(0xf1);
	/* create and initialize three runnable application services. the runnable service
	 * is quite similar to OS's process. however, the runnable service improves the 
	 * standard "process" with a data structure and event handler, which greatly simplied 
	 * the state machine pattern developing.  */
	
	asv1 = asv1_open( &m_svcmem1, sizeof(TiAppService1) );
	asv2 = asv2_open( &m_svcmem2, sizeof(TiAppService2) );
	asv3 = asv3_open( &m_svcmem3, sizeof(TiAppService3) );
	dbc_putchar(0xf2);

	/* put the runnable application service into osx. then osx can dispatch events to 
	 * these services. the services only run when it receives an event, namely, the 
	 * events drives the service to forward according to the state machine.  */
	
	osx_attach( 1, asv1_evolve, asv1 );
	osx_attach( 2, asv2_evolve, asv2 );

	osx_postx(1,asv1_evolve,asv1,asv1);	//JOE
	osx_postx(2,asv2_evolve,asv2,asv2);

	dbc_putchar(0xf3);

	/* configure the listener relation between service 2 and service 3.
	 * you can also use
	 *		osx_attach( 3, asv3_evolve, asv3 );
	 * however, the following code demonstrates how to implement complex relations 
	 * among services.  */
	
	asv2_setlistener( asv2, (TiFunEventHandler)asv3_evolve, (void *)asv3 );
	dbc_putchar(0xf4);

	/* when the osx kernel really executed, it will enable the interrupts so that the 
	 * whole program can accept interrupt requests.
     * attention: osx kernel already support sleep/wakeup because the sleep/wakeup 
	 * handler have been registered inside the osx itself.  */
 
	#ifndef CONFIG_TIMER_DRIVE
	dbc_putchar(0xf5);
 	osx_execute();
	#endif

	#ifdef CONFIG_TIMER_DRIVE
	osx_hardexecute();
	#endif
	dbc_putchar(0xf2);
}


/* This is the timer's listener function. Everytime the timer expired, this function
 * will be called. So we can generate the event and put it into the system in this 
 * function.
 *
 * In this case, we set newe.handler and newe.objectto to NULL. This will cause 
 * the kernel to use dispatcher to find appropriate service.
 */
void on_timer_expired( void * object, TiEvent * e )
{
	TiEvent newe;

	//led_on( LED2 );
	//while (1) {};

	g_count ++;
	if ((g_count % 15) == 0)
	{
	    //led_toggle( LED_RED );
		memset( &newe, 0x00, sizeof(TiEvent) );
        newe.id = ((g_count/15) % 3);
		if (newe.id == 0)
			newe.id = 3;

		#ifndef CONFIG_DISPATCHER_TEST_ENABLE
        if (g_count % 2 == 0)
        {
		    newe.handler = asv1_evolve;
		    newe.objectfrom = object;
		    newe.objectto = &m_svcmem1;
        }
        else{
		    newe.handler = asv2_evolve;
		    newe.objectfrom = object;
		    newe.objectto = &m_svcmem2;
        }
		#endif

		/* If the event's handler is NULL, then osx kernel will had to search for
		 * appropriate handler in the dispatcher table to process it. 
		 * 
		 * Since the event generator often doesn't know which object will process 
		 * the event, so the event dispatcher in the kernel is mandatory.
		 */
		#ifdef CONFIG_DISPATCHER_TEST_ENABLE
        if (g_count % 2 == 0)
        {
		    newe.handler = NULL;
		    newe.objectfrom = object;
		    newe.objectto = NULL;
        }
        else{
		    newe.handler = NULL;
		    newe.objectfrom = object;
		    newe.objectto = NULL;
        }
		#endif

		osx_post( (TiEvent *)(&newe) );
	}
	
	#ifdef CONFIG_AUTO_STOP
	if (g_count == 61)
    {
        timer_close( &m_timer );
        timer_destroy( &m_timer );
		g_count = 0;
		led_off(LED_RED);
    } 
	#endif
}

