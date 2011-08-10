
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_rtc.h"

void RTC_IRQHandler(void);
void RTCAlarm_IRQHandler( void );
void RTC_sec_test(void);
void RTC_alarm_test(void);

TiRtcAdapter m_rtc;

void main(void)
{
    RTC_sec_test();
    //RTC_alarm_test();

}

void RTC_sec_test(void)
{
    TiRtcAdapter *rtc;
    led_open();
    halUartInit(9600,0);
    led_on( LED_RED);
    hal_delay( 500);
    led_off( LED_RED);
    USART_Send( 0xf1);//todo for testing
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,NULL,1,1);
    rtc_setprscaler( rtc,3276);//此时基本单位是0.1秒//rtc_setprscaler( rtc,32767);//此时基本单位是1秒
    rtc_start( rtc);
    while (1)
    {
    }
}

void RTC_alarm_test(void)
{
    TiRtcAdapter *rtc;
    led_open();
    halUartInit(9600,0);
    led_on( LED_RED);
    hal_delay( 500);
    led_off( LED_RED);
    USART_Send( 0xf1);//todo for testing
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,NULL,3,1);
    rtc_setprscaler( rtc,3276);//此时基本单位是0.1秒//rtc_setprscaler( rtc,32767);//此时基本单位是1秒
    rtc_setalrm_count(rtc,2);
    rtc_start( rtc);
    while (1)
    {
        /*
        USART_Send( 0x02);
        hal_delay(1);//todo for testing
        led_toggle( LED_RED);
        rtc_setalrm_count(rtc,2);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        
        USART_Send( 0xab);
        */


        USART_Send( 0xac);

        rtc_setalrm_count(rtc,9);//interval = 9+1 =10(基本单位）
        
        //RTC_WaitForLastTask();
        //RTC_SetAlarm(RTC_GetCounter()+ 0);

       
        hal_delay( 1);//如果这一句延时不加上去，usart输出会出错。实际上是将rtc_setalrm_count(rtc,0);与 PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);保留间隔
        led_toggle( LED_RED);
       
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        
        USART_Send( 0xab);
        
        
    }
}

void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);

		
		led_toggle( LED_RED);
		USART_Send( 0xf1);
	}
}

void RTCAlarm_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line17);
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_ALR);
    }
}






