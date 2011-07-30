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
 * @ authored by Shimiaojing,
 * this module  is the pre-work for floodnode . and it is matching aloha_brocadcast 
 *****************************************************************************/

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_debugio.h"
#include "../common/svc/svc_aloha.h"

//#define CONFIG_TEST_LISTENER  

#define PANID			            0x0001
#define LOCAL_ADDRESS				0x01  //actually no use 
#define REMOTE_ADDRESS				0x03  //actuallhy no use
#define DEFAULT_CHANNEL				11
#define BROADCAST_ADDRESS			0xFFFF


static  TiCc2420Adapter             m_cc;
static  TiUartAdapter	            m_uart;
static  TiAloha						m_aloha;
static  TiTimerAdapter              m_timer;

static  char                        m_rxbufmem[OPF_SUGGEST_SIZE];

uint8 len;

#ifdef CONFIG_TEST_LISTENER
static void _aloha_listener( void * ccptr, TiEvent * e );
#endif

static void broadrecvnode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{
	broadrecvnode();
}

void broadrecvnode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
    TiAloha * mac;
	TiOpenFrame * opf;
	TiTimerAdapter   *timer;
	char * msg = "welcome to aloha recv test...";
    #ifndef CONFIG_TEST_LISTENER
  
    #endif

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	dbo_open( 0, 38400 );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
    mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	#ifdef CONFIG_TSET_LISTENER
	cc = cc2420_open( cc, 0, _aloha_listener, NULL, 0x00 );
	#else
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
	#endif


	#ifdef CONFIG_TEST_LISTENER
	mac = aloha_open( mac, cc,DEFAULT_CHANNEL,PANID,LOCAL_ADDRESS,timer, _aloha_listener, NULL,0x00 );
	#else
	mac = aloha_open( mac, cc,DEFAULT_CHANNEL,PANID,LOCAL_ADDRESS,timer,NULL, NULL,0x00 );
	#endif
 
 	cc2420_enable_autoack( cc );
	
	cc2420_enable_addrdecode( cc );
 
    opf = opf_open( (void *)(&m_rxbufmem), sizeof(m_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

    hal_enable_interrupts();

	#ifdef CONFIG_TEST_LISTENER	
	while (1) {}
	#endif
    
	/* Query the TiCc2420Adapter object if there's no listener */
	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{
	
       	len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{   
			_output_openframe( opf,&m_uart);
			//led_off( LED_RED );

			led_toggle( LED_RED );
        }
	}
	#endif
}


#ifdef CONFIG_TEST_LISTENER
void _aloha_listener( void * owner, TiEvent * e )
{
	TiAloha * mac = &m_aloha;
    TiOpenFrame * opf = (TiOpenFrame *)m_rxbufmem;
	uart_putchar( &m_uart, 0x77 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{    
			_output_openframe( opf, &m_uart);
			led_toggle( LED_RED );
			break;
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
}


