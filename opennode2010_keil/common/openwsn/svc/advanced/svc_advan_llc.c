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
/****************************************************************************** 
 * svc_openllc
 * The Logic Link Control Layer (LLC) is built on top of MAC. In OpenWSN network
 * communication architecture, it's hard to distinguish the boundary between MAC
 * and LLC. Some functionalities have already been moved into PHY or MAC layer. 
 * 
 * Currently, the LLC layer in OpenWSN:
 *	- multiplex the single physical communication channel from MAC layer as serveral
 *    logical links. Each logical link is identified by "port" and is dispatched 
 *    to a specifical application service such as Localization, Time Synchronization, 
 *    Topology Control and Network Discovery and Joiness. 
 *  - provides an abstraction layer of MAC functions. So the high level protocols
 *    needn't care what kind of MAC is used.
 *    => However, it only support the ALOHA MAC currently.
 * 
 *  - This module doesn't care topology. No topology information used by LLC now.
 *
 * @author zhangwei in 200611
 *	- first created
 * @modified by zhangwei in 200908
 *	- revision. compile passed.
 *
 *****************************************************************************/

#include "svc_configall.h"
#include "svc_foundation.h"
#include "svc_openllc.h"

/****************************************************************************** 
 * OpenLLC Frame Format
 * OpenLLC only uses two byte for itself. Though ZigBee recommends standand 802.2 
 * LLC, but we needn't to use such a complicated solution. 
 * 
 * [Port 1B][Control 1B]
 * And the highest bit of the port byte indicate this is mono channel or group channel.
 * It's quite similar to the 802.2 DSAP (Destination Service Access Point).
 *
 * [] Data Link Layer, http://en.wikipedia.org/wiki/Data_Link_Layer#Logical_Link_Control_sublayer
 * [] IEEE 802.2 specification, http://standards.ieee.org/getieee802/download/802.2-1998.pdf.
 * [] LLC£ºÂß¼­Á´Â·¿ØÖÆ £­ IEEE 802.2 £¨Logic Link Control £­ IEEE 802.2£©, 
 *    http://www.networkdictionary.cn/protocols/llc.php
 *
 *****************************************************************************/

#define CONFIG_LLC_MAX_LINK_COUNT 16

/* For simplicity, the OpenLLC doesn't allocate logic link information dynamically.
 * On the contrary, it uses a pre-allocated solution. Every upper service has a 
 * pre-assigned id, and the openwsn system uses this id to identify which service
 * uses the current logic link. 
 * 
 * Logic link id 0 means the default link.
 */
#define LLC_LINK_DEFAULT     0
#define LLC_LINK_ROUTE       1
#define LLC_LINK_TOPO        2
#define LLC_LINK_TIMESYNC    3

typedef struct{
	void * owner;
	TiFunEventHandler listener;
	void * lisowner;
}TiOpenLink;

typedef struct{
	TiOpenLLC * mac;
	_TiOpenLink logiclinks[CONFIG_LLC_MAX_LINK_COUNT];
}TiOpenLLC;

TiOpenLLC * llc_construct( void * buf, uint16 size )
{
	memset( buf, 0x00, size );
	hal_assert( sizeof(TiOpenLLC) <= size ); 
	return (TiOpenLLC *)buf;
}

void llc_destroy( TiOpenLLC * llc )
{
	return;
}

TiOpenLLC * llc_open( TiOpenLLC * llc, TiOpenMAC * mac, TiFunEventHandler listener, void * lisowner )
{
	uint8 i;

	/* assert: mac has already been constructed and opened successfully */
	llc->mac = mac;
	
	link = (_TiOpenLink *)&(llc->links[0]);
	link->owner = (void *)llc;
	link->listener = listener;
	link->lisowner = lisowner;

	for (i=1; i<CONFIG_LLC_MAX_LINK_COUNT)
	{
		link->owner = (void *)llc;
		link = (_TiOpenLink *)&(llc->links[i]);
		link->listener = NULL;
	}
}

void llc_close( TiOpenLLC * llc )
{
	for (i=0; i<CONFIG_LLC_MAX_LINK_COUNT)
	{
		link = (_TiOpenLink *)&(llc->links[i]);
		link->listener = NULL;
	}
}

TiOpenLink * llc_connect( TiOpenLLC * llc, uint8 linkid, TiFunEventHandler listener, void * lisowner )
{
	hal_assert( linkid < CONFIG_LLC_MAX_LINK_COUNT );

	link = (TiOpenLink *)&(llc->links[linkid]);
	link->listener = listener;
	link->lisowner = lisowner;
}

void llc_disconnct( TiOpenLLC * uint8 linkid )
{
	link = (TiOpenLink *)&(llc->links[linkid]);
	link->listener = NULL;
	link->lisowner = NULL;
}

uint8 llc_send( TiOpenLLC * llc, uint8 linkid, TiOpenFrame * opf, uint8 option )
{
	hal_assert( opf->data != NULL );
	opf->data[0] = linkid;
	return mac_send( llc->mac, opf, option );
}

uint8 llc_broadcast( TiOpenLLC * llc, uint8 linkid, TiOpenFrame * opf, uint8 option )
{
	hal_assert( opf->data != NULL );
	opf->data[0] = linkid;
	return mac_broadcast( llc->mac, opf, option );
}

uint8 llc_recv( TiOpenLLC * llc, uint8 linkid, TiOpenFrame * opf, uint8 option )
{
	hal_assert( opf->data != NULL );
	opf->data[0] = linkid;
	return mac_send( llc->mac, opf, option );
}

llc_broadcast( TiOpenLLC * llc, uint8 link, TiOpenFrame * opf, uint8 option );




net_



llc_broadcast_listener

link 0 is the default link, it's always connect

TiQueuedNotifiier/Dispatcher

attach
detach


TiOpenLogicLink

TiOpenLLC * llc_construct( void * buf, uint16 size );
void llc_destroy( TiOpenLLC * llc ) 
llc_open( TiOpenLLC * llc, 
llc_close
llc_send( linkid, 
llc_read( linkid, 
llc_setlistener(


TiLogicLink llc_openlink( llc, linkid, listener )
connect
disconnect


int8 llc_probe( TiOpenLLC * mac );
int8 llc_updatestatistics( TiOpenLLC * mac );
int8 llc_getnode( TiOpenLLC * mac, TiOpenLLCNode * node );
int8 llc_getneighbors( TiOpenLLC * mac, TiOpenAddress * addr[] );
uint8 llc_getlinkquality( TiOpenLLC * mac, TiOpenAddress * addr );
uint8 llc_getsignalstrength( TiOpenLLC * mac, TiOpenAddress * addr );

