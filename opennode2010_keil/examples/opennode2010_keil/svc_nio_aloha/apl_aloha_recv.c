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
 * 
 * @modified by jiangridong in 201108
 *  - tested ok.
 * @modified by zhangwei in 201108
 *  - no-ack mode tested ok.
 ******************************************************************************/


#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/rtl/rtl_random.h"
#include "openwsn/hal/hal_mcu.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/svc/svc_nio_aloha.h"

/* actually equal to 128 */
#define MAX_IEEE802FRAME154_SIZE    I802F154_MAX_FRAME_LENGTH

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


#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

// #define VTM_RESOLUTION              5


static TiFrameRxTxInterface                     m_rxtx;
static char                                     m_nac[NAC_SIZE];
static TiAloha                                  m_aloha;
static TiTimerAdapter                           m_timer2;
static char                                     m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static TiCc2520Adapter                          m_cc;


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
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
    TiNioAcceptor * nac;
    TiAloha * mac;
    TiTimerAdapter   *timer2;
    TiFrame * rxbuf;
    char * pc, * ptr;
    uint8 len;

    char * msg = "welcome to aloha recvnode...";
    uint8 i, seqid=0, option;

    target_init();
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );

    led_open();
    led_on( LED_ALL );
    hal_delayms( 500 );
    led_off( LED_ALL );

    cc = cc2520_construct((char *)(&m_cc), sizeof(TiCc2520Adapter));
    nac = nac_construct(&m_nac[0], NAC_SIZE);
    mac = aloha_construct((char *)(&m_aloha), sizeof(TiAloha));
    timer2= timer_construct((char *)(&m_timer2), sizeof(TiTimerAdapter));

    cc2520_open(cc, 0, NULL, NULL, 0x00 );

    timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 ); 
    timer_setinterval( timer2, 1000, 0 );
    timer_setscale( timer2,7999);

    dbc_putchar(0xff);

    rxtx = cc2520_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );
    nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
    aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, 
        CONFIG_ALOHA_LOCAL_ADDRESS, timer2 , NULL, NULL, 0x00 );
 
	cc2520_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2520_rxon( cc );							                // enable RX mode
	cc2520_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2520_enable_autoack( cc );
 
    rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    dbc_putchar(0x11);

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
        frame_reset(rxbuf, 3, 20, 0);
		len = aloha_recv(mac, rxbuf, 0x00);        
		if (len > 0)
		{   
            // output the frame data. it will first output F1, F2 and F3 as the start flag
            // just before the frame data. 
            frame_movelower( rxbuf );
            dbc_putchar(0xf1);
            dbc_putchar(0xf2);
            dbc_putchar(0xf3);
            pc = frame_startptr(rxbuf);
            for (i=0; i<frame_length(rxbuf); i++)
            {
                dbc_putchar(pc[i]);
            }
            frame_movehigher( rxbuf );
            
			/*
			dbc_write(frame_startptr(rxbuf), frame_length(rxbuf));//todo for testing
            ptr = frame_startptr(rxbuf);//todo for testing
			dbc_putchar(0xff);//todo for testing
			dbc_uint8( ptr[(frame_length(rxbuf)-2)]);//todo for testing
			dbc_putchar(0xff);//todo for testing
            frame_moveinner(rxbuf);//todo for testing
            */
            
			//led_off( LED_RED );

			/* warning: You shouldn't wait too long in the while loop, or else 
			 * you may encounter frame loss. However, the program should still 
			 * work properly even the delay time is an arbitrary value. No error 
			 * are allowed in this case. 
			 */
			//hal_delay( 500 );
			led_toggle( LED_RED );
		
        }

		aloha_evolve(mac, NULL );
	}
	#endif

    frame_close( rxbuf );
    aloha_close( mac );
    cc2520_close( cc );
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
            //_output_frame( frame, NULL );
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


