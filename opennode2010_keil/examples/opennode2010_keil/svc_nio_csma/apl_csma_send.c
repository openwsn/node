/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 * aloha_send
 * This project implements a frame sending node based on TiAloha component. This
 * component is in module "svc_aloha". 
 * 
 * @state
 *  - tested ok. released.
 *
 * @author ShiMiaojing
 * @modified by openwsn on 2010.08.23
 *  - compiled succssfully. the aloha_send.c only has slightly modifications comparing 
 *    to the aloha_send.c inside "gainz-winavr\simplealoha\".  that version has already
 *    tested successfully by Xu Fuzhen in 2010.07
 * @modified by zhangwei on 20090724
 *  - compile the whole application passed
 * @modified by zhangwei on 20090804
 *	- revision. compile passed.
 * @modified by Shi-Miaojing on 20090731
 *	- tested  ok
 * @modifeied by Shimiaojing(TongJi University, Peking University) on 20091031  
 *  - match the style of MAC frame and add cc2420_open 
 *  - tesk ok both ACK and non-ACK works
 * @modified by zhangwei on 20100713
 *  - replace TiOpenFrame with TiFrame. needs testing. 
 *  - upgrade rtl_assert, rtl_debugio
 * @modified by Xu-Fuzhen (xufz0726@126.com) in TongJi University in 2010.10
 *  - revised. tested.
 *
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 2
#define MAX_IEEE802FRAME154_SIZE                128

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/rtl/rtl_random.h"
#include "openwsn/rtl/rtl_dumpframe.h"
#include "openwsn/hal/hal_mcu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/svc/svc_nio_csma.h"


#define CONFIG_CSMA_PANID				        0x0001
#define CONFIG_CSMA_LOCAL_ADDRESS		        0x01
#define CONFIG_CSMA_REMOTE_ADDRESS		        0x02
#define CONFIG_CSMA_CHANNEL                     11


//JOE
#ifndef CONFIG_SIO_UART_ID
#define UART_ID 1
#else
#define UART_ID CONFIG_SIO_UART_ID
#endif

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiFrameRxTxInterface                     m_rxtx;
static char                                     m_nac[NAC_SIZE];
static TiNioMac                               	m_mac;
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                                     m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static TiCc2520Adapter                          m_cc;

static void csma_sendnode(void);


int main(void)
{
	csma_sendnode();
    return 0;
}


void csma_sendnode(void)
{ 
	intx retval=0;

    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiNioMac * mac;
	TiTimerAdapter * timer;
	TiFrame * txbuf;
	TiFrame * rxbuf;
	char * pc;
	uint8 i, seqid=0, len;

    target_init();

	led_open(LED_RED);
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nac[0], NAC_SIZE );
	mac = mac_construct( (char *)(&m_mac), sizeof(TiNioMac) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    
	cc2520_open(cc, 0, 0x00 );
    timer = timer_open(timer, 2, NULL, NULL, 0x00); 
    timer_setinterval(timer, 1000, 0);
    timer_setscale(timer,7999);

    rxtx = cc2520_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac_open(mac, rxtx, nac, CONFIG_CSMA_CHANNEL, CONFIG_CSMA_PANID, CONFIG_CSMA_LOCAL_ADDRESS, timer , 0x00 );
	mac_setsendprobability(mac,0xFF);
    
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 ); 
	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );    
   
    hal_enable_interrupts();
	while(1) 
	{
		mac_setremoteaddress( mac, CONFIG_CSMA_REMOTE_ADDRESS );
        frame_reset(txbuf, 3, 20, 25);
      
	    pc = frame_startptr(txbuf);
        len = min(10, frame_capacity(txbuf));
        for (i=0; i<len; i++)
            pc[i] = '0' + i;
        pc[0] = seqid ++;
		frame_setlength(txbuf, len);

        while (1)
        {  
            retval = mac_send(mac, CONFIG_CSMA_REMOTE_ADDRESS, txbuf, 1); 
			if( (retval == MAC_IORET_ERROR_NOACK) || (retval == MAC_IORET_ERROR_ACCEPTED_AND_BUSY) || (MAC_IORET_SUCCESS(retval)) )
            {	
                led_toggle( LED_RED );
                break;
            }
        }

        csma_evolve( mac, NULL );
        nac_evolve( mac->nac, NULL);
       
		// controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
        // attention: this long delay will occupy the CPU and it may lead to great 
        // frame conflictions and frame lossing.
        
		hal_delayms(1000);

	}
}

