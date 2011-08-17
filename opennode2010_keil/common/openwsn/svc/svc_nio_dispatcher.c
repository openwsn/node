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

/*
 * rtl_dispatcher.c
 * event dispatcher object. usually dispatch one event to one object. 
 *
 * @state
 *	tested
 *
 * @author zhangwei on 200903
 *	- first created. inspired by rtl_notifier. 
 */

#include "../rtl/rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../rtl/rtl_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_configall.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_mcu.h"
#include "svc_nio_aloha.h"
#include "svc_nio_dispatcher.h"


/*
inline _TiDispatcherItem * _dispa_items( TiDispatcher * dispa );
inline bool _dispa_search( _TiDispatcherItem * items, uint8 capacity, uint8 eid, uint8 * pidx );
inline bool _dispa_apply( _TiDispatcherItem * items, uint8 capacity, uint8 * pidx );

TiDispatcher * dispa_create( uint8 capacity )
{
	uint16 size = DISPA_HOPESIZE(capacity);
	void * buf;
	
	buf = malloc( size  );
	if (buf != NULL)
	{
		memset( buf, 0x00, size );
		dispa_construct( buf, size, capacity );
	}

	return (TiDispatcher *)buf;
}

void dispa_free( TiDispatcher * dpa )
{
	dispa_destroy(dpa);
}

TiDispatcher * dispa_construct( char * buf, uint16 size, uint8 capacity )
{
	TiDispatcher * dpa = (TiDispatcher *)buf;
	rtl_assert( capacity > 0 );
	dpa->size = size;
	dpa->capacity = capacity;
	dpa->owner = NULL;
	return dpa;
}

void dispa_destroy( TiDispatcher * dpa )
{
	return;
}

/**
 * attention
 * The "handler" paramter should NOT be NULL, or an assert() in dispa_send() will failed.
 * However, the "object" parameter can be NULL.
 */
/*
bool dispa_attach( TiDispatcher * dpa, uint8 id, TiFunEventHandler handler, void * object )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;
	bool found = false;

	idx = id % dpa->capacity;
	if ((items[idx].id == id) || (items[idx].id == 0))
	{
		found = true;
	}
	else{
		found = _dispa_search(items, dpa->capacity, id, &idx);
		if (!found)
		{
			idx = id % (dpa->capacity) + 1;
			found = _dispa_apply(items, dpa->capacity, &idx);
		}
	}

	if (found)
	{
		items[idx].id = id;
		items[idx].handler = handler;
		items[idx].object = object;
	}

	return found;
}

void dispa_detach( TiDispatcher * dpa, uint8 id )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;

	idx = id % (dpa->capacity);
	if (items[idx].id == id)
	{
		items[idx].id = 0;
		items[idx].handler = NULL;
		items[idx].object = NULL;
	}
	else{
		idx ++;
		if (_dispa_search(items, dpa->capacity, id, &idx))
		{
			items[idx].id = 0;
			items[idx].handler = NULL;
			items[idx].object = NULL;
		}
	}
}

/* attention
 *	- dispa_send() can be the listener of other components. because it's a standard 
 *    TiFunEventHandler type.
 */
/*
void dispa_send( TiDispatcher * dpa, TiEvent * e )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;
	bool found = false;

	rtl_assert((e != NULL) && (e->id != 0));
	if (e->id == 0)
		return;
	
	idx = e->id % (dpa->capacity);
	
	if (items[idx].id == e->id)
	{
		found = true;
	}
	else{
		idx ++;
		found = _dispa_search(items, dpa->capacity, e->id, &idx);
	}
	if (found)
	{	
		/* attention
		 * If this assert() failed, it often means the user gives wrong parameter 
		 * values of "handler" to the dispa_attach() function. The "handler" parameter
		 * shouldn't be NULL.
		 */ 
/*
		rtl_assert( items[idx].handler != NULL );
		items[idx].handler( items[idx].object, e );
	}
}

inline _TiDispatcherItem * _dispa_items( TiDispatcher * dpa )
{
	return (_TiDispatcherItem *)( (char*)dpa + sizeof(TiDispatcher) );
}

/* search for specific items in the list
 * parameter
 *  pidx			     the final index after execution if found.
 */
/*
inline bool _dispa_search( _TiDispatcherItem * items, uint8 capacity, uint8 id, uint8 * pidx )
{
	bool found = false;
	uint8 i=0, idx;

	idx = id % capacity;
	if (items[idx].id == id)
	{
		* pidx = idx;
		return true;
	}

	for (i=idx+1; i<capacity; i++)
	{
		if (items[i].id == id)
		{
			found = true;
			* pidx = i;
		}
	}
	if (!found)
	{
		for (i=0; i<idx-1; i++)
		{
			if (items[i].id == id)
			{
				found = true;
				* pidx = i;
			}
		}
	}

	return found;
}

/* allocate an empty item in the dispatcher item list
 * 
 * parameter
 *	pidx			(*pidx) is the initial start searching position
 *                  also the returned index if return true
 * 
 * return
 *  true when success, and false if failed. *pidx is the index if success.
 */
/*
inline bool _dispa_apply( _TiDispatcherItem * items, uint8 capacity, uint8 * pidx )
{
	bool found = false;
	uint8 i=0, idx;

	idx = *pidx;
	for (i=*pidx; i<capacity; i++)
	{
		if ((items[i].id == 0) && (items[i].handler == NULL))
		{
			found = true;
			*pidx = i;
		}
	}
	if (!found)
	{
		for (i=0; i<(*pidx)-1; i++)
		{
			if ((items[i].id == 0) && (items[i].handler == NULL))
			{
				found = true;
				*pidx = i;
			}
		}
	}

	return found;
}

/* release the item with specific id
 *
 * parameter
 *	idx             initial start search position
 */
/*
bool _dispa_release( _TiDispatcherItem * items, uint8 capacity, uint8 id, uint8 idx )
{
	bool found = false;
	//uintx i=0, idx;
/*
	// to do in the future
	rtl_assert( false );
*/
/*
	return found;
}

*/

TiNioNetLayerDispatcher * net_disp_construct( void * mem, uint16 memsize )
{
    memset( mem, 0x00, memsize );
    return (TiNioNetLayerDispatcher*)mem;
}


TiNioNetLayerDispatcher * net_disp_open( TiNioNetLayerDispatcher * dispatcher,TiAloha *mac)
{
    dispatcher->rxbuf = frame_open( (char*)(&dispatcher->rxbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 102 );
    dispatcher->txbuf = frame_open( (char*)(&dispatcher->txbak_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 102 );
    dispatcher->rxbake = frame_open( (char*)(&dispatcher->rxbake_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 102 );
    dispatcher->mac = mac;
}

TiNioNetLayerDispatcher *net_disp_close( TiNioNetLayerDispatcher * dispacher,TiAloha *mac)
{

}

uint8 net_disp_send( TiNioNetLayerDispatcher * dispacher,TiFrame * f ,uint16 addr,uint8 option) 
{
    uint8 count;
    count = aloha_send(dispacher->mac,addr,f,option);
    return count;
}

uint8 net_disp_broadcast( TiNioNetLayerDispatcher * dispacher,TiFrame * f ,uint8 option)
{
    uint8 count;
    count = aloha_broadcast(dispacher->mac,f,option);
    return count;
}

uint8 net_disp_recv(  TiNioNetLayerDispatcher * dispacher,TiFrame * f )
{
   uint8 count;
   count =0;
   frame_reset( f,3,20,0);
   if ( !frame_empty( dispacher->rxbuf))
   {
       count = frame_totalcopyfrom( f,dispacher->rxbuf);
       frame_clear( dispacher->rxbuf);
   }

   return count;
}

void net_disp_evolve(void* object, TiEvent * e )
{
    uint8 count;
    char *payload;
    uint8 proto_id;
    int i;
    TiNioNetLayerDispatcher * item = ( TiNioNetLayerDispatcher *)object;

    count = 0;
    
    if ( frame_empty( item->rxbuf))
    {
        frame_reset( item->rxbuf,3,20,0);
        count = aloha_recv(item->mac,item->rxbuf,0x00);
        if (count > 0)
        {
            payload = frame_startptr(item->rxbuf);
            proto_id = payload[0]&0x02;//if the second bit is set, then the protocal is the ndp.
            for ( i=0;i< CONFIG_NIO_NETLAYER_DISP_CAPACITY;i++)
            {
                if ( (item->items[i].proto_id&0x02) == proto_id)
                {
                    break;
                }
            }

            if ( i< CONFIG_NIO_NETLAYER_DISP_CAPACITY)
            {
                if ( item->items[i].rxhandler( item->items[i].object,item->rxbuf,item->rxbake,0x00)>0)
                {
                    if ( !frame_empty( item->rxbake))
                    {
                        frame_totalcopyfrom( item->rxbuf,item->rxbake);
                        frame_clear( item->rxbake);
                    }
                }
                else
                {
                    frame_clear( item->rxbuf);
                    frame_clear( item->rxbake);
                }
            }
               
        }
    }

    //todo 有必要让每一个evolve都跑一边吗？

    for ( i=0;i<CONFIG_NIO_NETLAYER_DISP_CAPACITY;i++)
    {
        if ( item->items[i].state)
        {
            item->items[i].evolve;//todo 这一句是否正确？
        }
    }
    
    
    
}



/**
 * @param proto_id: Protocol Identifier. 
 */
void net_disp_register(  TiNioNetLayerDispatcher * dispatcher,uint8 proto_id, void * object, TiFunRxHandler rxhandler, TiFunTxHandler txhandler, TiFunEventHandler evolve )
{
    uint8 i;
    bool found = false;

    for ( i=0;i<CONFIG_NIO_NETLAYER_DISP_CAPACITY;i++)
    {
        if ( dispatcher->items[i].state == 0)
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        dispatcher->items[i].state = 1;
        dispatcher->items[i].proto_id = proto_id;
        dispatcher->items[i].object = object;
        dispatcher->items[i].rxhandler = rxhandler;
        dispatcher->items[i].txhandler = txhandler;
        dispatcher->items[i].evolve = evolve;
    }
}

void net_disp_unregister(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id )
{
    int i;

    for ( i=0;i<CONFIG_NIO_NETLAYER_DISP_CAPACITY;i++)
    {
        if(dispatcher->items[i].proto_id==proto_id)
        {
            dispatcher->items[i].state=0;
        }
    }
}






