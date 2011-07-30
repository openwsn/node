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
 * adc_light
 *
 * @state
 *	tested ok
 * @author xu-xizhou(TongJi University), zhangwei(TongJi University) in 200907
 *	- first created
 * @modified by zhangwei on 20110614
 *  - revised.
 ******************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_luminance.h"

TiAdcAdapter  g_adc;
TiLightSensor g_light;
TiUartAdapter g_uart;

int main(void)
{
	TiAdcAdapter * adc;
	TiLightSensor * light;
	TiUartAdapter * uart;
    char * welcome = "welcome to adc_light";
    uint16 val;

	target_init();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_write( welcome, strlen(welcome) );

	adc = adc_construct( (void *)&g_adc, sizeof(TiAdcAdapter) );
	light = light_construct( (void *)&g_light, sizeof(TiLightSensor) );
	// uart = uart_construct( (void *)&g_uart, sizeof(TiUartAdapter) );

	// xiao-yuezhang changed the second parameter from 5 to 0
	adc_open( adc, 0, NULL, NULL, 0 );
	light_open( light, 0, adc );
	// uart_open( uart, 0, 38400, 8, 1, 0 );
    // uart_write( uart, welcome, strlen(welcome), 0x00 );


	while (1)
	{
		dbc_putchar( '>' );
		val = light_value( light );
		dbc_n16toa( val );
		hal_delay( 1000 );
	}

	light_close( light );
	adc_close( adc );
	uart_close( uart );
}
