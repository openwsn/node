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
#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"

/****************************************************************************** 
 * sinknode
 * this is the gateway node between wireless sensor networks and computer networks.
 * it will send the frames received through UART by wireless and send the one received 
 * through wireless by UART.
 *
 * @author zhangwei on 20090523
 * 	- first created
 * @modified by xxx on 20090524
 *	- revision
 *****************************************************************************/


#define CONFIG_RXBUF_SIZE 127
#define CONFIG_TXBUF_SIZE 254

/****************************************************************************** 
 * TiSinkNode
 * this is the gateway node between wireless sensor networks and computer networks.
 * it will send the frames received through UART by wireless and send the one received 
 * through wireless by UART.
 *
 * the current version is based on MAC layer and SioComm. 
 * the internal command interpreter may use more components. 
 * 
 * rxlen        data length of the wireless frame 
 * txlen        data length of the uart frame
 * rxbuf		holds the data received through wireless. they'll be forwarded through 
 *				uart. 
 * txbuf		holds the data received through uart. they'll be forwarded through wireless.
 * 
 *****************************************************************************/

typedef struct{
	TiUartAdapter * uart;
	TiOpenMAC * mac;
	uint8 rxlen;
	uint8 txlen;
	char rxbuf[CONFIG_RXBUF_SIZE];
	char txbuf[CONFIG_TXBUF_SIZE];
}TiSinkNode;

TiSinkNode * sinknode_construct( void * buf, uint size )
{
	TiSinkNode * node = (TiSinkNode *)buf;
	assert( sizeof(TiSinkNode) <= size );

	memset( buf, 0x00, size );
	node->rxlen = 0;
	node->txlen = 0;
}

void sinknode_destroy( TiSinkNode * node )
{
	return;	
}

void sinknode_connect( TiSinkNode * node, TiUartAdapter * uart, TiOpenMAC * mac )
{
	node->uart = uart;
	node->mac = mac;
}

/* assume: the TiCc2420Adapter, TiUartAdapter, MAC, SioComm are all prepared to
 * work. 
 */
void sinknode_evolve( TiSinkNode * node, TiEvent * e )
{
	switch (e->id)
	{
	case EVENT_UART_DATA_ARRIVAL:
		bufptr = (char*)&(node->txbuf[0]) + node->txlen;
		len = uart_read( node->uart, bufptr, CONFIG_TXBUF_SIZE - node->txlen );
		if (len > 0)
		{
			len = split_txhandle( uartbuf, txbuf ) 
			if newframe
				if this is for the 
		}

		siofilter-put
		if siofilter-read
			if this frame is for the sinknode itself then interpret it
			else mac_send

	case EVENT_WIRELESS_DATA_ARRIVAL:
		len = mac_read( node->mac, buf, size );
		if (len > 0)
		{
			split_rxhandle( ... )
			uart_write( ...)
		}
	}
}

void sinknode_interpret( TiSinkNode * node, char * buf, int len )
{
	switch (buf[0])
	{
	case CMD_NETWORK_STARTUP:
	case CMD_NETWORK_SHUTDOWN:
	case CMD_NETWORK_SLEEP:
	case CMD_NETWORK_WAKEUP:
	case CMD_PREPARE_ROUTING:
		break;

	case CMD_REQUEST_DATA:
		mac_send(flood);

	case CMD_TIME_SYNC:
	case CMD_LOCALIZE:
		break;
	}
}



//----------------------------------------------
