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
#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_framequeue.h"
#include "../rtl/rtl_lightqueue.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/opennode2010/hal_cc2520.h"
#include "../hal/opennode2010/hal_led.h"
#include "../hal/opennode2010/hal_debugio.h"
#include "../hal/opennode2010/hal_assert.h"
#include "../hal/opennode2010/hal_timesync.h"
#include "../hal/opennode2010/hal_uart.h"
//#include "../hal/gainz/hpl_cpu.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"

#ifdef CONFIG_DEBUG
#include "../rtl/rtl_dumpframe.h"
#endif

#ifdef CONFIG_DYNA_MEMORY
TiNioAcceptor * nac_create( void * mem, TiFrameTxRxInterface * rxtx, uint8 rxque_capacity, 
	uint8 txque_capacity )
{
	char * buf;
	uint16 size = NIOACCEPTOR_HOPESIZE(rxque_capacity, txque_capacity);
	TiNioAcceptor * nac;
	
	buf = (char *)malloc( size );
	if (buf != NULL)
	{
		nac = nac_construct( buf, size );
		nac = nac_open( nac, rxtx, rxque_capacity, txque_capacity );
	}
	else
		nac = NULL;
		
	return nac;	
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void nac_free( TiNioAcceptor * nac)
{
	nac_close( nac );
	free( nac );
}
#endif

TiNioAcceptor * nac_construct( void * mem, uint16 size )
{
	TiNioAcceptor * nac = (TiNioAcceptor *)mem;
	memset( mem, 0x00, size );
	nac->memsize = size;
	return nac;
}

void nac_detroy( TiNioAcceptor * nac )
{
	nac_close( nac);
}

/**
 * @pre You have already initialized the rxque and txque before open the network 
 * 		acceptor object.
 * 
 * @warning: You must guarantee the memory block size is large enough to hold rxque
 *      and txque! Or else the application will access inviolate memory space and 
 *      the whole device may probably rebooted when accessing the TiNioAcceptor object.
 */
TiNioAcceptor * nac_open( TiNioAcceptor * nac, TiFrameTxRxInterface * rxtx, 
	uint8 rxque_capacity, uint8 txque_capacity )
{
	char * buf;
	
// #ifdef CONFIG_DEBUG
	svc_assert( nac->memsize >= NIOACCEPTOR_HOPESIZE(rxque_capacity,txque_capacity) );
// #endif	

	nac->state = 0;
	memmove( &(nac->rxtx), rxtx, sizeof(TiFrameTxRxInterface) );
	
	buf = (char*)nac + sizeof(TiNioAcceptor);
	nac->rxque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(rxque_capacity) );
	buf += FRAMEQUEUE_HOPESIZE(rxque_capacity);
	nac->txque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(txque_capacity) );
	buf += FRAMEQUEUE_HOPESIZE(txque_capacity);
    nac->rxframe = frame_open( buf, FRAMEQUEUE_ITEMSIZE, 0, 0, 0 ); 

	// If you encounter problems here, please check whether you have input enough
	// memory block to hold the full frame.
	hal_assert((nac->rxframe != NULL) && (nac->txque != NULL) && (nac->rxque != NULL));

#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    //nac->rxtx->setlistener( rxtx, nac_on_frame_arrived_listener );
    cc2520_setlistner( rxtx->provider,nac_on_frame_arrived_listener, nac )
#endif

	return nac;
}

void nac_close( TiNioAcceptor * nac )
{
	fmque_destroy( nac->rxque );
	fmque_destroy( nac->txque );
	frame_close( nac->rxframe );
}

void nac_set_timesync_adapter( TiNioAcceptor * nac, TiTimeSyncAdapter * tsync )
{
    nac->timesync = tsync;
}

TiFrameTxRxInterface * nac_rxtx_interface( TiNioAcceptor * nac )
{
	return &(nac->rxtx);
}

/** Return the sending queue(txque) inside acceptor */
TiFrameQueue * nac_txque( TiNioAcceptor * nac )
{
	return nac->txque;
}

/** Return the receiving queue(rxque) inside acceptor */
TiFrameQueue * nac_rxque( TiNioAcceptor * nac )
{
	return nac->rxque;
}

/**
 * Push the frame object into the sending queue(txque) and wait for the acceptor
 * to send it out. If the txque is full, then this function will return false.
 * 
 * @param nac TiNioAcceptor object
 * @param item TiFrame object to be sent
 * @return true if success and false is failed.
 */
intx nac_send( TiNioAcceptor * nac, TiFrame * item, uint8 option )
{
    TiFrameRxTxInterface * rxtx;
    uint8 first;
    char *pc;
    uint8 length;
    uint8 proto_id;
    
    if (nac->timesync != NULL)
    {
        frame_movehigher(item);
        pc = frame_startptr(item);
        if (pc[0] == TSYNC_PROTOCAL_ID)
        {
            hal_tsync_txhandler(nac->timesync, item, item, 0x00);
        }
        frame_movelower( item);
    }

    //the code above is added for hal_timesynchro on 2011/8/13
    //

	 rxtx = &(nac->rxtx);
	 first = frame_firstlayer(item);
	//return rxtx->send( rxtx->provider, frame_layerstartptr(item,first), frame_layercapacity(item,first), item->option );
	return rxtx->send( rxtx->provider, frame_startptr( item), frame_length( item), item->option );

	// old version
	// The old version maintains an txque. But the txque seems meaningless in this layer.
	/*
	if (fmque_pushback( nac->txque, item ) > 0)
	{  
		fmque_rear(nac->txque)->option = option;
		nac_evolve( nac,NULL );
		return frame_length(item);
	}
	else{
		return 0;
	}
	*/
}

/**
 * Pop the front frame from the receiving queue (rxque) and place it into the
 * item.
 */
intx nac_recv( TiNioAcceptor * nac, TiFrame * item , uint8 option )
{
	TiFrame * front;
    intx ret = 0;

	nac_evolve( nac,NULL );

#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    __disable_irq();//hal_enter_critical();
#endif

    front = fmque_front( nac->rxque );
	if (front != NULL)
	{
        //frame_totalcopyfrom( item, front );
		frame_copyfrom(item,front);//todo for testing
		fmque_popfront( nac->rxque );
        ret = frame_length(front);
	}
	else
		ret = 0;
#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    __enable_irq();//hal_leave_critical();
#endif

    return ret;
}

/** 
 * Check whether there's incoming frame pending in the transceiver object. If there
 * has, then get it back and place it at the tail of the rxque.  Also check whether
 * there's a frame in txque wait for sent. If has, then send it.
 */
void nac_evolve ( TiNioAcceptor * nac, TiEvent * event )
{   
	TiFrameRxTxInterface * rxtx = &(nac->rxtx);
	uint8 count, first;
	TiFrame * f;
    char *pc;

    if (!fmque_empty(nac->txque))
	{   

		f =  fmque_front( nac->txque );
		/*
		#ifdef CONFIG_DEBUG
		ieee802frame154_dump(f);
		#endif
		*/
		
        /*
		// @attention: The developer often forget to call frame_setlength(f) after
		// copy the data into the frame. It's the developer's responsibility to set
		// correct length value of the frame, or else frame_length(f) may probably 
		// return 0, which lead to sending failure.
		// 
		// the following line is only for testing by replacing frame_length(f)
		// with frame_capacity(f). 
		//
		// @todo: 2011.03
		*/
		first = frame_firstlayer(f);
		
		// @attention
		//	- Bug fixed. You needn't judge whether the rxtx->send() is successfully
		// or not. We assume all the frames in the txque should be sent immediately.
		rxtx->send( rxtx->provider, frame_layerstartptr(f,first), frame_layercapacity(f,first), f->option );
		fmque_popfront( nac->txque );
		
		/* 
		//todo
		//发送函数中的frame_layerstartptr(f,first)是有问题的，其中，first = f->firstlayer,
		count = rxtx->send( rxtx->provider, frame_layerstartptr(f,first), frame_layercapacity(f,first), f->option );
		if (count > 0)
		{   
			fmque_popfront( nac->txque );
		}
		*/
	}
		
	if (!fmque_full(nac->rxque ))
	{   
		
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
		__disable_irq();//hal_enter_critical();
		#endif
        
        /*
        idx = 0x00;
        if (fmque_applyback(nac->rxque, &idx))
        {
            tmpframe = fmque_getbuf(nac->rxque, idx);
            frame_open(tmpframe, fmque_datasize(fmque), 0, 0, 0);
            count = rxtx->recv( rxtx->provider, frame_startptr(f), frame_capacity(f), f->option );
            if (count > 0)
            {
                frame_setlength( f, count );
                frame_setcapacity( f, count );
                
                if (nac->timesync != NULL)
                {
                    // Since currently there's only one layer in the TiFrame object, we 
                    // cannot use frame_movehigher() to change the current layer index
                    // to the higher one. We had no better choice but use "+12" to skip 
                    // the 802.15.4 header only. 
                    // 
                    // @warning: The 802.15.4 header not always occupy 12 bytes! So you 
                    // must adapte the following code to your own system.
                    //
                    pc = frame_startptr(item) + 12;
                    
                    if (pc[0] == TSYNC_PROTOCAL_ID)
                    {
                        hal_tsync_rxhandler(nac->timesync, item, item, 0x00);
                    }
                }
            }
            else
                fmque_popback(nac->rxque);
        }
        */
        
        
		// @pre nac->rxframe must be initialized correctly.
		f = nac->rxframe;
		hal_assert( f != NULL );
	    frame_reset( f, 0, 0, 0 );    		
		count = rxtx->recv( rxtx->provider, frame_startptr(f), frame_capacity(f), f->option );
		if (count > 0)
		{
            if (nac->timesync != NULL)
            {
                // Since currently there's only one layer in the TiFrame object, we 
                // cannot use frame_movehigher() to change the current layer index
                // to the higher one. We had no better choice but use "+12" to skip 
                // the 802.15.4 header only. 
                // 
                // @warning: The 802.15.4 header not always occupy 12 bytes! So you 
                // must adapte the following code to your own system.
                //
                pc = frame_startptr(f) + 12;
                
                if (pc[0] == TSYNC_PROTOCAL_ID)
                {
                    hal_tsync_rxhandler(nac->timesync, f, f, 0x00);
                }
            }

            //the code above is added for hal_timesynchro on 2011/8/13.
            //

            frame_setlength( f, count );
            frame_setcapacity( f, count );
			fmque_pushback( nac->rxque, f );
		}
		
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
		__enable_irq();//hal_leave_critical();
		#endif
	}
}

/*
TiNioSession * nac_getcursession( TiNioAcceptor * nac, TiNioSession * session )
{
	session->rxque = nac->rxque;
	session->txque = nac->txque;
	session->option = 0x00;
	return session;
}
*/

/*
 * This listener can be called by the transceiver component when a new frame arrives.
 * So you can move the frame received by transceiver into the rxque inside TiNioAcceptor.
 * 
 * @attention: This function isn't mandatory. You can also call nac_evolve() to 
 * do this. The only difference between nac_evolve() and nac_on_frame_arrived_listener()
 * is that: the listener can be executed in interrupt mode, while the evolve()
 * cannot do this.
 */
#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
void nac_on_frame_arrived_listener( TiNioAcceptor * nac, TiEvent * e )
{
	uint8 count;
	TiFrameRxTxInterface * rxtx = nac->rxtx;
	TiFrame * f = nac->rxframe;
	
	__disable_irq();//hal_atomic_begin();
	count = rxtx->recv( rxtx->provider, frame_startptr(f), frame_capacity(f), 0x00 );
	if (count > 0)
	{
		frame_setlength( f, count );
        frame_setcapacity( f, count );
		fmque_pushback( nac->rxque, f );
	}
	__enable_irq();//hal_atomic_end();
}
#endif

