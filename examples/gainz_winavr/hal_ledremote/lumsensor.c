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
/* This's a remote Luminance Sensor which can control the LED independently 
 * 
 * Reference
 *	- cc2420tx, cc2420rx
 *  - aloha
 *  - adc_luminance
 *  - isrqueue
 *
 * @author zhangwei, yanshixing(TongJi University) on 20091012
 *	- first developed
 */

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_debugio1.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"


#ifdef CONFIG_DEBUG
    #define GDEBUG
#endif

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define HIGH_BYTE(val) ((uint8)(val >> 8))
#define LOW_BYTE(val) ((uint8)(val & 0xFF))

static TiCc2420Adapter		g_cc;
static TiUartAdapter		g_uart;
static TiAdcAdapter         g_adc;
static TiLumSensor          g_lum;
static char                 g_opfmem[ OPF_SUGGEST_SIZE ];

void _lumsensor(void);


int main(void)
{
	_lumsensor();
}

void _lumsensor(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiAdcAdapter * adc;
	TiLumSensor * lum;
	TiOpenFrame * opf;
    uint16 val;

	char * msg = "welcome to lumsensor node...";
	uint8 i, total_length, seqid=0, option, len;
    uint16 fcf;

	target_init();
	HAL_SET_PIN_DIRECTIONS();

	wdt_disable();
	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );
	dbo_open( 38400);

	cc		= cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart    = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	adc     = adc_construct( (void *)&g_adc, sizeof(TiAdcAdapter) );
	lum     = lum_construct( (void *)&g_lum, sizeof(TiLumSensor) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	adc_open( adc, 0, NULL, NULL, 0 );
	lum_open( lum, 0, adc );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	cc2420_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK;            // 0x8821;    

	opf = opf_open( (void *)(&g_opfmem), sizeof(g_opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	hal_enable_interrupts();

	while(1)  
	{
		total_length = 30;					     
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, PANID );
		opf_set_shortaddrto( opf, REMOTE_ADDRESS );
		opf_set_panfrom( opf, PANID );
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );

		for (i=0; i<opf->msdu_len; i++)
			opf->msdu[i] = i;

		// using ACK mechanism in frame transmission
		option = 0x01;

		// read luminance data from the sensor
		val = lum_value( lum );
		dbo_n16toa( val );
		dbo_putchar(' ');
		opf->msdu[0] = LOW_BYTE(val);
		opf->msdu[1] = HIGH_BYTE(val);

		while (1)
        {
            if ((len=cc2420_write(cc, (char*)(opf_buffer(opf)), opf_datalen(opf), option)) > 0)
            {
				led_off( LED_RED );
				hal_delay( 10 );
				led_on( LED_RED );
				hal_delay( 10 );               
	 			// uart_putchar( &g_uart, len );
                // uart_putchar( &g_uart, seqid );
				//dbo_n8toa( len );
				//dbo_n8toa( opf_sequence(opf) );
                break;
            }
            hal_delay(10);
        }
		
		cc2420_evolve( cc );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay( 50 );
	}

	cc2420_destroy( cc );
	adc_destroy( adc );
	lum_destroy( lum );
	uart_destroy( uart );
}
