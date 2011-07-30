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
 * aloha_echo
 * The receiving test program based on ALOHA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * @state
 *	still in developing. test needed
 *
 * @author Shi-Miaojing on 20090802
 *	- first created
 * @modified by zhangwei on 20090802
 *	- revisioin. compile passed.
 * @tested by ShiMiaojing on 20090804 
 * @ modified by Shimiaojing on 20091103/04 test ok works well.
 * about frame structure : source addr & destination may be defined different with 
 * cc2420 protocal. 
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
#include "../common/svc/svc_aloha.h"

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define LOCAL_ADDRESS		        0x02
#define REMOTE_ADDRESS		        0x01

static  TiCc2420Adapter             g_cc;
static  TiUartAdapter	            g_uart;
static  TiAloha						g_aloha;
static  char                        g_rxbufmem[OPF_SUGGEST_SIZE];
static  TiTimerAdapter              g_timer;

static void echonode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{
	echonode();
}

void echonode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
    TiAloha * mac;
	TiOpenFrame * opf;
	TiTimerAdapter   *timer;
	char * msg = "welcome to aloha_echo-RX";
	uint8 len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	dbo_open( 0, 38400 );
	led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );
	led_on( LED_RED );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
    mac = aloha_construct( (char *)(&g_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&g_timer),sizeof(TiTimerAdapter));
    
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open(cc, 0, NULL, NULL, 0x00 );
	mac = aloha_open( mac, cc, CONFIG_ALOHA_DEFAULT_CHANNEL, CONFIG_ALOHA_DEFAULT_PANID, LOCAL_ADDRESS, timer,NULL, NULL,0x00);
	
    #ifdef CONFIG_TEST_ADDRESSRECOGNITION
    cc2420_enable_addrdecode( cc );					//使能地址译码
	#endif
	
	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif

    opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	hal_enable_interrupts();
	
	while(1)
	{
		len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{    
			dbo_putchar( 'R' );
			opf_set_datalen( opf, len );
			//_output_openframe(opf,uart);
			opf_swapaddress( opf );
			//hal_assert( opf_type(opf) == FCF_FRAMETYPE_DATA );
		
            // todo 200911
            // Shimiaojing: 如下两种情况：要求ACK和不要求ACK都要测试通过才行
			while (aloha_send(mac,opf,0x01)==0){}
			//while (aloha_send(mac,opf,0x00)==0){}
			dbo_putchar( 'S' );
			dbo_n8toa( *opf->sequence );
			_output_openframe(opf,uart);
	
            
		}
		
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
}
