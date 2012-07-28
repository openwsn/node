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
#include "../hal/hal_uart.h"
#include "svc_foundation.h"
#include "svc_nio_dispatcher.h"
#include "svc_nodebase.h"
#include "svc_nio_transdipatcher.h"

#define NIO_TRANSDISPA_STATE_IDLE        0
#define NIO_TRANSDISPA_STATE_SENDING     1
#define NIO_TRANSDISPA_STATE_RECVING     2

static uintx _nio_transdispa_trysend(TiNioTransLayerDispatcher * dispacher );
static uintx _nio_transdispa_tryrecv(TiNioTransLayerDispatcher * dispacher, __packed uint16 * paddr, TiFrame * f, uint8 option );
static intx _nio_transdispa_invoke_txhandler(TiNioTransLayerDispatcher * dispacher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option);
static intx _nio_transdispa_invoke_rxhandler(TiNioTransLayerDispatcher * dispacher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option);
static _TiNioTransLayerDispatcherItem * _nio_transdispa_search(TiNioTransLayerDispatcher * dispacher, uint8 proto_id );
      

TiNioTransLayerDispatcher * nio_transdispa_construct( void * mem, uint16 memsize )
{
    hal_assert( sizeof(TiNioTransLayerDispatcher) <= memsize );
    memset( mem, 0x00, memsize );
    return (TiNioTransLayerDispatcher*)mem;
}

void nio_transdispa_destroy(TiNioTransLayerDispatcher * dispatcher)
{
    return;
}

TiNioTransLayerDispatcher * nio_transdispa_open( TiNioTransLayerDispatcher * dispatcher, TiNodeBase * database, TiNioNetLayerDispatcher *netdispa)
{
    dispatcher->state = NIO_TRANSDISPA_STATE_IDLE;
    dispatcher->nbase = database;
//    dispatcher->rxbuf = frame_open((char*)(&dispatcher->rxbuf_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
//    dispatcher->txbuf = frame_open((char*)(&dispatcher->txbak_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
//    dispatcher->fwbuf = frame_open((char*)(&dispatcher->fwbuf_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 3, 30, 92 );  // todo ?
    dispatcher->rxbuf = frame_open((char*)(&dispatcher->rxbuf_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 4, 40, 82 );  // todo ?
    dispatcher->txbuf = frame_open((char*)(&dispatcher->txbak_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 4, 40, 82 );  // todo ?
    dispatcher->fwbuf = frame_open((char*)(&dispatcher->fwbuf_memory), NIO_TRANSDISPA_FRAME_MEMSIZE, 4, 40, 82 );  // todo ?
	dispatcher->netdispa=netdispa;
    return dispatcher;

}

void nio_transdispa_close(TiNioTransLayerDispatcher * dispacher)
{
    return;
}

/**
 * @return 
 *  - Postive value if the dispatcher object accept the frame. 
 */
uintx nio_transdispa_send(TiNioTransLayerDispatcher * dispatcher, uint16 addr, TiFrame * f, uint8 option)
{
    uintx count = 0, ioresult;
    _TiNioTransLayerDispatcherItem * item;
    uint8 proto_id;

    svc_assert(frame_length(f) > 0);
    
    if (frame_empty(dispatcher->txbuf) && frame_empty(dispatcher->fwbuf))
    {
        //payload[0] is the protocol id
        //proto_id = (frame_startptr(f)[0]);
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
                    // maybe unefficient and the aloha_send() may return false. So the 
                    // upper layer must decide appropriate processing if the aloha
                    // is busy or failed.
                    //
                    // frame_movelower(f);
                    frame_totalcopyto(dispatcher->fwbuf, dispatcher->txbuf);
                    dispatcher->txbuf->address = addr;
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
    
    _nio_transdispa_trysend(dispatcher);
    
    return count;
        
/*    
    count = aloha_send(dispacher->mac,addr,f,option);
    return count;
*/    
}

uintx _nio_transdispa_trysend(TiNioTransLayerDispatcher * dispatcher )
{
    uintx ioresult = 0;
    uintx option;
    
    if (!frame_empty(dispatcher->txbuf))	   //????????todo
    {
        // aloha_send() returns
        //  > 0: the frame is successfullly accepted by the mac
        //  = 0: mac is busy. should retry
        //  < 0: I/O failure.
        // Even aloha_send() reports successful, the frame is still maybe lost in 
        // the processing. aloha_send() > 0 only means the frame is successfully
        // accepted by MAC. it doesn't means the frame has already been successfully
        // sent by the PHY layer.
        option = dispatcher->txbuf->option;
		ioresult= nio_dispa_send(dispatcher->netdispa, dispatcher->txbuf->address, dispatcher->txbuf, option);
        //ioresult = aloha_send(dispatcher->mac,dispatcher->txbuf->address, dispatcher->txbuf, option);
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
uintx nio_transdispa_recv(TiNioTransLayerDispatcher * dispacher, uint16 * paddr, TiFrame * f, uint8 option)
{
    uint8 count = 0, ioresult;
    _TiNioTransLayerDispatcherItem * item;
    uint8 proto_id;
    uint16 addr;

    // the following assert helps to detect developing mistake. actually the frame
    // buffer can be larger than NIO_DISPA_FRAME_MEMSIZE.
    svc_assert(f->memsize == NIO_TRANSDISPA_FRAME_MEMSIZE);
    
    // if the dispatcher's internal rxbuf buffer is empty, then try to retrieve the
    // next frame from low level layer(the MAC layer) and process the frame using
    // rxhandler.
    
    count = _nio_transdispa_tryrecv(dispacher, &addr, dispacher->rxbuf, option);
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

uintx _nio_transdispa_tryrecv(TiNioTransLayerDispatcher * dispatcher, __packed uint16 * paddr, TiFrame * f, uint8 option )
{
    
    _TiNioTransLayerDispatcherItem * item;
    uint8 proto_id;
    uint16 addr;
    uint8 count = 0;
    uint8 ioresult=0;

    // the following assert helps to detect developing mistake. actually the frame
    // buffer can be larger than NIO_DISPA_FRAME_MEMSIZE.
    svc_assert(f->memsize == NIO_TRANSDISPA_FRAME_MEMSIZE);
    
    // if the dispatcher's internal rxbuf buffer is empty, then try to retrieve the
    // next frame from low level layer(the MAC layer) and process the frame using
    // rxhandler.
    if (frame_empty(f) )
    {
        // fwbuf here is uses as a temporary buffer only
        //ioresult = aloha_recv(dispatcher->mac,  f, 0x00);
		ioresult = nio_dispa_recv(dispatcher->netdispa, &addr, f, 0x00);///////////////////
        if (ioresult > 0)
        {
            proto_id = frame_startptr(f)[0];
            item = _nio_transdispa_search(dispatcher, proto_id);
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

intx _nio_transdispa_invoke_txhandler(TiNioTransLayerDispatcher * dispatcher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option)
{
    _TiNioTransLayerDispatcherItem * item;
    intx ioresult = 0;
    
    item = _nio_transdispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        ioresult = item->txhandler(item->object, frame, dispatcher->fwbuf, option);
        item->evolve(item->object, NULL);
    }
    return ioresult; 
}

intx _nio_transdispa_invoke_rxhandler(TiNioTransLayerDispatcher * dispatcher, uint8 proto_id, 
    TiFrame * frame, TiFrame * fwbuf, uint8 option)
{
    _TiNioTransLayerDispatcherItem * item;
    intx ioresult = 0;
    
    item = _nio_transdispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        ioresult = item->rxhandler(item->object, frame, dispatcher->fwbuf, option);
        item->evolve(item->object, NULL);
    }
    return ioresult; 
}

void nio_transdispa_evolve(void* object, TiEvent * e)
{
    uint8 count;
    char *payload;
    uint8 proto_id;
    int i;
    TiNioTransLayerDispatcher * dispatcher = ( TiNioTransLayerDispatcher *)object;
    
    _nio_transdispa_trysend(dispatcher);
    _nio_transdispa_tryrecv(dispatcher, &(dispatcher->rxbuf->address), dispatcher->rxbuf, 0);
    
    #ifdef NIO_DISPA_EVOLVE_DEBUG_WITHOUT_RXTX
    frame_clear(dispatcher->rxbuf);
    #endif


    for (i=0; i<CONFIG_NIO_TRANSLAYER_DISP_CAPACITY; i++)
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
bool nio_transdispa_register(TiNioTransLayerDispatcher * dispatcher, uint8 proto_id, void * object, 
    TiFunRxHandler rxhandler, TiFunTxHandler txhandler, TiFunEventHandler evolve)
{
    uint8 i;
    bool found = false;

    for (i=0; i<CONFIG_NIO_TRANSLAYER_DISP_CAPACITY; i++)
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

bool nio_transdisp_unregister(TiNioTransLayerDispatcher * dispatcher, uint8 proto_id)
{
    _TiNioTransLayerDispatcherItem * item;
    
    item = _nio_transdispa_search(dispatcher, proto_id);
    if (item != NULL)
    {
        item->state = 0;
        item->proto_id = 0;
    }
    
    return (item != NULL);
}

_TiNioTransLayerDispatcherItem * _nio_transdispa_search(TiNioTransLayerDispatcher * dispatcher, uint8 proto_id )
{
    _TiNioTransLayerDispatcherItem * item = NULL;
    uint8 i;
    
    for (i=0; i<CONFIG_NIO_TRANSLAYER_DISP_CAPACITY; i++)
    {
        item = &(dispatcher->items[i]);
        if ((dispatcher->items[i].state != 0) && (dispatcher->items[i].proto_id == proto_id))
        {
            break;
        }
    }
    
    return item;
}






