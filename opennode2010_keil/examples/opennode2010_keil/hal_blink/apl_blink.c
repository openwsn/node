/**
 * @section history
 * modified by zhangwei on 2011.09.11
 * - tested Ok on OpenNode 2010 (which is based on STM32F103)
 */

#include "apl_foundation.h"

int main (void)
{ 
	/**
	 * @attention: According to our specification, you should call target_init()
	 * firstly here. However, the LED demonstration is so simple that it can run
	 * successfully without calling target_init(). 
	 */
	
	//target_init();

	led_open();
	while(1) 
	{
	  	led_toggle( LED_RED);
	  	hal_delayms(10000);
  	}
  	return 0;
}


