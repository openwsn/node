 /*
 * 下面是定时器的程序
 */

#include "hal_commoninterrface_test.h"

#include "openwsn/hal/opennode2010/cm3/core/core_cm3.h"
#include "openwsn/hal/hal_timer.h"
#include "openwsn/hal/hal_rtc.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/hal/hal_common.h"
#include "openwsn/hal/hal_debugio.h"

                                   
static TiTimerAdapter g_timer3;
static TiTimerAdapter g_timer2;
static TiRtcAdapter   m_rtc;
static TiBasicTimerInterface m_tim2;
static TiBasicTimerInterface m_tim3;
static TiBasicTimerInterface m_rctintface;
static void _timer2_handler(void * object, TiEvent * e);
static void _timer3_handler(void * object, TiEvent * e);
static void _rtcalarm_handler(void * object, TiEvent * e);
static void timer_interface_test( void);
static void rtc_interface_test( void);

int main( void)
{
    //timer_interface_test();
     rtc_interface_test();
}

static void timer_interface_test( void)
{
    TiTimerAdapter * timer_3;
    TiTimerAdapter * timer_2;
    TiBasicTimerInterface * tim2;
    TiBasicTimerInterface * tim3;
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    led_open();
    led_on( LED_RED);
    hal_delayms( 500);
    led_off( LED_RED);
    timer_3 = timer_construct( (void *)&g_timer3,sizeof(g_timer3));
    timer_2 = timer_construct( (void *)&g_timer2,sizeof(g_timer2));

    timer_3 = timer_open( timer_3,3,NULL,NULL,0x01);
    timer_2 = timer_open( timer_2,2,NULL,NULL,0x01);

    tim2 = timer_basicinterface( timer_2,&m_tim2);
    tim3 = timer_basicinterface( timer_3,&m_tim3);
    
    tim2->setinterval( tim2->provider,1000,0);
    tim3->setinterval( tim3->provider,1000,0);
    tim2->setscale( tim2->provider,7999);
    tim3->setscale( tim3->provider,7999);
    hal_attachhandler( INTNUM_TIMER2, _timer2_handler, timer_2);
    hal_attachhandler( INTNUM_TIMER3,  _timer3_handler, timer_3 );

    tim2->start( tim2->provider);
    tim3->start( tim3->provider);
    __disable_irq();
    __enable_irq();
    while (1)
    {
       
    }

}

static void rtc_interface_test( void)
{
    TiRtcAdapter *rtc;
    TiBasicTimerInterface * rtc_interrface;
    led_open();
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    led_on( LED_RED);
    hal_delayms( 500);
    led_off( LED_RED);
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,NULL,3,1);


    rtc_interrface = rtc_basicinterface( rtc,&m_rctintface);

    rtc_interrface->setinterval( rtc_interrface->provider,1,0);
    rtc_interrface->setscale( rtc_interrface->provider,32767);

    hal_attachhandler( INTNUM_RTCALARM,  _rtcalarm_handler, rtc_interrface->provider );

    rtc_interrface->start( rtc_interrface->provider);
    while (1)
    {
        
        
    }
}

/*
void TIM3_IRQHandler(void)
{

    //TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    //GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));

    //GPIO_ResetBits( GPIOA,GPIO_Pin_8);
   
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        led_toggle( LED_RED);
    }
    
}

void TIM2_IRQHandler( void)
{
    
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        USART_Send( 0xff);
    }
    
}
*/

static void _timer2_handler(void * object, TiEvent * e)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        USART_Send( 0xff);
    }
}
static void _timer3_handler(void * object, TiEvent * e)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        led_toggle( LED_RED);
    }
}

static void _rtcalarm_handler(void * object, TiEvent * e)
{
   // m_rctintface.setinterval( (&m_rctintface).provider,1);
    rtc_setalrm_count(&m_rtc,1,0);
    led_toggle( LED_RED);
    dbc_putchar( 0xab);
    EXTI_ClearITPendingBit(EXTI_Line17);
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_ALR);
    }
}
