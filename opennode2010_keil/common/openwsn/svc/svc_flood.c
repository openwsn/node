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
 *****************************************************************************/

/* @attention: you can also replace module with "svc_adaptaloha.h" to enable the 
 * Adaptive ALOHA protocol 
 */
#include "svc_configall.h"
#include <string.h>
#include "../rtl/rtl_openframe.h"
#include "../hal/hal_target.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_aloha.h"
#include "svc_flood.h"

inline static bool _flood_cache_hit( TiFloodNetwork * net, TiOpenFrame * frame, uint8 * idx );
inline static uint8 _flood_cache_displace( TiFloodNetwork * net, TiOpenFrame * frame, uint8 idx );
inline static uint8 _flood_cache_visit( TiFloodNetwork * net, TiOpenFrame * frame );
inline static void _switch_ptr( TiOpenFrame **ptr1, TiOpenFrame ** ptr2 );

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
	uint8 i;

	net->state = FLOOD_STATE_IDLE;
	net->mac = mac;
	net->panto = pan;
	net->panfrom = pan;
	net->localaddress = localaddress;
	net->distance = ~0;
	net->listener = listener;
	net->lisowner = lisowner;


	net->txque = opf_open( net->opf1, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
	net->rxbuf =opf_open( net->opf2, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
    net->rxque =opf_open( net->opf3, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK,OPF_DEF_OPTION );
	//   three same type ::opf_open( net->rxque, (1 << OPF_OPTION_CRC) ); I replace it with OPF_DEF_OPTION
    //same replace in other places.what 
	//opf_cast( net->txque, OPF_PSDU_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK);
	//opf_cast( net->rxbuf, OPF_PSDU_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK );
	//opf_cast( net->rxque, OPF_PSDU_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK );  
//whether it is should be opf_psdu_size or opf_suggest_size   I think  psdu is right but not sure . 
//initial parameter is opf_frame_size what about it ?
	//opf_set_sequence( net->txque, 0 );
	//opf_set_sequence( net->rxbuf, 0 );
	//opf_set_sequence( net->rxque, 0 );
opf_clear(net->rxbuf);
opf_clear(net->rxque);
opf_clear(net->txque);  
	for (i=0; i<CONFIG_FLOOD_CACHE_SIZE; i++)
		net->cache[i].lifetime = 0;

	return net;
}

void flood_close( TiFloodNetwork * net )
{
	// todo: you should cancel all pending network transmission first before call 
	// this function.

	//net->txque = opf_open( net->opf1, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
	//net->rxbuf =opf_open( net->opf2, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
    //net->rxque =opf_open( net->opf3, OPF_SUGGEST_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK,OPF_DEF_OPTION );
	// three same type according to openframe.h they are same with opf_open( net->rxque, (1 << OPF_OPTION_CRC) );
	// but how about CRC ? doesn't need any way ? what 

	opf_close( net->txque );
	opf_close( net->rxbuf );
	opf_close( net->rxque );
}

/* Broadcast a frame in the network. In most cases, the frame should be able to 
 * reach every node in the network, but no guarantee about this.
 */
uint8 flood_broadcast( TiFloodNetwork * net, TiOpenFrame * frame, uint8 option )
{
	uint8 count=0;
	
	/* This function will try to put the frame into TiFloodNetwork's internal TX buffer. 
	 * The real sending processing is in flood_evolve(). 
	 */
	if (opf_empty(net->txque))
	{     
	
		count = opf_copyfrom( net->txque, frame ); 
		if (opf_parse( net->txque ,0))
		{    
			opf_set_panto( net->txque, net->panto );
			opf_set_shortaddrto( net->txque, OPF_BROADCAST_ADDRESS );
			opf_set_panfrom( net->txque, net->panfrom );
			opf_set_shortaddrfrom( net->txque, net->localaddress );
			PACKET_SET_HOPCOUNT( net->txque->msdu,0 );
			PACKET_SET_MAX_HOPCOUNT( net->txque->msdu, CONFIG_MAX_FLOOD_COUNT );
		}
		else{
			opf_clear( net->txque );
			count = 0;
		}
	}

	flood_evolve( net, NULL );
	return count;
}

/* Send a frame to a specific node. This function is almost the same as the flood_broadcast(). 
 * However, the receiver will discard this frame if the destination isn't match.
 */
uint8 flood_send( TiFloodNetwork * net, TiOpenFrame * frame, uint8 option )
{
	uint8 count=0;

	/* This function will try to put the frame into TiFloodNetwork's internal TX buffer. 
	 * The real sending processing is in flood_evolve(). 
	 */
	if (opf_empty(net->txque))
	{
		count = opf_copyfrom( net->txque, frame ); 
		if (opf_parse( net->txque ,0)) //why is 0??   what's meaning  about len
		{
			opf_set_panto( net->txque, net->panto );
			opf_set_panfrom( net->txque, net->panfrom );
			opf_set_shortaddrfrom( net->txque, net->localaddress );
			PACKET_SET_HOPCOUNT( &(net->txque->buf[0]), 0 );
			PACKET_SET_MAX_HOPCOUNT( &(net->txque->buf[0]), CONFIG_MAX_FLOOD_COUNT );
		}
		else{
			opf_clear( net->txque );
			count = 0;
		}
	}

	flood_evolve( net, NULL );
	return count;
}

/* Check for arrived frames, no matter who send them. The frame will be returned 
 * through the parameter "frame" only when the frame's destination matches the current 
 * node. The other frames will be forwarded or discarded.
 */
uint8 flood_recv( TiFloodNetwork * net, TiOpenFrame * frame, uint8 option )
{
	uint8 count=0;

	if (!opf_empty(net->rxque))
	{
		count = opf_copyto( net->rxque, frame ); 
		opf_clear( net->rxque );
	}
  
	flood_evolve( net, NULL );
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
			// Check if the TX queue is empty. If not then try to send the frame
			if (!opf_empty(net->txque))
			{   
				len = aloha_broadcast( net->mac, net->txque, 0x00 );
				if (len > 0)
					opf_clear( net->txque );

				// @attention
				// @todo: if the aloha_send() is asynchronous operation, then you should
				// transfer to FLOOD_STATE_WAITFOR_TXREPLY state to wait. You should empty
				// the buffer only when this service receives the reply from MAC layer.
				
				 //net->state = FLOOD_STATE_WAIT_TXREPLY
				 //done = false;
			}
		
			// Check if the RX queue is empty, then try to receive one from MAC layer
     

          	if ((opf_empty(net->rxbuf)) && opf_empty(net->rxque))
			{
				cont = true;
				count = aloha_recv( net->mac, net->rxbuf, 0x00 );
				if (count <= 0)
					cont = false;

				if (cont)
				{				
					if (!opf_parse( net->rxbuf,0 ))
					{
						opf_clear( net->rxbuf );
						cont = false;
					}
				}

				// The 802.15.4 ACK, BEACON,  and COMMNAD type frames are ignored
				// and discarded.
				if (cont)
				{
					if (opf_type(net->rxbuf) != FCF_FRAMETYPE_DATA)
					{   
						opf_clear( net->rxbuf );
						cont = false;
			     	}
				}

				// Check: whether the same frame has already been received. This
				// is by searching it in the cache. If it cannot find it in the 
				// cache, then put the frame in the cache.
	            if (cont)
				{
					if (_flood_cache_visit( net, net->rxbuf ))
					{    
						opf_clear( net->rxbuf );
						cont = false;
					}
				}

				if (cont)
				{     
					opf_copyto( net->rxbuf, net->rxque );
                  
					// Check whether this frame has reaches its maximum hopcount. 
					// Discard this frame if this is true.
					cur_hopcount = PACKET_CUR_HOPCOUNT( net->rxbuf->msdu );
					max_hopcount = PACKET_MAX_HOPCOUNT( net->rxbuf->msdu);
					cur_hopcount ++;
					if (cur_hopcount > max_hopcount)
						opf_clear( net->rxbuf );
					else
						PACKET_SET_HOPCOUNT( net->rxbuf->msdu, cur_hopcount );
                 
				}
			}

			// Check whether there's frame pending for forwarding
			if ((!opf_empty(net->rxbuf)) && opf_empty(net->txque))
			{
				// move the frame from rxbuf to txque. For efficiency reasons, this is implmented
				// by switching the two pointers.				 
				_switch_ptr( &(net->rxbuf), &(net->txque) );

				opf_set_panto( net->txque, net->panto );
				opf_set_shortaddrto( net->txque, OPF_BROADCAST_ADDRESS );
				opf_set_panfrom( net->txque, net->panfrom );
				opf_set_shortaddrfrom( net->txque, net->localaddress );
				cur_hopcount = PACKET_CUR_HOPCOUNT( net->rxbuf->msdu );
				PACKET_SET_HOPCOUNT( net->txque->msdu, cur_hopcount++ );
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

/* @return
 *	true                     when hitted. (found the frame in the cache). *idx is 
 *                           the item index in the cache.
 *  false                    not hitted. *idx is undefined in this case. 
 */
bool _flood_cache_hit( TiFloodNetwork * net, TiOpenFrame * frame, uint8 * idx )
{
	bool found=false;;
	uint8 i;
	_TiFloodCacheItem * item;

	found = false;

	// searching for the specified frame in the cache

	for (i=0; i<CONFIG_FLOOD_CACHE_SIZE; i++)
	{
		item = &(net->cache[i]);
		if ((item->panto == opf_panto(frame)) && (item->shortaddrto == opf_shortaddrto(frame))
			&& (item->panfrom == opf_panfrom(frame)) && (item->shortaddrfrom == opf_shortaddrfrom(frame))
			&& (item->seqid == opf_sequence(frame)) )
		{
			found = true;
			if (idx != NULL)
				*idx = i;
			item->lifetime = CONFIG_FLOOD_CACHE_MAX_LIFETIME;
			break;
		}
	}

	if (!found)
	{
		item->lifetime = CONFIG_FLOOD_CACHE_MAX_LIFETIME;
		item->panto = opf_panto(frame);
		item->shortaddrto = opf_shortaddrto(frame);
		item->panfrom = opf_panfrom(frame);
		item->shortaddrfrom = opf_shortaddrfrom(frame);
		item->seqid = opf_sequence(frame);
	}

	return found;
}

/* Put the specified frame into the cache. If the cache is full, then the eldest 
 * frame will be displaced with the new one. The eldest frame is with the smallest
 * lifetime value.
 * 
 * @return 
 *	the index of the new frame in the cache.
 */
uint8 _flood_cache_displace( TiFloodNetwork * net, TiOpenFrame * frame, uint8 idx )
{
	uint8 replace=0, i;
	_TiFloodCacheItem * item;

	item = &(net->cache[0]);
	if (item->lifetime == 0)
	{
		replace = 0;
	}
	else{
		replace=0;
		for (i=1; i<CONFIG_FLOOD_CACHE_SIZE; i++)
		{
			item = &(net->cache[i]);
			if (item->lifetime == 0)
			{
				replace = i;
				break;
			}
			if (item->lifetime < net->cache[replace].lifetime)
				replace = i;
		}
	}

	item = &(net->cache[replace]);
	item->panto = opf_panto(frame);
	item->shortaddrto = opf_shortaddrto(frame);
	item->panfrom = opf_panfrom(frame);
	item->shortaddrfrom = opf_shortaddrfrom(frame);
	item->seqid = opf_sequence(frame);
	item->lifetime = CONFIG_FLOOD_CACHE_MAX_LIFETIME;

	return replace;
}

/* Searching for the specified frame in the cache. It hitted/found, then returns 1.
 * Return 0 when not hitted. 
 * 
 * If not hitted, then this function will also put the new frame into the cache.
 *
 * @return
 *	true					when successfully found the item in the cache (hit the cache)
 *  false                   
 */
uint8 _flood_cache_visit( TiFloodNetwork * net, TiOpenFrame * frame )
{
	uint8 i;
	_TiFloodCacheItem * item;

	if (_flood_cache_hit(net, frame, 0))
		return true;

	_flood_cache_displace( net, frame, 0 );

	for (i=0; i<CONFIG_FLOOD_CACHE_SIZE; i++)
	{
		item = &(net->cache[i]);
		if (item->lifetime > 0)
			item->lifetime --;
	}

	return 0;
}

void _switch_ptr( TiOpenFrame **ptr1, TiOpenFrame ** ptr2 )
{
	TiOpenFrame * tmp;
	tmp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp;
}
