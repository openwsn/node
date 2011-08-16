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
#ifndef _SVC_NIO_DISPATCHER_H_8496_
#define _SVC_NIO_DISPATCHER_H_8496_

/******************************************************************************* 
 * rtl_dispatcher
 * this is an C language version of dispatcher object. dispatcher is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * dispatcher object is similar to notifier. but it simply route the events to 
 * their corresponding receiver. it won't duplicate the events and sent it to all
 * as notifier does. 
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 * @modified by zhangwei on 200903
 *	- revision 
 * @modified by zhangwei on 200905
 *	- improved performance. this version eliminate the using of TiVectorX, and 
 *    implement the event->handler map itself. 
 ******************************************************************************/ 

#include "../rtl/rtl_configall.h"
#include "../rtl/rtl_foundation.h"
#include "svc_nio_aloha.h"

/******************************************************************************* 
 * attention
 * You should avoid to use id 0. Event id 0 is used by the dispatcher.
 ******************************************************************************/ 

#ifndef MAX_IEEE802FRAME154_SIZE
#define MAX_IEEE802FRAME154_SIZE 128
#endif

#define NIO_DISPA_HOPESIZE(capacity) (sizeof(TiNioDispatcher) + sizeof(_TiNioDispatcherItem)*capacity)

#ifdef __cplusplus
extern "C" {
#endif

//typedef (uintx *)(* TiFunRxHandler)( void * object, TiFrame * input, TiFrame * output, uint8 option );
typedef uint8(* TiFunRxHandler)( void * object, TiFrame * input, TiFrame * output, uint8 option );

//typedef (uintx *)(* TiFunTxHandler)( void * object, TiFrame * input, TiFrame * output, uint8 option );
typedef uint8(* TiFunTxHandler)( void * object, TiFrame * input, TiFrame * output, uint8 option );

#pragma pack(1) 
typedef struct{
    uint8 state;
    void * object;
    TiFunRxHandler rxhandler;
    TiFunTxHandler txhandler;
    TiFunEventHandler evolve;
    uint8 proto_id;
}_TiNioNetLayerDispatcherItem;

#define CONFIG_NIO_NETLAYER_DISP_CAPACITY 4
#pragma pack(1) 
typedef struct{
    TiFrame * rxbuf;
    TiFrame * txbuf;
    TiFrame * rxbake;
    TiAloha * mac;
    char rxbuf_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    char rxbake_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    char txbak_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    _TiNioNetLayerDispatcherItem items[CONFIG_NIO_NETLAYER_DISP_CAPACITY];
}TiNioNetLayerDispatcher;

TiNioNetLayerDispatcher * net_disp_construct( void * mem, uint16 memsize );

TiNioNetLayerDispatcher * net_disp_open( TiNioNetLayerDispatcher * dispacher,TiAloha * mac);

TiNioNetLayerDispatcher * net_disp_close( TiNioNetLayerDispatcher * dispacher,TiAloha *mac);

uint8 net_disp_send( TiNioNetLayerDispatcher * dispacher,TiFrame * f ,uint16 addr,uint8 option);

uint8 net_disp_broadcast( TiNioNetLayerDispatcher * dispacher,TiFrame * f ,uint8 option);

uint8 net_disp_recv(  TiNioNetLayerDispatcher * dispacher,TiFrame * f );

void net_disp_evolve(void * object, TiEvent * e );



/**
 * @param proto_id: Protocol Identifier. 
 */
void net_disp_register(  TiNioNetLayerDispatcher * dispatcher,uint8 proto_id, void * object, TiFunRxHandler rxhandler, TiFunTxHandler txhandler, TiFunEventHandler evolve );


void net_disp_unregister(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id );











/*

typedef (uint8)(* TiNioProcess)( void * object, TiNioSession * session );

/* component id is used to map the index in the array */
/*
typedef struct{
	TiNioSession session;
	void * object;
	TiFunEventHandler process;
}_TiNioDispatcherItem;

typedef struct{
	uint8 parent;
	uint8 child;
	uint8 sibling; // actually sibling list
	uint8 dataptr;
	TiNioSession session;
	void * object;
	TiFunEventHandler process;
}_TiNioDispatcherTreeItem;

typedef struct{
	uint16 size;
	uint8 capacity;
	void * owner;
	uint8 current;
	TiFun getdispatchid(TiFrame);
	parent_session;
}TiNioDispatcher;

TiDispatcher *  nio_dispa_create( uint8 capacity );
void            nio_dispa_free( TiDispatcher * dpa );
TiDispatcher *  nio_dispa_construct( char * buf, uint16 size, uint8 capacity );
void            nio_dispa_destroy( TiDispatcher * dpa );
TiDispatcher *  nio_dispa_open( TiDispatcher * dpa, uint8 capacity );
void            nio_dispa_close( TiDispatcher * dpa );

id是唯一标识
在dispatch的时候，id要match

dispatch
gather
execute = dispatch + inversedispatch/aggregate
inversedispatch

2010.12.17
rx_process() = dispatch
{
	f = rxque.front
	while this frame is effective
		get id
		do dispatch by calling process functions
	end
	if this frame is still exists, then do nothing 
}

2010.12.17
tx_process() = inversedispatch/aggregate
{
	get rear frame from txque
	get id
	get parend by id 
	call parent to deal with this frame
}

nio_dispa_register( layer, index, id, component, rxhandler, txhandler )
nio_dispa_layercount


nio_dispatcher_attachroot
nio_dispa_attachchild
nio_dispa_attachsibling
nio_dispa_attachhandler fmque based handler + non queue based handler
nio_dispa_detach

nio_dispa_root
nio_dispa_current
nio_dispa_goto( id )   moveto
nio_dispa_goto_root  moveto_root
nio_dispa_goto_first moveto_first
nio_dispa_goto_next  moveto_next
nio_dispa_goto_prev
nio_dispa_goto_last


nio_dispa_attach( dpa, id, TiNioProcess, object ) = nio_dispa_attachsibling
nio_dispa_attachchild( dpa, id, TiNioProcess, object )
nio_dispa_attachsibling( dpa, id, TiNioProcess, object )

nio_dispa_detach()=NULL

nio_dispa_current
nio_dispa_root / parent / first / next / child
nio_dispa_moveroot / moveparent / movefirst / movechild

nio_dispa_curlayer
nio_dispa_curorder

nio_dispa_read(
nio_dispa_write(

bool            nio_dispa_attach( TiDispatcher * dpa, uint8 id, TiFunEventHandler handler, void * object );
void            nio_dispa_detach( TiDispatcher * dpa, uint8 id );
void            nio_dispa_send( TiDispatcher * dispa, TiEvent * e => TiFrame * frame );


delete the following



/* _TiDispatcherItem
 *	id			event id
 *	handler		handler, an function pointer with TiFunEventHandler type
 *	object      handler owner. = event.objectto
 */
/*
typedef struct{
	uint8       id;
	TiFunEventHandler handler;
	void *      object;
}_TiDispatcherItem;

typedef struct{
	uint16       size;
	uint8       capacity;
	void *      owner;
}TiDispatcher;

TiDispatcher *  nio_dispa_create( uint8 capacity );
void            nio_dispa_free( TiDispatcher * dpa );
TiDispatcher *  nio_dispa_construct( char * buf, uint16 size, uint8 capacity );
void            nio_dispa_destroy( TiDispatcher * dpa );
bool            nio_dispa_attach( TiDispatcher * dpa, uint8 id, TiFunEventHandler handler, void * object );
void            nio_dispa_detach( TiDispatcher * dpa, uint8 id );
void            nio_dispa_send( TiDispatcher * dispa, TiEvent * e => TiFrame * frame ); */

#ifdef __cplusplus
}
#endif

#endif



