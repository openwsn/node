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
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 0
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
#include "openwsn/svc/svc_nio_aloha.h"


#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_ALOHA_PANID				        0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS		        0x01
#define CONFIG_ALOHA_REMOTE_ADDRESS		        0x02
#define CONFIG_ALOHA_CHANNEL                    11

//#define TEST1
//#undef  TEST2
#define TEST2
#undef TEST1

#define TEST_ENABLE_ACK
#undef TEST_ENABLE_ACK

#define VTM_RESOLUTION                          5

//JOE
#ifndef CONFIG_SIO_UART_ID
#define UART_ID 1
#else
#define UART_ID CONFIG_SIO_UART_ID
#endif

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiFrameRxTxInterface                     m_rxtx;
static char                                     m_nac[NAC_SIZE];
static TiAloha                                  m_aloha;
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static TiCc2520Adapter                          m_cc;

TiUartAdapter m_uart;//JOE

static void aloha_sendnode(void);

int main(void)
{
	aloha_sendnode();
    return 0;
}

void aloha_sendnode(void)
{ 
  //JOE  
    TiUartAdapter * uart;
	    intx count;

    char * msg = "welcome to sendnode...\r\n";
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;
	TiTimerAdapter * timer;
	TiFrame * txbuf;
	char * pc;
	uint8 i, seqid=0, option, len;

    target_init();

	led_open(LED_RED);
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_RED );

	 		//JOE	
    uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);

    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem(msg, strlen(msg));

	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nac[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    
	cc2520_open(cc, 0, 0x00 );
    timer = timer_open(timer, 2, NULL, NULL, 0x00); 
    timer_setinterval(timer, 1000, 0);
    timer_setscale(timer,7999);

    rxtx = cc2520_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	aloha_open(mac, rxtx, nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        timer , 0x00 );
    
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );    
    hal_enable_interrupts();
	while(1) 
	{
		aloha_setremoteaddress( mac, CONFIG_ALOHA_REMOTE_ADDRESS );
        frame_reset(txbuf, 3, 20, 25);
      
        #ifdef TEST1
        pc = frame_startptr(txbuf);
        len = min(10, frame_capacity(txbuf));
        for (i=0; i<len; i++)
            pc[i] = '0' + i;
        pc[0] = seqid ++;
		frame_setlength(txbuf, len);
        #endif

        #ifdef TEST2
		frame_pushbyte(txbuf, seqid++);
        //frame_pushback(txbuf, seqid++);
        frame_pushback(txbuf, "1234567890123456789", 19); 
        #endif

        // if option is 0x00, then aloha send will not require ACK from the receiver. 
        // if you want to debugging this program alone without receiver node, then
        // suggest you use option 0x00.
        // the default setting is 0x01, which means ACK is required.
        //
        #ifdef TEST_ENABLE_ACK
        txbuf->option = 0x01;
        #endif
        #ifndef TEST_ENABLE_ACK
        txbuf->option = 0x00;
        #endif

        while (1)
        {  
            //if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, txbuf->option) > 0)
		    //if (aloha_send(mac, 0xffff, txbuf, 0x00) > 0)
			if(aloha_broadcast(mac,txbuf,0x00)>0)
            //if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, txbuf->option) > 0)
            {	
                led_toggle( LED_RED );
                break;
            }

            // @attention Needn't delay here. If there're really confliction occurs, 
            // then the aloha protocol should solve the backoff delay problem.
            // hal_delay(250);
        }
		
		// for simple aloha, you needn't to call aloha_evolve(). it's necessary for 
        // standard aloha.
   
        aloha_evolve( mac, NULL );
        nac_evolve( mac->nac, NULL);
       
		// controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
        // attention: this long delay will occupy the CPU and it may lead to great 
        // frame conflictions and frame lossing.
        
		hal_delayms(1000);

		// break;
	}

    timer_close( timer );
    frame_close( txbuf );
    nac_close( nac );
    aloha_close( mac );
    cc2520_close( cc );
}

