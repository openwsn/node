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

#include "../../common/openwsn/hal/hal_configall.h"  
#include "../../common/openwsn/svc/svc_configall.h"  
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_nio_one2many.h"

#define CONFIG_NODE_ADDRESS 0x02
#define CONFIG_NODE_PANID 0x01
#define CONFIG_NODE_CHANNEL 11
#define CONFIG_RESPONSE_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE 128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiTimerAdapter 		m_timeradapter;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiFrameRxTxInterface m_rxtx;
static TiAloha              m_aloha;
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
static TiOne2Many 			m_o2m;
static TiAdcAdapter         m_adc;
static TiLumSensor          m_lum;
static char                 m_nacmem[NAC_SIZE];

int main(void)
{

	uint16 value;
	uint8 len;
	uint16 addr;
	char * request;
	char * response;
	char * msg = "welcome to wlssensor node...";

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;

	TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;

	TiAdcAdapter * adc;
	TiLumSensor * lum;

	TiOne2Many * o2m;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );

	timeradapter = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	
	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );

	adc = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	lum = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );

	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );

	cc = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	timeradapter = timer_open( timeradapter, 0, vtm_inputevent, vtm, 0x01 ); 
	nac = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timeradapter, NULL, NULL,0x01);

	adc_open( adc, 0, NULL, NULL, 0 );
	lum_open( lum, 0, adc );

	rxbuf = iobuf_construct( (void *)(&m_rxbuf[0]), sizeof(m_rxbuf) );
	txbuf = rxbuf;
	
    o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	one2many_open( o2m, mac);

	//todo

	cc2420_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	//todo

	hal_enable_interrupts();

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
			for(uint8 i=0;i<len;i++)
				dbo_putchar(request[i]);

			// start measurement;
			// adc_start
			// adc_value
			value = lum_value( lum ); 
			led_toggle(LED_GREEN);
			response = iobuf_ptr( txbuf );
			response[0] = 0x02;                                   // set response type
			response[1] = request[3];                             // set destination address, 
			response[2] = request[4];                             // namely the gateway address

			addr = O2M_MAKEWORD( request[3],request[4]);

			response[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       // set source address, 
			response[4] = (char)(CONFIG_NODE_ADDRESS & 0x00FF);   // namely local address
			response[5] = (uint8)(value >> 8);
			response[6] = (uint8)(value & 0x0F);
			iobuf_setlength( txbuf, CONFIG_RESPONSE_SIZE );

			len = one2many_send( o2m,addr,txbuf, 0x00 );
			//len = one2many_send( o2m,0x01,txbuf, 0x00 );
			if (len <= 0)
			{
				dbo_string( "send response failed" );
			}
		}

		one2many_evolve(o2m, NULL);
	}
}
