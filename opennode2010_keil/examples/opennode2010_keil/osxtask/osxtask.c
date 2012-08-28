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
 * @modified by shizhirong on 20120628
 *  - using osx_tlsche.h
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
#include "../../../common/openwsn/hal/hal_timer.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/osx/osx_kernel.h"
#include "../../../common/openwsn/hal/hal_rtc.h"
#include "../../../common/openwsn/osx/osx_taskpool.h"
#include "../../../common/openwsn/osx/osx_taskheap.h"
#include "../../../common/openwsn/osx/osx_tlsche.h"
#include "asv_foundation.h"
#include "appsvc1.h"
#include "appsvc2.h"


#define CONFIG_AUTO_STOP
#undef  CONFIG_AUTO_STOP

#define CONFIG_DISPATCHER_TEST_ENABLE

static TiRtc						m_rtc;
static TiOsxTimeLineScheduler		m_sche;

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
//    osx_task_stop_mode();
}



void osx_task_stop_mode(void)
{
    TiAppService1 * asv1;
    TiAppService2 * asv2;
    TiRtc * rtc;
	TiOsxTimeLineScheduler * sche;

	sche = osx_tlsche_open(&m_sche, rtc);

	asv1 = asv1_open(&g_task1data, sizeof(TiAppService1),sche);
    asv2 = asv2_open(&g_task2data, sizeof(TiAppService2),sche);

	osx_tlsche_taskspawn(sche,(TiOsxTask)asv1_evolve,NULL,3,0,0x00);
    
	halUartInit(9600,0);
    
	led_open(LED_RED);
	led_on(LED_RED);
    hal_delayms(500);
    led_off(LED_RED);
    USART_Send(0xf1);//todo for testing

    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
	rtc = rtc_open(rtc,NULL,NULL,3,1);
	rtc = rtc_open(rtc,NULL,NULL,1,1);
	
	rtc_setlistener(rtc, osx_rtc_listener, sche); 
	rtc_setalrm_count(rtc,0,0);
    rtc_setprscaler(rtc,32767);//基本单位秒
    rtc_start(rtc);
	USART_Send(0xf2);//todo for testing


    while(1)
    {
        osx_tlsche_evolve(sche, NULL);
		rtc_setalrm_count(rtc,1,0);
        hal_delayms(1);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
    }
}



void osx_task_second_interrupt_rtc( void)
{
    TiAppService1 * asv1;
    TiAppService2 * asv2;
    TiRtc * rtc;
	TiOsxTimeLineScheduler * sche;
    
	halUartInit(9600,0);    

	led_open(LED_ALL);
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
//    rtc = rtc_open(rtc,NULL,_RTC_IRQHandler,NULL,1,1);
    rtc = rtc_open(rtc,NULL,NULL,1,1);
	rtc_setlistener(rtc, osx_rtc_listener, sche); 

    sche = osx_tlsche_open( &m_sche, rtc);

    asv1 = asv1_open( &g_task1data, sizeof(TiAppService1) ,sche);
    asv2 = asv2_open( &g_task2data, sizeof(TiAppService2) ,sche);
   	osx_tlsche_taskspawn(sche,(TiOsxTask)asv1_evolve,NULL,3,1,NULL);

    USART_Send( 0xf1);
	osx_tlsche_execute(sche);
}
