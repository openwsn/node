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

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "../../../common/openwsn/hal/hal_mcu.h"
#include "../../../common/openwsn/hal/hal_configall.h"  
#include "../../../common/openwsn/svc/svc_configall.h"  
#include "../../../common/openwsn/rtl/rtl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
//#include "../../common/openwsn/hal/hal_adc.h"
//#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../../common/openwsn/hal/hal_timer.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/hal/hal_cc2520.h"
#include "../../../common/openwsn/hal/hal_uart.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
#include "../../../common/openwsn/svc/svc_foundation.h"
#include "../../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../../common/openwsn/svc/svc_nio_aloha.h"
//#include "../../common/openwsn/svc/svc_timer.h"
//#include "../../common/openwsn/svc/svc_ledtune.h"
#include "../../../common/openwsn/svc/svc_nio_one2many.h"

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiTimerAdapter 		m_timer1;
static TiTimerAdapter 		m_timer2;
static TiTimerAdapter 		m_timer3;
static TiAloha              m_aloha;
static TiFrameRxTxInterface m_rxtx;
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
static TiOne2Many 			m_o2m;
static char                 m_nacmem[NAC_SIZE];

static char                 m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

static TiUartAdapter		m_uart;


TiCc2520Adapter m_cc; 		 //JOE



/* output the TiIoBuf content throught UART by default */

int main(void)
{
	TiUartAdapter * uart;			//JOE


	char * request;
	char * response;
	uint8 len;	
	char * msg = "welcome to gateway node...";

	TiTimerAdapter * timer2;
	TiTimerAdapter * timer1;
    TiTimerAdapter * timer3;

	TiCc2520Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;

	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
	TiOne2Many * o2m;

    TiFrame * mactxbuf;

    int i;

	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );
    
	//JOE
    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart,0, 9600, 8, 1, 0 );
    hal_assert( uart != NULL );
    //halUartInit(9600,0);
	//JOE

	timer2  = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );
	timer1 = timer_construct( (void *)(&m_timer1), sizeof(m_timer1) );
    timer3  = timer_construct( (void *)(&m_timer3), sizeof(m_timer3) );

	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );

    rxbuf = iobuf_construct( &m_rxbuf[0], sizeof(m_rxbuf) );
	txbuf = rxbuf;

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	cc   = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	nac  = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
    timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
    timer3 = timer_open( timer3, 3, NULL, NULL, 0x00 ); 
    timer1 = timer_open( timer1, 4, NULL, NULL, 0x00 ); 
	mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timer2, NULL, NULL,0x00);

	mac->txbuf = mactxbuf;

	o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	//one2many_open( o2m, mac);
	one2many_open( o2m, mac,timer1,GATEWAYTYPE);

	//todo

	cc2520_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2520_rxon(cc);							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode


    timer_setinterval( timer3,1000,7999); //JOE 1000
	while(1)
	{ 
		hal_delayms(500);  
		request = iobuf_ptr( txbuf );
		request[0] = 0x01;              // request type
		request[1] = 0xFF;              // set destination address
		request[2] = 0xFF;              // 0xFFFF is the broadcast address
		request[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       
		                                // set source address, 
		request[4] = (char)(CONFIG_NODE_ADDRESS & 0xFF);   
		                                // namely local address
        request[5] = 0x06;
        request[6] = 0x07;
		iobuf_setlength( txbuf, CONFIG_REQUEST_SIZE );
		len = one2many_broadcast( o2m, txbuf, 0x00 );
		if (len <= 0)                   // retry when sending request failed
		{
			hal_delayms( 250 );
			continue;
		}
		else//todo for testing
		{
			//USART_Send( 0xf0);//todo for testing	
			uart_putchar(uart,0xf0);	
			//led_toggle( LED_RED);//JOE	
		}

		// configure the time interval as 2 seconds. non-periodical
		// the "vti" timer will automatically stopped when it's expired.
		//
		timer_start(timer3);

		while (!timer_expired( timer3))
		{
            iobuf_clear( rxbuf);//todo for testing
			len = one2many_recv( o2m, rxbuf, 0x00 );
			
			if (len > 0)
			{	
				response = iobuf_ptr( rxbuf );

                for ( i=0;i<len;i++)//todo for testing
                {
                    //USART_Send( response[i]);//todo for testing
					uart_putchar(uart,response[i]);		

                }

				if(response[0] == 0x02)
				{
                    led_toggle( LED_RED);
				}
			}
		}	
		one2many_evolve(o2m, NULL);
	}
}
