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
 * aloha_recv
 * The receiving test program based on ALOHA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * @state
 *	still in developing. test needed
 *
 * @author Shi-Miaojing on 20090801
 *	- first created
 * @modified by  ShiMiaojing but not tested   
 *  - some wrong with assert.h  so  interupt is wrong.
 * @modified by zhangwei on 20090804
 *	- revisioin. compile passed.
 *modified  by ShMiaojing
 *modified by ShimMiaojing  test ok add cc2420_open and modifeid output_openframe 
 *but about macro define-config_test_listenner may be somewhat wrong but both two way works
 *
 * @modified by zhangwei on 2010520
 *  - upgraded to winavr20090313
 ******************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/svc/svc_simplealoha.h"
#include "apl_output_frame.h"


#define CONFIG_DEBUG


#define CONFIG_TEST_LISTENER  
#undef  CONFIG_TEST_LISTENER  

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define CONFIG_ALOHA_PANID			0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS	0x02
#define CONFIG_ALOHA_REMOTE_ADDRESS	0x00
#define CONFIG_ALOHA_CHANNEL		11

#define BROADCAST_ADDRESS			0xFFFF

#define MAX_IEEE802FRAME154_SIZE    128


#define TiAloha TiSimpleAloha
#define aloha_construct(buf,msize) saloha_construct((buf),(msize))
#define aloha_open(mac,rxtx,chn,panid,addr,timer,listener,lisobject,opt) saloha_open((mac),(rxtx),(chn),(panid),(addr),(timer),(listener),(lisobject),(opt))
#define aloha_close(mac) saloha_close((mac))
#define aloha_send(a,f,opt) saloha_send((a),(f),(opt))
#define aloha_recv(a,f,opt) saloha_recv((a),(f),(opt))
#define aloha_evolve(a,e) saloha_evolve((a),(e))

static TiCc2420Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;;
static TiAloha                      m_aloha;
static TiTimerAdapter               m_timer;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];


#ifdef CONFIG_TEST_LISTENER
static void _aloha_listener( void * ccptr, TiEvent * e );
#endif

static void recvnode(void);

int main(void)
{
	recvnode();
}

void recvnode(void)
{
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiAloha * mac;
	TiTimerAdapter   *timer;
	TiFrame * rxbuf;
	char * msg = "welcome to aloha recv test...";
    int len=0;

	target_init();

    // flash the led to indicate the software is successfully running now.
    //
	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );

    // initialize the runtime library for debugging input/output and assertion
    // hal_assert_report is defined in module "hal_assert"
    //
	//dbo_open( 38400 );
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_putchar( 0xF0 );
    dbc_mem( msg, strlen(msg) );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
    mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	#ifdef CONFIG_TSET_LISTENER
	// cc = cc2420_open( cc, 0, _aloha_listener, NULL, 0x00 );
    cc = cc2420_open( cc, 0, aloha_evolve, mac, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	mac = aloha_open( mac, rxtx, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        timer, _aloha_listener, NULL,0x00 );
	#endif

    #ifndef CONFIG_TSET_LISTENER
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	mac = aloha_open( mac, rxtx, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        timer, NULL, NULL,0x00 );
	#endif
 
	cc2420_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2420_enable_addrdecode( cc );
	#else	
	cc2420_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif
 
    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    #ifdef CONFIG_TEST_ACK
    //fcf = OPF_DEF_FRAMECONTROL_DATA_ACK; 
	#else
    //fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	#endif


    hal_enable_interrupts();

	/* Wait for listener action. The listener function will be called by the TiCc2420Adapter
	 * object when a frame arrives */
	#ifdef CONFIG_TEST_LISTENER	
	while (1) {}
	#endif
    
	/* Query the TiCc2420Adapter object if there's no listener */
	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{	
        frame_reset( rxbuf, 3, 20, 0 );

        len = aloha_recv( mac, rxbuf, 0x00 );
		if (len > 0)
		{   
			dbc_putchar( 0xF3 );
            dbc_mem( frame_startptr(rxbuf), frame_length(rxbuf) );

            //frame_moveouter( rxbuf );
            //_output_frame( rxbuf, NULL );
            //frame_moveinner( rxbuf );

			led_off( LED_RED );

			/* warning: You shouldn't wait too long in the while loop, or else 
			 * you may encounter frame loss. However, the program should still 
			 * work properly even the delay time is an arbitrary value. No error 
			 * are allowed in this case. 
			 */
			//hal_delay( 500 );
			led_toggle( LED_RED );
			//hal_delay( 500 );
        }

		aloha_evolve(mac, NULL );
	}
	#endif

    frame_close( rxbuf );
    aloha_close( mac );
    cc2420_close( cc );
}

#ifdef CONFIG_TEST_LISTENER
void _aloha_listener( void * owner, TiEvent * e )
{
	TiAloha * mac = &m_aloha;
    TiFrame * frame = (TiFrame *)m_rxbufmem;
    uint8 len;

	dbc_putchar( 0xF4 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = aloha_recv( mac, frame, 0x00 );
		if (len > 0)
		{    
            frame_moveouter( frame );
            _output_frame( frame, NULL );
            frame_moveinner( rxbuf );
			led_toggle( LED_RED );

			/* warning: You cannot wait too long in the listener. Because in the 
			 * current version, the listener is still run in interrupt mode. 
			 * you may encounter unexpect error at the application error in real-time
			 * systems. However, the program should still work properly even the 
			 * delay time is an arbitrary value here. No error are allowed in this case. 
			 *
			 * => That's why we cannot use hal_delay() to control the LED to make
			 * it observable for human eye. 
			 */
			// hal_delay( 500 );
			break;
        }
	}
}
#endif


