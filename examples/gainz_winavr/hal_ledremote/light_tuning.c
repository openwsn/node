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
#include "light_tuning.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_debugio.h"

static TiTimerAdapter g_timer;
static volatile uint8 g_count = 0;
static volatile uint16 g_env_lum = 0;

void timer_query_driven_test( uint8 id );
void timer_interrupt_driven_test( uint8 id );
void on_timer_expired( void * object, TiEvent * e );

int light_tune_init(void)
{
	// the parameter id can be 0-3 denoting timer hardware 0 to 3
	uint8 id = 0;

	TiTimerAdapter *timer;

	//hal_enable_interrupts();
	// assert: hal_enable_interrupts
    timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, id, on_timer_expired, NULL, 0x01 ); 
    timer_setinterval( timer, 8, 1 );
	timer_start( timer );

	return 0;
}

int light_tune_by_luminance( uint16 lum )
{
	// when it's dark, the lum sensor outputs a small value, and the frequency should be larger.
	int MIN_LUM = 0x000C;
	int MAX_LUM = 0x1FFC;
	int MIN_FREQ = 2;
	int MAX_FREQ = 20;
	int freq;

	if (lum < MIN_LUM) lum = MIN_LUM;
	if (lum > MAX_LUM) lum = MAX_LUM;

	freq = (MAX_LUM - lum) / (MAX_LUM - MIN_LUM) * (MAX_FREQ - MIN_FREQ) + MIN_FREQ;

	g_env_lum = freq * 8;
}

void on_timer_expired( void * object, TiEvent * e )
{
	g_count ++;
	if (g_count == 100)
	{
	    led_toggle( LED_RED );
		g_count = 0;

	}
    
/*	if (g_count == 150)
    {
        timer_close( &g_timer );
        timer_destroy( &g_timer );
		g_count = 0;
		led_off(LED_RED);
    }
	*/
}

