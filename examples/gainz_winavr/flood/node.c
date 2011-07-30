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
/******************************************************************************
 * node.c
 * general node in the network
 *
 * @attention
 * Usage:
 *  Before you use this program, you should change the node address macro LOCAL_ADDRESS.
 * Every node in the network should have unique address. 
 * 
 * @author zhangwei on 20061106
 *	- first created
 * @modified by zhangwei on 20090802
 *	- revision. compile passed.
 * @modified by Shi-Miaojing and Yan-Shixing (TongJi University) on 20090802
 *	- testing...
 * @modified by ShiMiaojing rehearse the whole text logic and some details that  may not be compatible with our recent 
 * our modification in other files  compiled ok.
 * test ok good performance on NOvember 17 2009 
 *****************************************************************************/

#include "../../commom/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../commom/openwsn/hal/hal_foundation.h"
#include "../../commom/openwsn/hal/hal_cpu.h"
#include "../../commom/openwsn/hal/hal_interrupt.h"
#include "../../commom/openwsn/hal/hal_led.h"
#include "../../commom/openwsn/hal/hal_assert.h"
#include "../../commom/openwsn/hal/hal_uart.h"
#include "../../commom/openwsn/hal/hal_cc2420.h"
#include "../../commom/openwsn/hal/hal_target.h"
#include "../../commom/openwsn/hal/hal_debugio.h"
#include "../../commom/openwsn/rtl/rtl_openframe.h"
#include "../../commom/openwsn/svc/svc_aloha.h"
#include "../../commom/openwsn/svc/svc_flood.h"

//#define CONFIG_TEST_LISTENER  
#define CONFIG_TEST_ADDRESSRECOGNITION
//#define CONFIG_TEST_ACK
//since I think there is no need to calling for an ACK 
//in aloha_bradocast ACK requeest bit has been cleared.  
#define PANID						0x0001
#define LOCAL_ADDRESS				0x0003
#define REMOTE_ADDRESS				0xffff
#define DEFAULT_CHANNEL				11

static TiCc2420Adapter              g_cc;
static TiUartAdapter	            g_uart;
static TiAloha						g_aloha;
static char                         g_rxbufmem[OPF_SUGGEST_SIZE];
static TiTimerAdapter               g_timer;
static TiFloodNetwork			    g_net;

#ifdef CONFIG_TEST_LISTENER
static void _aloha_listener( void * ccptr, TiEvent * e );
#endif

static void floodnode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );


/******************************************************************************
 * attention: 
 *	The simple node needn't to process the frames to be routed, because the "svc_flood"
 * module will deal with them. 
 *****************************************************************************/


int main(void)
{
	floodnode();
}

void floodnode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
    TiAloha * mac;
	TiFloodNetwork * net;
	TiOpenFrame * opf;
	TiTimerAdapter   *timer;
	char * msg = "welcome to aloha recv test...";
	uint8  len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	dbo_open( 0, 38400 );

	led_open();
	led_on( LED_RED );
	hal_delay( 1000 );
	led_off( LED_ALL );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
    mac = aloha_construct( (char *)(&g_aloha), sizeof(TiAloha) );
    timer = timer_construct( (char *)(&g_timer),sizeof(TiTimerAdapter) );
	net = flood_construct( (void *)(&g_net), sizeof(TiFloodNetwork) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    cc2420_open(cc, 0, NULL, NULL, 0x00 );
    //cc2420_settxpower( cc, CC2420_POWER_1);// changge power of the rx node 


	#ifdef CONFIG_TEST_LISTENER
	mac = aloha_open( mac,cc,DEFAULT_CHANNEL,PANID,LOCAL_ADDRESS,timer, _aloha_listener, NULL,0x01);
	#else
	mac = aloha_open( mac,cc,DEFAULT_CHANNEL,PANID,LOCAL_ADDRESS,timer,NULL, NULL,0x01);//0x00 01??   牵扯到ack 要一致！
	#endif


	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	//cc2420_enable_addrdecode( cc );					//使能地址译码
	#endif
	
	
	#ifdef CONFIG_TEST_LISTENER
	net = flood_open( net, mac, NULL, _flood_listener, NULL, PANID, LOCAL_ADDRESS );
	#else
	net = flood_open( net, mac, NULL, NULL, PANID, LOCAL_ADDRESS );
	#endif

  
     opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem) ,OPF_DEF_FRAMECONTROL_DATA , OPF_DEF_OPTION );

	hal_enable_interrupts();	

	#ifdef CONFIG_TEST_LISTENER
	while (1) {}
	#endif

	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{    
	
		flood_evolve( net, NULL );
		len = flood_recv( net, opf, 0x00 );
		if (len > 0)
		{   
		     
		_output_openframe( opf, &g_uart );

			opf_shortaddrto( opf);

			if (opf->msdu[2])
				led_on( LED_RED );
			else
				led_off( LED_RED );

			hal_delay(100);
		}
	}
	#endif
}

#ifdef CONFIG_TEST_LISTENER
void _flood_listener( void * owner, TiEvent * e )
{
	TiFloodNetwork * net = &g_flood;
    TiOpenFrame * opf = (TiOpenFrame *)g_rxbufmem;

	uart_putchar( &g_uart, 0x77 );
	led_toggle( LED_RED );
	while (1)
	{
		if (flood_recv(mac, opf, 0x00) > 0)
		{
		//	;_output_openframe( opf, &g_uart ); // used for uart test to check whether it has recieved data
		}
	}
}
#endif

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	if (opf->datalen > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (!opf_parse(opf, 0))
		{
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0] );
		}
		else{
	
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\n' );
	}
	// todo: LED
}


