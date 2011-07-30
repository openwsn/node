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

/*******************************************************************************
 * @author zhangwei on 20070629
 * - first created
 * @modified by zhangwei on 20090707
 * - hal_led module was modified by shi.miaojing(tongji university)
 ******************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_target.h"
#include "ledtest.h"

void led_test( void )
{     
	int i;

    target_init();

    led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	
	/* testing each single LED */

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
    led_off( LED_RED );
	
	i=0;
	while (i<3)
 	{
	    led_off(LED_GREEN) ; 
	    hal_delay(300);
	    led_on(LED_GREEN) ; 
	    hal_delay(300);
	    i++;
    }
    led_off( LED_GREEN );
    
	i=0;
    while (i<3)
 	{
	    led_off(LED_YELLOW) ; 
	    hal_delay(300);
	    led_on(LED_YELLOW) ; 
	    hal_delay(300);
	    i++;
	}
    led_off( LED_YELLOW );

	/* test the LEDs at the same time */
	
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

	/* testing LED twinkle 
     * there's a infinite loop in led_twinkle, so it will never come out*/

	led_off( LED_ALL ); 
    // led_twinkle( LED_RED, 300, 5 );
    // led_off( LED_RED );
	led_twinkle( LED_YELLOW, 300, 5 );
    led_off( LED_YELLOW );
    // led_twinkle( LED_GREEN, 300, 5 );
    // led_off( LED_GREEN );
}
