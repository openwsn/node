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
 * svc_datatree  
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
 *
 * @modified by zhangwei(openwsn@gmail.com) on 20091123
 *	- improve cache management. Now cache consume much fewer memory than it's in 
 *    the past.
 *  - correct possible bugs when dealing with sequence id. Now the master program
 *    should assign appropriate sequence id to the packet. 
 *
 * @modified by zhangwei(openwsn@gmail.com) on 20091123
 *	- Improve request/response sequence id management. Now the request and response 
 *    id are managed separately. The response id member variable is assigned when 
 *    this DTP service receives a request. 
 *
 * @modified by zhangwei(openwsn@gmail.com) on 20091126
 *	- revise the whole process in dtp_evolve(). 
 *  - add new function _dtp_rxbuf_recv_or_forward()
 *  - add depth support. this will enable the node to accept the shortest path 
 *    to root node. or else, the parent property may point to the wrong node because
 *    the node changes parent by the last maintain request arrived.
 *  - tested by shi-miaojing, yan-shixing(TongJi University)
 ******************************************************************************/  

#include "svc_configall.h"
#include <string.h>
#include "../hal/hal_mcu.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_cc2520.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_aloha.h"
#include "svc_nio_datatree.h"

static bool _dtp_evolve_recv_check( TiDataTreeNetwork * net );
static bool _dtp_evolve_startup_maintain_check( TiDataTreeNetwork * net );
static void _dtp_evolve_node_recv_or_forward( TiDataTreeNetwork * net );
static void _dtp_evolve_sink_recv( TiDataTreeNetwork * net );

inline static uint8 _net_get_frame_feature( TiFrame * frame, char * feature, uint8 size );
inline static void  _switch_ptr( TiFrame *__packed *ptr1, TiFrame *__packed * ptr2 );


/******************************************************************************* 
 * obsolete content
 * Frame Format of DataTree Data Collection Protocol
 * 
 * [MAC Header nB] [DTP Section Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] 
 * [Tree Id 1B] [Hopcount 1B] [Payload mB]
 * 
 * [DTP Section Length 1B]
 * Length of the DTP data. Not include the length byte itself.
 *
 * [DTP Identifier 1B]
 * Always 0x09. Indicate this is using Data Tree Data Collection Protocol. 
 *
 * [DTP Protocol Identifer 1B]
 * [DTP Command and Control 1B]
 * b1 b0  = 00 Broadcast 01 Multicast 10 Unicast 11 Response
 *		Broadcast is used to establish the tree and maintain the tree.
 *		Multicast will transmit the frame across a subtree. 
 *      Unicast will transmit the frame to the specified node.
 *      Response frame will always forwarded to the root node.
 *
 * [Tree Id 1B]
 * Actually, the entire tree Id should include the root node address. But the address
 * is already in MAC header. So we needn't to save a copy here. Now this value 
 * functions as the sequence number. 
 * 
 * [Hopcount 1B]
 * How many hops the frame is transmitted.
 *
 * DTP's Build Tree Frame 
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [HopCount 1B] 
 * Control = broadcast type
 *
 * DTP's Request Frame
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [Maximum Hopcount 1B] [User Data mB]
 * 
 * DTP's Response Frame
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [User Data mB]
 *
 ******************************************************************************/  

TiDataTreeNetwork * dtp_construct( void * mem, uint16 size )
{
	memset( mem, 0x00, size );
	hal_assert( sizeof(TiDataTreeNetwork) <= size );
	return (TiDataTreeNetwork *)(mem);
}

void dtp_destroy( TiDataTreeNetwork * net )
{
	dtp_close( net );
}

TiDataTreeNetwork * dtp_open( TiDataTreeNetwork * net, TiAloha * mac, uint16 localaddress, 
	TiFunEventHandler listener, void * lisowner, uint8 option )
{
	net->state = (option & 0x01) ? DTP_STATE_IDLE : DTP_STATE_STARTUP;
	net->option = option;
	net->pan = CONFIG_ALOHA_DEFAULT_PANID;
	net->localaddress = localaddress;
	net->root = 0x0000;
	net->parent = 0x0000;
	net->count = 0;//todo for testing
	net->depth = ~0;
	net->distance = ~0;
	net->mac = mac;
	net->listener = listener;
	net->lisowner = lisowner;
	net->txtrytime = DTP_MAX_TX_TRYTIME;
    net->rssi = 0;
	net->txque = frame_open( (char * )( &net->txque_mem), FRAME_HOPESIZE(DTP_MAX_FRAME_SIZE), 3, 20, 102);
	net->rxque = frame_open( (char *)( &net->rxque_mem), FRAME_HOPESIZE(DTP_MAX_FRAME_SIZE), 3, 20, 102);
	net->rxbuf = frame_open( (char *)( &net->rxbuf_mem), FRAME_HOPESIZE(DTP_MAX_FRAME_SIZE), 3, 20,102);

	net->cache = dtp_cache_open( (char *)( &net->cache_mem), DTP_CACHE_HOPESIZE );
	hal_assert( net->cache != NULL );

	net->request_id = 0;
	net->response_id = net->request_id;

	return net;
}

void dtp_close( TiDataTreeNetwork * net )
{
	frame_totalclear( net->rxbuf );
	frame_totalclear( net->txque );
	frame_totalclear( net->rxque );

	cache_close( net->cache );
}


/* Build the tree covering all the nodes in the network by flooding. 
 *
 * @attention
 * Only the sink node in the network can send maintain request in the network.
 * because the sender will be automatically regarded as the root node of the data
 * collection tree. 
 * 
 * Currently the DTP service doesn't support a new node to join the network. 
 * The new node should be powered on and must wait for the maintain request from 
 * the root node.
 *
 * In real applications, the sink node is usually as the root node. 
 * 
 * @attention
 * - This function should be called by the root node(sink node) only. If a sensor 
 * node call this function, then the sensor node will be the new root node!
 * - This function only sends the maintain request!
 *
 * @todo
 *	currently, the root node doesn't need the maintain response packet. So I don't
 * implement the maintain response in this service.
 */
uint8 dtp_maintain( TiDataTreeNetwork * net, TiFrame * f,uint8 max_hopcount )
{	
	char * request;
    uint8 len, count;

	//todo 函数内是不可以开辟frame的，否则程序就飞掉了！
	//todo for testing char  frame_mem[FRAME_HOPESIZE(DTP_MAX_FRAME_SIZE)];//todo
	
    //todo for testing TiFrame * f;
    //todo for testing f = frame_open( (char *)(&frame_mem), FRAME_HOPESIZE(DTP_MAX_FRAME_SIZE), 3, 20, 0);
   
    request = frame_startptr( f );
	
	// The to and from address here are the NET layer address. Do NOT confuse
	// them with the MAC layer address.

	
	DTP_SET_PROTOCAL_IDENTIFIER( request,DTP_PROTOCOL_IDENTIFER);
	
	DTP_SET_PACKETCONTROL( request, DTP_BROADCAST | DTP_MAINTAIN_REQUEST);//b1 b0  packet command type;b7 b6  transportation method.
	DTP_SET_SEQUENCEID( request,net->request_id);
	DTP_SET_SHORTADDRTO( request, 0xffff );		
	DTP_SET_SHORTADDRFROM( request,net->localaddress); 
	DTP_SET_HOPCOUNT( request,0);
	DTP_SET_MAX_HOPCOUNT( request,max_hopcount);
	
	DTP_SET_PATHDESC_COUNT( request,0);
    
	//frame_setlength( f, frame_capacity( f) );
	frame_setlength( f, 20 );
	
	
	/*
	request[0] = DTP_PROTOCOL_IDENTIFER;
	request[1] = DTP_BROADCAST | DTP_MAINTAIN_REQUEST;
	request[2] = net->seqid;
	request[3] = HIGH_BYTE(FRAME154_BROADCAST_ADDRESS);//todo 
	request[4] = LOW_BYTE(FRAME154_BROADCAST_ADDRESS);//todo
	request[5] = HIGH_BYTE(net->localaddress);//todo
	request[6] = LOW_BYTE(net->localaddress);//todo
	request[7] = 0; // current hop count
	request[8] = max_hopcount; // max hop count
	request[9] = 0; // path descriptor count
	*/
	
	// if the low level medium access protocol(MAC) is busy, then broadcast() will return 0.
	// in this case, we should repeat the broadcast again.
    
	count = 0;

	

	
    while (count < 0x04)
    {   

		
		len = dtp_broadcast( net, f, 0x00 );

		
		
		if (len > 0)
		{  
			net->request_id ++;
			
			hal_delayms( 100 );
			break;
		}

		/* delay a little while to give the other nodes the chance to send their packets
		 * out. Or else servere conflications maybe encountered due to the continuous 
		 * re-transmission.
		 * 
		 * suggested delay value: 5ms - 100ms
		 * It's better not too small nor too large.
		 *
		 * If the MAC layer is very strong, then we can also eliminate this delay
		 * and depends on MAC layer to solve the conflication. 
		 */
		hal_delayms( 100 );
		count ++;
	}

    
	return len;
}

/* dtp_fdsendto()
 * Send frames to a specific node by flooding. The frame will propagate in the network 
 * according to flood policy. This process is the same as dtp_broadcast(). However, 
 * only the receiver node will accept this frame. The other nodes will discard 
 * it because the destination address isn't match.
 * 
 * dtp_fdsendto() can be called by sink node or sensor node. 
 * 
 * @attention
 * you must call DTP_SET_SHORTADDRTO( pkt, destination ) before calling this function
 * to set the final destination address.
 */


uint8 dtp_fdsendto( TiDataTreeNetwork * net, uint16 shortaddrto, TiFrame * frame, uint8 option )
{
	uint8 count=0;
	char * pkt;

	/* This function will try to put the frame into TiDataTreeNetwork's internal TX buffer. 
	 * The real sending processing is in dtp_evolve(). 
	 */
	if (frame_empty(net->txque))
	{
		hal_assert( frame_capacity(net->txque) >= frame_length( frame) );
		count = frame_totalcopyfrom( net->txque, frame ); 
		hal_assert( count > 0 );
		pkt = frame_startptr( net->txque);
		if (pkt[7] < pkt[8]) 
		{
			/*
			opf_set_panto( net->txque, net->pan );
			opf_set_panfrom( net->txque, net->pan );
			opf_set_shortaddrto( net->txque, DTP_BROADCAST_ADDRESS );
			opf_set_shortaddrfrom( net->txque, net->localaddress );
			*/
			
			// the master must call DTP_SET_SHORTADDRTO( pkt, destination );     
			// before call this function.
			//pkt = net->txque->msdu;
			DTP_SET_SHORTADDRTO( pkt,shortaddrto);
		    //DTP_SET_SHORTADDRFROM( pkt, net->localaddress );//数据包的源地址若在此赋值会将之前的地址覆盖掉。
			DTP_SET_HOPCOUNT( pkt, 0 );
			if (DTP_MAX_HOPCOUNT(pkt) == 0)
			{
				DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_MAX_COUNT);
			}
			DTP_SET_PATHDESC_COUNT( pkt, 0 );
		}
		else{
			frame_totalclear( net->txque );
			count = 0;
		}
	}

	dtp_evolve( net, NULL );
	return count;
}

/* dtp_broadcast()
 * Broadcast a frame across the whole network. The frame will flood in the whole
 * network. This is different to the MAC layer's 1-hop only broadcasting.
 *
 * dtp_broadcast() can be called in either root mode or node mode.
 */
uint8 dtp_broadcast( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{ 
	
	uint8 count=0, len;
	char * pkt;
	char * pc;
	
	hal_assert( frame_length(frame) > 0 );

	// This function will try to put the frame into TiDataTreeNetwork's internal TX buffer. 
	// The real sending processing is in dtp_evolve(). 
	//
	if (frame_empty(net->txque))
	{
       
	//todo hal_assert( frame_capacity(net->txque) >= frame_length(frame) );这一句发出警报，不知道为什么错？
		
		count = frame_totalcopyfrom( net->txque, frame ); 

		frame_setlength( net->txque,frame_length( frame));//todo
		pkt = frame_startptr( net->txque );

		// If the current hopcount is euqal or more than the maximum hop count 
		// in the frame, then this frame should be discarded.
		// request[7] is the current hopcount, and request[8] is the maximum hop count.

		
		if (DTP_HOPCOUNT(pkt) < DTP_MAX_HOPCOUNT(pkt))
		{
			DTP_SET_PROTOCAL_IDENTIFIER( pkt, DTP_PROTOCOL_IDENTIFER);
			DTP_PACKETCONTROL( pkt) |= DTP_BROADCAST;
			
			DTP_SET_HOPCOUNT( pkt,DTP_HOPCOUNT(pkt) + 1 );
			
			// add the localaddress into the path descriptor inside the frame
			// request[9] is the node address count in the path descriptor.
            
            // save this node address into the packet's path section
			pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
			pc[1] = (net->localaddress ) & 0xFF;
			pc[2] = ( net->localaddress ) >> 8;
			
			
			DTP_SET_PATHDESC_COUNT( pkt,DTP_PATHDESC_COUNT(pkt) + 1 );
            
			// After sending a request frame, we'd better put it into the cache. This is
			// done by cache_visit(). This is used to reduce unecessary frame transmission
			// when flooding a frame among the network. 
			// 
			// Assume two nodes A and B, and A functions as the root node. If A broadcast  
			// a request to its neighbor nodes including B, and B will also broadcast the
			// received request. A will receive the frame returned from B. Attention in this
			// case, A shouldn't broadcast this frame. This is done by the cache. That's why
			// we should firstly save the frame into the cache here.

			 _net_get_frame_feature( frame, &(net->frame_feature[0]), sizeof(net->frame_feature) ); //todo 
			dtp_cache_visit( net->cache, &(net->frame_feature[0]) );
		}
		else{

			
			frame_totalclear( net->txque );
			count = 0;
		}
		
	}
	
	

	// The evolve function will push the state machine forward. During this process, 
	// the packet will be really sent.
	
	
	

   dtp_evolve( net, NULL );

   
	return count;
}

/* dtp_multicast()
 * multicast a frame in a tree. The root node of the tree is the destination node
 * specfied inside the opf. 
 * 
 * different to the broadcast(), the multicast() actually sends the frame to the 
 * multicast tree root along the tree edges. In most cases, the frame should be 
 * able to reach every node in the sub tree, but no guarantee about this.
 */
uint8 dtp_multicast( TiDataTreeNetwork * dtp, TiFrame * opf, uint8 option )
{
	// @attention
	// todo
	// DTP doesn't support multicast in the current version due to the complexity 
	// of multicast
    return 0;
}

/* Send a frame to a specific node(unicast). The network will transmit frames along the edges
 * in the tree. No broadcast is used. It doesn't guarantee the frame reach its destination. 
 * Actually, this function only submit the packet to the DTP service. The real transmission 
 * is done in dtp_evolve() function.
 * 
 * In the unicast algorithm, the router node will forward the packet according to
 * the path descriptor inside the packet or simply forward it to it's parent node.
 * This behavior depends on the type of the packet. If it's a DTP_XXX_REQUEST, then 
 * it will be unicasted according to the path descriptor. If it's DTP_XXX_RESPONSE,
 * then it will send to its parent node and finally to the root node of the tree.
 * All these process are processed inside dtp_evolve(...)
 * 
 * @attention
 * dtp_unicast() can be called by sink node or sensor node. If it's called by the
 * sink node, then the frame will be forwarded according to the path descriptor
 * inside the packet. If it's called by the sensor node, then the frame will be 
 * forwarded to the parent sensor node.
 *
 * @attention
 * Before calling this function, the packet control field in the frame must be assigned,
 * because dtp_unicast() will use different algorithm when transmit the frame inside 
 * the network according to different packet control settings.
 */
uint8 dtp_unicast( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{
	uint8 count=0;
    char * pkt;

	/* This function will try to put the frame into TiDataTreeNetwork's internal TX buffer. 
	 * The real sending processing is in dtp_evolve(). 
	 */
	if (frame_empty(net->txque))
	{
		hal_assert( frame_capacity(net->txque) >= frame_length(frame) );
		count = frame_totalcopyfrom( net->txque, frame ); 
		hal_assert( count > 0 );
		
		
		if ( frame_empty( net->txque)) 
		{
			pkt = frame_startptr( net->txque );
			switch (DTP_CMDTYPE(pkt))
			{
			case DTP_MAINTAIN_RESPONSE:
			case DTP_DATA_RESPONSE:
				/*
				opf_set_panto( net->txque, net->pan );
				opf_set_shortaddrto( net->txque, net->parent );
				opf_set_panfrom( net->txque, net->pan );
				opf_set_shortaddrfrom( net->txque, net->localaddress );
				*/

				DTP_SET_SHORTADDRTO( pkt, net->root );
				DTP_SET_SHORTADDRFROM( pkt, net->localaddress );
				DTP_SET_HOPCOUNT( pkt, 0 );
				if (DTP_MAX_HOPCOUNT(pkt) == 0)
				{
					DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_MAX_COUNT );
				}
				DTP_SET_PATHDESC_COUNT( pkt, 0 );
				break;

			case DTP_DATA_REQUEST:
				// todo: now, we don't support unicast a frame from gateway to sensor node.
				// opf_set_panto( net->txque, net->pan );
				// opf_set_shortaddrto( net->txque, GET_NEXT_HOP() );
				// opf_set_panfrom( net->txque, net->pan );
				// opf_set_shortaddrfrom( net->txque, net->localaddress );
				// opf_setackrequest( net->txque, true );  // todo

				// DTP_SET_SHORTADDRTO( pkt, net->root );
				// DTP_SET_SHORTADDRFROM( pkt, net->localaddress );
				// DTP_SET_HOPCOUNT( pkt, 0 );
				// if (DTP_MAX_HOPCOUNT(pkt) == 0)
				//	DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_DEF_MAX_HOPCOUNT );
				// DTP_SET_PATHDESC_COUNT( pkt, 0 );
				frame_totalclear( net->txque );
				count = 0;
				break;

			default:
				frame_totalclear( net->txque );
				count = 0;
				break;
			}
		}
		else{
			frame_totalclear( net->txque );
			count = 0;
		    }
	}

	dtp_evolve( net, NULL );
	return count;
}

/* dtp_unicast_leaftoroot()
 * 
 * attention this function should be called by the sensor node only. It's meaningless
 * for the gateway/sink node to call this function.
 */
uint8 dtp_unicast_leaftoroot( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{
	uint8 count=0;
    char * pkt;
	//todo TiIEEE802Frame154Descriptor desc;

	/* This function will try to put the frame into TiDataTreeNetwork's internal TX buffer. 
	 * The real sending processing is in dtp_evolve(). 
	 */


	if (frame_empty(net->txque))
	{
	    /*todo不知道为什么frame_capacity(net->txque)竟然是0！
		if (  frame_capacity(net->txque) < frame_length(frame))//todo for testing
		{
			while ( 1)
			{
				led_toggle( LED_YELLOW);
				dbc_uint8(frame_capacity(net->txque));
				dbc_uint8(frame_length(frame));
				hal_delay( 1000);

			}
		}
		hal_assert( frame_capacity(net->txque) >= frame_length(frame) );*/
		count = frame_totalcopyfrom( net->txque, frame ); 
		hal_assert( count > 0 );
		pkt = frame_startptr( net->txque);

		//frame_setlength( net->txque,frame_capacity( net->txque));
		frame_setlength( net->txque,frame_length( frame));

		if (count > 0)
		{     
			DTP_SET_PROTOCAL_IDENTIFIER( pkt,DTP_PROTOCOL_IDENTIFER);
			DTP_SET_PACKETCONTROL( pkt, DTP_UNICAST_LEAF2ROOT | DTP_DATA_RESPONSE );
			DTP_SET_SHORTADDRTO( pkt, net->root );
			DTP_SET_SHORTADDRFROM( pkt, net->localaddress );
			DTP_SET_HOPCOUNT( pkt, 0 );
			if (DTP_MAX_HOPCOUNT( pkt) == 0)
			{
				DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_MAX_COUNT );
			}
			DTP_SET_PATHDESC_COUNT( pkt, 0 );
		}
		else{
			frame_totalclear( net->txque );
			count = 0;
		}
	}

	dtp_evolve( net, NULL );
	return count;
}

/* dtp_unicast_leaftoroot()
 * 
 * attention this function should be called by the sink node(or gateway node/root 
 * node) only.
 */
uint8 dtp_unicast_roottoleaf( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{
	// todo 
/*	
	uint8 count=0;
    char * pkt;

	// This function will try to put the frame into TiDataTreeNetwork's internal TX buffer. 
	// The real sending processing is in dtp_evolve(). 
	//
	if (frame_empty(net->txque))
	{
		hal_assert( frame_capacity(net->txque) >= frame_length(frame) );
		count = frame_totalcopyfrom( net->txque, frame ); 
		hal_assert( count > 0 );
		
		if (count > 0)
		{
			DTP_SET_PROTOCOL_IDENTIFIER
			DTP_SET_CONTROL( request, DTP_UNICAST_LEAF2ROOT | DTP_DATA_RESPONSE );
			DTP_SET_SHORTADDRTO( request, net->root );
			DTP_SET_SHORTADDRFROM( request, net->localaddress );
			DTP_SET_HOPCOUNT( request, 0 );
			if (DTP_MAX_HOPCOUNT(request) == 0)
			{
				DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_DEF_MAX_HOPCOUNT );
			}
			DTP_SET_PATHDESC_COUNT( pkt, 0 );
		}
		else{
			frame_totalclear( net->txque );
			count = 0;
		}
	}

	dtp_evolve( net, NULL );
	return count;
*/
	return 0;
}

/* Check for arrived frames, no matter who send them. 
 * Only the following two kinds of frames will be put into "rxbuf" for dtp_recv()
 * - broadcast frames. the destination address of these frames are 0xFFFF.
 * - destination is the current node. 
 *
 * This function can be called by both the root node and the sensor node.
 */
uint8 dtp_recv( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{
	uint8 count=0;

	if (!frame_empty(net->rxque))
	{   

		count = frame_totalcopyto( net->rxque, frame); 
		frame_totalclear( net->rxque );
	}

    if ( count>0)
    {
        count = frame_length( frame);
    }
  
	dtp_evolve( net, NULL );
	return count;
}

/* dtp_send_request()
 * broadcast DTP_DATA_REQUEST among the network.
 *
 * This function should be called by the root node only. If a sensor node calls
 * this function to send data request, all the data response will still reply to 
 * the old root node.
 */
uint8 dtp_send_request( TiDataTreeNetwork * net, TiFrame * frame, uint8 max_hopcount )
{
	char * request;
    uint8 len;

    request = frame_startptr( frame);

	DTP_SET_PROTOCAL_IDENTIFIER( request,DTP_PROTOCOL_IDENTIFER);
	DTP_SET_PACKETCONTROL( request, DTP_BROADCAST | DTP_DATA_REQUEST);
	DTP_SET_SEQUENCEID( request, net->request_id );
	DTP_SET_HOPCOUNT( request,0);
	DTP_SET_MAX_HOPCOUNT( request, max_hopcount );

	//frame_setlength( frame,frame_capacity( frame));todo

	len = dtp_broadcast( net, frame, 0x00 );
    
	if (len > 0)
	{
		net->request_id ++;
		
	}
	

	return len;
}

/* dtp_send_response()
 * unicast the response to the root node of the data tree.
 *
 * This function should be called by the sensor node only. If the root node(sink 
 * node) calls this function, the packet won't be sent out because the DTP service
 * will regard the packet already reaches its destination. 
 */
uint8 dtp_send_response( TiDataTreeNetwork * net, TiFrame * frame, uint8 max_hopcount )
{
	char * response;
    uint8 len;

	response = frame_startptr( frame );

	DTP_SET_SEQUENCEID( response, net->response_id );
	DTP_SET_MAX_HOPCOUNT( response, max_hopcount );
    frame_setlength( frame,25);//todo for testing
	len = dtp_unicast_leaftoroot( net, frame, 0x00 );
	if ( len>0)
	{
		net->response_id ++;
	}
	return len;
}

void dtp_evolve( void * netptr, TiEvent * e )
{

	TiDataTreeNetwork * net = (TiDataTreeNetwork *)netptr;

	if (net->option & 0x01)
		dtp_evolve_sink( netptr, e );		
	else
		dtp_evolve_node( netptr, e );

	
}

/* This function receive events from other services/objects and drive the evolution 
 * of the TiDataTreeNetwork service. 
 *
 * This function can also be the listener of the MAC layer, Though it's not mandatory.
 * 
 * @attention: 
 *	This function cannot run in the interrupt mode.
 */
void dtp_evolve_node( void * netptr, TiEvent * e )
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)netptr;
	bool done = true;

	aloha_evolve( net->mac, e );

	
	/*
	// This event should be sent from MAC layer. Since the listener function is usually
	// implemented in upper application layers, the user can call dtp_recv() to get the
	// frame just arrived.
	//
	if (e->id == EVENT_DATA_ARRIVAL)
	{
		if (net->listener != NULL)
		{
			net->listener( net->lisowner, e );
			return;
		}
	}
	*/

	

	do{

		switch (net->state)
		{
		/* If the current program runs as a general wireless sensor node or router node, 
		 * then the first state should be STARTUP. It should first receive an MAINTAIN
		 * REQUEST packet before it can do normal operations in the IDLE state.
		 *
		 * @attention
		 * There's no STARTUP state for the sink/gateway node.
		 */
	

		case DTP_STATE_STARTUP:

			// In STARTUP state, we repeatly check the received frames for MAINTAIN_REQUEST.
			// All other type frames will be ignored and dropped. And we needn't to wait
			// for the net->rxbuf empty in the receiving process.
			frame_totalclear( net->rxbuf );
			
			// Check whether there's incoming frames. If there has, then further check 
			// whether this is a DTP_MAINTAIN_REQUEST type. The node needs to receive 
			// an DTP_MAINTAIN_REQUEST packet if it wants to leave the STARTUP state.
			//

           
			if (_dtp_evolve_recv_check( net ))                            
			{   
				if (_dtp_evolve_startup_maintain_check( net ))
				{
					  net->state = DTP_STATE_IDLE;
					  
				}

			}
			
			//net->state = DTP_STATE_IDLE;
			break;

			//todo 
			//当状态转入DTP_STATE_IDLE后，程序将不再回到DTP_STATE_STARTUP状态，所以上面的代码应该只会被执行一次。

		/* IDLE is the initial and default state of this state machine. */

    

		case DTP_STATE_IDLE:

			if (!frame_empty(net->txque))
			{ 

				switch (DTP_TRANTYPE(frame_startptr(net->txque)))
				{
				case DTP_BROADCAST:

					if (aloha_broadcast( net->mac, net->txque, 0x00 ) > 0)
					{
						frame_totalclear( net->txque );
						net->txtrytime = DTP_MAX_TX_TRYTIME;
					}
					else{
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				case DTP_UNICAST_LEAF2ROOT:
					// if the last bit of the forth parameter of aloha_send() is 
					// 1, then the aloha will use ACK mechanism.
					//
					//todo hal_assert(!(opf_shortaddrto(net->txque)==0x0000));
					//todo if (aloha_send( net->mac, net->txque, 0x01 ) > 0)


					if (aloha_send( net->mac,net->parent, net->txque, 0x01 ) > 0)
					{
						frame_totalclear( net->txque );
						net->txtrytime = DTP_MAX_TX_TRYTIME;
					}
					else{
						hal_delayms( 10);//todo for testing
						aloha_evolve( net->mac,NULL);//todo for testing
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				default:
					// for unrecognized frame types, just drop it.
					frame_totalclear( net->txque );
					break;
				}
			}

			/* @attention
			 * @modified by zhangwei(TongJi University) on 20091123
			 * - Design improvement:
			 * In the last version, aloha_recv() is called when both net->rxbuf and net->rxque
			 * are empty. The source code is as the following: 
			 *
			 *	 if (frame_empty(net->rxbuf) && frame_empty(net->rxque))
			 *
			 * This condition can guarantee the frame arrived can be put into appropriate 
			 * buffer space and wait for dtp_recv() to read it out. However, If the 
			 * dtp_recv() failed to read the frame out immediately, then the later 
			 * aloha_recv() will be blocked. This may lead to unexpected frame loss
			 * at the MAC layer or PHY layer. 
			 *
			 * Considering the fact the frame queue are often implemented in NET or 
			 * APPLICATION layer, I prefer to let the newcoming frame override the 
			 * old one. So I changed the condition as the following now:
			 * 
			 *   if (frame_empty(net->rxbuf))
			 *
			 * This can improve performance. Attention now, the new coming frame in 
			 * net->rxbuf maybe copied to net->rxque and wait for dtp_recv() to read 
			 * it out. If there's already a frame inside net->rxque, then the new 
			 * frame will overwrite the old one. I prefer the "overwrite" feature
			 * because this will let the whole system continue to run smoothly. 
			 */
             
			/* If the internal temporary buffer (net->rxbuf) is empty, then try to 
			 * receive one from MAC layer. 
			 * 
			 * @attention 
			 * @modified by zhangwei(openwsn@gmail.com) on 20091124
			 * In the most radical manner, you can call aloha_recv() directly without
			 * considering the status of rxbuf. If there's data inside rxbuf, then 
			 * it can be overwrite by aloha_recv(). I think this is also Ok.
			 */

        
	 		if (frame_empty(net->rxbuf))
			{
                frame_totalclear( net->rxbuf );//todo

				if (_dtp_evolve_recv_check(net))
				{
					_dtp_evolve_node_recv_or_forward( net );
				}
			}
			else{
				// try move the frame in rxbuf to rxque or txque 
				_dtp_evolve_node_recv_or_forward( net );
			}
			/*
			else//todo for testing
			{
				net->count++;//todo for testing
				if ( net->count>0x10)//todo for testing
				{
					frame_totalclear( net->rxbuf);//todo for testing
					frame_totalclear( net->rxque);//todo for testing
					frame_totalclear( net->txque);//todo for testing
					net->count = 0;//todo for testing
					hal_delay( 100);//todo for testing
				}

			}
			*/

			// If there's a frame inside net->txque, then try to send it out.
			// if aloha_broadcast() or aloha_send() failed this time, then it will 
			// be restarted in the next call of dtp_evolve(). so don't worry about 
			// the current failure.

			if (!frame_empty(net->txque))
			{  
				switch (DTP_TRANTYPE(frame_startptr(net->txque)))
				{
				case DTP_BROADCAST:

					if (aloha_broadcast( net->mac, net->txque, 0x00 ) > 0)
					{
						frame_totalclear( net->txque );
						net->txtrytime = DTP_MAX_TX_TRYTIME;
					}
					else{
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				case DTP_UNICAST_LEAF2ROOT:

					// if the last bit of the forth parameter of aloha_send() is 
					// 1, then the aloha will use ACK mechanism.
					//
					//todo hal_assert(!(opf_shortaddrto(net->txque)==0x0000));
					//todo if (aloha_send( net->mac, net->txque, 0x01 ) > 0)

					if (aloha_send( net->mac,net->parent, net->txque, 0x01 ) > 0)
					{   
						frame_totalclear( net->txque );
						net->txtrytime = DTP_MAX_TX_TRYTIME;
					}
					else{
						hal_delayms( 10);//todo for testing
						aloha_evolve( net->mac,NULL);//todo for testing
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				default:
					// for unrecognized frame types, just drop it.
					frame_totalclear( net->txque );
					break;
				}
			}
           
			break;	

		// todo
		// obsolete source codes
		// This's the state waiting for MAC layer's reply. The sending process of the 
		// MAC layer may last for quite a long time due to re-transmission. Only at the
		// end of the transmission, the MAC layer does possible to notify the NET layer 
		// the transmission results of the sending request.
		//
		/*case FLOOD_STATE_WAITFOR_TXREPLY:
			net->state = FLOOD_STATE_IDLE;
			break;*/


		default:
			net->state = DTP_STATE_IDLE;

		}
	}while (!done);

	
	return;
}

void dtp_evolve_sink( void * netptr, TiEvent * e )
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)netptr;
	bool done = true;
	char * pkt;
   
	aloha_evolve( net->mac, e ); 

/*	if ( !frame_empty(net->txque))
	{
		if (aloha_broadcast( net->mac, net->txque, 0x00 ) > 0)
		{
           frame_totalclear( net->txque );
		}
		
	}
	*/

	/*
	// This event should be sent from MAC layer. Since the listener function is usually
	// implemented in upper application layers, the user can call dtp_recv() to get the
	// frame just arrived.
	//
	if (e->id == EVENT_DATA_ARRIVAL)
	{
		if (net->listener != NULL)
		{
			net->listener( net->lisowner, e );
			return;
		}
	}
	*/ 
    
	do{
		switch (net->state)
		{
		/* IDLE is the initial and default state of this state machine. */
	
		case DTP_STATE_IDLE:

			/* @attention
			 * @modified by zhangwei(TongJi University) on 20091123
			 * - Design improvement:
			 * In the last version, aloha_recv() is called when both net->rxbuf and net->rxque
			 * are empty. The source code is as the following: 
			 *
			 *	 if (frame_empty(net->rxbuf) && frame_empty(net->rxque))
			 *
			 * This condition can guarantee the frame arrived can be put into appropriate 
			 * buffer space and wait for dtp_recv() to read it out. However, If the 
			 * dtp_recv() failed to read the frame out immediately, then the later 
			 * aloha_recv() will be blocked. This may lead to unexpected frame loss
			 * at the MAC layer or PHY layer. 
			 *
			 * Considering the fact the frame queue are often implemented in NET or 
			 * APPLICATION layer, I prefer to let the newcoming frame override the 
			 * old one. So I changed the condition as the following now:
			 * 
			 *   if (frame_empty(net->rxbuf))
			 *
			 * This can improve performance. Attention now, the new coming frame in 
			 * net->rxbuf maybe copied to net->rxque and wait for dtp_recv() to read 
			 * it out. If there's already a frame inside net->rxque, then the new 
			 * frame will overwrite the old one. I prefer the "overwrite" feature
			 * because this will let the whole system continue to run smoothly. 
			 */

			/* If the internal temporary buffer (net->rxbuf) is empty, then try to 
			 * receive one from MAC layer. 
			 * 
			 * @attention 
			 * @modified by zhangwei(openwsn@gmail.com) on 20091124
			 * In the most radical manner, you can call aloha_recv() directly without
			 * considering the status of rxbuf. If there's data inside rxbuf, then 
			 * it can be overwrite by aloha_recv(). I think this is also Ok.
			 */
 


	 		if (frame_empty(net->rxbuf))
			{
				if (_dtp_evolve_recv_check(net))
				{
					
					_dtp_evolve_sink_recv( net );
				}
			}
			
			

			// try to send the frame inside net->txque. 
			// if aloha_broadcast() or aloha_send() failed this time, then it will 
			// be restarted in the next call of dtp_evolve(). so don't worry about
			// when the frame in net->txque is really sent.

			if (!frame_empty(net->txque))
			{ 

				pkt = frame_startptr( net->txque );
				switch (DTP_TRANTYPE(pkt))
				{
				case DTP_BROADCAST:
					if (aloha_broadcast( net->mac, net->txque, 0x00 ) > 0)
					{   
						frame_totalclear( net->txque );
						net->txtrytime = DTP_MAX_TX_TRYTIME;
					}
					else{
						
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				case DTP_UNICAST_ROOT2LEAF:
					// if the last bit of the forth parameter of aloha_send() is 
					// 1, then the aloha will use ACK mechanism.
					//
					if ( DTP_SHORTADDRTO( pkt)!=0x0000 )
					{
						//todo if (aloha_send( net->mac, net->txque, 0x01 ) > 0)
						if (aloha_send( net->mac, DTP_SHORTADDRTO( pkt),net->txque, 0x00 ) > 0)//todo 这一句不确定
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					else{
						net->txtrytime --;
						if (net->txtrytime == 0)
						{
							frame_totalclear( net->txque );
							net->txtrytime = DTP_MAX_TX_TRYTIME;
						}
					}
					break;
				default:
					// attention
					// If you failed to send a frame, you must consider where to clear the frame
					// we doesn't support this yet.
					//
					// default send by call aloha_send() with option 0x01(ack required)
					// if (aloha_send( net->mac, net->txque, 0x01 ) > 0)
					//{
					//	frame_totalclear( net->txque );
					//}

					// for unrecognized frame types, just drop it.
					frame_totalclear( net->txque );
					break;
				}
			}
			
			break;	

		// todo
		// obsolete source codes
		// This's the state waiting for MAC layer's reply. The sending process of the 
		// MAC layer may last for quite a long time due to re-transmission. Only at the
		// end of the transmission, the MAC layer does possible to notify the NET layer 
		// the transmission results of the sending request.
		//
		/*case FLOOD_STATE_WAITFOR_TXREPLY:
			net->state = FLOOD_STATE_IDLE;
			break;*/

		default:
			net->state = DTP_STATE_IDLE;

		}
	}while (!done);
   
	

	//return;
}

/* _dtp_evolve_recv_check()
 * This function is used by dtp_evolve() only. It will call aloha_recv() to check
 * for possible incoming frame and place it into net->rxbuf.
 * 
 * attention: this function doesn't care whether the net->rxbuf is empty or not 
 * currently.
 *
 * return
 *	true when receives a frame and false when no frame in net->rxbuf.
 */
bool _dtp_evolve_recv_check( TiDataTreeNetwork * net )
{
	uint8 count, len;
	bool ret;
    TiIEEE802Frame154Descriptor * desc;

	ret = true;
	frame_reset( net->rxbuf,3,20,102);
	count = aloha_recv( net->mac, net->rxbuf, 0x00 );
	if (count <= 0)
	{	
		ret = false;
	}
	else{
       
		frame_moveouter( net->rxbuf );
        /*
        desc = ieee802frame154_format( &(net->mac->desc), frame_startptr( net->rxbuf), frame_capacity( net->rxbuf), 
            FRAME154_DEF_FRAMECONTROL_DATA );
            */
		desc = ieee802frame154_format( &(net->mac->desc), frame_startptr( net->rxbuf), frame_length( net->rxbuf), 
			FRAME154_DEF_FRAMECONTROL_DATA );
		
		if (!ieee802frame154_parse(desc, frame_startptr(net->rxbuf), frame_length(net->rxbuf)))
        //if (!ieee802frame154_parse(desc, frame_startptr( net->rxbuf), frame_capacity( net->rxbuf)))
		{
			frame_totalclear( net->rxbuf );
			ret = false;
		}
		frame_moveinner( net->rxbuf );
	}

	// The 802.15.4 ACK, BEACON,  and COMMNAD type frames should be ignored
	// and discarded.
	//
	// attention: I think the ACK, BEACON and COMMAND frames should be hide 
	// by the MAC layer. However, we should still enable the following 
	// judgement to make the program most robust.
	//
	
	if (ret)
	{
		if (ieee802frame154_type(desc) != FCF_FRAMETYPE_DATA)//if (ieee802frame154_type(net->rxbuf) != FCF_FRAMETYPE_DATA)
		{ 
			frame_totalclear( net->rxbuf );
			ret = false;
     	}
	}

	// Check: whether the same frame has already been received. This
	// is by searching it in the cache. If it cannot find it in the 
	// cache, then put the frame in the cache.


	if (ret)
	{
		// If the cache has this packet, which inidcates the current node 
		// has received this packet before, then we should drop it.
		//
		len = _net_get_frame_feature( net->rxbuf, &(net->frame_feature[0]), sizeof(net->frame_feature) );  
		if (cache_visit(net->cache, &(net->frame_feature[0]), len ))
		{    
			frame_totalclear( net->rxbuf );
			ret = false;
		}
	}
    
	return ret;
}

/* _dtp_evolve_startup_maintain_check()
 * This function is called in dtp_evolve() only. It was called in the STARTUP state
 * in order to check whether the frame received is an MAINTAIN_REQUEST or not.
 */
bool _dtp_evolve_startup_maintain_check( TiDataTreeNetwork * net )
{
	char * pkt;
	char * pc;
	uint8 cur_hopcount, max_hopcount;
	bool ret = false;

	// assume the incoming frame is in net->rxbuf
	hal_assert( !frame_empty(net->rxbuf) );

	// if the destination of this packet matches the address of the 
	// current node, then this packet arrives its destination.
	// This line assume the frame has already been parsed successfully
	// or else the value of "msdu" will be invalid.
	pkt = frame_startptr( net->rxbuf );

	switch (DTP_TRANTYPE(pkt))
	{
	// If this is a broadcast packet, then it will be copied into net->rxque and 
	// wait for dtp_recv() to read it out. At the same time, it should also stay 
	// in net->rxbuf and wait for forwarding to other nodes.
	case DTP_BROADCAST:

		// if the packet is sent from the current node, then simply ignore it.
		// this is usually happens when the current node is the gateway node.
		
		if (DTP_SHORTADDRFROM(pkt) == net->localaddress)
		{
			frame_totalclear( net->rxbuf );
			break;
		}

		// Currently, there're two kinds of packets have the broadcast property:
		// the DTP_MAINTAIN_REQUEST and DTP_DATA_REQUEST
		//
		switch (DTP_CMDTYPE(pkt))
		{
		case DTP_MAINTAIN_REQUEST:

			// @attention
			// only the first DTP_MAINTAIN_REQUEST frame received is used to reconfigure
			// the data tree. the hopcount of the current node is actually the depth.
			//
			cur_hopcount = DTP_HOPCOUNT( pkt );
			if (net->depth > cur_hopcount)
			{
				net->depth = cur_hopcount;
				net->root = DTP_SHORTADDRFROM( pkt );
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
				net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
				ret = true;
			}
			else
			{
				ret = false;
			}

			// this line shouldn't put into the "if (net->depth < cur_hopcount) {}"
			// because we'd better guarantee the response_id is the newest id.
			//
			net->response_id = DTP_SEQUENCEID( pkt );

			// before try to forward the packet, we firstly check whether it has 
			// already reaches its maximum hopcount. the packet will be discarded
			// if this is true.
			//
			max_hopcount = DTP_MAX_HOPCOUNT( pkt );

			if (cur_hopcount >= max_hopcount)
			{   
				frame_totalclear( net->rxbuf );
				break;
			}

			frame_totalcopyto( net->rxbuf, net->rxque );

			// If the txque is empty, then move the frame from rxbuf to txque. For 
			// efficiency reasons, this is implemented by switching the two pointers. 
			// if the txque is full currently, then the DTP_MAINTAIN_REQUEST packet 
			// will continue stay in net->rxbuf and will be moved to txque until 
			// the next call to function dtp_evolve().
			
			if (frame_empty(net->txque))
			{
				
				


				

				/*todo
				opf_set_panto( net->rxbuf, net->pan );
				opf_set_shortaddrto( net->rxbuf, OPF_BROADCAST_ADDRESS );
				opf_set_panfrom( net->rxbuf, net->pan );
				opf_set_shortaddrfrom( net->rxbuf, net->localaddress );
				*/
                cur_hopcount = DTP_HOPCOUNT( pkt );
				DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );


				// save this node address into the packet's path section
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
				pc[1] = (net->localaddress ) & 0xFF;
				pc[2] = ( net->localaddress ) >> 8;

				DTP_PATHDESC_COUNT( pkt)++;

				// temporarily comment the following
				// should uncomment in release version

				// save this node address into the packet's path section
				/*
				pc = DTP_PATHDESC_PTR(pkt) + ((cur_hopcount-1) << 1);
				pc[0] = ( net->localaddress ) >> 8;
				pc[1] = (net->localaddress ) & 0xFF;
				*/

                _switch_ptr( &(net->rxbuf), &(net->txque) );
				
			}
			break;
		
		default:
			frame_totalclear( net->rxbuf );
			break;
		}
		break;

	default:
		break;
	}
	
	return ret;
}

/* _dtp_evolve_node_recv_or_forward()
 * This's a static function and it's called by dtp_evolve() only. It was called just 
 * after the current node receives a frame and put it into net->rxbuf. 
 */
void _dtp_evolve_node_recv_or_forward( TiDataTreeNetwork * net )
{

	char * pkt;
	uint8 cur_hopcount, max_hopcount;
	uint8 count;
	char * pc;
	uint16 addr;
    uint8 rssi;//todo for testing
	uint8 depth;//todo for testing
	uint8 i;//todo for testing
	float newvalue;//todo for testing
	float oldvalue;//todo for testing
	uint8 hopcount;//todo for tesitng

	hal_assert( !frame_empty(net->rxbuf) );

	// if the destination of this packet matches the address of the 
	// current node, then this packet arrives its destination.
	// This line assume the frame has already been parsed successfully
	// or else the value of "msdu" will be invalid.
	pkt = frame_startptr( net->rxbuf );
    count = frame_length( net->rxbuf);//todo for testing
	switch (DTP_TRANTYPE(pkt))
	{
	// If this is a broadcast packet, then it will be copied into net->rxque and 
	// wait for dtp_recv() to read it out. At the same time, it should also stay 
	// in net->rxbuf and wait for forwarding to other nodes.
	case DTP_BROADCAST:

		// if the packet is sent from the current node, then simply ignore it.
		// this is usually happens when the current node is the gateway node.
		
		if (DTP_SHORTADDRFROM(pkt) == net->localaddress)
		{
			frame_totalclear( net->rxbuf );
			break;
		}

		// Currently, there're two kinds of packets have the broadcast property:
		// the DTP_MAINTAIN_REQUEST and DTP_DATA_REQUEST
		//
		switch (DTP_CMDTYPE(pkt))
		{
		case DTP_MAINTAIN_REQUEST:
             /**********************************************************/
			frame_moveouter( net->rxbuf);//todo for testing
			rssi = pkt[(frame_capacity( net->rxbuf)-14)];//todo for testing
			depth = cur_hopcount = DTP_HOPCOUNT( pkt );//todo for testing
			depth--;//todo for testing
	
			if ( (rssi >= net->rssi)&&(depth < net->depth))//todo for testing
			{
				net->rssi = rssi;//todo for testing
				net->depth = cur_hopcount;//这一句是不是应该写成呢他net->depth = depth?
				net->root = DTP_SHORTADDRFROM( pkt );
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
				net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
			}
			else if ( (( rssi - net->rssi) > 4)&&( depth == net->depth))//todo for testing
			{
				net->rssi = rssi;//todo for testing
				net->depth = cur_hopcount;
				net->root = DTP_SHORTADDRFROM( pkt );
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
				net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
			}
			else
			{
			}

			
			frame_moveinner( net->rxbuf);//todo for testing
            //frame_setlength( net->rxbuf,frame_capacity( net->rxbuf));//todo for testing
			frame_setlength( net->rxbuf,count);//todo for testing
		
			// @attention
			// only the first DTP_MAINTAIN_REQUEST frame received is used to reconfigure
			// the data tree. the hopcount of the current node is actually the depth.
			
			/*todo for testing
			cur_hopcount = DTP_HOPCOUNT( pkt );
			if (net->depth > cur_hopcount)
			{
				net->depth = cur_hopcount;
				net->root = DTP_SHORTADDRFROM( pkt );
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
				net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
			}
             */
			// this line shouldn't put into the "if (net->depth < cur_hopcount) {}"
			// because we'd better guarantee the response_id is the newest id.
			//
			net->response_id = DTP_SEQUENCEID( pkt );

			// before try to forward the packet, we firstly check whether it has 
			// already reaches its maximum hopcount. the packet will be discarded
			// if this is true.
			//
			max_hopcount = DTP_MAX_HOPCOUNT( pkt );
			if (cur_hopcount >= max_hopcount)
			{
				frame_totalclear( net->rxbuf );
				break;
			}

			// If the txque is empty, then move the frame from rxbuf to txque. For 
			// efficiency reasons, this is implemented by switching the two pointers. 
			// if the txque is full currently, then the DTP_MAINTAIN_REQUEST packet 
			// will continue stay in net->rxbuf and will be moved to txque until 
			// the next call to function dtp_evolve().
			
			/****todo for testing*********源代码********************************************
			if (frame_empty(net->txque))
			{
				frame_totalcopyto( net->rxbuf, net->rxque );
                
				
                cur_hopcount = DTP_HOPCOUNT( pkt );
				
				//DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
				
				//为了阻止后面的节点成为前面的节点的父亲节点，用下面语句替换上面的语句。
				if ( cur_hopcount < net->depth)//todo for testing
				{
					DTP_SET_HOPCOUNT( pkt,++net->depth);
				} 
				else
				{
                    DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
				}

				// save this node address into the packet's path section
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
				pc[1] = (net->localaddress ) & 0xFF;
				pc[2] = (net->localaddress ) >> 8;
				DTP_PATHDESC_COUNT( pkt)++;
				
				_switch_ptr( &(net->rxbuf), &(net->txque) );
			}
			/*************************************************************************************/
			if (!frame_empty(net->rxbuf))
			{
				cur_hopcount = DTP_HOPCOUNT( pkt );

				//DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );

				//为了阻止后面的节点成为前面的节点的父亲节点，用下面语句替换上面的语句。
				if ( cur_hopcount < net->depth)//todo for testing
				{
					hopcount = net->depth;
					DTP_SET_HOPCOUNT( pkt,++hopcount);

				} 
				else
				{
					DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
				}

				// save this node address into the packet's path section
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
				pc[1] = (net->localaddress ) & 0xFF;
				pc[2] = (net->localaddress ) >> 8;
				DTP_PATHDESC_COUNT( pkt)++;
                i = 0;
				while ( i< 0x04)
				{

					if (aloha_broadcast( net->mac, net->rxbuf, 0x00 ) > 0)
					{
						break;
					}
					i++;
					
				}
				frame_totalclear( net->rxbuf);
			}
				
			break;

		case DTP_DATA_REQUEST:

			// differnet to the DTP_MAINTAIN_REQUEST packet, the DTP_DATA_RQUEST
			// packet should be submitted to the application layer and also be forwarded
			// to other node. 

			net->response_id = DTP_SEQUENCEID( pkt );

			// before try to forward the packet, we firstly check whether it has 
			// already reaches its maximum hopcount. the packet will be discarded
			// if this is true.

			cur_hopcount = DTP_HOPCOUNT( pkt );
			max_hopcount = DTP_MAX_HOPCOUNT( pkt );
			if (cur_hopcount >= max_hopcount)
			{
				frame_totalclear( net->rxbuf );
				break;
			}
			
			// prepare for sending and receiving
			if (frame_empty(net->txque))
			{
				// copy the received packet to rxque and wait for dtp_recv() to read it out.
				//
				// @warning
				// If net->rxque still has packets, then the following operation will 
				// override that packet. This is a little dangerous. However, if 
				// we don't do this, the dtp_evolve() may stopped working because
				// the rxbuf is full. There's no free space to accept new coming packets.
				// openwsn still prefer to override the elder packets in the design. 
				// this enable the network to process newer frames.
				//
				frame_totalcopyto( net->rxbuf, net->rxque );

				// move the frame in net->rxbuf to net->rxque and wait for MAC layer 
				// to send it out. for efficiency reasons, this is done by switching 
				// the pointers.

				/*todo
				opf_set_panto( net->rxbuf, net->pan );
				opf_set_shortaddrto( net->rxbuf, OPF_BROADCAST_ADDRESS );
				opf_set_panfrom( net->rxbuf, net->pan );
				opf_set_shortaddrfrom( net->rxbuf, net->localaddress );
				*/

                cur_hopcount = DTP_HOPCOUNT( pkt );
				DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );

				// temporarily comment the following
				// should uncomment in release version

				// save this node address into the packet's path section
				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
				pc[1] = (net->localaddress ) & 0xFF;
				pc[2] = (net->localaddress ) >> 8;

				DTP_PATHDESC_COUNT(pkt)++;

				_switch_ptr( &(net->rxbuf), &(net->txque) );
			}
			break;

		//case DTP_MAINTAIN_RESPONSE:
		//case DTP_DATA_RESPONSE:
		default:
			// if the node reaches its destination, then move the packet 
			// from net->rxbuf into net->rxque for dtp_recv() using. 
			// For efficiency reason, the moving process is replaced by 
			// switch the two pointers.
			//
			if (DTP_SHORTADDRTO(pkt) == net->localaddress)
			{
				// @warning: If the dtp_recv() cannot read out the data in rxque fast 
				// enough, then the new frame will override the old one.
				//
				_switch_ptr( &(net->rxbuf), &(net->rxque) );
				frame_totalclear( net->rxbuf );
			}
			else{
				// @attention
				// currently, we don't support this branch. however, you still shouldn't  
				// enable the following assert(). This is because sometimes the broadcasting
				// frames may encounter confliction and corrupt. DTP_CMDTYPE(pkt) in 
				// this case will return an unexpected byte. This will cause the 
				// receiving process run into this branch, and cause assertion failed.
				// you can output DTP_CMDTYPE(pkt) to check its value.
				//
				// hal_assert( false );
				//
				frame_totalclear( net->rxbuf );
			}
			break;
		}
		break;

	// not support multicast yet. The multicast packet will be simply dropped.
	case DTP_MULTICAST:
		frame_totalclear( net->rxbuf );
		break;

	// not support yet. The packet will be simply dropped.
	case DTP_UNICAST_ROOT2LEAF:
		frame_totalclear( net->rxbuf );
		break;

	case DTP_UNICAST_LEAF2ROOT:
		addr = DTP_SHORTADDRTO( pkt );

		// if the node reaches its destination, then move the packet 
		// from net->rxbuf into net->rxque for dtp_recv() using. 
		// For efficiency reason, the moving process is replaced by 
		// switch the two pointers.
		//
		if (addr == net->localaddress)
		{
			// @warning: If the dtp_recv() cannot read out the data in rxque fast 
			// enough, then the new frame will override the old one.
			//
			_switch_ptr( &(net->rxbuf), &(net->rxque) );
			frame_totalclear( net->rxbuf );
		}
		else{ 
			// forwarding the packet to its parent node in the data tree.
			// for safety reasons, we still check hopcount in order to avoid 
			// "forever-live" packets in the network, though such packets 
			// shouldn't come out therorectically.  But if there's one, then 
			// the network will collapse.
			//
			cur_hopcount = DTP_HOPCOUNT( pkt );
			max_hopcount = DTP_MAX_HOPCOUNT( pkt );
			if (cur_hopcount >= max_hopcount+1)
			{
				frame_totalclear( net->rxbuf );
			}
			else if (frame_empty(net->txque))
			{   
				cur_hopcount ++;
				DTP_SET_HOPCOUNT( pkt, cur_hopcount );

				pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo for testing
				pc[1] = (net->localaddress ) & 0xFF;//todo for testing
				pc[2] = (net->localaddress ) >> 8;//todo for testing

				DTP_PATHDESC_COUNT(pkt)++;//todo for testing
				_switch_ptr( &(net->rxbuf), &(net->txque) );
			}
			else{
			}
		}
		break;

	default:
		// you shouldn't remove this default. because if there're conflictions and
		// the current node really receive a corrupt frame and the program runs into
		// this default, then we should clear the rxbuf. 
		// 
		// if you remove this section, then the frame will continue stay in net->rxbuf.
		// 
		frame_totalclear( net->rxbuf );
		break;

	} // switch
}

/* _dtp_evolve_sink_recv()
 * This's a static function and it's called by dtp_evolve() only. Further more, it
 * was used in gateway mode only. Different to the sensor node, the gateway will 
 * accept almost all frames/packets from neighbor sensor nodes and submit them 
 * to the upper application layer. 
 *
 * attention
 *	This function assume net->rxbuf has an frame
 */
void _dtp_evolve_sink_recv( TiDataTreeNetwork * net )
{
	char * pkt;
	//uint8 cur_hopcount, max_hopcount;
	uint16 addr;

	hal_assert( !frame_empty(net->rxbuf) );

	// if the destination of this packet matches the address of the 
	// current node, then this packet arrives its destination.
	// This line assume the frame has already been parsed successfully
	// or else the value of "msdu" will be invalid.
	pkt = frame_startptr( net->rxbuf );

	switch (DTP_TRANTYPE(pkt))
	{
	// If this is a broadcast packet, then it will be copied into net->rxque and 
	// wait for dtp_recv() to read it out. At the same time, it should also stay 
	// in net->rxbuf and wait for forwarding to other nodes.
	case DTP_BROADCAST:

		// if the packet is sent from the current node, then simply ignore it.
		// this is usually happens when the current node is the gateway node.
		
		if (DTP_SHORTADDRFROM(pkt) == net->localaddress)
		{
			frame_totalclear( net->rxbuf );
			break;
		}

		// Currently, there're two kinds of packets have the broadcast property:
		// the DTP_MAINTAIN_REQUEST and DTP_DATA_REQUEST
		//
		switch (DTP_CMDTYPE(pkt))
		{
		case DTP_MAINTAIN_REQUEST:
		case DTP_DATA_REQUEST:
		case DTP_MAINTAIN_RESPONSE:
			frame_totalclear( net->rxbuf );
			break;

		//case DTP_DATA_RESPONSE:
		default:
			// if the node reaches its destination, then move the packet 
			// from net->rxbuf into net->rxque for dtp_recv() using. 
			// For efficiency reason, the moving process is replaced by 
			// switch the two pointers.
			//
			if (DTP_SHORTADDRTO(pkt) == net->localaddress)
			{
				// @warning: If the dtp_recv() cannot read out the data in rxque fast 
				// enough, then the new frame will override the old one.
				//
				_switch_ptr( &(net->rxbuf), &(net->rxque) );
				frame_totalclear( net->rxbuf );
			}
			else{
				// @attention
				// currently, we don't support this branch. however, you still shouldn't  
				// enable the following assert(). This is because sometimes the broadcasting
				// frames may encounter confliction and corrupt. DTP_CMDTYPE(pkt) in 
				// this case will return an unexpected byte. This will cause the 
				// receiving process run into this branch, and cause assertion failed.
				// you can output DTP_CMDTYPE(pkt) to check its value.
				//
				// hal_assert( false );
				//
			     frame_totalclear( net->rxbuf );
			     }
			break;
		}
		break;

	// not support multicast yet. The multicast packet will be simply dropped.
	case DTP_MULTICAST:
		_switch_ptr( &(net->rxbuf), &(net->rxque) );
		frame_totalclear( net->rxbuf );
		break;

	// not support yet. The packet will be simply dropped.
	case DTP_UNICAST_ROOT2LEAF:
		frame_totalclear( net->rxbuf );
		break;

	case DTP_UNICAST_LEAF2ROOT:
		addr = DTP_SHORTADDRTO( pkt );

		// if the node reaches its destination, then move the packet 
		// from net->rxbuf into net->rxque for dtp_recv() using. 
		// For efficiency reason, the moving process is replaced by 
		// switch the two pointers.
		//

		
		if (addr == net->localaddress)
		{   
			
			// @warning: If the dtp_recv() cannot read out the data in rxque fast 
			// enough, then the new frame will override the old one.
			//
			_switch_ptr( &(net->rxbuf), &(net->rxque) );
			frame_totalclear( net->rxbuf );
		}
		else{ 
		      frame_totalclear( net->rxbuf );
		    }
		break;

	default:
		// you shouldn't remove this default. because if there're conflictions and
		// the current node really receive a corrupt frame and the program runs into
		// this default, then we should clear the rxbuf. 
		// 
		// if you remove this section, then the frame will continue stay in net->rxbuf.
		// 
		_switch_ptr( &(net->rxbuf), &(net->rxque) );
		frame_totalclear( net->rxbuf );
		break;

	} // switch
}

/**
 * Summary the frame feature and place them into the feature buffer. The feature
 * includes: command and control(1B), sequence id(1B), destination address(2B)
 * and source address(2B).
 */ 
uint8 _net_get_frame_feature( TiFrame * frame, char * feature, uint8 size )
{
	memset( feature, 0x00, size );
	memmove( feature, (char*)frame_startptr( frame)+1, size );

	return size;
}

inline void _switch_ptr( TiFrame * __packed *ptr1, TiFrame *__packed * ptr2 )
{
	TiFrame * tmp;
	tmp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp;
}
