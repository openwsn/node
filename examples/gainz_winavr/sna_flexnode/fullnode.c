/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#include "..\foundation.h"
#include "..\service\svc.h"
#include "..\global.h"
#include "fullnode.h"

/* the following two macro CONFIG_SINKNODE and CONFIG_GENERALNODE are used to define the type
 * of the node. they two cannot be defined at the same time 
 */

#define CONFIG_SINKNODE
//#undef CONFIG_SINKNODE

#define CONFIG_GENERALNODE
#undef CONFIG_GENERALNODE


static TOpenMAC				m_mac;
static TOpenNET				m_net;
static TSensorService		m_sensors;
static TLocationService		m_lcs;

static TOpenFrame			g_txframe;
static TOpenFrame			g_rxframe;
static char *				g_txframebuf = NULL;
static char *				g_rxframebuf = NULL;
static char *				g_txpktbuf = NULL;
static char *				g_rxpktbuf = NULL;


// the first WORD is the PAN id, the second WORD is the node id
// they two forms a standard 802.15.4 short address.
// you can freely changing this setting according to your own requirements.
//
static uint16 LOCAL_ADDRESS[2] = {0x2420, 0x5678};
static void gnode_execute( void );
static void sinknode_execute( void ); 
static int8 gnode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe );


void fullnode_test( void )
{
	#ifdef CONFIG_GENERALNODE
	gnode_execute();
	#endif
	
	#ifdef CONFIG_SINKNODE
	sinknode_execute();
	#endif
	
	return;
}

// for generic node in sensor network, the main program is as the following.
// if it received a packet from other node, it will forward it to its next hop.
//
#ifdef CONFIG_GENERALNODE
void gnode_execute( void )
{

	char *txdata = NULL, *rxdata = NULL, *buf = NULL;
	uint8 txlen = 0, rxlen = 0, len = 0, count = 0,n;
	
       target_init();
	global_construct();

	spi_configure( g_spi );
	uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
	
	g_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	g_net = net_construct( (char*)(&m_net), sizeof(m_net) );
	g_sensors = sen_construct( (char*)(&m_sensors), sizeof(m_sensors) );
	g_lcs = lcs_construct( (char*)(&m_lcs), sizeof(m_lcs) );
	
	
	mac_init( g_mac, g_cc2420, g_timer1 );
	mac_configure( g_mac, MAC_BASIC_INIT, 0);
	net_init( g_net, g_mac, NULL );
	
	opf_init( g_txframebuf, sizeof(g_txframe) );
	opf_init( g_rxframebuf, sizeof(g_rxframe) );


	g_txframebuf = (char *)(&g_txframe);
	g_rxframebuf = (char *)(&g_rxframe);
	g_txpktbuf = opf_packet( g_txframebuf );
	g_rxpktbuf = opf_packet( g_rxframebuf );
	       
	opf_setpanid( g_txframebuf, 0x2420);
	opf_setpanid( g_txframebuf, 0x2420);
	net_setlocaladdress( g_net, 0x2420, 0x1234);
	opf_setaddrfrom(g_txframebuf,0x1234);
        opf_setaddrto(g_txframebuf,0x5678);
	
	cc2420_receive_on(g_cc2420);  
        IRQEnable();
	
	g_txframebuf[0] = 11 + 10;
		
		
	for (n = 0; n < 10; n++) {
            g_txframebuf[n+10] = 0x77;
	}
	timer_start(g_timer1);
	while (1)
	{
		// if a packet can be read out through the NET layer interface of net_read()
		// and net_rawread(), then this is a packet to current node. the other
		// packets will be processed by NET layer itself, so they will not be 
		// read out by net_read() functions. these functions include those packets
		// to be forwarded.
		//

		
		if (rxlen == 0)

		{
			count = net_rawread( g_net, g_rxframebuf, 128, 0x00 );
			if (count > 0)
			{
				uart_write( g_uart, g_rxframebuf, rxlen, 0x00 );
			        rxlen = 0;
			}
		}
		
		
		
		if (opf_length(g_txframebuf) > 0)
		{
                        //led_twinkle(LED_GREEN,2);
			count = net_rawwrite( g_net, g_txframebuf, g_txframebuf[0], 0x00 );
                        	
		}
                
		//debug_evolve( g_debugio );
                led_twinkle(LED_GREEN,1);
		net_evolve( g_net );
		led_twinkle(LED_RED,1);
		//mac_evolve( g_mac );
	}
		
	global_destroy();	
	
	return;
}
#endif


// for sink nodes in sensor network, the main program is as the following.
// different to generic node in the network, it will forward the packet received
// to host computer through the UART/SIO hardware. while, it will also send 
// the packet received from the host to others nodes in the network.
//
#ifdef CONFIG_SINKNODE
void sinknode_execute( void )
{
	uint8 txlen=0, rxlen=0, count = 0,n;
	
	
	target_init();
	global_construct();


	spi_configure( g_spi );
	uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
	
	g_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	g_net = net_construct( (char*)(&m_net), sizeof(m_net) );
	g_sensors = sen_construct( (char*)(&m_sensors), sizeof(m_sensors) );
	g_lcs = lcs_construct( (char*)(&m_lcs), sizeof(m_lcs) );
	
	
	//mac_init( g_mac, g_cc2420, g_timer1 );
	mac_configure( g_mac, MAC_BASIC_INIT, 0);
	net_init( g_net, g_mac, NULL );

	
	
	
	opf_init( g_txframebuf, sizeof(g_txframe) );
	opf_init( g_rxframebuf, sizeof(g_rxframe) );


	g_txframebuf = (char *)(&g_txframe);
	g_rxframebuf = (char *)(&g_rxframe);
	g_txpktbuf = opf_packet( g_txframebuf );
	g_rxpktbuf = opf_packet( g_rxframebuf );

	opf_setpanid( g_txframebuf, 0x2420);
	opf_setpanid( g_txframebuf, 0x2420);
	net_setlocaladdress( g_net, LOCAL_ADDRESS[0], LOCAL_ADDRESS[1] );
	opf_setaddrfrom(g_txframebuf,0x5678);
        opf_setaddrto(g_txframebuf,0x1234);
	//net_configure
	//net_setaddress


        cc2420_receive_on(g_cc2420);  
        IRQEnable(); 


        g_txframebuf[0] = 11 + 10;
		
	txlen = g_txframebuf[0];	
	for (n = 0; n < 10; n++) {
            g_txframebuf[n+10] = 0x77;
	}
	timer_start(g_timer1);
	while (1)
	{
		
		
		// try to read data from the UART/SIO connected to the host.
		// if data received, then try to send them through net_rawwrite()
		if (txlen == 0)
		{
			
			count = uart_read( g_uart, g_txframebuf + txlen, sizeof(TOpenFrame) - txlen, 0x00 );
			txlen += count;
		}
		
		if (opf_length(g_txframebuf) > 0)
		{
                        //led_twinkle(LED_GREEN,2);
			count = net_rawwrite( g_net, g_txframebuf, g_txframebuf[0], 0x00 );
                        	
		}
		
		
		// try to read data from the network. if data received, then try to send 
		// them to the host through the UART/SIO.
		
	
		
		if (rxlen == 0)
		{

			count = net_rawread( g_net, g_rxframebuf, 128, 0x00 );

			if (count > 0)
			{
				uart_write( g_uart, g_rxframebuf, rxlen, 0x00 );
			        rxlen = 0;
			}
		}
		
		if ((rxlen > 0) && (rxlen <= opf_length(g_rxframebuf)))
		{ 

			count = opf_length(g_rxframebuf) - rxlen;
			count = uart_write( g_uart, g_rxframebuf + count, rxlen, 0x00 );

			if (count > 0)
			{
				if (rxlen <= count)
					rxlen = 0;
				else 
					rxlen -= count;
			}
		}
		
		//debug_evolve( g_debugio );
		net_evolve( g_net );
		led_twinkle(LED_GREEN,1);
	}
		
	global_destroy();	
	
	return;
}
#endif

// this function will interpret the frame/packet received in txframebuf, and 
// put the processing reply into rxframebuf
//
int8 gnode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe )
{
	char *txdata, *rxdata;
	int8 ret=0;

	if ((rxframe->length == 0) || (txframe->length > 0))
	{
		return -1;
	}

	rxdata = opt_data(opf_packet((char*)rxframe)); 
	txdata = opt_data(opf_packet((char*)txframe)); 

	// these request are usually sent by the sink node. and the sink node hope the 
	// sensor nodes can return some reply information.
	// rxdata[0] is the data type
	switch (rxdata[0])
	{
	// if the received packet is a LOCATION request, then fill the "txframe" with 
	// location information and send it back. usually, the sink node will flooding 
	// a LOCATION request and expect to receive the location information of all the 
	// node. this is the APPLICATION layer mechanism. don't mix it with the "TLocationService"
	// while, the "TLocationService" will send/received frames based on MAC layer 
	// interface. 
	//
	case ODA_TYPE_LOCATION_REQUEST:
		//lcs_evolve( g_lcs, &m_lcs, g_cc2420 );
		txdata[0] = ODA_TYPE_LOCATION;
		memmove( txdata + 1, (char*)(&m_lcs), sizeof(m_lcs) );
		txframe->length = OPF_HEADER_SIZE + 1 + sizeof(m_lcs);
		break;

	case ODA_TYPE_TEMPSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_TEMPSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;

	case ODA_TYPE_VIBSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_VIBSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;

	case ODA_TYPE_LIGHTSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_LIGHTSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;
	}

	return ret;
}

