/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
#include "svc_configall.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_led.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_timer.h"
#include "svc_foundation.h"
#include "svc_ledtune.h"

static void _ledtune_on_timer_expired( void * svcptr, TiEvent * e );
static void _ledtune_set_luminance( TiLedTune * ledtune, uint16 lum );
static void _ledtune_adjust_ledcount( TiLedTune * ledtune, uint16 env_lum );


TiLedTune * ledtune_construct( void * mem, uint16 memsize )
{
}

void ledtune_destroy( TiLedTune * ledtune )
{
}

/* ledtune_open()
 * attention
 * - You must call target_init() before calling this function or else ledtune_open()
 * will encounter unexpected hardware initialization error.
 * - You also need to guarantee the global interrupt has been enabled because the 
 * timer will fire interrupts.
 */
TiLedTune * ledtune_open( TiLedTune * ledtune, uint16 freq )
{
	TiTimerAdapter * timer;
    timer = timer_construct( (void *)&(ledtune->timer), sizeof(ledtune->timer) );
    timer = timer_open( timer, 0, _ledtune_on_timer_expired, NULL, 0x01 ); 
	hal_assert( timer != NULL );

    timer_setinterval( timer, 8, 1 );
	timer_start( timer );
}

void ledtune_close( TiLedTune * ledtune )
{
}

void ledtune_write( TiLedTune * ledtune, uint16 lum )
{
	_ledtune_adjust_ledcount( lum );
	_ledtune_set_luminance( lum );
}


void _ledtune_on_timer_expired( void * svcptr, TiEvent * e )
{
	TiLedTune * ledtune = (TiLedTune *)svcptr;
	ledtune->count ++;
	if (ledtune->count == ledtune->delaycount)
	{
		led_toggle( LED_RED );
		ledtune->count=0;
	}
}

// 调节LED灯的亮度
// Input
//	env_lum   luminance of the environment
// Output 
//	None
//
void _ledtune_adjust_ledcount( TiLedTune * ledtune, uint16 env_lum )
{	
	led_off(LED_YELLOW);
	led_off(LED_GREEN);

	if (env_lum < 0x0100)
	{
        // todo
		//led_off(LED_ALL);
		//led_on(LED_RED);
		led_on(LED_YELLOW);
		led_on(LED_GREEN);
	    
	}
	else if (env_lum < 0x0230)
	{
        // todo
		//led_off(LED_ALL);
		//led_on(LED_YELLOW);
		led_on(LED_GREEN);

	}
	else
	{
        // todo
		//led_off(LED_ALL);
		//led_on(LED_YELLOW);
	}
	
}

void _ledtune_set_luminance( TiLedTune * ledtune, uint16 lum )
{
	// when it's dark, the lum sensor outputs a small value, and the frequency should be larger.
	int MIN_LUM = 0x0077;
	int MAX_LUM = 0x03AD;
	int MIN_FREQ = 2;
	int MAX_FREQ = 20;
	int freq;

	if (lum < MIN_LUM) lum = MIN_LUM;
	if (lum > MAX_LUM) lum = MAX_LUM;

	freq = (MAX_FREQ - MIN_FREQ) * (MAX_LUM - lum) / (MAX_LUM - MIN_LUM) + MIN_FREQ;

	ledtune->delaycount = 1000/8/freq;

	if(ledtune->delaycount < ledtune->count)
		ledtune->count=0;
	
	return 0;
}
