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

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1
#define MAX_IEEE802FRAME154_SIZE                128
#include "apl_foundation.h"
#include "openwsn/hal/opennode2010/cm3/core/core_cm3.h"
#include "openwsn/hal/hal_mcu.h"
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
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/rtl/rtl_dumpframe.h"
#include "openwsn/svc/svc_nio_aloha.h"
#include "openwsn/hal/hal_timesync.h"
#include "openwsn/hal/hal_rtc.h"
#include "openwsn/hal/hal_interrupt.h"


#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_ALOHA_PANID				        0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS		        0x01
#define CONFIG_ALOHA_REMOTE_ADDRESS		        0x02
#define CONFIG_ALOHA_CHANNEL                    11

#define VTM_RESOLUTION                          5


#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)
//static TiCc2520Adapter		                    m_cc;//todo 用2520.c中的全局变量
static TiFrameRxTxInterface                     m_rxtx;
static char                                     m_nacmem[NAC_SIZE];
static TiAloha                                  m_aloha;
static TiTimerAdapter                           m_timer2;
TiRtcAdapter                                    m_rtc;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                                     m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiTimeSyncAdapter                               m_syn;
TiCc2520Adapter                                 m_cc;

void aloha_sendnode(void);
//void RTC_IRQHandler(void);
static void _rtc_handler(void * object, TiEvent * e);


int main(void)
{
	aloha_sendnode();
    return 0;
}

void aloha_sendnode(void)
{   
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor        * nac;
    TiAloha * mac;
	TiTimerAdapter   *timer2;
	TiFrame * txbuf;
    TiFrame * mactxbuf;
    TiRtcAdapter * rtc;
    TiTimeSyncAdapter *syn;
	char * pc;
	uint8 i, seqid=0, option;

	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );
	
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );

	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );//todo
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer2= timer_construct(( char *)(&m_timer2),sizeof(TiTimerAdapter));
    
	
	cc2520_open(cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2520_interface( cc, &m_rxtx );

    hal_assert( rxtx != NULL );
    
    timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 ); 
    
    timer_setinterval( timer2,1000,7999);

    
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, 
	CONFIG_ALOHA_LOCAL_ADDRESS, timer2 , NULL, NULL, 0x00 );
    mac->txbuf = mactxbuf;
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 ); 

    aloha_setremoteaddress( mac, CONFIG_ALOHA_REMOTE_ADDRESS );

    rtc = rtc_construct( (void *)(&m_rtc),sizeof(m_rtc));
    rtc = rtc_open(rtc,NULL,NULL,1,1);
    rtc_setprscaler( rtc,327);//rtc_setprscaler( rtc,32767);
    syn =  hal_tsync_open( &m_syn,rtc);
    nac_set_timesync_adapter( nac, syn);
    hal_attachhandler(  INTNUM_RTC, _rtc_handler,rtc );
    rtc_start( rtc);
	
	while(1) 
	{
        frame_reset(txbuf, 3, 20, 25);

       
        pc = frame_startptr( txbuf );
        pc[0] = 0x04;
        for (i=1; i<10; i++)
        {
            pc[i] = 0;
        }
        frame_setlength( txbuf,10);
        
		

        while (1)
        {  
   
            if (aloha_send(mac,0xffff, txbuf, 0x00) > 0)
            {	
                USART_Send( 0xa0);
                break;
            }
			else{
				nac_evolve( mac->nac, NULL);
			}
        }
		
   
        aloha_evolve( mac, NULL );
       
		
		hal_delayms(1000);

		//break;
	}

    frame_close( txbuf );
    aloha_close( mac );
    cc2520_close( cc );
}
/*
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
/*
        RTC_ClearITPendingBit(RTC_IT_SEC);
        m_rtc.currenttime++;
        led_toggle( LED_RED);
        USART_Send( 0xf1);
    }
}
*/

static void _rtc_handler(void * object, TiEvent * e)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);
        m_rtc.currenttime++;
        led_toggle( LED_RED);
        USART_Send( 0xf1);
    }
}

