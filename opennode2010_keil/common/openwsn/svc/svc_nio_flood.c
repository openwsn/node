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
 * Flood Routing Protocol
 * Flood is the most simple routing protocol. This module implements the most simple 
 * flooding mechanism. It can run on top of ALOHA (in the module svc_aloha or svc_adaptaloha)
 * MAC. 
 * 
 * Due to the performance issues, this module is mainly for demonstration and teaching.
 *
 * @status
 *	Finished. Needs testing.
 * 
 * @author zhangwei in 2007.06
 *	- first created
 * @modified by zhangwei in 2009.07
 *	- revision. done
 *
 * @modified the whole text to match the changes in other file, and compiled it passed.
 * Shimiaojing 2009.11.02
 * but there is somepoint related to our logical frame struct  I am not sure need to be certained .
 *line 80 ,85, 106,158,297
 *  test ok with no problems
 ******************************************************************************/

/* @attention: you can also replace module with "svc_adaptaloha.h" to enable the 
 * Adaptive ALOHA protocol 
 */
#include "svc_configall.h"
#include <string.h>
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_targetboard.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_nio_aloha.h"
#include "svc_nio_flood.h"

/* Network layer packet format used in this flooding module:
 *  [1B Hopcount] [1B Maximum Hopcount] [1B Sequence Id]
 */
#define PACKET_CUR_HOPCOUNT(msdu) ((msdu)[0])
#define PACKET_MAX_HOPCOUNT(msdu) ((msdu)[1])
#define PACKET_SET_HOPCOUNT(msdu,hop) ((msdu)[0] = (hop))
#define PACKET_SET_MAX_HOPCOUNT(msdu,hop) ((msdu)[1] = (hop))

#define PACKET_CUR_SEQID(msdu) ((msdu)[2])
#define PACKET_SET_CURSEQID(msdu,id) ((msdu)[2]=(id))

/* switch the value of two pointer variables */
inline static void _switch_ptr( TiFrame **ptr1, TiFrame ** ptr2 );

TiFloodNetwork * flood_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiFloodNetwork) <= size );
	memset( mem, 0x00, size );
	return (TiFloodNetwork *)mem;
}

void flood_destroy( TiFloodNetwork * net )
{
	return;
}

TiFloodNetwork * flood_open( TiFloodNetwork * net, TiAloha * mac, TiFunEventHandler listener, 
	void * lisowner, uint16 pan, uint16 localaddress )
{
	net->state = FLOOD_STATE_IDLE;
	net->mac = mac;
	net->panto = pan;
	net->panfrom = pan;
	net->localaddress = localaddress;
	net->distance = ~0;
	net->seqid = 0;
	net->listener = listener;
	net->lisowner = lisowner;

	net->txque = frame_open( (char * )( &net->txque_mem), FLOOD_FRAMEOBJECT_SIZE, 3, 20, 0);
	net->rxque = frame_open( (char *)( &net->rxque_mem), FLOOD_FRAMEOBJECT_SIZE, 3, 20, 0);
	net->rxbuf = frame_open( (char *)( &net->rxbuf_mem), FLOOD_FRAMEOBJECT_SIZE, 3, 20, 0);

	net->cache = flood_cache_open( (char *)( &net->cache_mem), FLOOD_CACHE_HOPESIZE );
	hal_assert( net->cache != NULL );

	return net;
}

void flood_close( TiFloodNetwork * net )
{
	// todo: you should cancel all pending network transmission first before call 
	// this function.

	frame_close( net->txque );
	frame_close( net->rxbuf );
	frame_close( net->rxque );
}

/* Broadcast a frame in the network. In most cases, the frame should be able to 
 * reach every node in the network, but no guarantee about this.
 */
uintx flood_broadcast( TiFloodNetwork * net, TiFrame * frame, uint8 option )
{
	uintx count=0;
	uintx i = 0;//todo 
	char * pc;
	
	/* This function will try to put the frame into TiFloodNetwork's internal TX buffer. 
	 * The real sending processing is in flood_evolve(). 
	 */
    i = frame_length( frame);//todo
	
	if (frame_empty(net->txque)) 
	{     
		net->txque->option = option;
		count = frame_totalcopyfrom( net->txque, frame );
        

		frame_skipouter( net->txque, 4, 0 );//todo 执行这一局后frame_length又变回0了！

		frame_setlength( net->txque,(i+4));//todo

		
		// assert( frame_skipouter must be success );
		pc = frame_startptr( net->txque );
		
		PACKET_SET_HOPCOUNT( pc,0 );
		PACKET_SET_MAX_HOPCOUNT(pc , CONFIG_FLOOD_MAX_COUNT );
		PACKET_SET_CURSEQID(pc, net->seqid );
	}

	flood_evolve( net, NULL );
	return count;
}

/* Send a frame out. 
 * 
 * @attention This function is designed to send to a specific node. However, this
 * version of flood doesn't support address mechanism, so this function cannot 
 * specify the destination node. It's essentially the same as flood_broadcast() now.
 */
uintx flood_send( TiFloodNetwork * net, uint16 shortaddrto, TiFrame * frame, uint8 option )
{
	return flood_broadcast( net, frame, option );
}

/* Check for arrived frames, no matter who send them. The frame will be returned 
 * through the parameter "frame" only when the frame's destination matches the current 
 * node. The other frames will be forwarded or discarded.
 */
uintx flood_recv( TiFloodNetwork * net, TiFrame * frame, uint8 option )
{
	uintx count=0;

	flood_evolve( net, NULL );

	if (!frame_empty( net->rxque))
	{
		
		count = frame_totalcopyfrom( frame, net->rxque );
		frame_bufferclear( net->rxque );
	}
	
	return count;
}

/* Set the callback listener function of this service. The listener function will
 * be called if a frame arrived. So it can replace the query-based frame arrival 
 * checking.
 */
void flood_set_listener( TiFloodNetwork * net, TiFunEventHandler listener, void * owner )
{
	net->listener = listener;
	net->lisowner = owner;
}

/* This function receive event from other services/objects and drive the evolution 
 * of the TiFloodNetwork service. 
 * 
 * This function can also be the listener of the MAC layer, Though it's not mandatory.
 * 
 * @attention: 
 *	This function cannot run in the interrupt mode.
 */
void flood_evolve( void * netptr, TiEvent * e )
{
	TiFloodNetwork * net = (TiFloodNetwork *)netptr;
	uint8 len, count, cur_hopcount, max_hopcount;
	bool done = true, cont= false;
	char * pc;

	aloha_evolve( net->mac,e );

	// This event is sent from MAC layer. 
	//
	// @todo
	// Currently, all the frames from MAC layer will submitted the listener function.
	// Actually, this isn't necessary. Only those to this node should submitted to 
	// the listener.
	//
	if (e->id == EVENT_DATA_ARRIVAL)
	{
		if (net->listener != NULL)
		{
			net->listener( net->lisowner, e );
			return;
		}
	}

	do{ 
		
		switch (net->state)
		{
		// IDLE is the initial and default state of this state machine. 
		case FLOOD_STATE_IDLE:

			
			// Check if the TX queue is empty. If not then try to send the frame.
			if (!frame_empty(net->txque))
			{   
				
				len = aloha_broadcast( net->mac, net->txque, net->txque->option );
				
				if (len > 0)
				{
					frame_bufferclear( net->txque );
				}

				// @attention
				// @todo: if the aloha_send() is non-blocked operation, then you should
				// transfer to FLOOD_STATE_WAITFOR_TXREPLY state to wait. You should empty
				// the buffer only when this service receives the reply from MAC layer.
				
				 //net->state = FLOOD_STATE_WAIT_TXREPLY
				 done = true;
			}
		
			// Check if the RX queue is empty, then try to receive one from MAC layer
          	if ((frame_empty(net->rxbuf)) && frame_empty(net->rxque))
			{   
				cont = true;
				count = aloha_recv( net->mac, net->rxbuf, 0x00 );
				
				if (count <= 0)
					cont = false;

				// Check: whether the same frame has already been received. This
				// is by searching it in the cache. If it cannot find it in the 
				// cache, then put the frame in the cache.
				//
				// @modified by zhangwei on 2011.03.14
				// - we use the first 4 bytes in the frame (which is essentially 
				// the header of flood protocol as the key to visit the cache.
				//
	            if (cont)
				{   
					if (flood_cache_visit( net->cache, (char*)frame_startptr(net->rxbuf) ))//todo 我将CONFIG_FLOOD_CACHE_CAPACITY改成了1原先为8，不改的话不再接收新的帧.
					{   
						
						frame_bufferclear( net->rxbuf );
						cont = false;
					}
				}

				if (cont)
				{     
					frame_totalcopyto( net->rxbuf, net->rxque );
                  
					// Check whether this frame has reaches its maximum hopcount. 
					// Discard this frame if this is true.
					
					char * pc;
					pc = frame_startptr( net->rxbuf);

					cur_hopcount = PACKET_CUR_HOPCOUNT( pc );
					max_hopcount = PACKET_MAX_HOPCOUNT( pc);
					cur_hopcount ++;
					if (cur_hopcount > max_hopcount)
						frame_bufferclear( net->rxbuf );
					else
						PACKET_SET_HOPCOUNT( pc, cur_hopcount );
				}
				
				done = true;
			}

			// Check whether there's frame pending for forwarding
			if ((!frame_empty(net->rxbuf)) && frame_empty(net->txque))
			{   
				pc = frame_startptr( net->txque);
				// move the frame from rxbuf to txque. For efficiency reasons, this is implmented
				// by switching the two pointers.				 
				_switch_ptr( &(net->rxbuf), &(net->txque) );
				
				// @attention
				// generally, you should change the frame in "txque" with the PAN id
                // and BROADCAST_ADDRESS configured in this flood component. However,
				// here we assume the low level medium access protocol helps us to do so.

				cur_hopcount = PACKET_CUR_HOPCOUNT( pc );
				PACKET_SET_HOPCOUNT( pc, cur_hopcount++ );
				done = false;
			}	
			break;


		// This's the state waiting for MAC layer's reply. The sending process of the 
		// MAC layer may last for quite a long time due to re-transmission. Only at the
		// end of the transmission, the MAC layer does possible to notify the NET layer 
		// the transmission results of the sending request.
		//
		/*case FLOOD_STATE_WAITFOR_TXREPLY:
			 ioresult = aloha_ioresult(net->mac);
			 if ioresult == IDLE|TXDONE
			 {
					opf_clear( net->txque );
					net->state = FLOOD_STATE_IDLE;
			 }
			// if ioresult indicate there's error or failure
			//	report to the master program by call net->listener
			net->state = FLOOD_STATE_IDLE;
			break;*/

		default:
			net->state = FLOOD_STATE_IDLE;
		}
	}while (!done);

	return;
}

void _switch_ptr( TiFrame **ptr1, TiFrame ** ptr2 )
{
	TiFrame * tmp;
	tmp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp;
}
