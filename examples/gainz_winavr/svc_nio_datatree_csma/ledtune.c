#include "../common/svc/svc_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_timer.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_timer.h"
#include "ledtune.h"

static void _ledtune_on_timer_expired( void * svcptr, TiEvent * e );
static void _ledtune_set_luminance( TiLedTune * ledtune, uint16 lum );
static void _ledtune_adjust_ledcount( TiLedTune * ledtune, uint16 env_lum );


/* ledtune_open()
 * 
 * attention
 * - You must call target_init() before calling this function or else ledtune_open()
 * will encounter unexpected hardware initialization error.
 * - You also need to guarantee the global interrupt has been enabled because the 
 * timer will fire interrupts.
 */
TiLedTune * ledtune_construct( void * mem, uint16 memsize, TiTimer * timer )
{
	TiLedTune * ledtune;

	/* The timer should be initialized first */

	hal_assert( sizeof(TiLedTune) <= memsize );
	ledtune = (TiLedTune *)mem;
	ledtune->luminance = 0;
	ledtune->timer = vti_open( timer, _ledtune_on_timer_expired, ledtune );
	hal_assert( timer != NULL );

    return ledtune;
}

void ledtune_destroy( TiLedTune * ledtune )
{
	ledtune_close( ledtune );
}

TiLedTune * ledtune_open( TiLedTune * ledtune )
{
	led_off( LED_RED );
	vti_setscale( ledtune->timer, 1 );
	vti_setinterval( ledtune->timer, 1000, 1 );
	vti_start( ledtune->timer );
    return ledtune;
}

void ledtune_close( TiLedTune * ledtune )
{
	vti_stop( ledtune->timer );
	led_off( LED_RED );
}

void ledtune_write( TiLedTune * ledtune, uint16 lum )
{
	// _ledtune_adjust_ledcount( ledtune, lum );
	_ledtune_set_luminance( ledtune, lum );
}


void _ledtune_on_timer_expired( void * svcptr, TiEvent * e )
{
	led_toggle( LED_RED );
}

/* adjust LED count. If the environment is very bright, then there's only the RED
 * led flashes. If it's very dark, the all the three leds will flash.
 * 
 * @param
 *	env_lum   luminance of the environment
 */
void _ledtune_adjust_ledcount( TiLedTune * ledtune, uint16 env_lum )
{	
	led_off(LED_YELLOW);
	led_off(LED_GREEN);

	if (env_lum < 0x0100)
	{
		led_on(LED_YELLOW);
		led_on(LED_GREEN);
	    
	}
	else if (env_lum < 0x0230)
	{
		led_on(LED_GREEN);
	}
	else{
	}
	
}

void _ledtune_set_luminance( TiLedTune * ledtune, uint16 lum )
{
	// when it's dark, the lum sensor outputs a small value, and the frequency should be larger.
	int MIN_LUM = 0x0077;
	int MAX_LUM = 0x03AD;
	int MIN_PERIOD = 50;   // frequency 20
	int MAX_PERIOD = 500;  // frequency 2
	int period;

	if (lum < MIN_LUM) lum = MIN_LUM;
	if (lum > MAX_LUM) lum = MAX_LUM;

	period = (lum - MIN_LUM) * (MAX_PERIOD - MIN_PERIOD) / (MAX_LUM - MIN_LUM) + MIN_PERIOD;

	vti_setscale( ledtune->timer, 1 );
	vti_setinterval( ledtune->timer, period, 1 );
}
