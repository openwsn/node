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

/*****************************************************************************
 * @author zhangwei on 20070115
 *  first created, determined the interface.
 ****************************************************************************/ 

/*****************************************************************************
 * The key idea of OpenNET
 * the running of OpenNET needs the support from libsink. libsink will periodically
 * send packets to maintain the network routing, thus the developing of OpenNET 
 * on sensor node can be more simple.
 * 
 * there're 3 type packets in OpenNET
 * ADV		broadcast to the whole network by libsink
 * 			from sink node to all the other nodes
 * 			usually the command packet.
 * 			different to QUERY type, ADV packets are transported by broadcasting.
 * QUERY 	similar to ADV type. but it has a PATH indicator in the packet.
 * 			the path indicator indicate the approximate query range of the 
 * 			QUERY. for example, 2 hops near node 3. the path indicator can also 
 * 			do help to its routing.
 * 			may contains other query criterions
 * DATA		data type packet. it has a PATH indicator in the packet.
 * 
 * for every node
 * 1. check the received packet
 * 2. if the packet is QUERY packet, the check whether itself is the destination.
 * 	 2.1 if the current node is the destination, then interpret the packet and 
 * 		sends the DATA packet back.
 *   2.2 if the current node is not the destination, then check local buffer. 
 * 			if the packet has been received in the past, then drop it
 * 			else forward the packet to neighbors
 * 3. if the packet is DATA packet, then check local buffer. if not exists, then
 * 	forward it to the next hop according to the path indicator in the packet.  
 * 4. if the packet is ADV packet, and the packet not in local buffer, 
 * 	then broadcast the packet to its neighbors.
 * 
 *****************************************************************************
 * NET layer packet format:
 * 
 * for DATA packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] [mB data]
 * 		[mB data] usually is [1B sensor type][(m-1)B sensor data]
 * 
 * for QUERY packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] 
 * 		[1B query command][mB query parameter]
 * 
 * for ADV packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] 
 * 		[1B command][mB command parameter]
 * 
 * the packet control byte
 * b7 b6.... b0
 * 
 * b7 b6  	packet type
 * 			00 DATA		(from sensor to sink, not broadcast)
 * 			01 QUERY	(from sink to sensor, not broadcast)
 * 			02 ADV		(from sink to sensor, broadcast)
 * 			03 (reserved, maybe from sensor to sensor/sink and broadcast)
 * b5 = 0	(reserved)
 * b4 = 1   using path indicator
 * b3 b2 b1 b0 
 * 			TTL, maximum hop this packet can endure
 ****************************************************************************/ 

#include "svc_foundation.h"
#include "svc_opennet.h"

typedef struct{
  uint16 dest;
  uint16 next;
  uint8 metric;
  uint8 quality;
}TRouteTableItem;

TRouteTableItem g_routetable[ NET_ROUTE_TABLE_SIZE ];
TiOpenNET g_opennet;

TiOpenNET * net_construct( char * buf, uint16 size )
{
	//assert( sizeof(TiOpenNET) <= size );
	memset( buf, 0x00, size );
	return (TiOpenNET *)buf;
}

void net_destroy( TiOpenNET * net )
{
	return;
}

void net_init( TiOpenNET * net, TiOpenMAC * mac, TActionScheduler * actsche )
{
	net->mac = mac;
}

void net_configure( TiOpenNET * net, uint8 ctrlcode, uint8 value )
{
}

int8 net_setlocaladdress( TiOpenNET * net, uint16 pan, uint16 nodeid )
{
	mac_configure( net->mac, MAC_CONFIG_PANID, pan);
	mac_configure( net->mac, MAC_CONFIG_LOCALADDRESS, nodeid);
	//mac_setlocaladdress( );
	return 0;
}

void net_getrmtaddress( TiOpenNET * net, uint16 * pan, uint16 * nodeid )
{
	*pan = net->panid;
	*nodeid = net->nodeid;
}

int8 net_read( TiOpenNET * net, TiOpenFrame * frame, uint8 size, uint8 opt )
{
	return mac_read( net->mac, frame, size, opt );
}

int8 net_rawread( TiOpenNET * net, char * framebuf, uint8 size, uint8 opt )
{
	return mac_rawread( net->mac, framebuf, size, opt );
}

int8 net_write( TiOpenNET * net, TiOpenFrame * frame, uint8 len, uint8 opt )
{
	return mac_write( net->mac, frame, len, opt );
}

int8 net_rawwrite( TiOpenNET * net, char * framebuf, uint8 len, uint8 opt )
{
	return mac_rawwrite( net->mac, framebuf, len, opt );
}

int8 net_forward( TiOpenNET * net, TiOpenFrame * frame, uint8 len, uint8 opt )
{
	//return mac_forward( net->mac, frame, size, opt );
	return 0;
}

int8 net_rawforward( TiOpenNET * net, char * framebuf, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_evolve( TiOpenNET * net )
{
	mac_evolve( net->mac );
	return 0;
}

int8 net_sleep( TiOpenNET * net )
{
	mac_sleep( net->mac );
	return 0;
}

int8 net_wakeup( TiOpenNET * net )
{
	mac_wakeup( net->mac );
	return 0;
}

int8 net_installnotify( TiOpenNET * net, TEventHandler * callback, void * data )
{
	return 0;
}


