/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
#include <string.h>
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_framequeue.h"
#include "../rtl/rtl_lightqueue.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/hal_cpu.h"
//#include "../hal/hal_cc2520.h" 
#include "../hal/hal_led.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_timesync.h"
#include "../hal/hal_uart.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"

#ifdef CONFIG_DEBUG
#include "../rtl/rtl_dumpframe.h"
#endif

static void _nac_tryrecv( TiNioAcceptor * nac );
static int _nac_isack( TiFrame * f );


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
 * @pre You have already call nac_construct() on specified memory block.
 * 
 * @warning: You must guarantee the memory block size is large enough to hold rxque
 *      and txque! Or else the application will access inviolate memory space and 
 *      the whole device may probably rebooted when accessing the TiNioAcceptor object.
 * 
 * @modified by JiangRidong in 2011.07
 *  - The acceptor object give up txque because it's useless.
 */
TiNioAcceptor * nac_open( TiNioAcceptor * nac, TiFrameTxRxInterface * rxtx, 
	uint8 rxque_capacity, uint8 txque_capacity )
{
	char * buf;
	
#ifdef CONFIG_DEBUG
	svc_assert(nac->memsize >= NIOACCEPTOR_HOPESIZE(rxque_capacity,txque_capacity));
    svc_assert((rxque_capacity > 0) && (txque_capacity >= 0));
#endif	

	nac->state = 0;
    nac->rxtx = rxtx;
	
	buf = (char*)nac + sizeof(TiNioAcceptor);
	nac->rxque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(rxque_capacity) );
	buf += FRAMEQUEUE_HOPESIZE(rxque_capacity);
    
    nac->txque = NULL;
	// nac->txque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(txque_capacity) );
	// buf += FRAMEQUEUE_HOPESIZE(txque_capacity);
    
    nac->ackbuf = frame_open( buf, FRAME_HOPESIZE(I802F154_ACK_FRAME_SIZE), 0, 0, 0 ); 
    nac->timesync = NULL; // @todo

#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    //nac->rxtx->setlistener( rxtx->provider, nac_on_frame_arrived_listener, nac );
    cc2520_setlistener( rxtx->provider, nac_on_frame_arrived_listener, nac );
#endif

	return nac;
}

void nac_close( TiNioAcceptor * nac )
{
    if (nac != NULL)
    {
        //cc2520_setlistener( nac->rxtx->provider, NULL, NULL );
        nac->rxtx->setlistener( rxtx, NULL );
        fmque_destroy( nac->rxque );
        // fmque_destroy( nac->txque );
        frame_close( nac->ackbuf );
    }
}

void nac_set_timesync_adapter( TiNioAcceptor * nac, TiTimeSyncAdapter * tsync )
{
    nac->timesync = tsync;
}

TiFrameTxRxInterface * nac_rxtx_interface( TiNioAcceptor * nac )
{
	return nac->rxtx;
}

/** 
 * Return the sending queue(txque) inside acceptor. You can manipulate this queue
 * directly in some program. 
 * 
 * Recommend to use nac_send() instead.
 */
TiFrameQueue * nac_txque( TiNioAcceptor * nac )
{
	return nac->txque;
}

/** 
 * Return the receiving queue(rxque) inside acceptor.
 * Recommend to use nac_recv() instead.
 */
TiFrameQueue * nac_rxque( TiNioAcceptor * nac )
{
	return nac->rxque;
}

/** 
 * Push the frame object into the sending queue(txque) and wait for the acceptor
 * to send it out. If the txque is full, then this function will return false.
 * Generally, the acceptor will send the frame immediately without delay.
 * 
 * @attention Any frame placed into txque will be sent immediatly without delay. 
 * So in the new version of TiNioAcceptor, the "txque" is eliminated. nac_send()
 * will call the transceiver's send() method or broadcast() method directly.
 * 
 * @param nac TiNioAcceptor object
 * @param item TiFrame object to be sent
 *
 * @return Data length successfully sent.
 */
intx nac_send( TiNioAcceptor * nac, TiFrame * item, uint8 option )
{
    char *pc;
	TiFrameTxRxInterface * rxtx = nac->rxtx;
    
    // Check whether this is an time synchronization REQUEST frame. If it is, then
    // the hal_timesync module will place the current local time into this frame.
    // 
    if (nac->timesync != NULL)
    {
        frame_movehigher(item);
        pc = frame_startptr(item);
        if (pc[0] == TSYNC_PROTOCAL_ID)
        {
            hal_tsync_txhandler(nac->timesync, item, item, 0x00);
        }
        frame_movelower(item);
    }

    // @modified by zhangwei in 2011.05
    // - The txque is unused to avoid ambiguous understanding. Consider everything
    //   in the txque should be sent immediately without any delay, we simply call
    //   the transceiver's send() method to send the frame out.
    //
	return rxtx->send( rxtx->provider, frame_startptr(item), frame_length(item), item->option );
}

/** 
 * Retrieve an frame received inside rxque into frame object. 
 * @return Frame length.
 */
intx nac_recv( TiNioAcceptor * nac, TiFrame * item , uint8 option )
{
	TiFrame * front;
    intx retval = 0;

	nac_evolve(nac, NULL);

#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    hal_enter_critical();
#endif

    if (!frame_empty(nac->ackbuf))
    {
        frame_totalcopyto(nac->ackbuf, item);
        retval = frame_length(nac->ackbuf);
        frame_clear(nac->ackbuf);
    }
    else{
        front = fmque_front( nac->rxque );
        if (front != NULL)
        {
            frame_totalcopyto(front, item);
            fmque_popfront( nac->rxque );
            retval = frame_length(item);
        }
        else
            retval = 0;
    }
    
#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
    hal_leave_critical();
#endif

    return retval;
}

/** 
 * Check whether there's incoming frame pending in the transceiver object. If there
 * has, then get it back and place it at the tail of the rxque.  Also check whether
 * there's a frame in txque wait for sent. If has, then send it.
 */
void nac_evolve ( TiNioAcceptor * nac, TiEvent * event )
{   
	TiFrameRxTxInterface * rxtx = nac->rxtx;
	TiFrame * f;
/*
    if ((nac->txque != NULL) && (!fmque_empty(nac->txque)))
	{   
		f =  fmque_front( nac->txque );

		// #ifdef CONFIG_DEBUG
		// ieee802frame154_dump(f);
		// #endif
		
		// @attention: The developer often forget to call frame_setlength(f) after
		// copy the data into the frame. It's the developer's responsibility to set
		// correct length value of the frame, or else frame_length(f) may probably 
		// return 0, which lead to sending failure.
		// 
		// the following line is only for testing by replacing frame_length(f)
		// with frame_capacity(f). 
		//
		// @attention
		//	- Bug fixed. You needn't judge whether the rxtx->send() is successfully
		// or not. Since the sending process is in evolve() function, we had to assume 
        // the frame be sent successfully, because we have no better idea to pass
        // information to the upper layer yet.
        //
        // @todo Maybe an listener can help us to solve the above problem.
        
		frame_movelowest(f);
		rxtx->send( rxtx->provider, frame_startptr(f), frame_capacity(f), f->option );
		fmque_popfront( nac->txque );
		
		// @obsolete Old version source code is as the following
        // The problem is that if the frame failed to sending (count < 0), then it
        // will not be removed from the txque, so the failed frame will block the
        // whole sending process from now on.
        //
        // Actually, you should always do popfront no matter the frame is sent 
        // successfully or not. If it's failed, then the above layer should do with 
        // it. The acceptor needn't do more. But it do need some mechanism to pass
        // this result to upper layers.
        // 
		// count = rxtx->send( rxtx->provider, frame_layerstartptr(f,first), frame_layercapacity(f,first), f->option );
		// if (count > 0)
		// {   
		//	fmque_popfront( nac->txque );
		// }
	}
*/    
		
	if (!fmque_full(nac->rxque))
	{   
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
        hal_enter_critical();
		#endif
        
        _nac_tryrecv(nac);
        
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
        hal_leave_critical();
		#endif
	}
    
    nac->rxtx->evolve( nac->rxtx, NULL );
}

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
    _nac_tryrecv(nac);
}
#endif

void _nac_tryrecv( TiNioAcceptor * nac )
{
	TiFrameRxTxInterface * rxtx = nac->rxtx;
    char * buf;
	TiFrame * f;
	uint8 idx = 0;
	intx count;
    char *pc;

    // @attention
    // if the RX queue is full, then we had to lost the incoming frame or drop an 
    // frame in the RX queue. Currently, we prefer keep the existing frames and 
    // let the low level software to drop the frame if this really happens.
    //
    // if (fmque_full(nac->rxque))
    // {
    //     frmque_popfront(nac->rxque);
    // }
    
	if (!fmque_full(nac->rxque))
	{   
        if (fmque_applyback(nac->rxque, &idx))	  //JOE 从rxque中申请一个空间，idx为其id号
        {
            buf = fmque_getbuf(nac->rxque, idx);	  //将idx索引号的空间给buf
            svc_assert(buf != NULL);
            svc_assert(fmque_datasize(nac->rxque) == FRAMEQUEUE_ITEMSIZE);
            // f = frame_open(buf, fmque_datasize(nac->rxque), 0, 0, 0);
            f = frame_open(buf, FRAMEQUEUE_ITEMSIZE, 0, 0, 0);						   //格式化buf到f
            count = rxtx->recv( rxtx->provider, frame_startptr(f), frame_capacity(f), f->option );	//接收到f	  
            if (count > 0)
            {
                frame_setlength(f, count);

                // If this frame is the ACK frame, then move it to a independent 
                // buffer for ACK frame only. And then release the current queue item.
                //
                if (_frame_isack(f))
                {
                    frame_clear(f);
                    frame_moveto(f, nac->ackbuf);
                    fmque_popback(nac->rxque);
                }
                else if (nac->timesync != NULL)
                {
                    // Since currently there's only one layer in the TiFrame object, we 
                    // cannot use frame_movehigher() to change the current layer index
                    // to the higher one. We had no better choice but use "+12" to skip 
                    // the 802.15.4 header only. 
                    // 
                    // @warning: The 802.15.4 header not always occupy 12 bytes! So you 
                    // must adapte the following code to your own system.
                    //
                    buf = frame_startptr(f) + 12;
                    
                    if (buf[0] == TSYNC_PROTOCAL_ID)
                    {
                        hal_tsync_rxhandler(nac->timesync, f, f, 0x00);
                    }
                }
            }
            else{
                fmque_poprear(nac->rxque);		  //接收不成功 释放该区域
            }
        }
        
    }
}

int _nac_isack( TiFrame * f )
{
    int retval = 0;
    
    ptr = frame_startptr(frame);
    if (ptr != NULL)
    {
        // get the pointer to the frame control field according to 802.15.4 frame format
        // we need to check whether the current frame is a DATA type frame.
		// only the DATA type frame will be transfered to upper layers. The other types, 
        // such as COMMAND, BEACON and ACK will be ignored here.
		// buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
        //
		fcf = I802F154_MAKEWORD(ptr[2], ptr[1]);
        if (FCF_FRAMETYPE(fcf) == FCF_FRAMETYPE_ACK)
            retval = 1;
    }
    
    return retval;
}

//------------------------------------------------------------------------------
// @todo to be reorganized

/*
TiNioSession * nac_getcursession( TiNioAcceptor * nac, TiNioSession * session )
{
	session->rxque = nac->rxque;
	session->txque = nac->txque;
	session->option = 0x00;
	return session;
}
*/

