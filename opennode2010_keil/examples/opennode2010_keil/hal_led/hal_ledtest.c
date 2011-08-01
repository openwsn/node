
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"


int  main (void)
{ 
  led_open();
  
  while( 1) 
  {
	  led_toggle( LED_RED);
	  hal_delay(1000);
 
  }
  return 0;
}


