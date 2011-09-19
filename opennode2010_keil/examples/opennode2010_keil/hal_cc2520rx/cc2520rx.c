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

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_targetboard.h"

#define CONFIG_LISTENER    
#undef  CONFIG_LISTENER    

#define UART_ID 1

#define TEST_CHOICE 1
//#define TEST_ACK
//#undef  TEST_ACK

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define DEFAULT_CHANNEL     11

// This macro is actually an constant which equal to 128. You cannot change its value.
#define MAX_IEEE802FRAME154_SIZE                I802F154_MAX_FRAME_LENGTH

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter             m_cc;
TiUartAdapter               m_uart;

#if (TEST_CHOICE == 1)
static void recvnode1(void);
#endif

#if (TEST_CHOICE == 2)
static void recvnode2(void);
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif

int main(void)
{
    #if (TEST_CHOICE == 1)
	recvnode1();
    #endif

    #if (TEST_CHOICE == 2)
	recvnode2();
    #endif
}

#if (TEST_CHOICE == 1)
void recvnode1(void)
{
	char * msg = "welcome to recvnode...";
    TiCc2520Adapter * cc;
    TiUartAdapter * uart;
	TiFrame * rxbuf;
	uint8 len;
    uint8 i;
    char *pc;
    
	target_init();

	led_open();
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );

    uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);
	rtl_init( uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar_wait, hal_assert_report );
	dbc_mem( msg, strlen(msg) );

	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );

	cc2520_open( cc, 0, NULL, NULL, 0x00 );
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );							    // enable RX
	cc2520_enable_addrdecode( cc );					// enable address
	cc2520_setpanid( cc, PANID );					// network identifier 
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// node identifier in sub-network

	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	// when use this scan mode to receive data, interrupt should be disable; otherwise the data will be
	// read twice and in the second time there are no data actually which leads to a assert.
 	// Attention: in this scan mode, MCU always try to read and in my  test it is faster than the transmission of data. 
	// Thus, after 4 times, there no data at all, and the MCU still want to read, which lead to an assert. So we'd better
	// not use this scan mode.
    
	while(1) 
	{
		frame_reset(rxbuf, 0, 0, 0);
		cc2520_evolve(cc);

        // Query the arrived frame and put it into frame object.
		len = cc2520_read(cc, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00);
        if (len > 0)
        {
			frame_setlength(rxbuf, len);
            pc = frame_startptr(rxbuf);
            for (i=0; i<len; i++)
            {
                //uart_putchar(uart, pc[i]);
                dbc_putchar(pc[i]);
            }
			led_toggle( LED_RED);
        }
	}
}
#endif

#if (TEST_CHOICE == 2)
void recvnode2(void)
{
}
#endif

#if (TEST_CHOICE == 2)
void _cc2420_listener( void * ccptr, TiEvent * e )
{
}
#endif
