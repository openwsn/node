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
/* ledremote
 * 该application将接收来自wireless luminance sensor的指令，调整led亮度
 * 形成一个环境光=>lum sensor => led light => 环境光的闭环演示
 * 
 * 该演示将用到简单的aloha mac协议，所以它也是一个演示如何使用mac层协议的例子
 *
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
#include "../../common/openwsn/hal/hal_debugio1.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_timer.h"


#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define MAKEWORD(low,high) ((low & 0xFF) | (((uint16)(high)) << 8))

TiCc2420Adapter             g_cc;
TiUartAdapter	            g_uart;
TiTimerAdapter g_timer;
char                        g_opfmem[OPF_SUGGEST_SIZE];
static volatile uint16 g_count = 0;
static volatile uint16 g_delaycount = 0;


static void _ledremote(void);
static void _cc2420_listener( void * ccptr, TiEvent * e );
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );
void _led_tune( uint16 env_lum );
void on_timer_expired( void * object, TiEvent * e );
int light_tune_by_luminance( uint16 lum );

int main(void)
{
	_ledremote();
}

void _ledremote(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
	TiTimerAdapter *timer;

	char * msg = "welcome to _ledremote...";

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	//led_on( LED_RED );

    timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    timer_open( timer, 0, on_timer_expired, NULL, 0x01 ); 
    timer_setinterval( timer, 8, 1 );
	timer_start( timer );

	dbo_open(38400);


	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	cc2420_open( cc, 0, _cc2420_listener, cc, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_enable_addrdecode( cc );				    //使能地址译码
	cc2420_setpanid( cc, PANID );				    //网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );    //网内标识
	cc2420_enable_autoack(cc);

	opf = opf_open( (void *)(&g_opfmem), sizeof(g_opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	hal_enable_interrupts();
 
	while(1) {};
}

void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
    TiOpenFrame * opf = (TiOpenFrame *)(&g_opfmem[0]);
    uint8 len=0;

	while (1)
	{
		// If there're more than 1 frames pending inside cc2420 adapter object, then
		// you should repeat to call cc2420_read() until it returns 0. 
		//
		len = cc2420_read(cc, (char*)opf_buffer(opf), opf_size(opf), 0x00);
		if (len > 0)
		{
			opf_set_datalen( opf, len );
			_output_openframe( opf , NULL );
		}
		else 
			break;
	}
}


void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	//uint8 i;

    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\r' );
		dbo_putchar( '\n' );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( '.' );
    }

	_led_tune( MAKEWORD(opf->msdu[0], opf->msdu[1]) );
	light_tune_by_luminance( MAKEWORD(opf->msdu[0], opf->msdu[1]) );
}

// 调节LED灯的亮度
// Input
//	env_lum   luminance of the environment
// Output 
//	None
//
void _led_tune( uint16 env_lum )
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

int light_tune_by_luminance( uint16 lum )
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

	g_delaycount = 1000/8/freq;

	if(g_delaycount<g_count)
	g_count=0;

	dbo_putchar((freq>>8));
	dbo_putchar(freq);

	dbo_putchar(0x88);

	dbo_putchar((g_delaycount>>8));
	dbo_putchar(g_delaycount);
	
	return 0;
}



void on_timer_expired( void * object, TiEvent * e )
{
	    g_count++;
		if(g_count==g_delaycount)
		{
			led_toggle( LED_RED );
			g_count=0;
		}
}







