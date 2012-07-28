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

#include "apl_foundation.h"
#include "../../../common/openwsn/hal/hal_mcu.h"
#include "../../../common/openwsn/hal/hal_configall.h"  
#include "../../../common/openwsn/svc/svc_configall.h"  
#include "../../../common/openwsn/rtl/rtl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
#include "../../../common/openwsn/hal/hal_timer.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/hal/hal_cc2520.h"
#include "../../../common/openwsn/hal/hal_uart.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
//#include "../../common/openwsn/hal/hal_adc.h"
//#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../../common/openwsn/svc/svc_foundation.h"
#include "../../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../../common/openwsn/svc/svc_nio_one2many.h"

#define CONFIG_NODE_ADDRESS 0x11//02
#define CONFIG_NODE_PANID 0x01
#define CONFIG_NODE_CHANNEL 11
#define CONFIG_RESPONSE_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE 128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)


static TiUartAdapter		m_uart;
static TiTimerAdapter 		m_timer2;	   
static TiTimerAdapter 		m_timer1;
static TiFrameRxTxInterface m_rxtx;
static TiAloha              m_aloha;
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
static TiOne2Many 			m_o2m;
static TiCc2520Adapter		m_cc;
static char                 m_nacmem[NAC_SIZE];
static char                 m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

int main(void)
{

	uint16 value;
	uint8 len;
    uint8 i;
	uint16 addr;
	char * request;
	char * response;
	char * msg = "welcome to wlssensor node...";

	TiTimerAdapter * timer2;
	TiTimerAdapter * timer1;

	TiCc2520Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;
	TiUartAdapter * uart;


	TiOne2Many * o2m;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;

    TiFrame * mactxbuf;


	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

//	halUartInit(9600,0);
	timer2 = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );
	timer1 = timer_construct( (void *)(&m_timer1), sizeof(m_timer1) );	
	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );

	//adc = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	//lum = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );


	cc = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 ); 
	timer1 = timer_open( timer1, 3, NULL, NULL, 0x00 ); 

	nac = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timer2, NULL, NULL,0x00);

	//adc_open( adc, 0, NULL, NULL, 0 );
	//lum_open( lum, 0, adc );
    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart,1, 9600, 8, 1, 0 );

	rxbuf = iobuf_construct( (void *)(&m_rxbuf[0]), sizeof(m_rxbuf) );
	txbuf = rxbuf;

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

    mac->txbuf = mactxbuf;
	
    o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	one2many_open( o2m, mac,timer1,SENSORTYPE);

	//todo

	cc2520_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2520_rxon( cc );							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	//cc2420_enable_autoack( cc );

	//todo
    value = 0;
	while(1)
	{
		len = one2many_recv( o2m, rxbuf, 0x00 );
		
		if (len > 0)
		{
			// request[0] == 0x01 indicate this is REQUEST type. 0x02 indicate this is RESPONSE.
			//dbo_putchar(len);
			
			request = iobuf_ptr( rxbuf );
			if (request[0] != 0x01)
				continue;
			for( i=0;i<len;i++)
				uart_putchar(uart,request[i]);

			// start measurement;
			// adc_start
			// adc_value
			//value = lum_value( lum ); 
            //todo for testing
			led_toggle(LED_RED);
			response = iobuf_ptr( txbuf );
			response[0] = 0x02;                                   // set response type
			response[1] = request[3];                             // set destination address, 
			response[2] = request[4];                             // namely the gateway address

			addr = O2M_MAKEWORD( request[3],request[4]);

			response[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       // set source address, 
			response[4] = (char)(CONFIG_NODE_ADDRESS & 0x00FF);   // namely local address
			response[5] = (uint8)(value >> 8);
			response[6] = (uint8)(value & 0xfF);
			iobuf_setlength( txbuf, CONFIG_RESPONSE_SIZE );


			len = one2many_send( o2m,addr,txbuf, 0x01 );	   //00
			//len = one2many_send( o2m,0x01,txbuf, 0x00 );
			if (len <= 0)
			{
				//dbo_string( "send response failed" );
                uart_putchar(uart, 0x00);
			}
			value++;
		}

		one2many_evolve(o2m, NULL);
	}
}
