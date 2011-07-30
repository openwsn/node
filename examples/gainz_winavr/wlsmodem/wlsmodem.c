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
 * wireless modem
 * 
 * @statues
 * - in testing
 * 
 * @author Zhang Wei(TongJi University) and Yan Shixing on 20091112
 *	- first created
 *
 * 
 ******************************************************************************/


#include "../common/hal/hal_configall.h"  
#include "../common/svc/svc_configall.h"  
#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"
#include "svc_wlsmodem.h"


#define CONFIG_ALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS		0x01
#define CONFIG_ALOHA_DEFAULT_REMOTE_ADDRESS		0x02
#define CONFIG_ALOHA_DEFAULT_CHANNEL            11

static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static TiWlsModem			m_modem;
static TiTimerAdapter *		m_timeradapter;

int main(void){
	
	target_init();
	wdt_disable();
	

	TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiWlsModem * modem;


	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );


	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );


	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	cc2420_open(cc, 0, NULL, NULL, 0x00 );//it is necessary since we have pick it outside from aloha_open 
	aloha_open( mac, cc, CONFIG_ALOHA_DEFAULT_CHANNEL, CONFIG_ALOHA_DEFAULT_PANID, CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS,m_timeradapter, NULL, NULL,0x00);

	modem = wlsmodem_construct( (void *)(&m_modem), sizeof(TiWlsModem) );
	modem = wlsmodem_open( modem, uart, mac );
	
	hal_enable_interrupts();
	
	while(1)
	{	
		wlsmodem_evolve( modem, NULL );
	}
}




	/*	if((len = aloha_recv( mac, opf, option )) > 0 )
		{	
			if(opf_parse(opf, 0))
			{
				uart_write( uart, opf->msdu, opf->msdu_len, 0x01 );
			}
			else dbo_putchar('X');
		}

		if((len = uart_read(uart, buf, sizeof(buf), 0x00 )) > 0 )
		{
			opf->msdu=buf;
        	opf_set_sequence( opf, 0x88 );
			opf_set_panto( opf, CONFIG_ALOHA_DEFAULT_PANID );
			opf_set_shortaddrto( opf, CONFIG_ALOHA_DEFAULT_REMOTE_ADDRESS );
			opf_set_panfrom( opf, CONFIG_ALOHA_DEFAULT_PANID);
			opf_set_shortaddrfrom( opf, CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS );

			// attention
			// aloha_send() will only try to send the frame. However, it cannot 
			// guarantee whether the frame is sent successfully. 
			// 因此, 应该判断aloha_send返回值,如>0表示成功发送,然后清除缓冲区, 允许再次被uart_read使用
			aloha_send( mac, opf, option );
		}
		*/
