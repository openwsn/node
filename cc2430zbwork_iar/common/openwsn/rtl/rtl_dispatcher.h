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
#ifndef _RTL_DISPATCHER_H_8496_
#define _RTL_DISPATCHER_H_8496_

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

#include "rtl_configall.h"
#include "rtl_foundation.h"

/******************************************************************************* 
 * attention
 * You should avoid to use id 0. Event id 0 is used by the dispatcher.
 ******************************************************************************/ 

#ifdef __cplusplus
extern "C" {
#endif

#define DISPA_HOPESIZE(capacity) (sizeof(TiDispatcher) + sizeof(_TiDispatcherItem)*capacity)

/* _TiDispatcherItem
 *	id			event id
 *	handler		handler, an function pointer with TiFunEventHandler type
 *	object      handler owner. = event.objectto
 */
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

TiDispatcher *  dispa_create( uint8 capacity );
void            dispa_free( TiDispatcher * dpa );
TiDispatcher *  dispa_construct( char * buf, uint16 size, uint8 capacity );
void            dispa_destroy( TiDispatcher * dpa );
bool            dispa_attach( TiDispatcher * dpa, uint8 id, TiFunEventHandler handler, void * object );
void            dispa_detach( TiDispatcher * dpa, uint8 id );
void            dispa_send( TiDispatcher * dispa, TiEvent * e );

#ifdef __cplusplus
}
#endif

#endif 


