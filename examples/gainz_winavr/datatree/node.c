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
 * @author zhangwei and shimiaojing(TongJi University) in 200911
 *	- first created
 ******************************************************************************/

#include "../common/hal/hal_configall.h"  
#include "../common/svc/svc_configall.h"  
#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_adc.h"
#include "../common/hal/hal_luminance.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"
#include "../common/svc/svc_datatree.h"
#include "ledtune.h"
#include "output_frame.h"

#define CONFIG_NODE_PANID                0x01
#define CONFIG_NODE_ADDRESS              0x03
#define CONFIG_NODE_CHANNEL              11

#undef  CONFIG_DEBUG
#define CONFIG_DEBUG
#undef  CONFIG_DEMO
#define CONFIG_DEMO

//#define CONFIG_RESPONSE_SIZE             7

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
static TiAloha                           m_aloha;
static TiUartAdapter		             m_uart;
static char                 			 m_opfmem[ OPF_SUGGEST_SIZE ];	
static TiDataTreeNetwork                 m_dtp;
static TiAdcAdapter                      m_adc;
static TiLumSensor                       m_lum;
//static TiLedTune                         m_ledtune;


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
	TiTimer * vti;
	TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiAdcAdapter * adc;
	TiLumSensor * lum;
	TiDataTreeNetwork * dtp;
	TiOpenFrame * rxbuf;
	TiOpenFrame * txbuf;
//	TiLedTune * ledtune;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open( 0, 38400 );

	timeradapter   = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm            = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc             = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac            = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	dtp            = dtp_construct( (char *)(&m_dtp), sizeof(TiDataTreeNetwork) );
	uart           = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	adc            = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	lum            = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );
	rxbuf          = opf_construct( (void *)(&m_opfmem[0]), sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	txbuf          = rxbuf;

	// timeradapter is used by the vtm(virtual timer manager). vtm require to enable the 
	// period interrupt modal of vtm

	timeradapter   = timer_open( timeradapter, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm            = vtm_open( vtm, timeradapter, CONFIG_VTM_RESOLUTION );
	cc             = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	mac            = aloha_open( mac, cc, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,
		             NULL, NULL, NULL,0x01);
	adc            = adc_open( adc, 0, NULL, NULL, 0 );
	lum            = lum_open( lum, 0, adc );
	rxbuf          = opf_open( &m_opfmem[0], sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );
	txbuf          = rxbuf;
	uart           = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	dtp            = dtp_open( dtp, mac, CONFIG_NODE_ADDRESS, NULL, NULL, 0x00 );
	rxbuf          = opf_open( &m_opfmem[0], sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );
	cc2420_settxpower( cc, CC2420_POWER_2);
	cc2420_enable_autoack( cc );

	vti            = vtm_apply( vtm );
	vti            = vti_open( vti, NULL, NULL );
//	ledtune        = ledtune_construct( (void*)(&m_ledtune), sizeof(m_ledtune), vti );
//	ledtune        = ledtune_open( ledtune );

	/* assert: all the above open() functions return non NULL values */

	hal_assert((vtm != NULL) && (cc != NULL) && (mac != NULL) && (adc != NULL) && (lum != NULL)
		&& (rxbuf != NULL) && (txbuf != NULL) && (uart != NULL) && (dtp != NULL));
	
	uart_write( uart, msg, strlen(msg), 0x00 );

	hal_enable_interrupts();

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
			output_openframe( rxbuf, uart );
			request = opf_msdu( rxbuf );

			switch (DTP_CMDTYPE(request))
			{
			/* if the frame is DTP_DATA_REQUEST, then the node will measure the data and 
			 * encapsulate the data into the txbuf, which is a TiOpenFrame and sent it back.
			 */
			case DTP_DATA_REQUEST:
				led_toggle(LED_GREEN);

				payload = DTP_PAYLOAD_PTR( opf_msdu(txbuf) );
				//ledtune_write( ledtune, MAKE_WORD(payload[1], payload[0]) );

				// response frame = PHY Length 1B 
				//	+ Frame Control 2B 
				//	+ Sequence No 1B
				//	+ Destination Pan & Address 4B 
				//	+ Source Pan & Address 4B 
				//	+ DTP Section 15B

				opf_cast( txbuf, 50, OPF_DEF_FRAMECONTROL_DATA_ACK );
				response = opf_msdu( txbuf );

				value = lum_value( lum ); 
				payload = DTP_PAYLOAD_PTR(response);
				payload[0] = 0x13;
				payload[1] = 0x14;

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

                for (count=0; count<1000; count++)
                {
					if (dtp_send_response(dtp, txbuf, 0x00) > 0)
                    {  
						break;
                    }
					hal_delay( 5 );
				}
				break;

			default:
			    //hal_assert(false);
                break;
			}
		}

		dtp_evolve( dtp, NULL );
	}
}


