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
 * 
 * @modified by zhangwei on 20090724
 *  - compile the whole application passed
 * @authered by ShiMiaojing
 * @modified by zhangwei on 20090804
 *	- revision. compile passed.
 * @modified by Shi-Miaojing on 20090731
 *	- tested  ok
 * @modifeied by Shimiaojing on 20091031  
 *  - match the style of MAC frame and add cc2420_open 
 *  - tesk ok both ACK and non-ACK works
 * @modified by zhangwei on 20100713
 *  - replace TiOpenFrame with TiFrame 
 *  - upgrade rtl_assert, rtl_debugio
 *
 ******************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_simplealoha.h"


#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_ALOHA_PANID				        0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS		        0x01
#define CONFIG_ALOHA_REMOTE_ADDRESS		        0x02
#define CONFIG_ALOHA_CHANNEL                    11

#define MAX_IEEE802FRAME154_SIZE                128

static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;;
static TiSimpleAloha                            m_aloha;
//static TiUartAdapter		                    m_uart;
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

#define TiAloha TiSimpleAloha
#define aloha_construct(buf,msize) saloha_construct((buf),(msize))
#define aloha_open(mac,rxtx,chn,panid,addr,timer,listener,lisobject,opt) saloha_open((mac),(rxtx),(chn),(panid),(addr),(timer),(listener),(lisobject),(opt))
#define aloha_close(mac) saloha_close((mac))
#define aloha_send(a,f,opt) saloha_send((a),(f),(opt))
#define aloha_recv(a,f,opt) saloha_recv((a),(f),(opt))
#define aloha_evolve(a,e) saloha_evolve((a),(e))

void aloha_sendnode(void);

int main(void)
{
	aloha_sendnode();
    return 0;
}

void aloha_sendnode(void)
{   
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiSimpleAloha * mac;
	TiTimerAdapter   *timer;
	TiFrame * txbuf;
	char * pc;

	char * msg = "welcome to simplealoha sendnode...";
	uint8 i, seqid=0, option;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );
	

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiSimpleAloha) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );
    aloha_open( mac,rxtx, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        timer, NULL, NULL, 0x01);

    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );
    
	hal_enable_interrupts();
	while(1) 
	{
        saloha_setremoteaddress( mac, CONFIG_ALOHA_REMOTE_ADDRESS );

        dbc_putchar(0x24);
        frame_reset(txbuf, 3, 20, 25);

	    #define TEST1

        #ifdef TEST1
        pc = frame_startptr( txbuf );
        for (i=0; i<frame_capacity(txbuf); i++)
            pc[i] = i;
        #endif

        #ifdef TEST2
        frame_pushback( txbuf, "01234567890123456789", 20 ); 
        #endif


        // if option is 0x00, then aloha send will not require ACK from the receiver. 
        // if you want to debugging this program alone without receiver node, then
        // suggest you use option 0x00.
        // the default setting is 0x01, which means ACK is required.
        //
		option = 0x00;

        while (1)
        {
            if (aloha_send(mac, txbuf, option) > 0)
            {			
                led_toggle( LED_YELLOW );
                dbo_putchar( 0x11);
                dbo_putchar( seqid );
                break;
            }
			else{
				dbo_putchar(0x22);
                dbo_putchar( seqid );
			}
            hal_delay(1000);
        }
		
		// for simple aloha, you needn't to call aloha_evolve(). it's necessary for 
        // standard aloha.
   
        aloha_evolve( mac, NULL );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
        // attention: this long delay will occupy the CPU and it may lead to frame lossing.
        
		hal_delay(1000);

		//break;
	}

    frame_close( txbuf );
    saloha_close( mac );
    cc2420_close( cc );
}

