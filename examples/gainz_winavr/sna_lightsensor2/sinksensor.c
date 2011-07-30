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
/* This is a test program for gateway testing only. 
 * The sinksensor program will do the following: 
 * 1. Accept REQUEST packet from gateway computer throught RS232 and start 
 * 		sampling according to REQUEST indication; Encapsulate sampled data as RESPONSE 
 * 		and send it back to the gateway computer through RS232.
 * 2. Periodically sampling the luminance and send it to the gateway 
 * 		computer as VARDATA packet
 * 3. Broadcast what received from the gateway computer through the wireless transceiver. 

 * 	@Created by zhangwei on 20100406
 */

//#include "../../common/openwsn/configall.h"
//#include "../../common/openwsn/foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/rtl/rtl_textspliter.h"
#include "../../common/openwsn/rtl/rtl_textcodec.h"
#include "../../common/openwsn/rtl/rtl_lightqueue.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
//#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_device.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_ledtune.h"
#include "../../common/openwsn/svc/svc_one2many.h"
#include "../../common/openwsn/svc/svc_siocomm.h"


/* todo
 * yanshixing
 * please finish rtl_textspliter and svc_siocomm first 
 * however, before working on these two modules, please firstly try to finish the main program 
 * you can understand more about the interface of textspliter and siocomm
 */

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define CONFIGURE_COMMAND				0x11
#define RESTART_COMMAND					0x22
#define TEST_CMD						0x33

#define VTM_RESOLUTION 					7

/* attention
 * You should guarantee the following memory doesn't exceed the MCU's memory limit!
 * or else you may encounter unexpected errors!
 */
static TiTimerAdapter 		m_timeradapter;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
//static TiOne2Many 			m_o2m;
static TiUartAdapter		m_uart;
static TiSioComm        	m_sio;
static TiTextSpliter		m_spliter;
static TiAdcAdapter         m_adc;
static TiLumSensor          m_lum;

static TiBlockDeviceInterface m_intf;

//static TiLedTune          m_ledtune;
static char                 m_txbuf[ IOBUF_HOPESIZE(0x7F) ];
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
//static char                 m_txque[ LIGHTQUEUE_HOPESIZE(sizeof(TiIoBuf), 3) ];
//static char                 m_rxque[ LIGHTQUEUE_HOPESIZE(sizeof(TiIoBuf), 3) ];

/* output the TiIoBuf content throught UART by default */
int8 _interpret( char * cmd, uint8 len );
void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart );
void _gatwsink();
uint8 _text_digit2hexchar( uint8 num )
{
	static char t_digit2hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return (t_digit2hextable[num & 0x0F]);
}	
int main(void)
{
	_gatwsink();
	return 0;
}

void _gatwsink()
{
	uint8 count;	
	char * msg = "welcome to gateway sink node...";
	char * pkt;
	uint16 val;

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti;

	TiCc2420Adapter * cc;
    TiAloha * mac;

	TiUartAdapter * uart;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
	//TiOne2Many * o2m;
	TiSioComm * sio;
    //TiLedTune * ledtune;

	// Each item in the queue is an TiIoBuf object which contains an entire packet.
	//TiLightQueue * rxque;
	//TiLightQueue * txque;

	//???
	TiTextSpliter * spliter;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open(0, 38400);

	timeradapter = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	txbuf = iobuf_construct( &m_txbuf[0], sizeof(m_txbuf) );
	rxbuf = iobuf_construct( &m_rxbuf[0], sizeof(m_rxbuf) );
	//txque = lwque_construct( &m_txque[0], sizeof(m_txque), sizeof(m_txbuf) );
	//rxque = lwque_construct( &m_rxque[0], sizeof(m_rxque), sizeof(m_rxbuf) );

	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	aloha_open( mac, cc, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timeradapter, NULL, NULL,0x01);
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );
	vti = vtm_apply( vtm );
	vti_open( vti, NULL, NULL );
    //ledtune = ledtune_construct( &m_ledtune[0], sizeof(m_ledtune), vti );
    //ledtune_open( ledtune );

	// assume: we use One2Many architecture to collect data from other nodes. 
	// todo: DataTree Protocol should be used to on the sink node in the future.

	//o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	//one2many_open( o2m, mac);

	spliter = tspl_construct((void *)(&m_spliter), sizeof(TiTextSpliter));
	sio = sio_construct( (char *)(&m_sio), sizeof(TiSioComm) );
	sio_open( sio, uart_get_blockinterface(uart,&m_intf), 0x00 );

	uart_write( uart, msg, strlen(msg), 0x00 );

	TiLumSensor * lum;
	TiAdcAdapter * adc;

	adc     = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	lum     = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );

	adc_open( adc, 0, NULL, NULL, 0 );
	lum_open( lum, 0, adc );

	hal_enable_interrupts();

	vti_setscale( vti, 1 );
	vti_setinterval( vti, 3000, 0x00 );
	vti_start( vti );

	while(1)
	{
		// Periodically sampling the luminance and send it to the gateway 
		// computer as VARDATA packet
		/*count = uart_read(uart, iobuf_ptr(rxbuf), iobuf_size(rxbuf), 0x00);
		if(count>0)
		{
			iobuf_setlength(rxbuf, count);
			iobuf_copyto(rxbuf, txbuf);
			uart_write(uart, iobuf_data(txbuf), iobuf_length(txbuf), 0x00);
		}
		if(vti_expired(vti))//if time is up
		{
			val = lum_value( lum );//sensor the lumince
			iobuf_pushbyte(txbuf, HIGH_BYTE(val));
			iobuf_puchbyte(txbuf, LOW_BYTE(val));
			if(!iobuf_empty(txbuf))
			{
				sio_write(sio, txbuf);//send it to gateway
			}
			//restart the timer
			vti_setscale( vti, 1 );
			vti_setinterval( vti, 3000, 0x00 );
			vti_start( vti );
			led_toggle(LED_RED);
		}*/
		//else//if time is not up
		{
			count = sio_read(sio, rxbuf, 0x00);//check the inbox
			
			if( count>0 )//if there are messages
			{
				pkt = iobuf_data(rxbuf);
				/*len = iobuf_length(rxbuf);
				dbo_putchar(len);
				for(uint8 i=0;i<len;i++)
				{
					dbo_putchar(pkt[i]);
				}*/
				if( pkt[28] == 0x77 )//judge whether it is request
				{
					led_toggle(LED_GREEN);
					char * myid = "VARDATA";
					iobuf_pushback(txbuf, myid, 7);
					val = lum_value( lum );//if so, sensor and send
					iobuf_pushbyte(txbuf, _text_digit2hexchar(HIGH_BYTE(val)));
					iobuf_pushbyte(txbuf, _text_digit2hexchar(LOW_BYTE(val)));
					if(!iobuf_empty(txbuf))
					{
						count = sio_write(sio, txbuf, 0x00);
						iobuf_clear(txbuf);
					}
				}//or do nothing
			}//or do nothing
		}
		//sio_evolve( sio, NULL );
		led_toggle(LED_RED);
		hal_delay(1000);
	}
}

/* return
 *	1	command execute success
 *	0	this isn't an valid command
 *	-1	command execution failed
 */
int8 _interpret( char * cmd, uint8 len )
{
	int8 result=0;

	switch (cmd[0])
	{
	case CONFIGURE_COMMAND:
		// execute the command
		result = 1;
		break;

	case RESTART_COMMAND:
		// execute the command
		result = -1;
		break;

	case 0x44:
		switch (cmd[1])
		{
		case TEST_CMD:
			result = 1;
			break;
		}
		break;

	default:
		result = 0;
	}

	return result;
}

void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart )
{
	char * response;
	uint8 len = 0, i=0;
	len = iobuf_length(buf);

	if ( len > 0)
	{
		response = iobuf_ptr(buf);
		
		for( i=0; i<len; i++ )
		{
			dbo_n8toa(response[i]);
			dbo_putchar(' ');
		}
		/*dbo_string( "response:\r\n" );

		dbo_string( "shortaddrto: " );
		dbo_n8toa( response[1] );
		dbo_n8toa( response[2] );

		dbo_string( "\r\nshortaddrfrom: " );
		dbo_n8toa( response[3] );
		dbo_n8toa( response[4] );

		dbo_string( "\r\nsensor value:" );
		dbo_n8toa( response[5] );
		dbo_n8toa( response[6] );
		dbo_string( "\r\n" );
		*/
		//dbo_putchar(response[5]);
		//dbo_putchar(response[6]);
	}
}
