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
 * wireless sensor node (wlssensor, S nodes)
 * This demonstration/testing program will accept DATA_REQUEST frames from the gateway
 * node and start the measurement. The measured data will be encapsulated as DATA_RESPONSE
 * and be sent back to the gateway node (G node)
 * 
 * @author Yan-shixing and zhangwei(TongJi University) in 200911
 *	- first created
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 2

#include "../../common/openwsn/hal/hal_configall.h"  
#include "../../common/openwsn/svc/svc_configall.h"  
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_ledtune.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/svc/svc_nio_one2many.h"

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)


//static TiTimerAdapter 		m_timeradapter;
//static TiTimerManager 		m_vtm;
static TiTimerAdapter 		m_hwtimer0;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiFrameRxTxInterface m_rxtx;
static TiUartAdapter		m_uart;
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
static TiOne2Many 			m_o2m;
static TiLedTune            m_ledtune;
static char                 m_nacmem[NAC_SIZE];



/* output the TiIoBuf content throught UART by default */
void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart );

int main(void)
{
	char * request;
	char * response;
	uint8 len;	
	char * msg = "welcome to gateway node...";

	TiTimerAdapter * hwtimer0;
	TiTimerManager * vtm;
	TiTimer * vti;

	TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;

	TiUartAdapter * uart;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
	TiOne2Many * o2m;
    TiLedTune * ledtune;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );

	hwtimer0  = timer_construct( (void *)(&m_hwtimer0), sizeof(m_hwtimer0) );
	vtm = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	//uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	rxbuf = iobuf_construct( &m_rxbuf[0], sizeof(m_rxbuf) );
	txbuf = rxbuf;
	cc   = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	nac  = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,hwtimer0, NULL, NULL,0x01);

	//uart_open( uart, 0, 38400, 8, 1, 0x00 );
	hwtimer0	   	= timer_open( hwtimer0, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm             = vtm_open( vtm, hwtimer0, VTM_RESOLUTION);
	vti             = vtm_apply( vtm );
	vti             = vti_open( vti, NULL, NULL );

/*
	hwtimer0	   	= timer_open( hwtimer0, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm             = vtm_open( vtm, hwtimer0, GATW_VTM_RESOLUTION );
	vti             = vtm_apply( vtm );
	vti             = vti_open( vti, NULL, NULL );
*/


    //ledtune = ledtune_construct((char *)(&m_ledtune), sizeof(m_ledtune), vti );todo 为什么加上这两句定时器就不运行了呢？？？？
    //ledtune_open( ledtune );

	o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	one2many_open( o2m, mac);

	//todo

	cc2420_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	//todo

	//uart_write( uart, msg, strlen(msg), 0x00 );

	hal_enable_interrupts();

	while(1)
	{   
		request = iobuf_ptr( txbuf );
		request[0] = 0x01;              // request type
		request[1] = 0xFF;              // set destination address
		request[2] = 0xFF;              // 0xFFFF is the broadcast address
		request[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       
		                                // set source address, 
		request[4] = (char)(CONFIG_NODE_ADDRESS & 0xFF);   
		                                // namely local address
		iobuf_setlength( txbuf, CONFIG_REQUEST_SIZE );

		len = one2many_broadcast( o2m, txbuf, 0x00 );
		if (len <= 0)                   // retry when sending request failed
		{
			led_toggle(LED_RED);
			hal_delay( 250 );
			continue;
		}
		else//todo for testing
		{
			led_toggle( LED_RED);
			
		}

		// configure the time interval as 2 seconds. non-periodical
		// the "vti" timer will automatically stopped when it's expired.
		//
		vti_setscale( vti, 1 );
		vti_setinterval( vti, 1000, 0x00 );
		vti_start( vti );

		while (!vti_expired(vti))
		{
			len = one2many_recv( o2m, rxbuf, 0x00 );
			
			if (len > 0)
			{	
				led_toggle( LED_GREEN);
				response = iobuf_ptr( rxbuf );
				if(response[0] == 0x02)
				{
					_output_iobuf( rxbuf, uart );
                    //ledtune_write( ledtune, O2M_MAKEWORD(response[2], response[1]) );
				}
			}
		}
		
		one2many_evolve(o2m, NULL);
		vtm_evolve( vtm, NULL );
	}
}

void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart )
{
	char * response;

	if (iobuf_length(buf) > 0)
	{
		response = iobuf_ptr(buf);
		/*dbc_string( "response:\r\n" );

		dbc_string( "shortaddrto: " );
		dbc_n8toa( response[1] );
		dbc_n8toa( response[2] );

		dbc_string( "\r\nshortaddrfrom: " );
		dbc_n8toa( response[3] );
		dbc_n8toa( response[4] );

		dbc_string( "\r\nsensor value:" );
		dbc_n8toa( response[5] );
		dbc_n8toa( response[6] );
		dbc_string( "\r\n" );
		*/
		dbc_putchar(response[5]);
		dbc_putchar(response[6]);
	}
}
