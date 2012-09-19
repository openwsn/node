#include "apl_foundation.h"
#include "openwsn/hal/hal_rtc.h"

void RTC_sec_test(void);
void RTC_alarm_test(void);

TiRtcAdapter m_rtc;
static TiUartAdapter		m_uart;


int main(void)
{
    //RTC_sec_test();
    RTC_alarm_test();
    return 0;
}

void RTC_sec_test(void)
{
    TiRtcAdapter *rtc;
	TiUartAdapter * uart;

    led_open(LED_RED);
    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart,1, 9600, 8, 1, 0 );

    led_on( LED_RED);
    hal_delayms( 500);
    led_off( LED_RED);
	uart_putchar(uart,0xf1);
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
	TiUartAdapter * uart;

    led_open(LED_RED);
    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart,1, 9600, 8, 1, 0 );

    led_on( LED_RED);
    hal_delayms( 500);
    led_off( LED_RED);
	//uart_putchar(uart,0xf1);
    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));

	rtc=rtc_open(rtc,NULL,NULL,1,1);
	rtc_start(rtc);
	rtc_close(rtc);
	rtc_stop(rtc);

    rtc = rtc_open(rtc,NULL,NULL,3,1);
	rtc_setprscaler( rtc,32767);//此时基本单位是1秒	 //  rtc_setprscaler( rtc,3276);//此时基本单位是0.1秒
    rtc_start( rtc);
    while (1)
    {
	    //hal_delayms( 1500);
        rtc_setalarm_count(rtc,5,0);//interval = 9+1 =10(基本单位）
		hal_delayms(1);
        //led_toggle( LED_RED);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    }
}








