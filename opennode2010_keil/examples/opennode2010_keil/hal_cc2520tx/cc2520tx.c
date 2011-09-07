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

// The following macro is acutally an constant 128. You cannot change its value.
#define MAX_IEEE802FRAME154_SIZE FRAME154_MAX_FRAME_LENGTH


#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define DEFAULT_CHANNEL     11

static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiIEEE802Frame154Descriptor m_desc;
TiCc2520Adapter             m_cc;
void sendnode1(void);
//void sendnode2(void);

int main(void)
{
    sendnode1();
}

void sendnode1(void)
{
    TiCc2520Adapter * cc;
    TiFrame * txbuf;
    TiIEEE802Frame154Descriptor * desc;

    uint8 i, first, seqid, option, len;
    char * ptr;

    seqid = 0;

    led_open();

    led_on( LED_ALL);
    hal_delayms( 500 );
    led_off( LED_ALL );
    
    halUartInit( 9600,0);
    cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
    cc2520_open( cc, 0, NULL, NULL, 0x00 );
    cc2520_setchannel( cc, DEFAULT_CHANNEL );
    cc2520_rxon( cc );							    // Enable RX
    cc2520_enable_addrdecode( cc );					// enable address decoding and filtering
    cc2520_setpanid( cc, PANID );					// set network identifier 
    cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// set node identifier in a sub-network
    cc2520_enable_autoack( cc );

    desc = ieee802frame154_open( &m_desc );
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    option = 0x00;

    while(1)  
    {
        frame_reset( txbuf,3,20,0);
        ptr = frame_startptr( txbuf);

        for ( i = 0;i< 6;i++)
            ptr[i] = i;
        frame_skipouter( txbuf,12,2);
        desc = ieee802frame154_format( desc, frame_startptr( txbuf), frame_capacity( txbuf ), 
            FRAME154_DEF_FRAMECONTROL_DATA ); 
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, seqid); 
        ieee802frame154_set_panto( desc, PANID );
        ieee802frame154_set_shortaddrto( desc, REMOTE_ADDRESS );
        ieee802frame154_set_panfrom( desc, PANID );
        ieee802frame154_set_shortaddrfrom( desc, LOCAL_ADDRESS );
        frame_setlength(txbuf, 20);
        first = frame_firstlayer(txbuf);

        //len = cc2420_write(cc, frame_layerstartptr(txbuf,first), frame_layercapacity(txbuf,first), option);
        len = cc2520_write(cc, frame_layerstartptr(txbuf,first), frame_length( txbuf), option);

        if (len > 0)
        {
            led_toggle(LED_RED);
            seqid++;
        }
        hal_delayms(1000);
    }
}
