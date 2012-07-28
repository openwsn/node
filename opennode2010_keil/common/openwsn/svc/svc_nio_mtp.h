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


/* datatree  
 * This module implements a simple routing protocol DTP(Data Tree Protocol).
 *
 *	- Support frame flooding among the whole network. This is useful to discover 
 *	  new nodes and new topology. 
 *  - Support frame up-down transmission along the tree topology constraint. These
 *    frames include control frames from application layer, time synchronization
 *    frames, data collection commands frames. The frame can tranmitted to a specific 
 *    node or broadcast inside the tree. 
 *  - Support frame down-up transmission along the tree. 
 *  - Doesn't support transimitting between arbitrary node pair currently. 
 *  - Support multiple tree in a same physical network. They're distinguished by
 *    tree identifier. These trees can belong to a same sink node or belong to 
 *    different sink nodes.
 *  - Theorectically, the protocol doesn't contraint the alogorithm to build the
 *    tree. You can choose some energy or life time maximization policy to build
 *    the tree. However, the current implementation only support build the tree
 *    by flooding. 
 */  



 

/* flood routing protocol
 * flood is the most simple routing protocol. this component implements the most 
 * simple flooding mechanism which can send request(commands) to every nodes and 
 * collect their response back. 
 */

typedef struct 
{
	TiCc2420Frame rxframe;
	TiCc2420Frame txframe;
	uintx distance;
	uintx parent;
}TiFloodRouter;

TiMultiTreeRouteProtocol

typedef struct{
};

dtp_construct
dtp_destroy
dtp_open
dtp_close
dtp_mode
dtp_setmode
dtp_evolve

dtp_root_maitain
dtp_root_flood( not along the tree )
dtp_root_broadcast( along the tree )
dtp_root_send( to a single node )
dtp_root_recv( from all the nodes )
dtp_root_listener
dtp_root_evolve

dtp_leaf_probe
dtp_leaf_join
dtp_leaf_flood
dtp_leaf_broadcast( send to all the child nodes )
dtp_leaf_send
dtp_leaf_recv
dtp_leaf_leave
dtp_leaf_evolve
dtp_leaf_listener


typedef struct{
	mode
}TiMTP;

mtp_cast_as( root node or midium node )

mtp_construct
mtp_destroy
mtp_open
mtp_close
mtp_mode
mtp_setmode

mtp_root_newtree
mtp_root_deltree
mtp_root_maitain/buildtree
mtp_root_flood
mtp_root_broadcast
mtp_root_send
mtp_root_sendtoall
mtp_root_recv
mtp_root_listener
mtp_root_keepalive
mtp_root_evolve

mtp_leaf_probe
mtp_leaf_join
mtp_leaf_send
mtp_leaf_recv
mtp_leaf_leave
mtp_leaf_evolve
mtp_leaf_listener





mtp_



TiTreeRouteProtocol

trp_

ctp_broadcast
ctp_cast
ctp_

net


