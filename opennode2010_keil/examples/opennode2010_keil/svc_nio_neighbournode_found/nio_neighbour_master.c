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
#include "../../../common/openwsn/hal/opennode2010/hal_mcu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"  
#include "../../../common/openwsn/svc/svc_configall.h"  
#include "../../../common/openwsn/rtl/rtl_foundation.h"
#include "../../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_timer.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../../common/openwsn/svc/svc_foundation.h"
#include "../../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../../common/openwsn/svc/svc_nio_neighbournode_found.h"

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define VTM_RESOLUTION 7
#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)


static TiTimerAdapter 		m_timer2;
static TiTimerAdapter 		m_timer3;
static TiAloha              m_aloha;
static TiFrameRxTxInterface m_rxtx;
//static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
TiNeighbourNode             m_neibour;
static char                 m_nacmem[NAC_SIZE];

static char                 m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];




/* output the TiIoBuf content throught UART by default */

int main(void)
{
	char * request;
	char * response;
	uint8 len;	
	char * msg = "welcome to gateway node...";

	TiTimerAdapter * timer2;

    TiTimerAdapter * timer3;

	TiCc2520Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;

	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
    TiNeighbourNode *neibour;
    TiFrame * mactxbuf;

    int i;

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
    
    halUartInit(9600,0);
	timer2  = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );

    timer3  = timer_construct( (void *)(&m_timer3), sizeof(m_timer3) );

	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );

    //rxbuf = iobuf_construct( &m_rxbuf[0], sizeof(m_rxbuf) );
	//txbuf = rxbuf;

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	cc   = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	nac  = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
    timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
    timer3 = timer_open( timer3, 3, NULL, NULL, 0x00 ); 
	mac =  aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timer2, NULL, NULL,0x00);

	mac->txbuf = mactxbuf;

    neibour = neighbournode_construct( (void *)(&m_neibour),sizeof( m_neibour));

    neibour = neighbournode_open( neibour,mac);

	cc2520_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2520_rxon(cc);							            
	cc2520_setpanid( cc, CONFIG_NODE_PANID  );					
	cc2520_setshortaddress( cc, CONFIG_NODE_ADDRESS );	


    timer_setinterval( timer3,2000,7999);
	while(1)
	{
        frame_reset( neibour->tmpbuf ,3,20,0);
		request = frame_startptr( neibour->tmpbuf);
		request[0] = 0x01;              // request type
		request[1] = 0xFF;              // set destination address
		request[2] = 0xFF;              // 0xFFFF is the broadcast address
		request[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       
		                                // set source address, 
		request[4] = (char)(CONFIG_NODE_ADDRESS & 0xFF);   
		                                // namely local address
        request[5] = 0x06;
        request[6] = 0x07;
		frame_setlength( neibour->tmpbuf, CONFIG_REQUEST_SIZE );
		len = neighbournode_broadcast( neibour, neibour->tmpbuf, 0x00 );
		if (len <= 0)                   // retry when sending request failed
		{
			hal_delay( 250 );
            USART_Send( 0xf0);//todo for testing
			continue;
		}
		else//todo for testing
		{
			
		}

		// configure the time interval as 2 seconds. non-periodical
		// the "vti" timer will automatically stopped when it's expired.
		//
		timer_start(timer3);

		while (!timer_expired( timer3))
		{
            len = neighbournode_found( neibour);

            if ( len)
            {
                for ( i=0;i<neibour->neighbourmaxcount;i++)
                {
                    if ( neibour->nodeinf[i].state!=0)
                    {
                        dump_nodeinf(neibour,i);
                        neighbournode_delete( neibour,i);

                    } 
                }
                
                led_toggle( LED_RED);
            }
            
		}
		
		neighbournode_evolve(neibour, NULL);
	}
}

