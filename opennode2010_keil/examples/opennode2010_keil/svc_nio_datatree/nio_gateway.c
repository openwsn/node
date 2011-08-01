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
 * gateway node (gateway, G nodes)
 * This demonstration/testing program will send frames from the gateway
 * node and then wait for the feedback. 
 * 
 * @author zhangwei(TongJi University) and jiang ridong in 201103
 *	- first created
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_mcu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"  
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_timer.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
//#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
//#include "../../common/openwsn/hal/hal_adc.h"
//#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../../common/openwsn/rtl/rtl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/svc/svc_configall.h"  
#include "../../../common/openwsn/svc/svc_foundation.h"
#include "../../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../../common/openwsn/svc/svc_nio_datatree.h"


#define CONFIG_NODE_CHANNEL             11
#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_REMOTE_PANID				0x0002
#define CONFIG_REMOTE_ADDR				0x0001

#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

#define GATW_VTM_RESOLUTION 7

#define GATW_PHY_HEADER_LENGTH 1
#define GATW_MAC_HEADER_LENGTH 13

#define GATW_MAX_HOPCOUNT 5
#define GATW_PAYLOAD_LENGTH 2

#define GATW_TOTAL_SIZE (GATW_PHY_HEADER_LENGTH + GATW_MAC_HEADER_LENGTH + DTP_HEADER_SIZE(GATW_MAX_HOPCOUNT) + GATW_PAYLOAD_LENGTH)

#define HIGHBYTE(w) ((uint8)(w>>8))
#define LOWBYTE(w) ((uint8)(w & 0xFF))

static TiTimerAdapter 		m_timer2;
static TiTimerAdapter 		m_timer3;

static TiFrameRxTxInterface m_rxtx;
static TiAloha              m_aloha;
static char                 m_nacmem[NAC_SIZE];

static TiDataTreeNetwork    m_dtp;
static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                 m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

int main(void)
{
	uint8 len, i, count=0;
	char * msg = "welcome to gateway node...";
	char * request;
    char * payload;
	uint16 value;

	TiTimerAdapter * timer2;
    TiTimerAdapter * timer3;
	TiCc2520Adapter * cc;
	TiFrameRxTxInterface * rxtx;
    TiAloha * mac;
	TiNioAcceptor * nac;
	TiFrame * rxbuf;
	TiFrame * txbuf;
	TiDataTreeNetwork * dtp;

    TiFrame * mactxbuf;
    char * pc;


	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	halUartInit(9600,0);

	timer2        = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );
    timer3        = timer_construct( (void *)(&m_timer3), sizeof(m_timer3) );

	cc              = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac             = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac             = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	txbuf           = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	rxbuf           = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	//adc             = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	//lum             = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );

	cc              = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx            = cc2520_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nac             = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	hal_assert( nac != NULL ); 
	
	

	// configure the time interval as 2 seconds. non-periodical
	// the "vti" timer will automatically stopped when it's expired.
	timer2	   	= timer_open( timer2, 2, NULL, NULL, 0x00 );
    timer3	   	= timer_open( timer3, 3, NULL, NULL, 0x00 ); 
	mac             = aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timer2, NULL, NULL,0x00);

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    mac->txbuf = mactxbuf;


	dtp             = dtp_construct( (void *)(&m_dtp), sizeof(m_dtp) );
	dtp             = dtp_open( dtp, mac, CONFIG_NODE_ADDRESS, NULL, NULL, 0x01 );
	//adc             = adc_open( adc, 0, NULL, NULL, 0 );
	//lum             = lum_open( lum, 0, adc );
	//ledtimer        = vtm_apply( vtm );
	//ledtimer        = vti_open( ledtimer, NULL, NULL );
	//ledtune         = ledtune_construct( (void*)(&m_ledtune), sizeof(m_ledtune), ledtimer );
	//ledtune         = ledtune_open( ledtune );

	//todo
	
	cc2520_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2520_rxon( cc );							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	//cc2520_enable_autoack( cc );
	
	//todo
	
	//cc2420_settxpower( cc, CC2420_POWER_1);//cc2420_settxpower( cc, CC2420_POWER_2);CC2420_POWER_1
	//cc2420_enable_autoack( cc );

	

	count = 0;
	while(1)
	{
		/* Maintain the data tree structure in the network. This tree is used when 
		 * the sensor nodes send data response back to the root node. 
		 * 
		 * send maintain request every 4 data request.
		 */
		if (count % 4 == 0)
		{   
			txbuf    = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );//todo reset txbuf

			dtp_maintain( dtp, txbuf,GATW_MAX_HOPCOUNT );
            
			hal_delay( 200 );
		}

		// sending DTP_DATA_REQUEST packet and try to receive the DTP_DATA_RESPONSE.
        //
		// assume the frame length is 60 bytes. This length should larger than the
		// total value of PHY length byte(1B), MAC header size and DTP header size. 
		// You can adjust this value to adapt to your own application requirement.
		//
		//todo opf_cast( txbuf, GATW_TOTAL_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK );

		// move the luminance sensor value into payload field of the request
       txbuf    = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );//todo reset txbuf

        request = frame_startptr( txbuf );
		
		//payload = DTP_PAYLOAD_PTR(request);
		//value = lum_value( lum ); 
		//payload[0] = LOWBYTE( value );
		//payload[1] = HIGHBYTE( value );

		for( i=DTP_HEADER_SIZE( GATW_MAX_HOPCOUNT)+2; i < 25; i++)
			request[i] = 0x81;

		//frame_setlength( txbuf,frame_capacity( txbuf));//todo 
		frame_setlength( txbuf,21);

         while (1)
        {
		    if ( dtp_send_request(dtp, txbuf, GATW_MAX_HOPCOUNT) > 0)
                break;
            hal_delay( 200 );
        }

		// todo: you should check whether the packet is data response type.
		// try to receive the DTP_MAINTAIN_RESPONSE. If time out, then go back to the loop.
		// todo: you should check whether the packet is maintain response type.
	
		timer_setinterval( timer3,2000,7999);
		timer_start( timer3 );
		while (!timer_expired(timer3))
		{

			if( frame_empty( rxbuf))//todo
			{   

				frame_reset( rxbuf,3,20,0);//todo
			    len = dtp_recv( dtp, rxbuf, 0x00 );
			    if (len > 0)
			    {	
					pc = frame_startptr(rxbuf);
				    for ( i=0;i<len;i++)
				    {
                        USART_Send( pc[i]);
				    }

				    led_toggle( LED_RED );

					frame_totalclear( rxbuf);//todo for testing
			    }
				
			}
		}


		nac_evolve( dtp->mac->nac,NULL);//todo for testing
		aloha_evolve( dtp->mac,NULL);//todo for testing
		dtp_evolve(dtp, NULL);



		/* modified by zhangwei on 20091230
		 * - bug fix.
		 * - Zhangwei comment the following hal_delay(). I know the programmar wants
		 * to add some delay here, but we shouldn't call hal_delay() here, because 
		 * hal_delay() will occupy the entire processing time of the CPU. This will
		 * cause the main program haven't chances to call dop_recv(). So the incoming
		 * packets may lost. 
		 */
		hal_delay(100); 

		count ++;
	} /* while (1) */

	dtp_destroy( dtp );
	//ledtune_destroy( ledtune );
}


