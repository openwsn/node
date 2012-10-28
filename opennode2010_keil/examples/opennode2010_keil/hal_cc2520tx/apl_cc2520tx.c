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
 * @history
 * - modified by zhangwei on 2011.09.23
 *   add hal_enable_interrupts() before while loop. This is mandatory.
 *   Revised and tested ok.
 ******************************************************************************/

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_interrupt.h"

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif
//#define TEST_ACK_REQUEST

#define UART_ID 1

// The following macro is acutally an constant 128. You cannot change its value.
#define MAX_IEEE802FRAME154_SIZE FRAME154_MAX_FRAME_LENGTH


#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define DEFAULT_CHANNEL     11

static TiUartAdapter        m_uart;      
static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

static TiIEEE802Frame154Descriptor m_desc;
static TiCc2520Adapter      m_cc;

void sendnode1(void);
//void sendnode2(void);

int main(void)
{
    sendnode1();
}

void sendnode1(void)
{
	char * msg = "welcome to sendnode...";
    TiCc2520Adapter * cc;
    TiUartAdapter * uart;
    TiFrame * txbuf;
	TiFrame * rxbuf;

    TiIEEE802Frame154Descriptor * desc;
    uint8 initlayer;
    uint8 initlayerstart;
    uint8 initlayersize;
    uint8 tmp=0;

    uint8 i, first, seqid, option, len,seqid_ack,len_ack;
    char * ptr;
	char * ptr_ack;

    seqid = 0;

	target_init();
	led_open(LED_RED);
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_RED );

	halUartInit(9600,0);

    uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);
	rtl_init( uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar_wait, hal_assert_report );
	dbc_mem( msg, strlen(msg) );
    
    cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
    cc2520_open( cc, 0, 0x00 );
    cc2520_setchannel( cc, DEFAULT_CHANNEL );
    cc2520_rxon( cc );							    // Enable RX
    cc2520_enable_addrdecode( cc );					// enable address decoding and filtering
    cc2520_setpanid( cc, PANID );					// set network identifier 
    cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// set node identifier in a sub-network
    cc2520_enable_autoack( cc );
	//cc2520_disable_autoack( cc );

    desc = ieee802frame154_open( &m_desc );
    option = 0x00;

    hal_enable_interrupts();

    while(1)  
    {
        //led_off(LED_RED);

        // @attention
        // - When you open the frame, you must guarantee there're at least two empty
        //   byte space for later frame_skipouter(), or else you'll encounter assertion
        //   failure in the rtl_frame module.
        initlayer = 3;
        initlayerstart = 13;
        initlayersize = 6;

        txbuf = frame_open((char*)(&m_txbuf), sizeof(m_txbuf), initlayer, initlayerstart, initlayersize );
		rxbuf = frame_open((char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0);

        // assign some random data into the frame. for demostration only.        
        ptr = frame_startptr(txbuf);
        for (i = 0; i< 6; i++)
            ptr[i] = '0' + i;

        // create the 802.15.4 protocol header. attention it requires at least 12 bytes 
        // for the header and 2 bytes for the tail(CRC checdum).
        frame_skipouter(txbuf, initlayerstart-1, 2);
        desc = ieee802frame154_format(desc, frame_startptr(txbuf), frame_capacity(txbuf), 
            FRAME154_DEF_FRAMECONTROL_DATA); 
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, seqid); 
        ieee802frame154_set_panto( desc, PANID );
        ieee802frame154_set_shortaddrto( desc, REMOTE_ADDRESS );
        ieee802frame154_set_panfrom( desc, PANID );
        ieee802frame154_set_shortaddrfrom( desc, LOCAL_ADDRESS );
        frame_setlength(txbuf, initlayerstart + initlayersize - 1 + 2);
        first = frame_firstlayer(txbuf);

		option=1;
        len = cc2520_write(cc, frame_layerstartptr(txbuf,first), frame_length(txbuf), option);
		USART_Send(seqid);
        if (len > 0)
        {
			frame_reset(rxbuf, 0, 0, 0);
            led_toggle(LED_RED);
            //led_on(LED_RED);
            //hal_delayms(500);

            seqid++;

//			len_ack = cc2520_read(cc, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00);
//        	if (len_ack > 0)
//        	{
//				ptr_ack = frame_startptr(rxbuf);
//				USART_Send(ptr_ack[3]);
//			}
				
/*
            tmp = FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE);
            dbc_uint8(tmp);                     // size of the TiFrame object
            tmp = sizeof(TiFrame); 
            dbc_uint8(tmp);                     // size of the TiFrame description header size
            dbc_uint8(frame_capacity(txbuf));   // frame capacity
            dbc_uint8(frame_length(txbuf));     // data length in the current layer of the frame
            dbc_uint8(len);                     // data length actually sent
*/
            
           // tmp = FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE);
            //dbc_uint8(tmp);                     // size of the TiFrame object
            //tmp = sizeof(TiFrame); 
           // dbc_uint8(tmp);                     // size of the TiFrame description header size
           // dbc_uint8(frame_capacity(txbuf));   // frame capacity
            //dbc_uint8(frame_length(txbuf));     // data length in the current layer of the frame
            //dbc_uint8(len);                     // data length actually sent
			hal_delayus(5);
        }
        else{
        }
    }
}
