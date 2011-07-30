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
/*
*****************************************************************************
 * 
 * @modified by zhangwei on 20090724
 *  - compile the whole application passed
 * @authered by ShiMiaojing
 * @modified by zhangwei on 20090804
 *	- revision. compile passed.
 * @modified by Shi-Miaojing on 20090731
 *	- tested  ok
 * @modified by Shimiaojing on 20091103/1104  test ok.
 *****************************************************************************/

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_debugio.h"
#include "../common/svc/svc_aloha.h"


#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define MAX_RETRY_COUNT     0x3FFF
#define LOCAL_ADDRESS		0x01
#define REMOTE_ADDRESS		0x02


static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static TiTimerAdapter       m_timer;

uint8   chn=11;
uint16  panid=0x0001; 
uint16  address=0x01;
uint16 len;

static void aloha_sendnode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{
	aloha_sendnode();
    return 0;
}

void aloha_sendnode(void)
{   
    TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiTimerAdapter   *timer;
	
    char opfmem[OPF_SUGGEST_SIZE];
	TiOpenFrame * opf;

	char * msg = "welcome to aloha sendnode...";
	uint8 i, total_length, seqid=0, option;
    uint16 fcf, count;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );
	dbo_open( 0, 38400 );

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    	
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open(cc,panid,NULL,NULL,0x00);
	aloha_open( mac,cc,chn,panid,address,timer, NULL, NULL,0x00);

	//aloha_setchannel( mac, CONFIG_ALOHA_DEFAULT_CHANNEL );
	//aloha_setpanid( mac, CONFIG_ALOHA_DEFAULT_PANID );					//网络标识
	//aloha_setlocaladdr( mac, CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS );		//网内标识

    opf = opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	hal_enable_interrupts();

	while(1) 
	{
        fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK;    // 0x8801;     // = 0x8841;
		total_length = 30;					      // frame length
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, CONFIG_ALOHA_DEFAULT_PANID );
		opf_set_shortaddrto( opf, REMOTE_ADDRESS );
		opf_set_panfrom( opf, CONFIG_ALOHA_DEFAULT_PANID);
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );

		for (i=0; i<opf->datalen; i++)
			opf->msdu[i] = i;
	

		option = 0x01;		// ACK request

	    len = aloha_send( mac, opf, option );  
        // if aloha send failed
        if (len <= 0)
        {
			dbo_putchar( 'F' );
            hal_delay( 200 );
            continue;            
        }

        // if aloha send successfully
        {
            //dbo_led( 0x01 );
            dbo_putchar( 'S' );
            dbo_n8toa( seqid );
	        //_output_openframe( opf, uart );

            // try to receive the frame replied by the echo node. attention we should 
            // wait long enough in order not to miss the response frame.
            count = 0;
            while (count < 1000)
            {
                if ((len=aloha_recv(mac, opf, option)) > 0)
                {
                    //dbo_led( 0x02 );
				    led_toggle(LED_GREEN);
                    dbo_putchar( 'R' );
                    dbo_n8toa( seqid );
					opf_set_datalen( opf, len );
				    _output_openframe( opf, uart );
                    break;
                }
                count ++;
            }

        }
	
		aloha_evolve( mac, NULL );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay(1000);

		//break;
	}
}

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	//uint8 i;

    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\r' );
		dbo_putchar( '\n' );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( '.' );
    }
}
