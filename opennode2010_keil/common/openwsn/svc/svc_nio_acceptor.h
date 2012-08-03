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

#ifndef _SVC_NIO_ACCEPTOR_H_8432_
#define _SVC_NIO_ACCEPTOR_H_8432_

/*******************************************************************************
 * @file
 * @brief TiNioSession is an data structure to transfer REQUEST/RESPONSE pair between 
 * components.
 *
 * @author Jiang Ridong (jrd072623@163.com)
 * @since 1.2
 * @see svc_nio_dispatch, svc_nio_session, svc_nio_service  
 * @link TiNioSession, TiNioDispatcher, TiNioService
 * 
 * @state
 * 	- finished developing. more testing needed
 *
 * @history
 * @author OpenWSN on 2010.12.05
 *	- first created
 * @modified by zhangwei on 2010.12.12
 * 	- Regualted the interface functions.
 * 
 * @modified by zhangwei on 2011.04.10
 * 	- Bug fix. You should try send the frame in the txque for only one time. The 
 *    frame should be removed from the txque no matter success or failed. Or else
 *    the nac_evolve() function will continue try to sending it. Every frame wrotten
 *    into the txque in nio acceptor is assumed to be transmitted imdiately and 
 *    successfully. Whether the sending is really received successfully depends 
 *    on the ACK mechanism.
 * @modified by zhangwei on 2011.04.11
 *	- Revised.
 * @modified by zhangwei on 2011.04.11
 *	- The member variable rxtx is changed from "TiCc2420Adapter" to 
 *    "TiFrameTxRxInterface *".
 ******************************************************************************/ 

#ifndef CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 
#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#endif

#ifndef CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1
#endif

/**
 * @attention 
 * Both the listener and rxfilter mechanism can be used to retrieve incoming frame 
 * from the low level transceiver adapter. But generally you'd better to use only 
 * one of them to avoid potential confliction unless you know the transceiver adapter
 * well.
 */
#undef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
#define CONFIG_NIOACCEPTOR_RXFILTER_ENABLE 

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_framequeue.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timesync.h"
#include "svc_foundation.h"

/** 
 * NIOACCEPTOR_HOPESIZE(...)
 * To estimate the memory block size the TiNioAcceptor object needed. The macro 
 * FRAMEQUEUE_ITEMSIZE is defined FRAME_HOPESIZE(128) currently. 
 * @see rtl_framequeue.h, rtl_frame.h, rtl_ieee802frame154.h
 */
#define NIOACCEPTOR_HOPESIZE(rxque_capacity,txque_capacity) \
	(sizeof(TiNioAcceptor)+FRAMEQUEUE_HOPESIZE((rxque_capacity))+(FRAMEQUEUE_ITEMSIZE)) + FRAME_HOPESIZE(I802F154_ACK_FRAME_SIZE)
//	(sizeof(TiNioAcceptor)+FRAMEQUEUE_HOPESIZE((rxque_capacity))+FRAMEQUEUE_HOPESIZE((txque_capacity))+(FRAMEQUEUE_ITEMSIZE))
    
    
#define nac_rxque_front(nac) fmque_front(nac_rxque(nac))
#define nac_rxque_rear(nac) fmque_rear(nac_rxque(nac))
//#define nac_txque_front(nac) fmque_front(nac_txque(nac))
//#define nac_txque_rear(nac) fmque_rear(nac_txque(nac))

#define nac_rxque_empty(nac) fmque_empty(nac_rxque(nac))
#define nac_rxque_full(nac) fmque_full(nac_rxque(nac))
//#define nac_txque_empty(nac) fmque_empty(nac_txque(nac))
//#define nac_txque_full(nac) fmque_full(nac_txque(nac))
    

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief TiNioAcceptor: Maintains rxque and txque, and the transformation between 
 * raw frame buffer and TiFrame object.
 *
 * This service enhances the fundamental hardware adapter such as TiCc2420Adapter by:
 *	- Provide an frame queue for frame buffering in sending and recving
 *	- Provide high level send()/recv() function and listening mechanism. But all
 *    these are running in non-interrupt mode. So you can freely use them.
 *    While, the TiCc2420Adapter's listener is actually running in interrupt mode.
 * 
 * The design of this service is affected greatly by Apache MINA Project:
 *	- http://mina.apache.org/
 *  - http://mina.apache.org/conferences.data/Mina_in_real_life_ASEU-2009.pdf
 * 
 * The architecture of MINA is:
 *	Remote Peer ~~~ IoService -- IoFilterChain{IoFilters} -- IoHandler
 *  IoService := [IoAcceptor + IoProcessor] | [IoConnector]
 * 
 * We simplified MINA's architecture by eliminating the processor part.
 * 
 * @modified by zhangwei on 2011.08.15
 * - The TiNioAcceptor object has three main functions now:
 *   1) Convert the {char*buf, size/len} I/O interface of transceiver object 
 *      to an TiFrame based I/O interface, which helps upper layer developing.
 *   2) Provides an queue to buffer the incomming frames to avoid lossing.
 *   3) Support time synchronization in the HAL layer.
 */
#pragma pack(1) 
typedef struct{
	uint16 memsize;
	uint8 state;
	TiFrameTxRxInterface * rxtx;
    TiFunRxHandler rxhandler;
    void * rxhandlerowner;
	TiFrameQueue * rxque;
	TiFrameQueue * txque;
    TiFrame * ackbuf;
    TiTimeSyncAdapter * timesync;
}TiNioAcceptor;

#ifdef CONFIG_DYNA_MEMORY
TiNioAcceptor * nac_create( void * mem, TiFrameTxRxInterface * rxtx, uint8 rxque_capacity, 
	uint8 txque_capacity );
#endif

#ifdef CONFIG_DYNA_MEMORY
void nac_free( TiNioAcceptor * nac);  
#endif

TiNioAcceptor * nac_construct( void * mem, uint16 size);
void nac_detroy( TiNioAcceptor * nac);  
TiNioAcceptor * nac_open( TiNioAcceptor * nac, TiFrameTxRxInterface * rxtx, 
	uint8 rxque_capacity, uint8 txque_capacity );
void nac_close( TiNioAcceptor * nac );

void nac_setrxhandler( TiNioAcceptor * nac, TiFunRxHandler rxhandler, void * rxhandlerowner );
void nac_set_timesync_adapter( TiNioAcceptor * nac, TiTimeSyncAdapter * tsync );

TiFrameTxRxInterface * nac_rxtx_interface( TiNioAcceptor * nac );

/** 
 * Return the sending queue(txque) inside acceptor. You can manipulate this queue
 * directly in some program. 
 * 
 * Recommend to use nac_send() instead.
 */
TiFrameQueue * nac_txque( TiNioAcceptor * nac );

/** 
 * Return the receiving queue(rxque) inside acceptor.
 * Recommend to use nac_recv() instead.
 */
TiFrameQueue * nac_rxque( TiNioAcceptor * nac );
TiFrame * nac_rxquefront( TiNioAcceptor * nac );
void nac_rxquepopfront( TiNioAcceptor * nac );

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
intx nac_send( TiNioAcceptor * nac, TiFrame * frame, uint8 option );

/** 
 * Retrieve an frame received inside rxque into frame object. 
 * @return Frame length.
 */
intx nac_recv( TiNioAcceptor * nac, TiFrame * frame, uint8 option);

/**
 * Drive the state machine in the TiNioAcceptor object to run.
 */
void nac_evolve ( TiNioAcceptor * nac, TiEvent * event );


/**
 * This function is registered into the transceiver component. If there's a new frame
 * arrives, then this function will be invoked. This function fetchs the new incoming
 * frame from the transceiver and append it at the tail of the rxque.
 * 
 * In principle, this listener can be replaced by the evolve function. We finally
 * design a separate listener function in order to improve the efficiency. 
 * 
 * Attention this function may execute in interrupt mode. This's depend on how the 
 * transceiver component implemented. This is different from the evolve() function
 * because it's generally designed to be executed in non-interurpt mode.
 */
#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
void nac_on_frame_arrived_listener( TiNioAcceptor * nac, TiEvent * e );
#endif

//------------------------------------------------------------------------------
// @todo to be reorganized

/**
 * @brief Get the current active session from the acceptor. 
 * 
 * The session management is quite different in OpenWSN system and other resource-rich 
 * systems. Considering the highly restricted memory, the OpenWSN decides to allow
 * only one active session in the system. The application can only deal with the 
 * next session after finished the previous one.
 *
 * And one session contains an REQUEST and an RESPONSE. The REQUEST and RESPONSE
 * can be empty. 
 * 
 * Different to the embedded linux, the session here is only used to pass REQUEST/RESPONSE
 * pair between objects. The life time is pretty short.
 */
//TiNioSession * nac_getcursession( TiNioAcceptor * nac, TiNioSession * session );

#ifdef __cplusplus
}
#endif
#endif
