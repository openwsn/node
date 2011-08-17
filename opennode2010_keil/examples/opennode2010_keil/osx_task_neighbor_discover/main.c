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
#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "asv_configall.h"
#include <string.h>
#include "apl_foundation.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_timer.h"//why include hal_timer?
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/osx/osx_kernel.h"
#include "openwsn/hal/hal_rtc.h"
#include "openwsn/hal/hal_mcu.h"
#include "openwsn/hal/hal_configall.h"  
#include "openwsn/svc/svc_configall.h"  
#include "openwsn/rtl/rtl_iobuf.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/svc/svc_foundation.h"
#include "openwsn/svc/svc_nio_acceptor.h"
#include "openwsn/svc/svc_nio_aloha.h"
#include "openwsn/svc/svc_nodebase.h"
#include "openwsn/svc/svc_nio_dispatcher.h"
#include "openwsn/svc/svc_nio_neighbor_discover.h"
#include "openwsn/osx/osx_tlsche.h"
#include "openwsn/osx/osx_taskpool.h"
#include "openwsn/osx/osx_taskheap.h"
#include "asv_foundation.h"

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

#define CONFIG_AUTO_STOP
#undef  CONFIG_AUTO_STOP

#define CONFIG_DISPATCHER_TEST_ENABLE

#define CONFIG_UART_ID              0
#define CONFIG_TIMER_ID             1


uint16                              g_count=0;
uint16                              rtccount=0;
int16                               tasktimeline=0;

static TiNioNeighborDiscover        m_ndp;
static char                         m_nacmem[NAC_SIZE];
static TiRtc m_rtc;


//TiOsxTaskPool m_taskpool;
//TiOsxTaskHeap m_taskheap;

TiOsxTimeLineScheduler              m_scheduler;
static TiTimerAdapter 		        m_timer2;
//static TiTimerAdapter 		        m_timer3;
static TiAloha                      m_aloha;
static TiFrameRxTxInterface         m_rxtx;

TiNodeBase                          m_nodebase;
static TiNioNetLayerDispatcher      m_dispatcher;
static TiCc2520Adapter              m_cc;


int16 tasktimeline;



void on_timer_expired( void * object, TiEvent * e );
void _osx_priority_queue_popfront_test( TiOsxTaskHeap * heap );
void heap_task_evovle(TiOsxTaskHeap *heap,TiRtc *rtc);
void RTC_IRQHandler(void);
void RTCAlarm_IRQHandler( void );
void osx_task_second_interrupt_rtc( void);
void osx_task_stop_mode( void);
static void _rtc_handler(void * object, TiEvent * e);

/******************************************************************************* 
 * main()
 ******************************************************************************/

int main()
{
   osx_task_second_interrupt_rtc();
    //osx_task_stop_mode();
}

void osx_task_second_interrupt_rtc( void)
{
    char * request;
    char * response;
    uint8 len;	
    int i;

    TiRtc * rtc;
    TiOsxTimeLineScheduler * scheduler;
    TiTimerAdapter * timer2;
    TiTimerAdapter * timer3;
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
    TiNioAcceptor * nac;
    TiAloha * mac;
    TiNodeBase * nodebase;
    TiNioNetLayerDispatcher *dispatcher;
    TiNioNeighborDiscover *ndp;
    TiFrame * mactxbuf;


    //target_init();
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    led_open();
    led_on( LED_RED);
    hal_delayms( 500);
    led_off( LED_RED);
    USART_Send( 0xf1);//todo for testing
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,NULL,1,1);
    
    //rtc_setprscaler( rtc,32767);
    //rtc_start( rtc);
    scheduler = osx_tlsche_open( &m_scheduler,rtc);

    timer2  = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );

    cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
    nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );

    cc   = cc2520_open(cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2520_interface( cc, &m_rxtx );
    nac  = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
    timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
    mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timer2, NULL, NULL,0x00);
    
    nodebase = nbase_construct( (void *)&m_nodebase,sizeof(m_nodebase));
    nodebase = nbase_open( nodebase,0x01,CONFIG_NODE_PANID,CONFIG_NODE_ADDRESS,CONFIG_NODE_CHANNEL,3);

    dispatcher = net_disp_construct( (void *)&m_dispatcher,sizeof(m_dispatcher));
    dispatcher = net_disp_open( dispatcher,mac);

    ndp = ndp_construct( ( void *)&m_ndp,sizeof(m_ndp));
    ndp = ndp_open( ndp,dispatcher,nodebase,scheduler,NULL);
    
    net_disp_register( ndp->dispatcher,0x02,ndp,nio_ndp_rxhandler,nio_ndp_txhandler,nio_ndp_response_evolve);
    
    cc2520_setchannel( cc, CONFIG_NODE_CHANNEL );
    cc2520_rxon(cc);							            
    cc2520_setpanid( cc, CONFIG_NODE_PANID  );					
    cc2520_setshortaddress( cc, CONFIG_NODE_ADDRESS );
    dbc_putchar( 0xf4);//todo for testing
    
    osx_tlsche_taskspawn( scheduler, nio_ndp_request_evolve,NULL,0,0,0);
    dbc_putchar( 0xf5);//todo for testing
    
    rtc_setprscaler( rtc,32767);
    hal_attachhandler( INTNUM_RTC,  _rtc_handler, rtc );
    dbc_putchar( 0xf6);//todo for testing
    rtc_start( rtc);
    //hal_enable_interrupts();
    dbc_putchar( 0xf2);//todo for testing
    while(1)
    {
        //heap_task_evovle(heap,rtc);
        osx_tlsche_evolve( scheduler, NULL );
    }
}

/*
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {*/
        /* Clear the RTC Second interrupt */
/*
        RTC_ClearITPendingBit(RTC_IT_SEC);

        osx_tlsche_stepforward( &m_scheduler, 1 );
        led_toggle( LED_RED);
        USART_Send( 0xf1);
    }
}
*/

/*
void RTCAlarm_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line17);
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {*/
        /* Clear the RTC Second interrupt */
/*
        RTC_ClearITPendingBit(RTC_IT_ALR);
        g_count ++;
        led_toggle( LED_RED);
    }
}
*/

static void _rtc_handler(void * object, TiEvent * e)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */

        RTC_ClearITPendingBit(RTC_IT_SEC);

        osx_tlsche_stepforward( &m_scheduler, 1 );
        led_toggle( LED_RED);
        USART_Send( 0xf1);
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
