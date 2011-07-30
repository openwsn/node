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

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h> 
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "echonode.h"

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

TiCc2420Adapter             g_cc;
TiUartAdapter	            g_uart;

static void _change(uint8 x,uint8 y);

void echonode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	char * msg = "welcome to echonode...";
	uint8 buf[BUF_SIZE];
	uint8 len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_on(LED_ALL);
	
	hal_delay( 500 );
	led_off( LED_ALL );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							// Enable RX
	cc2420_enable_addrdecode( cc );					// 使能地址译码
	cc2420_setpanid( cc, PANID );					// 网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	// 网内标识

	hal_enable_interrupts();
 
	while(1) 
	{   
	   
		cc2420_evolve( cc );
		len = cc2420_read( cc, (char*)(&buf[0]), BUF_SIZE, 0x00 );
		if (len > 6)
		{
			uart_putchar( uart, '>' );
            
           

			// switch source address and destination address
			_change(buf[3], buf[7]);
			_change(buf[4], buf[8]);
			_change(buf[5], buf[9]);
			_change(buf[6], buf[10]);
	  
			buf[29] = 56; // simulate data read from sensor
			cc2420_write( cc, (char*)(&buf[0]), len, 0x00 );
			hal_delay( 10 );
		
			// @attention
			// You needn't call cc2420_setrxmode() here, because the hal_cc2420 
			// module will handle it inside. 
			// cc2420_setrxmode( cc ); 
		}
	}
}

void _change(uint8 x,uint8 y)
{
	char t;
	t=x;
	x=y;
	y=t;
}

