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
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_bit.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_assert.h"
#include "svc_maca.h"


/* MACA medium access control 
 *
 * @author zhangwei on 20060724
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - add state transfer machine implementation
 */

#define IEEE802154_FRAMECONTROL_DATA_NOACK     0x8801
#define IEEE802154_FRAMECONTROL_DATA_ACK       0x8821

#define BROADCAST_PAN 0xFFFF
#define BROADCAST_ADDRESS 0xFFFF

#define bit8_set(token,index) ((token) |= (_BV(index)))
#define bit8_get(token,index) ((token) & (_BV(index)))
#define bit8_clr(token,index) ((token) &= (~_BV(index)))

/******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][1 Command Type][n Command Payload][2 FCS]
 *
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 * 
 *****************************************************************************/


/** 
 * maca RTS/CTS format
 *
 * All frames are composed by the following components:
 *
 *  +----------+------------+----------------------------------------+--------+
 *  | Preamble | PHY Header |                 MAC Data               | CRC 2B |
 *  +----------+------------+----------------------------------------+--------+
 * 
 * 
 * MAC Frame
 *  +------------------+-------------+---------------+------------+
 *  | Frame Control 2B | Sequence 1B | Address 4-10B | Payload nB |
 *  +------------------+-------------+---------------+------------+
 *
 *
 * RTS Frame Format
 * attention in our design, the RTS/CTS frame are still 802.15.4 data frames. this
 * is used to avoid potential confliction with standard 802.15.4 command frames in 
 * a 802.15.4 based network such zigbee systems.
 * 
 *  +------------------+-------------+-------+-------+------------+-------------+
 *  | Frame Control 2B | Sequence 1B | RA 4B | TA 4B | Command 1B | Duration 1B | 
 *  +------------------+-------------+-------+-------+------------+-------------+
 *
 * 2B crc are padded at the end of the RTS/CTS frame. the crc bytes are actually 
 * belong to PHY layer.
 *
 *  RA: receiver address := [Destinatioin Pan 2B ][Destination Short Address 2B]
 *  TA: who transmit the RTS frame  := [Source Pan 2B ][Source Short Address 2B]
 *  duration: in microseconds, required to transmit the next Data or Management frame,
 *      plus one CTS frame, plus one ACK frame, plus three SIFS intervals.
 * 
 * so the total RTS frame is 1B length + 13B MAC + 2B CRC = 16B
 * 
 * 
 * CTS Frame Format
 * 
 *  +------------------+-------------+-------+-------+------------+
 *  | Frame Control 2B | Sequence 1B | RA 4B | TA 4B | Command 1B |
 *  +------------------+-------------+-------+-------+------------+
 * 
 * in the 802.11 design, there's still an Duration field(2B) in the CTS. for energy
 * saving reasons, this field is eliminated from this design.
 * 
 * sequence field is unnecessary in RTS/CTS frame in theory. however, we design the 
 * RTS/CTS based on standard 802.15.4 data type frame, we had to place this sequence 
 * byte here according to 802.15.4 standard.
 * 
 * so the total CTS frame is 1B length + 12B MAC + 2B CRC = 15B
 *
 * ACK Frame Format
 * using the standard 802.15.4 ACK frame supported by the cc2420 transceiver.
 *
 * reference
 * - 802.11 Frame Format.
 * - 802.15.4 frame format
 * - cc2420 datasheet from TI.com (formerly chipcon.com)
 */

#define MACA_RTS_FULLSIZE 15
#define MACA_CTS_FULLSIZE 14

static char m_rts[MACA_RTS_FULLSIZE];
static char m_cts[MACA_CTS_FULLSIZE];

static void _maca_init_rts( TiMACA * mac );
static void _maca_init_cts( TiMACA * mac );
static void _maca_set_rts( TiMACA * mac, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom, uint8 duration );
static void _maca_set_cts( TiMACA * mac, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom );
static bool _maca_is_rts( TiMACA * mac, TiFrame * frame );
static bool _maca_is_cts( TiMACA * mac, TiFrame * frame );
static uintx _maca_check_rts_cts( TiMACA * mac, TiFrame * frame );
static uint16 _maca_get_backoff( TiMACA * mac );

static uintx _maca_trysend( TiMACA * mac, TiFrame * frame, uint8 option );
static uintx _maca_broadcast_rts( TiMACA * mac );
static uintx _maca_broadcast_cts( TiMACA * mac );
static uintx _maca_tryrecv( TiMACA * mac, TiFrame * frame, uint8 option );

TiMACA * maca_construct( char * buf, uint16 size )  
{
    TiMACA * mac = (TiMACA *)buf;

	hal_assert( sizeof(TiMACA) <= size );
	memset( buf, 0x00, size );
    mac->txbuf = frame_construct( &mac->txbufmem[0], MACA_TXBUF_MEMSIZE );
    mac->rxbuf = frame_construct( &mac->rxbufmem[0], MACA_RXBUF_MEMSIZE );
    mac->state = MACA_STATE_NULL;
	return mac;
}

void maca_destroy( TiMACA * mac )
{
    frame_destroy( mac->txbuf );
    frame_destroy( mac->rxbuf );
}

TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * rxtx, void * rxtxowner, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner )
{
	hal_assert( (rxtx != NULL) && (timer != NULL) && (mac->state == MACA_STATE_NULL) );

	mac->rxtx = rxtx;
    mac->rxtxowner = rxtxowner;
	mac->timer = timer;
    mac->loadfactor = 5;
    mac->request = 0x00;
    mac->retry = 0;
	mac->listener = listener;
	mac->lisowner = lisowner;

    frame_clear( mac->rxbuf );
    frame_clear( mac->txbuf );

    // assume: the transceiver(mac->rxtx) has already opened before calling this function

	mac->rxtx->setchannel( mac->rxtxowner, CONFIG_MACA_DEFAULT_CHANNEL );
	mac->rxtx->enable_addrdecode( mac->rxtxowner );					
	mac->rxtx->setpanid( mac->rxtxowner, CONFIG_MACA_DEFAULT_PANID );
	mac->rxtx->setshortaddress( mac->rxtxowner, CONFIG_MACA_DEFAULT_LOCAL_ADDRESS );
	mac->rxtx->switchtomode( mac->rxtxowner, FTRX_MODE_RX );

    _maca_init_rts( mac );
    _maca_init_cts( mac );

    rand_open( 78 );
	timer_open( timer, 0, NULL, NULL, 0x00 );

    mac->state = MACA_STATE_IDLE;
    return mac;
}

/* TiMACA doesn't call transceiver object's close. so you need to close the 
 * transceiver object manually yourself. 
 */
void maca_close( TiMACA * mac )
{
	timer_close( mac->timer );
    rand_close();
    mac->state = MACA_STATE_NULL;
}

/* this function will try to accept the input data and copy them into internal buffer. 
 * the real data sending is done in maca_evolve(). the caller should 
 * repeatedly call evolve() to drive the MAC to run. 
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx maca_send( TiMACA * mac, TiFrame * frame, uint8 option )
{
    uintx ret=0;

    if (frame_empty(mac->txbuf))
    {
        ret = frame_copyfrom( mac->txbuf, frame );

        // @modified by zhangwei on 2010.05.09
        // in the past, the mac is automatically wakeup by the following source code
        // if there's a frame for sending. however, according to the low low power design
        // principal, we'd better do this manually to avoid uncessary wakeup and 
        // forgotten sleepings. so i giveup automatically wakeup in the end.
        //
        // maca_wakeup( mac );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code:
        // do {
        //      maca_evolve( mac, NULL );
        // }while (mac->state != MACA_STATE_IDLE);
    }

    maca_evolve( mac, NULL );
    return ret;
}

/** 
 * broadcast a frame out. the difference between broadcast() and send() is that the 
 * broadcast function will fill the destination address field with broadcast address.
 */
uintx maca_broadcast( TiMACA * mac, TiFrame * frame, uint8 option )
{
    TiIEEE802Frame154Descriptor desc;
    uintx ret=0;

    if (frame_empty(mac->txbuf))
    {
        ret = frame_copyfrom( mac->txbuf, frame );
        ieee802frame154_open( &desc );
        ieee802frame154_parse( &desc, frame_startptr(frame), frame_length(frame) );
        ieee802frame154_set_shortaddrto( &desc, BROADCAST_ADDRESS );
        ieee802frame154_close( &desc );

        // @modified by zhangwei on 2010.05.09
        // in the past, the mac is automatically wakeup by the following source code
        // if there's a frame for sending. however, according to the low low power design
        // principal, we'd better do this manually to avoid uncessary wakeup and 
        // forgotten sleepings. so i giveup automatically wakeup in the end.
        //
        // maca_wakeup( mac );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code:
        // do {
        //      maca_evolve( mac, NULL );
        // }while (mac->state != MACA_STATE_IDLE);
    }

    maca_evolve( mac, NULL );
    return ret;
}

uintx maca_recv( TiMACA * mac, TiFrame * frame, uint8 option )
{
    uintx count=0;

    if (frame_empty(mac->rxbuf)) 
    {
        hal_assert( mac->state != MACA_STATE_SLEEP );
        hal_assert( mac->state != MACA_STATE_POWERDOWN );

        // if ((mac->state == MACA_STATE_IDLE) || (mac->state == MACA_STATE_RETRY_SEND))
        if (mac->state == MACA_STATE_IDLE)
        {
            _maca_tryrecv( mac, frame, option );
            count = _maca_check_rts_cts( mac, frame );
        }
    }
    else /* if (!frame_empty(mac->rxbuf)) */
    {
        count = frame_copyto( mac->rxbuf, frame );
        frame_clear( mac->rxbuf );
    }

    maca_evolve( mac, NULL );
    return count;
}

/* this function can be used as TiFrameTxRxInterface's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using aloha_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void maca_evolve( void * macptr, TiEvent * e )
{
	TiMACA * mac = (TiMACA *)macptr;
    TiFrameTxRxInterface * transceiver = mac->rxtx;
    TiEvent newevent;

    if (mac->state == MACA_STATE_NULL)
        return;

    switch (mac->state)
    {
	case MACA_STATE_IDLE:   
        // try to retrieve frame from PHY layer. we should do receiving process
        // more frequently to avoid frame lossing. 
        // 
        if (_maca_tryrecv(mac, mac->rxbuf, 0x00) > 0)
        {
            // until here, only the 802.15.4 DATA type frame is accepted. the other types 
            // are rejected. we need further check whether the incoming frame is RTS 
            // type or not.

            if (_maca_check_rts_cts(mac, mac->rxbuf) > 0)
            {
                if (mac->listener != NULL)
                {
                    memset( &newevent, 0x00, sizeof(newevent) );
                    newevent.id = MACA_EVENT_FRAME_ARRIVAL;
	                mac->listener( mac->lisowner, &newevent );
                }
            }
        }

        if (!frame_empty(mac->txbuf))
        {
            ieee802frame154_open( &mac->desc );
            ieee802frame154_parse( &mac->desc, frame_fullstartptr(mac->txbuf), frame_fulllength(mac->txbuf) );
            _maca_set_rts( mac, ieee802frame154_panto(&mac->desc), ieee802frame154_shortaddrto(&mac->desc), 
                ieee802frame154_shortaddrfrom(&mac->desc), 10 );
            ieee802frame154_close( &mac->desc );

            if (_maca_broadcast_rts(mac) > 0)
            {
                timer_restart( mac->timer, CONFIG_MACA_WAIT_CTS_DURATION, 0 );
                mac->state = MACA_STATE_WAIT_CTS;
            }
        }


        if (mac->state == MACA_STATE_IDLE)
        {
            // only in the idle state, we check for the sleeping request bit. if
            // this bit in the request variable is set by the sleep() function, 
            // then we call transceiver's sleep() function and force it into sleep
            // mode. in the sleeping mode, all the frames in the mac layer are still 
            // pending inside it, so the frames won't lost. different to the POWERDOWN
            // mode, the whole application can recover from SLEEP mode fast and continue
            // previous state.
            //
            if (bit8_get(mac->request, MACA_SLEEP_REQUEST))
            {
                transceiver->switchtomode( mac->rxtxowner, FTRX_MODE_SLEEP );
                bit8_clr( mac->request, MACA_SLEEP_REQUEST );
                mac->state = MACA_STATE_SLEEP;
            }
        }
        break;

    case MACA_STATE_WAIT_CTS:
        if (_maca_tryrecv( mac, mac->rxbuf, 0x00) > 0)
        {
            // todo: check whether this is an CTS frame
            if (_maca_is_cts(mac, mac->rxbuf))
            {
                // if the CTS frame received, then we start the transmission of 
                // pending frames immediately without hesitation. if we failed
                // in the sending, we should backoff for some time, and restart
                // the RTS-CTS-DATA process again.
                //
                if (_maca_trysend(mac, mac->txbuf, 0x00) > 0)
                {
                    // sending the frame successfully
                    mac->retry = 0;
                    mac->state = MACA_STATE_IDLE;
                }
                else{
                    if (mac->retry > CONFIG_MACA_MAX_RETRY)
                    {
                        // failed to sending the frame after 3 try. we had to accept
                        // the situation and go back to IDLE state
                        //
                        frame_clear( mac->txbuf );
                        mac->retry = 0;
                        // mac->failed ++;
                        mac->state = MACA_STATE_IDLE;
                    }
                    else{
                        timer_restart( mac->timer, _maca_get_backoff(mac), 0 );
                        mac->retry ++;
                        mac->state = MACA_STATE_RETRY_SEND;
                    }
                }
            }
            else{
                // @attention
                // another type frame came to this node when it hopes an CTS frame
                // do nothing and continue waiting for CTS. if the caller doesn't
                // retrieve this frame out from "mac->rxbuf" then it will be
                // overwrite by next frame 
            }
        }
        else if (timer_expired(mac->timer))
        {
            // the sending process failed this time. we need to restart the whole
            // sending process now. first, we need to delay some time. this duration
            // is generally known as backoff time. 
            //
            timer_restart( mac->timer, _maca_get_backoff(mac), 0 );
            mac->retry ++;
            mac->state = MACA_STATE_RETRY_SEND;
        }
        break;    

    case MACA_STATE_RETRY_SEND:
        // since the retry backoff time is pretty long, so I design this MACA to
        // retrieve frame from PHY layer in order to decrease receving latency. 
        // this can avoid some frame lossing. 
        // 
        if (_maca_tryrecv(mac, mac->rxbuf, 0x00) > 0)
        {
            // until here, only the 802.15.4 DATA type frame is accepted. the other types 
            // are rejected. we need further check whether the incoming frame is RTS 
            // type or not.

            if (_maca_check_rts_cts(mac, mac->rxbuf) > 0)
            {
                if (mac->listener != NULL)
                {
                    memset( &newevent, 0x00, sizeof(newevent) );
                    newevent.id = MACA_EVENT_FRAME_ARRIVAL;
	                mac->listener( mac->lisowner, &newevent );
                }
            }
        }

        // if the backoff duration expired, then we call PHY layer functions to 
        // send the frame immediately.
        //
        if (timer_expired(mac->timer))
        {
            // for simplicity, assume the CTS already sent successfully here
            _maca_broadcast_rts( mac );
            timer_restart( mac->timer, CONFIG_MACA_WAIT_CTS_DURATION, 0 );
            mac->state = MACA_STATE_WAIT_CTS;
        }
        break;

    case MACA_STATE_SLEEP:
        if (bit8_get(mac->request, MACA_WAKEUP_REQUEST))
        {
            transceiver->switchtomode( mac->rxtxowner, FTRX_MODE_RX );
            bit8_clr( mac->request, MACA_WAKEUP_REQUEST );
            mac->state = MACA_STATE_IDLE;
        }
        break;

    default:
        // this should never happen.  the following source code can pull the state 
        // to IDLE and let the state machine to continue in case of state disorders.
        hal_assert( false );
        mac->state = MACA_STATE_IDLE;
    }

    if (mac->request > 0)
    {
        if (bit8_get(mac->request, MACA_SHUTDOWN_REQUEST))
        {
            // no matter what the current state is, you can do shutdown. the power will 
            // lost. so the data inside the SRAM will be lost. 

            // if timer is already running, then we should stop it. 
            timer_stop( mac->timer );
            frame_clear( mac->txbuf );
            frame_clear( mac->rxbuf );

            transceiver->switchtomode( mac->rxtxowner, FTRX_MODE_POWERDOWN );
            bit8_clr( mac->request, MACA_SHUTDOWN_REQUEST );
            mac->state = MACA_STATE_POWERDOWN;
        }

        if (bit8_get(mac->request, MACA_STARTUP_REQUEST))
        {
            if (mac->state == MACA_STATE_POWERDOWN)
            {
                transceiver->switchtomode( mac->rxtxowner, FTRX_MODE_RX );
                bit8_clr( mac->request, MACA_STARTUP_REQUEST );
                mac->state = MACA_STATE_IDLE;
            }
        }
    }

    if (e != NULL)
    {
	    switch (e->id)
	    {
        case MACA_EVENT_FRAME_ARRIVAL:
            // if the incoming event indicates that the transceiver receives an frame, 
            // then it simply pass the event to listener object. the listener object
            // will call maca_recv() to retrieve the frame out. 
            //
            // @attention
            // - if the caller cann't check for incoming frames fast enough, then the 
            // new frames will overwrite the old ones inside the transceiver object.
            // it's the application developer's responsibility to guarantee there
            // no packet loss due to slow querying and insufficient buffering spaces.
            //
            // - however, for some transceiver such as 802.15.4 compatible chip cc2420, 
            // it may report ACK/BECON/COMMAND frames according to 802.15.4 specification. 
            // however, the upper layer doesn't need these frames. the maca receiving 
            // mechanism will filter them, and the RTS/CTS frames. only the valid data
            // frame are reported to the upper layer.
            //            
            if (mac->listener != NULL)
	        {
		        mac->listener( mac->lisowner, e );
	        }
            break;

        default:
		    break;
	    }
    }

	return;
}

/*
 *  +------------------+-------------+-------+-------+------------+-------------+
 *  | Frame Control 2B | Sequence 1B | RA 4B | TA 4B | Command 1B | Duration 1B |
 *  +------------------+-------------+-------+-------+------------+-------------+
 */
void _maca_init_cts( TiMACA * mac )
{
    uint16 ctrl = IEEE802154_FRAMECONTROL_DATA_NOACK;

    hal_assert( sizeof(m_rts) == 13 );
    memset( &m_rts, 0x00, sizeof(m_rts) );
    m_rts[0] = 12;                      // frame length. not including the length byte itself
    m_rts[1] = (char)(ctrl >> 8);       // frame control
    m_rts[2] = (char)(ctrl & 0xFF);
    m_rts[12] = 0x01;                   // command 0x01 indicates this is a RTS frame
}

/*
 *  +------------------+-------------+-------+-------+------------+
 *  | Frame Control 2B | Sequence 1B | RA 4B | TA 4B | Command 1B |
 *  +------------------+-------------+-------+-------+------------+
 */
void _maca_init_rts( TiMACA * mac )
{
    uint16 ctrl = IEEE802154_FRAMECONTROL_DATA_NOACK;

    hal_assert( sizeof(m_rts) == 12 );
    memset( &m_rts, 0x00, sizeof(m_rts) );
    m_rts[0] = 11;                      // frame length. not including the length byte itself
    m_rts[1] = HIGH_BYTE(ctrl);         // frame control
    m_rts[2] = LOW_BYTE(ctrl);
    m_rts[12] = 0x02;                   // command 0x02 indicates this is a RTS frame
}

void _maca_set_rts( TiMACA * mac, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom, uint8 duration )
{
    m_rts[3]  = 0x00;
    m_rts[4]  = HIGH_BYTE(pan);
    m_rts[5]  = LOW_BYTE(pan);
    m_rts[6]  = HIGH_BYTE(shortaddrto);
    m_rts[7]  = LOW_BYTE(shortaddrto);
    m_rts[8]  = HIGH_BYTE(pan);
    m_rts[9]  = LOW_BYTE(pan);
    m_rts[10] = HIGH_BYTE(shortaddrfrom);
    m_rts[11] = LOW_BYTE(shortaddrfrom);
    m_rts[13] = duration;
}

void _maca_set_cts( TiMACA * mac, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom )
{
    m_cts[3]  = 0x00;
    m_cts[4]  = HIGH_BYTE(pan);
    m_cts[5]  = LOW_BYTE(pan);
    m_cts[6]  = HIGH_BYTE(shortaddrto);
    m_cts[7]  = LOW_BYTE(shortaddrto);
    m_cts[8]  = HIGH_BYTE(pan);
    m_cts[9]  = LOW_BYTE(pan);
    m_cts[10] = HIGH_BYTE(shortaddrfrom);
    m_cts[11] = LOW_BYTE(shortaddrfrom);
}

bool _maca_is_rts( TiMACA * mac, TiFrame * frame )
{
    return ((frame_length(frame) == MACA_RTS_FULLSIZE) && (frame_fullstartptr(frame)[12] == 0x01));
}

bool _maca_is_cts( TiMACA * mac, TiFrame * frame )
{
    return ((frame_length(frame) == MACA_CTS_FULLSIZE) && (frame_fullstartptr(frame)[12] == 0x02));
}


/* check whether the current frame is an RTS frame or CTS frame. it it is, then do
 * corresponding processing and clear itself from the frame. after the call, the frame
 * should be empty or contains an data frame. 
 */
uintx _maca_check_rts_cts( TiMACA * mac, TiFrame * frame )
{
    TiIEEE802Frame154Descriptor desc;
    uint16 localaddress;

    // until here, only the 802.15.4 DATA type frame is accepted. the other types 
    // are rejected. we need further check whether the incoming frame is RTS 
    // type or not.
    
    ieee802frame154_open( &desc );
    ieee802frame154_parse( &desc, frame_fullstartptr(frame), frame_fulllength(frame) );

    // if this frame is an RTS frame
    if (_maca_is_rts(mac, frame))
    {
        // if this RTS frame destination is the current node, then it should
        // prepare for receiving, or else it should go into alert sleep mode
        // for some while. 
                
        localaddress = mac->rxtx->getshortaddress( mac->rxtxowner );
        if (ieee802frame154_shortaddrfrom(&desc) != localaddress)
        {
            frame_clear( frame );

            // mac->sleeprequest_duration = 10;
            bit8_set( mac->request, MACA_SLEEP_REQUEST );
        }
        else{
            frame_clear( frame );

            // the RTS destination is current node. prepare for receving incoming
            // frames. the MACA caller must call maca_evolve() or maca_recv()
            // fast enough to avoid lossing frame.

            _maca_set_cts( mac, ieee802frame154_panfrom(&desc), ieee802frame154_shortaddrfrom(&desc), localaddress );
            _maca_broadcast_cts( mac );

            // @attention
            // after sending CTS frame related to the RTS received just now,
            // we simply stay in the IDLE state. and in the next call of 
            // the evolve() function or maca_recv() function, the data frame
            // will be retrieved from PHY layer and put into mac->rxbuf.
        }
    }
    // if we receive the CTS frame in IDLE state, just clear the buffer and 
    // ignore it. we only keep the data frames inside mac->rxbuf.
    else if (_maca_is_cts(mac, frame))
    {
        frame_clear( frame );
    }

    ieee802frame154_close( &desc );
    return frame_length( frame );
}

/* currently, the duration is configured as:
 * loadfactor => duration
 *      0           1
 *      1           1*8
 *      2           2*8
 *      3           3*8
 *      4           4*8
 *      ...
 */
inline uint16 _maca_get_backoff( TiMACA * mac )
{
    return rand_uint16(((uint16)(mac->loadfactor))  << 3);
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
#ifdef CONFIG_MACA_TRX_SUPPORT_ACK
uintx _maca_trysend( TiMACA * mac, TiFrame * frame, uint8 option )
{
    return mac->rxtx->send( mac->rxtxowner, frame_fullstartptr(frame), frame_fulllength(frame), option );
}
#endif

/* call the PHY layer functions and try to send the frame immdietely. if ACK frame 
 * required, then wait for ACK. 
 * 
 * attention: this function is used internally. it doesn't care what kind of state
 * the TiMACA object is in. 
 * 
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - revision
 */
#ifndef CONFIG_MACA_TRX_SUPPORT_ACK
uintx _maca_trysend( TiMACA * mac, TiFrame * frame, uint8 option )
{
	uintx count = 0;
    uint16 ctrl;
    bool expired;
    char * ptr;
	
	while (!mac->rxtx->ischannelclear( mac->rxtxowner ))
		continue;

	mac->rxtx->switchtomode( mac->rxtx, FTRX_MODE_TX );
    count = mac->rxtx->send( mac->rxtx, frame, option );
	mac->rxtx->switchtomode( mac->rxtx, FTRX_MODE_RX );
	
    if (count > 0)
	{
		/* If the frame has been successfully sent by the transceiver, the program will
		 * start to wait for the ACK frame replied from the receiver. The maximum waiting
		 * duration is configured by macro CONFIG_ALOHA_ACK_RESPONSE_TIME. The time unit
		 * is milli-seconds.
		 */

        /* wait for ack frame from the receiver. if we cannot receive the ack frame
         * in time, then we can regard the frame just send is lost during transmission.
         * this duration here is for the receiver to send the ack back. it consists 
         * the processing time inside receiver node and the transmission time in 
         * the air.
         */ 
		timer_restart( mac->timer, CONFIG_MACA_ACK_RESPONSE_TIME, 0 ); 
		expired = true;
		while (!timer_expired( mac->timer ))
		{
            hal_delay( 1 );
			if (mac->rxtx->recv(mac->rxtx, mac->rxbuf, 0x00) > 0)
			{
				/* 2 bytes for frame control */
                ptr = frame_ptr( mac->rxbuf );
				ctrl = (((uint16)(ptr[1])) << 8) | (ptr[2]);

				/* If the incoming frame is ACK frame, then we can return success. 
				 * other type frames will be ignored. 
                 * 
                 * @attention
				 *  - Theoretically speaking, the current frame arrived may possibly be the 
                 * data type from other nodes. in the current version, this frame will be 
                 * overwrotten, which leads to unnecessary frame loss.
				 */
				if (FCF_FRAMETYPE(ctrl) == FCF_FRAMETYPE_ACK)
				{
					timer_stop( mac->timer );
                    frame_clear( mac->rxbuf );
					expired = false;
					break;
				}
			}
		}
		
		if (expired)
		{
			count = 0;
		}
	}

	return count;
}
#endif

/** 
 * this function will send an RTS frame out immediately by calling the send() function
 * in the lower level transceiver's interface. 
 */
uintx _maca_broadcast_rts( TiMACA * mac )
{
    while (1)
    {
        if (mac->rxtx->send( mac->rxtxowner, &m_rts[0], sizeof(m_rts), 0x00) > 0)
            break;

        mac->rxtx->evolve( mac->rxtxowner, NULL );
    }
    return 1;
}

/** 
 * this function will send an CTS frame out immediately by calling the send() function
 * in the lower level transceiver's interface. 
 */
uintx _maca_broadcast_cts( TiMACA * mac )
{
    while (1)
    {
        if (mac->rxtx->send( mac->rxtxowner, &m_cts[0], sizeof(m_cts), 0x00) > 0)
            break;

        mac->rxtx->evolve( mac->rxtxowner, NULL );
    }
    return 1;
}

/* try to fetch an frame from the transceiver layer. 
 * attention: this function doesn't care what state the TiMACA is in. generally, 
 * you should call this function only in IDLE state. If you call it in other state,
 * you may violate the running of the state machine in the evolve() function.
 */ 
uintx _maca_tryrecv( TiMACA * mac, TiFrame * frame, uint8 option )
{
    uintx count=0;
    uint16 ctrl=0;
    char * buf;

    count = mac->rxtx->recv( mac->rxtx, frame_fullstartptr(frame), frame_capacity(frame), option );
    if (count > 0)
    {
    	/* according to ALOHA protocol, the program should send ACK/NAK after receiving
	     * a data frame. acoording to 802.15.4 protocol, there maybe COMMAND, BEACON or 
         * ACK frames in the air. however, these type of frames are all used inside the 
         * MAC layer only. we should filter them and keep the DATA frames only.
         * 
		 * 2 bytes for frame control
		 * only the DATA type frame will be accepted here. The other types, such as 
         * COMMAND, BEACON or ACK will be ignored. 
		 */
        buf = frame_fullstartptr(frame);
		ctrl = (((uint16)buf[1]) << 8) | (buf[2]);

		if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
        {
			count = 0;
            frame_clear( frame );
        }
        else
            frame_setlength( frame, count );
    }

    return count;
}
