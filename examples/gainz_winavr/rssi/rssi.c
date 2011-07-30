/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005-2010 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_target.h"

/*****************************************************************************
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 *
 * @author zhangwei on 20070423
 * 	- revision today.
 * @modified by yan-shixing 20090725
 *  - revision. tested ok
 ****************************************************************************/ 

#define CONFIG_LISTENER    
//#undef  CONFIG_LISTENER    

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01
#define REMOTE_ADDRESS		0x00
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11


TiCc2420Adapter             g_cc;
TiUartAdapter               g_uart;
//char                        g_rxbuf[BUF_SIZE];
char                        g_rxbufmem[ OPF_SUGGEST_SIZE ];


#ifdef CONFIG_LISTENER
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif

static void rssi(void);
static void _format_output( char * buf, int len, TiUartAdapter * uart, TiCc2420Adapter * cc );

int main(void)
{
    rssi();
}

void rssi(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
	char * msg = "welcome to rssi...";
	#ifndef CONFIG_LISTENER
    uint8 len;
    #endif

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	#ifdef CONFIG_LISTENER
	cc = cc2420_open( cc, 0, _cc2420_listener, NULL, 0x00 );
	#else
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
	#endif

    /* @attention
     * - configure the cc2420 transceiver's adapter. currently, it's set to disable 
     * the address decoding. this means this node can receive any frames in the air.
     * but better to enable address decode, i think. 
     */
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_setpanid( cc, PANID );					//网络标识, seems no use in sniffer mode
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识, seems no use in sniffer mode
	cc2420_disable_addrdecode( cc );				// disable address recognition

    opf = opf_open( (void *)(&g_rxbufmem[0]), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	hal_enable_interrupts();
 
	#ifdef CONFIG_LISTENER
	while(1) {}
	#endif

	#ifndef CONFIG_LISTENER
	while(1) 
	{
		len = cc2420_read( cc, opf->buf, opf->size, 0x00 );
		if (len > 0)
		{
			opf_set_datalen( opf, len );
			led_toggle(LED_RED);
            _format_output( &buf[0], len, uart, cc );
			hal_delay(50);
        }
		cc2420_evolve( cc );
	}
	#endif
}

#ifdef CONFIG_LISTENER
void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
	// char * msg = "_cc2420_listener: ";
	char buf[BUF_SIZE];
    uint8 len=0;

	// uart_putchar( &g_uart, '*' );
	// uart_putchar( &g_uart, 0x77 );
	// uart_write( &g_uart, msg, strlen(msg), 0x01 );	

	while (1)
	{
        // try to receive an frame from the cc2420 adapter object. the listener will
        // only break the while loop until all the frames inside cc2420 adapter object
        // are read out.
        //
		len = cc2420_read(cc, &buf[0], BUF_SIZE, 0x00);
		if (len > 0)
        {
			led_toggle(LED_RED);
            _format_output( &buf[0], len, &g_uart, cc );
        }
        else
    		break;
	}
}
#endif

/** 
 * output the frame and rssi information
 *
 * @param buf where the frame is placed
 * @param len length of the data inside the frame
 * @return null
 */
void _format_output( char * buf, int len, TiUartAdapter * uart, TiCc2420Adapter * cc )
{
    if (len > 3)
    {
        // output two 0x88 character as the start flag of the current frame
        uart_putchar( uart, 0x88 );
        uart_putchar( uart, 0x88 );

        // output the length of the frame received inside buffer. 
        // warning: i'm not sure whether the data inside buf by cc2420_read 
        // contains the len value has already count the last two bytes(RSSI byte
        // and the LQI byte)
        uart_putchar( uart, len );

        // output the whole frame
        // for(int i=0;i<len;i++)
        // 	uart_putchar(uart, buf[i]);

        // this will be the RSSI byte according to cc2420 datasheet
        uart_putchar( uart, buf[len-2] );

        // this will be the LQI byte according to cc2420 data
        uart_putchar( uart, buf[len-1] );

        uart_putchar( uart, cc2420_rssi(cc) );
        uart_putchar( uart, cc2420_lqi(cc) );

        // 0x99 as a special flag to indicate the end of this frame's output
        uart_putchar(uart, 0x99);
    }
}

