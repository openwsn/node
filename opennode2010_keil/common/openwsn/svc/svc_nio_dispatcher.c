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
 * @modified by Zhang Wei in 2010
 * @modified by Jiang Ridong in 2011
 */

#include "svc_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../rtl/rtl_configall.h"
#include "../rtl/rtl_foundation.h"
//#include "../hal/hal_debugio.h"
//#include "../hal/hal_configall.h"
//#include "../hal/hal_foundation.h"
#include "svc_foundation.h"
#include "svc_nio_aloha.h"
#include "svc_nio_dispatcher.h"

#define NIO_DISPA_STATE_IDLE        0
#define NIO_DISPA_STATE_SENDING     0
#define NIO_DISPA_STATE_RECVING     0

static uintx _nio_dispa_trysend(TiNioNetLayerDispatcher * dispacher );
static uintx _nio_dispa_tryrecv(TiNioNetLayerDispatcher * dispacher, uint16 * paddr, TiFrame * f, uint8 option );

TiNioNetLayerDispatcher * nio_dispa_construct( void * mem, uint16 memsize )
{
    memset( mem, 0x00, memsize );
    mem->memsize = memsize;
    return (TiNioNetLayerDispatcher*)mem;
}

void nio_dispa_destroy(TiNioNetLayerDispatcher * dispatcher)
{
    return;
}

TiNioNetLayerDispatcher * nio_dispa_open( TiNioNetLayerDispatcher * dispatcher, TiNodeBase * database, TiAloha *mac)
{
    // The memory block must be large enough to hold the dispatcher object.
    svc_assert( sizeof(TiNioNetLayerDispatcher) <= dispatcher->memsize );
    
    dispatcher->state = NIO_DISPA_STATE_IDLE;
    dispatcher->nbase = database;
    dispatcher->rxbuf = frame_open((char*)(&dispatcher->rxbuf_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 20, 102 );  // todo ?
    dispatcher->txbuf = frame_open((char*)(&dispatcher->txbak_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 20, 102 );  // todo ?
    dispatcher->fwbuf = frame_open((char*)(&dispatcher->fwbuf_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 20, 102 );  // todo ?
    dispatcher->mac = mac;
}

void nio_dispa_close(TiNioNetLayerDispatcher * dispacher)
{
    return;
}

/**
 * @return 
 *  - Postive value if the dispatcher object accept the frame. 
 */
uintx nio_dispa_send(TiNioNetLayerDispatcher * dispacher, uint16 addr, TiFrame * f, uint8 option) 
{
    uintx count = 0, ioresult;
    _TiNioNetLayerDispatcherItem * item;
    uint8 proto_id;

    svc_assert(frame_length(f) > 0);
    
    if ((frame_empty(dispatcher->txbuf) && frame_empty(dispatcher->fmbuf))
    {
        // payload[0] is the protocol id
        proto_id = (frame_startptr(f)[0]);
        item = nio_dispa_search(dispatcher, proto_id);
        if (item != NULL)
        {
            count = frame_length(f);
            f->address = addr;
            
            // txhandler should check whether dispatcher->txbuf and dispatcher->fwbuf
            // is empty and make appropriate processings. It cannot simply assume these 
            // buffer are always available for use.
            //
            // the result frame after processing has two directions:
            // - still in input buffer. which means it should be transfer to low
            //   level protocols
            // - in fwbuf, which means it should be process by other services in the 
            //   same layer.
            
            ioresult = item->txhandler(item->object, f, dispatcher->fwbuf, option);
            
            // if ioresult > 0, then the input frame is processed successfully
            // if ioresult ==0, then nothing to do with the frame. This is usually
            //      due to the buffer isn't available.
            // if ioresult < 0, then abnormal error encountered. means failure.
            
            if (ioresult <= 0)
            {
                count = ioresult;
            }
            else{
                if (!frame_empty(f))
                {
                    // the frame is still in its original buffer instead of the forward
                    // buffer, it should be placed into txbuf for later sending. We don't
                    // recommend call mac->send() directly here because the txhandler()
                    // maybe unefficient and the aloha_send() may return false. So the 
                    // upper layer must decide appropriate processing if the aloha
                    // is busy or failed.
                    //
                    // frame_movelower(f);
                    frame_totalcopyto(f, frame->txbuf);
                }
                else if (!frame_empty(dispatcher->fwbuf))
                {
                    // @todo we don't support packet forward in the same layer currently.
                    // svc_assert(0);
                }
            }
        }
    }
    
    _nio_dispa_trysend(dispatcher, f);
    
    return count;
        
/*    
    count = aloha_send(dispacher->mac,addr,f,option);
    return count;
*/    
}

uintx _nio_dispa_trysend(TiNioNetLayerDispatcher * dispacher)
{
    uintx ioresult = 0;
    
    if (!frame_empty(dispatcher->txbuf))
    {
        // aloha_send() returns
        //  > 0: the frame is successfullly accepted by the mac
        //  = 0: mac is busy. should retry
        //  < 0: I/O failure.
        // Even aloha_send() reports successful, the frame is still maybe lost in 
        // the processing. aloha_send() > 0 only means the frame is successfully
        // accepted by MAC. it doesn't means the frame has already been successfully
        // sent by the PHY layer.
        
        ioresult = aloha_send(dispacher->mac, dispatcher->txbuf, option);
        if (ioresult > 0)
            frame_clear(dispatcher->txbuf);
        else if (ioresult < 0)
            frame_clear(dispatcher->txbuf); // frame lost
    }
    
    return ioresult;
}


/**
 * @param f: The buffer containing the frame received. Attention the f buffer 
 *      should be opended or constructed some where. But this recv() function doesn't
 *      use its internal structure. It will override the buffer space.
 */
uintx nio_dispa_recv(TiNioNetLayerDispatcher * dispacher, uint16 * paddr, TiFrame * f, uint8 option)
{
    uint8 count = 0, ioresult;
    _TiNioNetLayerDispatcherItem * item;
    uint8 proto_id;
    uint16 addr;

    // the following assert helps to detect developing mistake. actually the frame
    // buffer can be larger than NIO_DISPA_FRAME_MEMSIZE.
    svc_assert(f->memsize == NIO_DISPA_FRAME_MEMSIZE);
    
    // if the dispatcher's internal rxbuf buffer is empty, then try to retrieve the
    // next frame from low level layer(the MAC layer) and process the frame using
    // rxhandler.
    
    count = _nio_dispa_tryrecv(dispacher, &addr, dispacher->rxbuf, option);
    if (count > 0)
    {
        // received something and rxhandle process successfully
    }

    if (!frame_empty(dispacher->rxbuf))
    {
        frame_totalcopyto(dispacher->rxbuf, f);
        count = frame_length(dispacher->rxbuf);
        *paddr = f->address;
        frame_clear(dispacher->rxbuf);
    }
    
    return count;
/*
    count =0;
    frame_reset( f,3,20,0);
    if ( !frame_empty( dispacher->rxbuf))
    {
       count = frame_totalcopyfrom( f,dispacher->rxbuf);
       frame_clear( dispacher->rxbuf);
    }
    return count;
*/    
}

uintx _nio_dispa_tryrecv(TiNioNetLayerDispatcher * dispacher, uint16 * paddr, TiFrame * f, uint8 option )
{
    uint8 count = 0, ioresult=0;
    _TiNioNetLayerDispatcherItem * item;
    uint8 proto_id;
    uint16 addr;

    // the following assert helps to detect developing mistake. actually the frame
    // buffer can be larger than NIO_DISPA_FRAME_MEMSIZE.
    svc_assert(f->memsize == NIO_DISPA_FRAME_MEMSIZE);
    
    // if the dispatcher's internal rxbuf buffer is empty, then try to retrieve the
    // next frame from low level layer(the MAC layer) and process the frame using
    // rxhandler.
    
    if (frame_empty(f) && frame_empty(dispacher->txbuf)))
    {
        // fwbuf here is uses as a temporary buffer only
        ioresult = aloha_recv(dispatcher->mac, paddr, f, 0x00);
        if (ioresult > 0)
        {
            proto_id = frame_startptr(f)[0];
            item = nio_dispa_search(dispatcher, proto_id);
            if (item != NULL)
            {
                count = frame_length(f);
            
                // rxhandler should check whether dispatcher->txbuf and dispatcher->fwbuf
                // is empty and make appropriate processings. It cannot simply assume these 
                // buffer are always available for use.
                //
                // the result frame after processing has two directions:
                // - still in input buffer. which means it should be transfer to high
                //   level protocols
                // - in fwbuf, which means it should be process by other services in the 
                //   same layer. (usually forwarding)
                
                ioresult = item->rxhandler(item->object, f, dispatcher->txbuf, option);
                if (ioresult <= 0)
                {
                    frame_clear(f);
                    ioresult = 0;
                }
            }
        }
        else{
            // If we cannot find an component reponsible to processing the frame
            // then we had to give up it as if we had never receive it.
            frame_clear(f);
            ioresult = 0;
        }
    }
    
    return ioresult;
}

void nio_dispa_evolve(void* object, TiEvent * e)
{
    uint8 count;
    char *payload;
    uint8 proto_id;
    int i;
    TiNioNetLayerDispatcher * dispa = ( TiNioNetLayerDispatcher *)object;
    
    _nio_dispa_trysend(dispacher);
    _nio_dispa_tryrecv(dispacher, &(f->rxbuf->address), dispatcher->rxbuf, option);
    
    #ifdef NIO_DISPA_EVOLVE_DEBUG_WITHOUT_RXTX
    frame_clear(dispatcher->rxbuf);
    #endif

/*    
    switch (dispatcher->state)
    {
    case NIO_DISPA_STATE_IDLE:
        send txbuf
    
        _nio_dispa_trysend(dispa);
        if (!frame_empty(dispacher->rxbuf))
        {
            frame_totalcopyto(dispacher->rxbuf, f);
            count = frame_length(dispacher->rxbuf);
            frame_clear(dispacher->rxbuf);
        }
        break;

    case NIO_DISPA_STATE_SENDING:
    case NIO_DISPA_STATE_RECVING:
    default:
        dispatcher->state = NIO_DISPA_STATE_IDLE;
        break;
    }

    count = 0;
    
    if ( frame_empty( dispatcher->rxbuf))
    {
        frame_reset( dispatcher->rxbuf,3,20,0);
        count = aloha_recv(dispatcher->mac,dispatcher->rxbuf,0x00);
        if (count > 0)
        {
            payload = frame_startptr(dispatcher->rxbuf);
            proto_id = payload[0]&0x02;//if the second bit is set, then the protocal is the ndp.
            for ( i=0;i< CONFIG_NIO_NETLAYER_DISP_CAPACITY;i++)
            {
                if ( (dispatcher->items[i].proto_id&0x02) == proto_id)
                {
                    break;
                }
            }

            if ( i< CONFIG_NIO_NETLAYER_DISP_CAPACITY)
            {
                if ( dispatcher->items[i].rxhandler( dispatcher->items[i].object,dispatcher->rxbuf,dispatcher->rxbake,0x00)>0)
                {
                    if ( !frame_empty( dispatcher->rxbake))
                    {
                        frame_totalcopyfrom( dispatcher->rxbuf,dispatcher->rxbake);
                        frame_clear( dispatcher->rxbake);
                    }
                }
                else
                {
                    frame_clear( dispatcher->rxbuf);
                    frame_clear( dispatcher->rxbake);
                }
            }
               
        }
    }
*/
    for (i=0; i<CONFIG_NIO_NETLAYER_DISP_CAPACITY; i++)
    {
        if (dispatcher->items[i].state > 0)
        {
            dispatcher->items[i].evolve(dispatcher->items[i].object, NULL);
        }
    }
}

/**
 * @param proto_id: Protocol Identifier. 
 */
bool nio_dispa_register(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id, void * object, 
    TiFunRxHandler rxhandler, TiFunTxHandler txhandler, TiFunEventHandler evolve)
{
    uint8 i;
    bool found = false;

    for (i=0; i<CONFIG_NIO_NETLAYER_DISP_CAPACITY; i++)
    {
        if (dispatcher->items[i].state == 0)
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
    
    return found;
}

bool nio_disp_unregister(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id)
{
    int i;
    bool found = false;

    for (i=0; i<CONFIG_NIO_NETLAYER_DISP_CAPACITY; i++)
    {
        if (dispatcher->items[i].state!=0) && (dispatcher->items[i].proto_id==proto_id)
        {
            dispatcher->items[i].state = 0;
            dispatcher->items[i].proto_id = 0;
            found = true;
            break;
        }
    }
    
    return found;
}



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







