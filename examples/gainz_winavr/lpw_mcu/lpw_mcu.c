#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/sleep.h>
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_targetboard.h"

/*********************************************************************************/

int main(void)
{ 

	target_init();
	led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );
    
	ACSR=(1<<ACD);//模拟比较器关闭

	ADCSRA=(0<<ADEN);//ADC关闭

	WDTCR=(1<<WDCE)|(1<<WDE);
	WDTCR=0x00;//关闭WDT

    MCUCSR=(1<<JTD);//关JTAG接口与片内调试系统*

/*熔丝位置位，在connect的fuse选项卡里不选BODEN，其默认值为1，
  则掉电检测器禁用*/

	while(1)
	{
	  set_sleep_mode(SLEEP_MODE_EXT_STANDBY);//扩展STANDBYY模式
	  sleep_enable();
	  sleep_cpu();
	  sleep_disable();
	}

}    

/*****************************************************************
各种睡眠模式：
   进入空闲模式
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
    进入ADC 噪声抑制模式
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sleep_cpu();
    sleep_disable();

    进入掉电模式
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	进入省电模式
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	进入STANDBY模式
    set_sleep_mode(SLEEP_MODE_STANDBY);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	进入扩展STANDBY模式
    set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
	*/
