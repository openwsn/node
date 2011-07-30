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
 * Sink node 
 * this program test the function that the sinknode try to get command from the
 * gateway computer and send data request to sensornodes. And then collect the 
 * data response and send it back to gateway computer.
 * @author Yan-shixing on 20100519
 *	- first created
 ******************************************************************************/

#include "../../common/openwsn/hal/hal_configall.h"  
#include "../../common/openwsn/svc/svc_configall.h"  
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_xtppacket.h"
#include "../../common/openwsn/rtl/rtl_variant.h"
#include "../../common/openwsn/rtl/rtl_varsequ.h"
#include "../../common/openwsn/rtl/rtl_dataexchange.h"

#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/svc/svc_siocomm.h"
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_ledtune.h"
#include "../../common/openwsn/svc/svc_one2many.h"



#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define VTM_RESOLUTION 7

#define WAIT_COMMAND	0x01
#define WAIT_DATA		0x02

#define DATA_REQUEST	0x01

#define CONFIG_XTPSEQU_CAPACITY 10

static TiTimerAdapter 		m_timeradapter;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static char                 m_siobuf[ IOBUF_HOPESIZE(0x7F) ];
static char 				m_opfmem[ OPF_SUGGEST_SIZE ];
static TiOne2Many 			m_o2m;
static TiSioComm			m_sio;
static TiBlockDeviceInterface 		m_intf;

/* output the TiIoBuf content throught UART by default */
void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart );

int main(void)
	{
	char * msg = "welcome to sinknode ...";

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti;

	TiCc2420Adapter * cc;
    TiAloha * mac;

	TiUartAdapter * uart;
	TiIoBuf * buf;
	TiOne2Many * o2m;
	TiSioComm * sio;
	TiOpenFrame * opf;

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
	buf = iobuf_construct( &m_siobuf[0], sizeof(m_siobuf) );
	opf = opf_open( &m_opfmem[0], sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );

	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	aloha_open( mac, cc, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timeradapter, NULL, NULL,0x01);
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );
	vti = vtm_apply( vtm );
	vti_open( vti, NULL, NULL );

	o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	one2many_open( o2m, mac);

	sio = sio_construct( (char *)(&m_sio), sizeof(TiSioComm) );
	sio_open( sio, uart_get_blockinterface(uart, &m_intf), 0x00 );

	uart_write( uart, msg, strlen(msg), 0x00 );

	hal_enable_interrupts();

	uint8 state = WAIT_COMMAND;
	uint8 ctrl, flowno;
	char timepoint[10];
	uint8 command, len, count;
	char * pc;

	TiXtpPacketDescriptor cmd_desc;
	TiXtpVarSequence * cmd_sequ;
	cmd_sequ = xtp_varsequ_create(CONFIG_XTPSEQU_CAPACITY);

	TiVariant * cmd_var;

	while(1)
	{
		
		//waiting for command from gateway computer
		while( state == WAIT_COMMAND )
		{
			count = sio_read( sio, buf, 0x00);
			if( count > 0 )
			{
				pc = iobuf_ptr(buf);
				for(uint8 i=0;i<iobuf_length(buf);i++)
					dbo_n8toa( pc[i] );
				//check if the information is a command
				xtp_pktdesc_parse( &cmd_desc, buf );
				dxc_unpack(&ctrl, &flowno, &timepoint[0], cmd_sequ, cmd_desc.payload, cmd_desc.length-28);
		dbo_putchar('\n');		
		dbo_putchar('k');
		dbo_n8toa(ctrl);
		dbo_putchar('\n');
		dbo_putchar('m');
		dbo_n8toa(flowno);
		dbo_putchar('\n');
		dbo_putchar('n') ;
		for(uint8 i=0;i<10;i++)
		{
			dbo_n8toa(timepoint[i]);
		}
		dbo_putchar('\n');				

				if( ctrl == 0x06 )
				{
					//if so, check if the first variable is a dataRequest command
					cmd_var = xtp_varsequ_first( cmd_sequ );
					if( cmd_var != NULL )
					{
						command = cmd_var->mem.uint8value;
						dbo_putchar('S');
						dbo_n16toa(cmd_var->length);
						dbo_n8toa(cmd_var->type);
						dbo_n8toa(cmd_var->mem.uint8value);
						switch(command)
						{
							case DATA_REQUEST:
								dbo_putchar('T');
								iobuf_clear(buf);
								iobuf_pushbyte( buf, 0x88 );
								iobuf_pushbyte( buf, 0x00 );
								iobuf_pushbyte( buf, 0x00 );
								iobuf_pushbyte( buf, 0x00 );
								iobuf_pushbyte( buf, CONFIG_NODE_ADDRESS );
								one2many_broadcast(o2m, buf, 0x00);
								dbo_putchar('T');
								state = WAIT_DATA;
								break;
							default:
								break;
						}

					}
				}
			}
		}
		//get the command and waitfor data for 1 min and send it out from uart
		vti_setscale( vti, 1 );
		vti_setinterval( vti, 1000, 0x00 );
		vti_start( vti );

		while (!vti_expired(vti))
		{
			len = aloha_recv( mac, opf, 0x00 );
			if (len > 0)
			{	
				pc = opf_msdu(opf);
				if(pc[0] == 0x02)
				{
					iobuf_clear(buf);
					memmove(iobuf_ptr(buf), &opf->buf[0], opf_framelength(opf)+1);
					sio_write( sio, buf, 0x00 );
				}
			}
		}


	}
}

void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart )
{
	char * response;

	if (iobuf_length(buf) > 0)
	{
		response = iobuf_ptr(buf);
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
		dbo_putchar(response[5]);
		dbo_putchar(response[6]);
	}
}
