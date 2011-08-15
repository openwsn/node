
#include "apl_foundation.h"
#include "openwsn/hal/hal_led.h"


int  main (void)
{ 
  led_open();
  
  while( 1) 
  {
	  led_toggle( LED_RED);
	  hal_delayms(1000);
 
  }
  return 0;
}


