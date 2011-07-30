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
 *
 * This demonstration/testing program will accept DTP_DATA_REQUEST frames from the gateway
 * node and start the measurement. The measured data will be encapsulated as DATA_RESPONSE
 * and be sent back to the gateway node (G node)
 * 
 * It will also accept the DTP_MAINTAIN_REQUEST frame and try to join or establish 
 * the ad-hoc network.
 * 
 * @author zhangwei and jiangridong(TongJi University) in 201103
 *	- first created
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "../../common/openwsn/hal/hal_configall.h"  
#include "../../common/openwsn/svc/svc_configall.h"  
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_nio_datatree.h"
#include "ledtune.h"
#include "output_frame.h"

#define CONFIG_NODE_PANID                0x01
//#define CONFIG_NODE_ADDRESS              0x03
#define CONFIG_NODE_ADDRESS              0x04//todo 另一个节点使用

//#define CONFIG_NODE_ADDRESS                0x05//todo 第三个节点使用

#define CONFIG_NODE_CHANNEL              11

#undef  CONFIG_DEBUG
#define CONFIG_DEBUG
#undef  CONFIG_DEMO
#define CONFIG_DEMO


#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

#define CONFIG_VTM_RESOLUTION 7

/* DTP is an network layer protocol, which includes the following fields:
 * [Frame Control 1B][Hopcount 1B][Maximum Hop Count 1B][Path Descriptor Length 1B]
 * [Path Descriptor 10B] [Payload Length nB]
 */

#define HIGHBYTE(w) ((uint8)(w>>8))
#define LOWBYTE(w) ((uint8)(w & 0xFF))

static TiTimerAdapter 		             m_timeradapter;
static TiTimerManager 		             m_vtm;
static TiCc2420Adapter		             m_cc;
static TiFrameRxTxInterface              m_rxtx;

static TiAloha                           m_aloha;
static char                              m_nacmem[NAC_SIZE];
static TiDataTreeNetwork                 m_dtp;
static char                              m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                              m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static TiAdcAdapter                      m_adc;
static TiLumSensor                       m_lum;


int main(void)
{
	uint16 value, count;
	uint8 len;
	char * request;
	char * response;
    char * payload;
	char * msg = "welcome to node...";

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * mac_timer;
	TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;
	TiAdcAdapter * adc;
	TiLumSensor * lum;
	TiDataTreeNetwork * dtp;
	TiFrame * rxbuf;
	TiFrame * txbuf;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );

	timeradapter   = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm            = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc             = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac            = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac            = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	dtp            = dtp_construct( (char *)(&m_dtp), sizeof(TiDataTreeNetwork) );
	adc            = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	lum            = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );
	txbuf          = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	rxbuf          = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	// timeradapter is used by the vtm(virtual timer manager). vtm require to enable the 
	// period interrupt modal of vtm

	//timeradapter   = timer_open( timeradapter, 0, NULL, NULL, 0x01 ); 
	vtm            = vtm_open( vtm, timeradapter, CONFIG_VTM_RESOLUTION );
	cc             = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx           = cc2420_interface( cc, &m_rxtx );
	mac_timer      = vtm_apply( vtm );
	mac_timer      = vti_open( mac_timer, NULL, mac_timer);
	hal_assert( rxtx != NULL );
	nac            = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	hal_assert( nac != NULL ); 
    

	mac             = aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,mac_timer, NULL, NULL,0x01);

	adc            = adc_open( adc, 0, NULL, NULL, 0 );
	lum            = lum_open( lum, 0, adc );
	
	dtp            = dtp_open( dtp, mac, CONFIG_NODE_ADDRESS, NULL, NULL, 0x00 );


	//todo

	cc2420_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	//todo
	
	cc2420_settxpower( cc, CC2420_POWER_1);//cc2420_settxpower( cc, CC2420_POWER_2);
	cc2420_enable_autoack( cc );

	
//	ledtune        = ledtune_construct( (void*)(&m_ledtune), sizeof(m_ledtune), vti );
//	ledtune        = ledtune_open( ledtune );

	/* assert: all the above open() functions return non NULL values */

	hal_assert((timeradapter != NULL) && (cc != NULL) && (mac != NULL) && (adc != NULL) && (lum != NULL)
		&& (rxbuf != NULL) && (txbuf != NULL) && (dtp != NULL));

	hal_enable_interrupts();

    dtp->state = DTP_STATE_IDLE;//todo for testing 临时用这一句必须删掉

	//todo for testing
   
	//dtp->root = 0x01;//todo for testing
	//dtp->parent = 0x03;//todo for testing
   /*
	while ( 1)//todo for testing
	{
		
		response = frame_startptr( txbuf );

		value = lum_value( lum ); 
		payload = DTP_PAYLOAD_PTR(response);
		payload[0] = 0x13;
		payload[1] = 0x14;
		if (dtp_send_response(dtp, txbuf, 0x01) > 0)
		{ 
			led_toggle( LED_RED);//todo for testing
			
		}

        dtp_evolve( dtp, NULL );
		
		hal_delay( 2000);//todo for testing

	}

     */




	while(1)
	{
		/* Only the following two kinds of frames will be put into "rxbuf" by dtp_recv()
		 * - broadcast frames. the destination address of these frames are 0xFFFF.
		 * - destination is the current node. 
		 */
		//dbo_putchar(0x33);


		len = dtp_recv( dtp, rxbuf, 0x00 );
		if (len > 0)
		{   
            
  		    //ieee802frame154_dump( rxbuf);

			request = frame_startptr( rxbuf );

			switch (DTP_CMDTYPE(request))
			{
			/* if the frame is DTP_DATA_REQUEST, then the node will measure the data and 
			 * encapsulate the data into the txbuf, which is a TiOpenFrame and sent it back.
			 */


		

			case DTP_DATA_REQUEST:
				//payload = DTP_PAYLOAD_PTR( frame_startptr(txbuf) );
				//ledtune_write( ledtune, MAKE_WORD(payload[1], payload[0]) );

				// response frame = PHY Length 1B 
				//	+ Frame Control 2B 
				//	+ Sequence No 1B
				//	+ Destination Pan & Address 4B 
				//	+ Source Pan & Address 4B 
				//	+ DTP Section 15B

				//opf_cast( txbuf, 50, OPF_DEF_FRAMECONTROL_DATA_ACK );
				response = frame_startptr( txbuf );

				value = lum_value( lum );
				DTP_SET_MAX_HOPCOUNT( response,0x03);//todo for testing
				payload = DTP_PAYLOAD_PTR(response);

                //payload[0] = 0x17;//todo 第三个节点数据
				//payload[1] = 0x18;//todo 第三个节点数据

			    //payload[1] = 0x13;
				//payload[2] = 0x14;

				payload[1] = 0x15;//todo 另一个节点
			    payload[2] = 0x16;//todo 另一个节点

				/* call dtp_send_response() to send the data in txbuf out.
				 * 
				 * modified by zhangwei on 20091230
				 *	- Bug fix. In the past, there's no delay between two adjacent 
				 * dtp_send_response() calls. This policy is too ambitious and this
				 * node will occupy the whole time so that the other nodes will lost 
				 * chances to send, or encounter much higher frame collision probabilities. 
				 * so I add a little time delay here. 
				 *    Attention the delay time here shouldn't be too large because
				 * we don't want the hal_delay() to occupy all the CPU time. If this 
				 * occurs, it may lead to unnecessary frame lossing. 
				 */
				// try some times until the RESPONSE is successfully sent

			
               
                for (count=0; count<10; count++)
                {   
					//hal_delay( 500);
					if (dtp_send_response(dtp, txbuf, 0x03) > 0)
                    { 
                        led_toggle( LED_RED);//todo for testing
						break;
                    }
					//hal_delay( 50 );
				}
                
				break;


			default:
			    //hal_assert(false);
                break;
			}
		}
        nac_evolve( nac,NULL);//todo for tesitng
		aloha_evolve( mac,NULL);//todo for testing
		dtp_evolve( dtp, NULL );
		hal_delay( 50 );
	}
}


