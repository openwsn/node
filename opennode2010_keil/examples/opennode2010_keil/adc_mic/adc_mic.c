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
#include "../common/hal/hal_microphone.h"

//请仿效adc_light.c开发本程序

// @attention
// since light sensor uses ADC channel 0, we suggest using other ADC channels
// here to do with microphone.

//int main(void)
//{
//}

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_microphone.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"

TiAdcAdapter  g_adc;
TiMicrophone g_mic;
TiUartAdapter g_uart;

int main(void)
{
	TiAdcAdapter * adc;
	TiMicrophone * mic;
	TiUartAdapter * uart;
    char * welcome = "welcome to adc_mic";
    uint16 val;
	char buf[20];

	// in the future, you should eliminate the following macro
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );
	dbo_open(0, 38400);

	adc = adc_construct( (void *)&g_adc, sizeof(TiAdcAdapter) );
	mic = mic_construct( (void *)&g_mic, sizeof(TiMicrophone) );
	uart = uart_construct( (void *)&g_uart, sizeof(TiUartAdapter) );

	adc_open( adc, 0 , NULL, NULL, 0 );
	mic_open( mic, 0, adc );
	uart_open( uart, 0, 38400, 8, 1, 0 );
    uart_write( uart, welcome, sizeof(welcome), 0x00 );


	while (1)
	{
		dbo_putchar( '>' );
		val = mic_value( mic, (char *) &buf, sizeof(buf) );
		if (val>0)
		{
			for(uint8 i=0;i<val;i++)
				dbo_n16toa( buf[i] );
		}
		else
			dbo_n16toa( val );
		hal_delay( 1000 );
	}

	mic_close( mic );
	adc_close( adc );
	uart_close( uart );
}

