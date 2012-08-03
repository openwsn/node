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

 * @modified by Shi Zhirong  2012.7.23
 */

#include "svc_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../rtl/rtl_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../hal/hal_uart.h"
#include "svc_foundation.h"
#include "svc_nio_csma.h"
#include "svc_nio_dispatcher"
#include "svc_nodebase.h"
#include "../osx/osx_tlsche.h"

#define NIO_DISPA_STATE_IDLE        0
#define NIO_DISPA_STATE_SENDING     0
#define NIO_DISPA_STATE_RECVING     0

static uintx _nio_dispa_trysend(TiNioNetLayerDispatcher * dispacher );
static uintx _nio_dispa_tryrecv(TiNioNetLayerDispatcher * dispacher, __packed uint16 * paddr, TiFrame * f, uint8 option );
static intx _nio_dispa_invoke_txhandler(TiNioNetLayerDispatcher * dispacher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option);
static intx _nio_dispa_invoke_rxhandler(TiNioNetLayerDispatcher * dispacher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option);
static _TiNioNetLayerDispatcherItem * _nio_dispa_search(TiNioNetLayerDispatcher * dispacher, uint8 proto_id );
      

TiNioNetLayerDispatcher * nio_dispa_construct( void * mem, uint16 memsize )
{
    hal_assert( sizeof(TiNioNetLayerDispatcher) <= memsize );
    memset( mem, 0x00, memsize );
    return (TiNioNetLayerDispatcher*)mem;
}

void nio_dispa_destroy(TiNioNetLayerDispatcher * dispatcher)
{
    return;
}

//TiNioNetLayerDispatcher * nio_dispa_open( TiNioNetLayerDispatcher * dispatcher, TiNodeBase * database, TiCsma *mac)
TiNioNetLayerDispatcher * nio_dispa_open( TiNioNetLayerDispatcher * dispatcher, TiNodeBase * database, TiCsma *mac,TiOsxTimeLineScheduler * scheduler)
{
    // The memory block must be large enough to hold the dispatcher object.
    //svc_assert( sizeof(TiNioNetLayerDispatcher) <= dispatcher->memsize );//non memsize
    dispatcher->state = NIO_DISPA_STATE_IDLE;
    dispatcher->nbase = database;
    dispatcher->rxbuf = frame_open((char*)(&dispatcher->rxbuf_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
    dispatcher->txbuf = frame_open((char*)(&dispatcher->txbak_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
    dispatcher->fwbuf = frame_open((char*)(&dispatcher->fwbuf_memory), NIO_DISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
    dispatcher->mac = mac;
	dispatcher->scheduler=scheduler;
    return dispatcher;
}



void nio_dispa_close(TiNioNetLayerDispatcher * dispacher)
{
    return;
}

/**
 * @return 
 *  - Postive value if the dispatcher object accept the frame. 
 */
uintx nio_dispa_send(TiNioNetLayerDispatcher * dispatcher, uint16 addr, TiFrame * f, uint8 option) 
{
    uintx count = 0, ioresult;
    _TiNioNetLayerDispatcherItem * item;
    uint8 proto_id;

    svc_assert(frame_length(f) > 0);	
    
    if (frame_empty(dispatcher->txbuf) && frame_empty(dispatcher->fwbuf))
    {
        // payload[0] is the protocol id
        // proto_id = (frame_startptr(f)[0]);
        //item = _nio_dispa_search(dispatcher, proto_id);
        item = &dispatcher->items[0];
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
                if (!frame_empty(dispatcher->fwbuf))
                {
                    // the frame is still in its original buffer instead of the forward
                    // buffer, it should be placed into txbuf for later sending. We don't
                    // recommend call mac->send() directly here because the txhandler()
                    // maybe unefficient and the csma_send() may return false. So the 
                    // upper layer must decide appropriate processing if the csma
                    // is busy or failed.
                    //
                    // frame_movelower(f);
                    frame_totalcopyto(dispatcher->fwbuf, dispatcher->txbuf);
                    frame_totalclear( dispatcher->fwbuf);
                }
                else if (!frame_empty(f))
                {
                    // @todo we don't support packet forward in the same layer currently.
                    // svc_assert(0);
                }
            }
        }
    }
    
    _nio_dispa_trysend(dispatcher);
    
    return count;
        
/*    
    count = csma_send(dispacher->mac,addr,f,option);
    return count;
*/    
}

uintx _nio_dispa_trysend(TiNioNetLayerDispatcher * dispatcher)
{
    intx ioresult = 0;
    uintx option;
    
    if (!frame_empty(dispatcher->txbuf))
    {
        // csma_send() returns
        //  > 0: the frame is successfullly accepted by the mac
        //  = 0: mac is busy. should retry
        //  < 0: I/O failure.
        // Even csma_send() reports successful, the frame is still maybe lost in 
        // the processing. csma_send() > 0 only means the frame is successfully
        // accepted by MAC. it doesn't means the frame has already been successfully
        // sent by the PHY layer.
        option = dispatcher->txbuf->option;
        ioresult = csma_send(dispatcher->mac,dispatcher->txbuf->address, dispatcher->txbuf, option);
        if ( CSMA_IORET_SUCCESS(ioresult) || CSMA_IORET_ERROR_NOACK || CSMA_IORET_ERROR_CHANNEL_BUSY )
		{
			frame_clear(dispatcher->txbuf);				
		}
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
}

uintx _nio_dispa_tryrecv(TiNioNetLayerDispatcher * dispatcher, __packed uint16 * paddr, TiFrame * f, uint8 option )
{
    
    _TiNioNetLayerDispatcherItem * item;
    uint8 proto_id;
    uint16 addr;
    uint8 count = 0;
    uint8 ioresult=0;

    // the following assert helps to detect developing mistake. actually the frame
    // buffer can be larger than NIO_DISPA_FRAME_MEMSIZE.
////////////////////////////////////////////    svc_assert(f->memsize == NIO_DISPA_FRAME_MEMSIZE);
    
    // if the dispatcher's internal rxbuf buffer is empty, then try to retrieve the
    // next frame from low level layer(the MAC layer) and process the frame using
    // rxhandler.
    if (frame_empty(f) )
    {
        // fwbuf here is uses as a temporary buffer only
        ioresult = csma_recv(dispatcher->mac,  f, 0x00);
        if (ioresult > 0)
        {
            proto_id = frame_startptr(f)[0];
            item = _nio_dispa_search(dispatcher, proto_id);
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

intx _nio_dispa_invoke_txhandler(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option)
{
    _TiNioNetLayerDispatcherItem * item;
    intx ioresult = 0;
    
    item = _nio_dispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        ioresult = item->txhandler(item->object, frame, dispatcher->fwbuf, option);
        item->evolve(item->object, NULL);
    }
    return ioresult; 
}

intx _nio_dispa_invoke_rxhandler(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option)
{
    _TiNioNetLayerDispatcherItem * item;
    intx ioresult = 0;
    
    item = _nio_dispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        ioresult = item->rxhandler(item->object, frame, dispatcher->fwbuf, option);
        item->evolve(item->object, NULL);
    }
    return ioresult; 
}

void nio_dispa_evolve(void* object, TiEvent * e)
{
    uint8 count;
    char *payload;
    uint8 proto_id;
    int i;
    TiNioNetLayerDispatcher * dispatcher = ( TiNioNetLayerDispatcher *)object;
    
    _nio_dispa_trysend(dispatcher);
    _nio_dispa_tryrecv(dispatcher, &(dispatcher->rxbuf->address), dispatcher->rxbuf, 0);
    
    #ifdef NIO_DISPA_EVOLVE_DEBUG_WITHOUT_RXTX
    frame_clear(dispatcher->rxbuf);
    #endif

	//JOE 0709
	if(dispatcher->scheduler!=NULL)
	{
		osx_tlsche_taskspawn(dispatcher->scheduler, nio_dispa_evolve,dispatcher,1,0,0);
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

bool nio_dispa_register_default(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id, void * object, 
    TiFunRxHandler rxhandler, TiFunTxHandler txhandler, TiFunEventHandler evolve)
{
    uint8 i;
    bool found = false;
	
	if( dispatcher->items[0].state == 0)
	{
		dispatcher->items[0].state = 1;
        dispatcher->items[0].proto_id = proto_id;
        dispatcher->items[0].object = object;
        dispatcher->items[0].rxhandler = rxhandler;
        dispatcher->items[0].txhandler = txhandler;
        dispatcher->items[0].evolve = evolve;
		found = true;
	}
	else
	{
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
			dispatcher->items[i].proto_id = dispatcher->items[0].proto_id;
			dispatcher->items[i].object = dispatcher->items[0].object;
			dispatcher->items[i].rxhandler = dispatcher->items[0].rxhandler;
			dispatcher->items[i].txhandler = dispatcher->items[0].txhandler;
			dispatcher->items[i].evolve = dispatcher->items[0].evolve;
			
			dispatcher->items[0].proto_id = proto_id;
			dispatcher->items[0].object = object;
			dispatcher->items[0].rxhandler = rxhandler;
			dispatcher->items[0].txhandler = txhandler;
			dispatcher->items[0].evolve = evolve;
		}
	}
	return found;
}

bool nio_disp_unregister(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id)
{
    _TiNioNetLayerDispatcherItem * item;
    
    item = _nio_dispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        item->state = 0;
        item->proto_id = 0;
    }
    return (item != NULL);
}

_TiNioNetLayerDispatcherItem * _nio_dispa_search(TiNioNetLayerDispatcher * dispatcher, uint8 proto_id )
{
    _TiNioNetLayerDispatcherItem * item = NULL;
    uint8 i;
    
    for (i=0; i<CONFIG_NIO_NETLAYER_DISP_CAPACITY; i++)
    {
        item = &(dispatcher->items[i]);
        if ((dispatcher->items[i].state != 0) && (dispatcher->items[i].proto_id == proto_id))
        {
            break;
        }
    }
    return item;
}

