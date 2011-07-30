#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_debugio.h"

static TiTimerAdapter g_timer;
static volatile uint8 g_count = 0;

void timer_query_driven_test( uint8 id );
void timer_interrupt_driven_test( uint8 id );
void on_timer_expired( void * object, TiEvent * e );

int main(void)
{
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
    
	//hal_init( NULL, NULL );

	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open(0, 38400);

	// the parameter id can be 0-3 denoting timer hardware 0 to 3
     	timer_interrupt_driven_test( 2 );
    // 	timer_query_driven_test( 2 );
  		while(1){}
}

void timer_query_driven_test( uint8 id )
{
	TiTimerAdapter *timer;
	timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, id, NULL, NULL, 0x00 ); 
	timer_setinterval( timer,5, 1 );  //(timer,interval,repeat)  the range of the interval  1~8
	timer_start( timer );
//next is the function for the query model
	int i;

	while (1)
	{

/*  		for(i=0;i<100;i++)              //
		{
//		   loop_until_bit_is_set(TIFR,OCF0);   
//		   TIFR=TIFR|0x02;  
		  // be caution: the flag should be clear after it is queried by set the OCF0 bit to be 1


		  //   loop_until_bit_is_set(TIFR,OCF2);		    
		  //	 TIFR=TIFR|0x80; 
		  	
		//	dbo_putchar(OCR1AH);
		//	dbo_putchar(OCR1AL);
			
		//	loop_until_bit_is_set(TIFR,OCF1A);
		//	TIFR =TIFR| (1 << OCF1A); 
		   // led_toggle(LED_GREEN);

		//	loop_until_bit_is_set(ETIFR,OCF3A);
		//	ETIFR =ETIFR| (1 << OCF3A); 
			}
*/
			i=0;
			while(i<100){
				if(timer_expired(timer))
							i++;

			}
		led_toggle( LED_RED );
		led_toggle(LED_GREEN);

	}

  //  timer_close( timer );
}

void timer_interrupt_driven_test( uint8 id )
{ 
	TiTimerAdapter *timer;

	hal_enable_interrupts();
    timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, id, on_timer_expired, NULL, 0x01 ); 
    timer_setinterval( timer, 1, 1 );  //(timer,interval,repeat)  the range of the interval  1~8
	timer_start( timer );
	
	while (1) {
	dbo_putchar(0x2);
	};
}

void on_timer_expired( void * object, TiEvent * e )
{   	g_count ++;
	if (g_count == 100)
	{
	    led_toggle( LED_RED );
		g_count = 0;
	}
    
}

