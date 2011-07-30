
/*****************************************************************************
 * @author zhangwei on 20070629
 * first created
 ****************************************************************************/

#include "../common/foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"

void led_test( void )
{     
	int i;
	
	//target_init();
	//global_construct();
    led_init();
	
	// testing LED twinkle
	
	led_off( LED_ALL ); 
    led_twinkle( LED_RED, 300 );
	led_twinkle( LED_YELLOW, 300 );
    led_twinkle( LED_GREEN, 300 );
	hal_delay( 100 );
	
	// testing each single LED
	
	led_off( LED_ALL ); 
	i=0;
	while (i<3)
	{
		led_off(LED_RED) ; 
	    hal_delay(300);
	    led_on(LED_RED) ; 
	    hal_delay(300);
	    i++;
    }
	
	i=0;
	while (i<3)
 	{
	    led_off(LED_GREEN) ; 
	    hal_delay(300);
	    led_on(LED_GREEN) ; 
	    hal_delay(300);
	    i++;
    }
    
	i=0;
    while (i<3)
 	{
	    led_off(LED_YELLOW) ; 
	    hal_delay(300);
	    led_on(LED_YELLOW) ; 
	    hal_delay(300);
	    i++;
	}
	
	// test the LEDs at the same time
	
	led_off( LED_ALL ); 
	i = 0;
	while (i<3)
	{
		led_toggle( LED_RED ); 
		led_toggle( LED_YELLOW ); 
		led_toggle( LED_GREEN ); 
	    hal_delay(500);
		i++;
	}
	
	while (TRUE)
	{
		led_off( LED_ALL ); 
	    hal_delay(300);
		led_on( LED_ALL ); 
	    hal_delay(300);
	}
	
	//global_destroy();
}
