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
 * floodsender.c
 * command sender node in the network
 *
 * Periodically sending LED on/off commands to control all the LEDs in the network.
 * 
 * @author zhangwei on 20061106
 *	- first created
 * @modified by zhangwei on 20090801
 *	- revision. compile passed.
 * @modified by Shi-Miaojing and Yan-Shixing (TongJi University) on 20090802
 *	- testing...
 * @modified by ShiMiaojing on 20091101 uncompiled need to modified svc_flood.c
 * test ok on November 17,2009  good performance.
 *****************************************************************************/
 
#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif 

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include "openwsn/hal/hal_mcu.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/rtl/rtl_configall.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_timer.h"
#include "openwsn/svc/svc_nio_acceptor.h"
#include "openwsn/svc/svc_nio_aloha.h"
#include "openwsn/svc/svc_nio_flood.h"

#define PANID				0x0001
#define BROADCAST_PAN       0xffff
#define LOCAL_ADDRESS		0x0009   
#define BROADCAST_ADDRESS	0xffff
#define DEFAULT_CHANNEL     11

#define UART_ID 1

#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)


static TiFrameRxTxInterface m_rxtx;
static char                 m_nacmem[NAC_SIZE];
static TiAloha              m_aloha;
static TiFloodNetwork       m_net;
static TiTimerAdapter       m_timer;
static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter             m_cc;
static TiUartAdapter        m_uart;

static void ledflood( uint16 ontime, uint16 offtime );

int main(void)
{
	// all the led will be turned on for 500 ms and off for 5000 ms 
	ledflood( 1000, 1000 );
}

void ledflood( uint16 ontime, uint16 offtime )
{
    TiCc2520Adapter * cc;
	TiUartAdapter * uart;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;
	TiFloodNetwork * net;
	TiFrame * txbuf;
    TiTimerAdapter * timer2;
	uint8 state;
	char * msg = "welcome to floodsender";
	uint8 seqid=0;
	int k;

	char * pc;//todo

    /***************************************************************************
	 * Device Startup and Initialization 
     **************************************************************************/
	 
	target_init();

	led_open();
	led_on(LED_ALL);
	hal_delayms( 1000 );
	led_off( LED_ALL );

	dbc_mem(msg, strlen(msg));

	//halUartInit(9600,0);
	uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);
    /***************************************************************************
	 * Flood Startup
     **************************************************************************/

	cc = cc2520_construct( (char *)&m_cc, sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)&m_aloha, sizeof(TiAloha) );
	net = flood_construct( (void*)&m_net, sizeof(TiFloodNetwork) );
    timer2= timer_construct( (char *)(&m_timer), sizeof(TiTimerAdapter));

	// open the transceiver driver. we use TiCc2420Adapter in this example.
	
	cc2520_open( cc, 0, NULL, NULL, 0x00 );
    // cc2420_settxpower( cc, CC2420_POWER_2 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nac = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	hal_assert( nac != NULL ); 
	
	// open the medium access control protocol for sending and receiving
	// should use a 16bit or 32bit timer for the MAC. 8 bit timer will lead to wrong
	// delay results.
	
	timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
	mac = aloha_open( mac, rxtx, nac, DEFAULT_CHANNEL, PANID,LOCAL_ADDRESS, timer2, 
		NULL, NULL, 0x00);
	//flood_open( net, (TiNioNetLayerDispatcher *) mac, NULL, NULL, PANID, LOCAL_ADDRESS );
	net = flood_open( net, mac, NULL, NULL, PANID, LOCAL_ADDRESS );
	hal_assert( (timer2 != NULL) && (mac != NULL) );
	state = 0;
	while(1) 
	{
		txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 25, 6 );
		pc = frame_startptr( txbuf );
		pc[0] = state;
		state = !state; 
//        pc[0] = !pc[0]; // command byte, controls the LED state
		pc[1] = seqid ++;
		for (k = 2; k<6; k++)
		{
			pc[k] = '0' + k;
		}
		
		frame_setlength( txbuf, 6);

		while (1)
        {
            if (flood_broadcast(net, txbuf, 0x00) > 0)
            {   
                led_toggle( LED_RED);
                break;
            }
			
			// delay some time before retry in order to avoid occupy the wireless 
			// channel all the time.
            hal_delayms(50);                       
        }
		

		aloha_evolve( mac, NULL );
		flood_evolve( net, NULL );

		// delay the LED long enough so we can observe its flash
		if( pc[0])
			hal_delayms( 200 );
		else
			hal_delayms( 1000 );
	}	
}
