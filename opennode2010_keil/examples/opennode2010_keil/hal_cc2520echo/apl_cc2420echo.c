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

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_debugio.h"


#define CONFIG_ACK_ENABLE
//#undef  CONFIG_ACK_ENABLE

#define PANID				0x0001
#define LOCAL_ADDRESS		0x0002   
#define REMOTE_ADDRESS		0x0001
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

static TiCc2420Adapter		g_cc;
static TiUartAdapter		g_uart;

//static char * g_buf=NULL;
//static unsigned char flag=0;

//void echonodetx(void);
//void echonoderx(void);
//void _cc2420_listener( void * owner, TiEvent * e );
static void echonode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{	
	echonode();
	//echonode();
    //echonodetx();
	//echonoderx();
}

void echonode(void)
{
    char opfmem[OPF_SUGGEST_SIZE];
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;

	char * msg = "welcome to echo...";
	//uint8 i, total_length, seqid=0, option, len;
    uint8 option, len;
    //uint16 fcf;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );
	dbo_open( 38400);

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    opf = opf_construct( (void *)(&opfmem), sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
    opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_enable_addrdecode( cc );				 //使能地址译码
	cc2420_setpanid( cc, PANID );				 //网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS ); //网内标识

	#ifdef CONFIG_ACK_ENABLE
	cc2420_enable_autoack( cc );
	#endif

	hal_enable_interrupts();

	while(1) 
	{
		// try receive a frame 
		while (1)
        {
            len = cc2420_recv(cc, (char*)(opf_buffer(opf)), 127, 0x00 );
			if (len > 0)
            {
				if (!opf_parse(opf, len))
				{
					// the incoming frame is invalid
					dbo_putchar( 'F' );
					continue;
				}

				led_on( LED_YELLOW );
				hal_delay( 500 );               
				led_off( LED_YELLOW );
				dbo_putchar( 'R' );
				_output_openframe( opf, &g_uart );
                break;
            }
            hal_delay(10);
        }
	

		// swap the sender and receiver pan id and address.
		opf_swapaddress( opf );

		//if (opf->shortaddrto != NULL){
		//dbo_putchar(0x99);}


		#ifdef CONFIG_ACK_ENABLE
		option = 0x01;
		#else
		option = 0x00;
		#endif

		// try send the frame back to its original sender
		while (1)
        {
            len = cc2420_send(cc, (char*)(opf_buffer(opf)), opf_datalen(opf)-1, option);
			if (len > 0)
            {
				led_on( LED_RED );
				hal_delay( 500 );               
				led_off( LED_RED );
				dbo_putchar( 'T' );
				_output_openframe( opf, &g_uart );
                break;
            }
            hal_delay(10);
        }
		

		cc2420_evolve( cc );

		// attention
        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value and eliminate all other hal_delay().
		hal_delay( 500 );
	}


}

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	if (opf_datalen(opf) > 0)
	{
		dbo_putchar( '>' );
	 	dbo_n8toa( opf_datalen(opf) );

		if (!opf_parse(opf, 0))
		{
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0] );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf_datalen(opf) );
		}
		dbo_putchar( '\n' );
	}
}


/*
void echonodetx(void)
{
	TiCc2420Adapter * cc;
    TiUartAdapter * uart;
    char * msg = "welcome to sendnode...";
    uint8 frame[BUF_SIZE];
    uint8 len,j;

    target_init();
    HAL_SET_PIN_DIRECTIONS();
    wdt_disable();

	dbo_open(0, 38400);

    led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

    cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
    uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

    uart_open( uart, 0, 38400, 8, 1, 0x00 );
    uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open( cc, 0, _cc2420_listener, NULL, 0x00 );

    cc2420_setchannel( cc, DEFAULT_CHANNEL );
    cc2420_setrxmode( cc );								//Enable RX
    //cc2420_enable_addrdecode( cc );                   //
    cc2420_setpanid( cc, PANID );						//网络标识
    cc2420_setshortaddress( cc, LOCAL_ADDRESS );		//网内标识

    hal_enable_interrupts();

	len = 21;
    frame[0] = len;
    frame[1] = 0x01;
    frame[2] = 0x02;
    for(j=3; j<19; j++)
    {
		frame[j] =j;
    }
    frame[19] = 19;
    frame[20] = 20;
    frame[len] =21;


	while(1)
	{
		len=cc2420_write( cc, (char*)(&frame[0]), len, 0x00 );
		msg="we have sent ";
	    uart_write( uart, msg, strlen(msg), 0x00 );
        	if((len>>4)<0x0A)
			{	
				uart_putchar(&g_uart, ((len>>4)&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, ((len>>4)&0x0F)+0X37);
			}
			if((len&0x0F)<0x0A )
			{	
				uart_putchar(&g_uart, (len&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, (len&0x0F)+0X37);
			}		
		msg=" bytes";
	    uart_write( uart, msg, strlen(msg), 0x00 );
		cc2420_setrxmode( cc );								//Enable RX
		hal_delay(4000);


		

	}
}



void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
	char * msg = "And we get  ";
	char buf[BUF_SIZE];
    uint8 len=0;
	uart_write( &g_uart, msg, strlen(msg), 0x01 );	
	len = cc2420_read(cc, &buf[0], BUF_SIZE, 0x00);
	len=len-1;
	if((len>>4)<0x0A)
			{	
				uart_putchar(&g_uart, ((len>>4)&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, ((len>>4)&0x0F)+0X37);
			}
			if((len&0x0F)<0x0A )
			{	
				uart_putchar(&g_uart, (len&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, (len&0x0F)+0X37);
			}		

	msg = " , they are:  ";
	uart_write( &g_uart, msg, strlen(msg), 0x01 );	

	if (len> 0)
	{
		for(int i=1;i<=len;i++)
		{	
			if((buf[i]>>4)<0x0A)
			{	
				uart_putchar(&g_uart, ((buf[i]>>4)&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, ((buf[i]>>4)&0x0F)+0X37);
			}
			if((buf[i]&0x0F)<0x0A )
			{	
				uart_putchar(&g_uart, (buf[i]&0x0F)+0X30);
			}
			else
			{
				uart_putchar(&g_uart, (buf[i]&0x0F)+0X37);
			}
			uart_putchar(&g_uart, ' ');		
		}
	}
	g_buf=buf;
	flag=len;
	uart_putchar(&g_uart, flag);

}



void echonoderx(void)
{
	TiCc2420Adapter * cc;
    TiUartAdapter * uart;
    char * msg = "welcome to revenode...";


    target_init();
    HAL_SET_PIN_DIRECTIONS();
    wdt_disable();

    led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

    cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
    uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

    uart_open( uart, 0, 38400, 8, 1, 0x00 );
    uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open( cc, 0, _cc2420_listener, NULL, 0x00 );

    cc2420_setchannel( cc, DEFAULT_CHANNEL );
    cc2420_setrxmode( cc );								//Enable RX
    //cc2420_enable_addrdecode( cc );                   //
    cc2420_setpanid( cc, PANID );						//网络标识
    cc2420_setshortaddress( cc, LOCAL_ADDRESS );		//网内标识

    hal_enable_interrupts();

	while(1)
	{	
		led_on( LED_YELLOW );
		hal_delay( 500 );
		led_off( LED_YELLOW );
		hal_delay( 500 );


		if(flag>0)
		{	
			msg = "I am sending back.....";
    		uart_write( uart, msg, strlen(msg), 0x00 );
			cc2420_write( cc, (char*)(&g_buf[0]), flag, 0x00 );
			flag=0;
		}
	}
}
*/
