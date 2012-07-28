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
 * @modified by shizhirong on 20120627
 *  - tested success.
 ******************************************************************************/

#include "asv_configall.h"
#include <string.h>
#include "apl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_debugio.h"
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
#include "../../../common/openwsn/hal/hal_timer.h"//why include hal_timer?
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/osx/osx_kernel.h"
#include "../../../common/openwsn/hal/hal_rtc.h"
#include "../../../common/openwsn/osx/osx_taskpool.h"
#include "../../../common/openwsn/osx/osx_taskheap.h"
#include "asv_foundation.h"
#include "appsvc1.h"
#include "appsvc2.h"


#define CONFIG_AUTO_STOP
#undef  CONFIG_AUTO_STOP

#define CONFIG_DISPATCHER_TEST_ENABLE

#define CONFIG_UART_ID              0
#define CONFIG_TIMER_ID             1

//#define target_init(void);
//#define USART_Send
//#define halUartInit
//#define hal_delay

static TiUartAdapter         	    m_uart;      
static TiAppService1				m_svcmem1;
static TiAppService2				m_svcmem2;
static uint16						g_count=0;
static uint16						rtccount=0;
static int16						tasktimeline=0;
static TiRtc						m_rtc;


TiOsxTaskPool m_taskpool;
TiOsxTaskHeap m_taskheap;

int16 tasktimeline;

TiOsxTaskPool * tpl;
TiOsxTaskHeap * heap;


void on_timer_expired( void * object, TiEvent * e );
void _osx_priority_queue_popfront_test( TiOsxTaskHeap * heap );
void heap_task_evovle(TiOsxTaskHeap *heap,TiRtc *rtc);
void _RTC_IRQHandler(void);
void _RTCAlarm_IRQHandler( void );
void osx_task_second_interrupt_rtc( void);
void osx_task_stop_mode( void);

/******************************************************************************* 
 * main()
 ******************************************************************************/

int main()
{
    osx_task_second_interrupt_rtc();
    //osx_task_stop_mode();
}


void osx_task_stop_mode( void)
{
    TiAppService1 * asv1;
    TiAppService2 * asv2;

    TiRtc * rtc;

    int8 idx;
    TiOsxTaskHeapItem item;

    tpl = osx_taskpool_construct( (void *)&m_taskpool, sizeof(TiOsxTaskPool) );
    heap = osx_taskheap_open( &m_taskheap, tpl );//构造一个堆

    osx_assert( tpl != NULL );
    osx_assert( heap != NULL );	

    asv1 = asv1_open( &m_svcmem1, sizeof(TiAppService1) );//打开两个task
    asv2 = asv2_open( &m_svcmem2, sizeof(TiAppService2) );

    tasktimeline=3;
    memset( &item, 0x00, sizeof(item) );//此时先产生一个task1，并把它放入堆中
    item.taskfunction =asv1_evolve;
    item.taskdata = NULL;
    item.timeline = tasktimeline;//让其定时器打开后三秒运行
    item.priority = 1;

    idx = osx_taskheap_insert( heap, &item );


    led_open();
    halUartInit(9600,0);
    led_on( LED_RED);
    hal_delay( 500);
    led_off( LED_RED);
    USART_Send( 0xf1);//todo for testing
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,_RTCAlarm_IRQHandler,NULL,3,1);
    rtc_setalrm_count(rtc,0,0);
    rtc_setprscaler( rtc,32767);//基本单位秒
    rtc_start( rtc);
	USART_Send( 0xf2);//todo for testing


    while(1)//暂时用RTC的中断来实现控制task的执行，也就是on_timer_expired,heap_task_evole应该更适用一般，此时就可以不用on_timer_expired
    {
		heap_task_evovle(heap,rtc);
        rtc_setalrm_count(rtc,1,0);
        hal_delayms(1);//todo for testing
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    }
}



void osx_task_second_interrupt_rtc( void)
{
	char * msg = "welcome to sendnode...";
    TiUartAdapter * uart;

    TiAppService1 * asv1;
    TiAppService2 * asv2;

    TiRtc * rtc;

    int8 idx;
    TiOsxTaskHeapItem item;
    
	target_init();
    halUartInit(9600,0);    
	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

    //uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    //uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);
	//rtl_init( uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar_wait, hal_assert_report );
	dbc_mem( msg, strlen(msg) );
   
   
	tpl = osx_taskpool_construct( (void *)&m_taskpool, sizeof(TiOsxTaskPool) );
    heap = osx_taskheap_open( &m_taskheap, tpl );//构造一个堆

    osx_assert( tpl != NULL );
    osx_assert( heap != NULL );	

    asv1 = asv1_open( &m_svcmem1, sizeof(TiAppService1) );//打开两个task
    asv2 = asv2_open( &m_svcmem2, sizeof(TiAppService2) );

    tasktimeline=3;
    memset( &item, 0x00, sizeof(item) );//此时先产生一个task1，并把它放入堆中
    item.taskfunction =asv1_evolve;
    item.taskdata = NULL;
    item.timeline = tasktimeline;//让其定时器打开后三秒运行
    item.priority = 1;

    idx = osx_taskheap_insert( heap, &item );

    USART_Send( 0xf1);
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,_RTC_IRQHandler,NULL,1,1);
    rtc_setprscaler( rtc,32767);
    rtc_start( rtc);
    USART_Send( 0xf2);

    while(1)//暂时用RTC的中断来实现控制task的执行，也就是on_timer_expired,heap_task_evole应该更适用一般，此时就可以不用on_timer_expired
    {
        heap_task_evovle(heap,rtc);
    }
}

void on_timer_expired( void * object, TiEvent * e )//每秒钟过后执行这个函数
{
	led_toggle(LED_YELLOW);

	g_count ++;
/*	
	TiOsxTaskHeapItem item;

	if(!osx_taskheap_empty(heap))//非空的话取出来
	{
		item = osx_taskheap_root(heap);
		rtl_assert( item->heapindex == 0 );

		if(g_count>=item->timeline&&item->taskfunction != NULL)//如果计时达到预定时间时（秒级，可能不精确）就执行
		{
		    osx_taskheap_deleteroot( heap );
			
			
			for(int i=0;i<heap->count;++i)//此时应该把堆里余下的task的时间减去过去的时间
			{
				heap->items[i]->timeline=heap->items[i]->timeline-g_count;
			}
			g_count=0;
			
			item->taskfunction(item->taskdata,item);
		}
		else return;

	}
*/
}

void _RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);

        g_count ++;
        led_toggle( LED_RED);
        USART_Send( 0x00);
    }
}

void _RTCAlarm_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line17);
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_ALR);
        g_count ++;
        led_toggle( LED_RED);
		USART_Send( 0x01);
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
void heap_task_evovle(TiOsxTaskHeap *heap,TiRtc *rtc)//原理同上 我想用rtc里的函数代替g_count来计时，见注释部分
{
    int i;
	TiOsxTaskHeapItem *item;
	if(!osx_taskheap_empty(heap))
	{
		item = osx_taskheap_root(heap);
		rtl_assert( item->heapindex == 0 );

		if(g_count>=item->timeline&&item->taskfunction != NULL)
		{
			osx_taskheap_deleteroot( heap );
			
			
			for( i=0;i<heap->count;++i)
			{
				heap->items[i]->timeline=heap->items[i]->timeline-g_count;
			}
			g_count=0;

			item->taskfunction(item->taskdata,(void *)item);
		}
		else return;

/***********************************************************
        int16 currenttime=rtc->current_time->sec+
			              rtc->current_time->min*60+
						  rtc->current_time->hour*3600;

		if(currenttime)>=item->timeline&&item->taskfunction!=NULL)
		{
		    osx_taskheap_deleteroot( heap );
			
			
			for(int i=0;i<heap->count;++i)
			{
				heap->items[i]->timeline-=currenttime;
			}

			currenttime=0;
			rtc->current_time->sec = 0;
			rtc->current_time->min = 0;
			rtc->current_time->hour = 0;
			rtc->current_time->day = 1;
			rtc->current_time->month = 1;
			rtc->current_time->year = 2000;
			
			item->taskfunction(item->taskdata,item);
		}
		*****************************************************/

	}
}


