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


/* MACA medium access control protocol
 * MACA means Multiple Access with Collision Avoidance (MACA)
 *
 * @author zhangwei on 20060724
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - add state transfer machine implementation
 *
 * @modified by xu-fuzhen(Control Department, TongJi University) on 2010.10.18
 *  - revision. tested ok. 
 * 
 * @modified by zhangwei(Control Department, TongJi University) on 2010.10.18
 *  - revision.
 */

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_bit.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "svc_maca.h"

#define MACA_SLEEP_REQUEST                     1
#define MACA_WAKEUP_REQUEST                    2
#define MACA_SHUTDOWN_REQUEST                  3
#define MACA_STARTUP_REQUEST                   4

#define MACA_EVENT_FRAME_ARRIVAL               234 // todo

#define HIGH_BYTE(n16) ((uint8)((n16) >> 8))
#define LOW_BYTE(n16) ((uint8)((n16) & 0xFF))

#define bit8_set(token,index) ((token) |= (_BV(index)))
#define bit8_get(token,index) ((token) &  (_BV(index)))
#define bit8_clr(token,index) ((token) &= (~_BV(index)))

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
 * saving reasons, this field is eliminated from this version design.
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

/* reference
 * - CSMA, Carrier sense multiple access, http://en.wikipedia.org/wiki/Carrier_sense_multiple_access;
 * - CSMA/CA, Carrier sense multiple access with collision avoidance, http://en.wikipedia.org/wiki/CSMA_CA;
 * - IEEE 802.11 RTS/CTS, http://en.wikipedia.org/wiki/IEEE_802.11_RTS/CTS;
 * - MACA, Multiple Access with Collision Avoidance, 
 *   http://en.wikipedia.org/wiki/Multiple_Access_with_Collision_Avoidance;
 * - Multiple Access with Collision Avoidance for Wireless, http://en.wikipedia.org/wiki/MACAW;
 */


#define MACA_RTS_SIZE 14
#define MACA_CTS_SIZE 15

static char m_rts[MACA_RTS_SIZE];
static char m_cts[MACA_CTS_SIZE];

static void _maca_init_rts( TiMACA * mac );
static void _maca_init_cts( TiMACA * mac );
static void _maca_set_rts( TiMACA * mac, uint8 seqid, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom, uint8 duration );
static void _maca_set_cts_from_rts( TiMACA * mac, char * cts, char * rts );
//static void _maca_set_cts( TiMACA * mac, uint8 seqid, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom );
static uint8 get_rts_duration( TiMACA * mac, char * rts );
static bool _maca_is_rts( TiMACA * mac, char * buf, uint8 len );
static bool _maca_is_cts( TiMACA * mac, char * buf, uint8 len );
static bool _maca_is_data( TiMACA * mac, char * buf, uint8 len );
static uint16 _maca_get_backoff( TiMACA * mac );

static uintx _maca_broadcast_rts( TiMACA * mac );
static uintx _maca_broadcast_cts( TiMACA * mac );
static uintx _maca_trysend( TiMACA * mac, TiFrame * frame, uint8 option );
static uintx _maca_tryrecv( TiMACA * mac, TiFrame * frame, uint8 option );

TiMACA * maca_construct( char * buf, uint16 size )  
{
    TiMACA * mac = (TiMACA *)buf;

	hal_assert( sizeof(TiMACA) <= size );
	memset( buf, 0x00, size );
    mac->state = MACA_STATE_SHUTDOWN;
	return mac;
}

void maca_destroy( TiMACA * mac )
{
	return;
}

TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid,
    uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner )
{
    char * provider;

    // assert( the rxtx driver has already been opened );
    // assert( mac->timer is already opened but not start yet );
	hal_assert( (rxtx != NULL) && (timer != NULL) && (mac->state == MACA_STATE_SHUTDOWN) );
	mac->state = MACA_STATE_IDLE;
    mac->rxtx = rxtx;
    mac->timer = timer;
    mac->loadfactor = 5;
    mac->request = 0x00;
    mac->retry = 0;
	mac->listener = listener;
	mac->lisowner = lisowner;
	mac->retry = 0;
	mac->backoff = CONFIG_MACA_MIN_BACKOFF;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;
    mac->sendoption = 0x00;
    mac->sendfailed = 0;
	mac->option = 0x00;
    mac->txbuf = frame_open( &(mac->txbuf_memory[0]), FRAME_HOPESIZE(CONFIG_MACA_MAX_FRAME_SIZE), 0, 0, 0 );
    mac->rxbuf = frame_open( &(mac->txbuf_memory[0]), FRAME_HOPESIZE(CONFIG_MACA_MAX_FRAME_SIZE), 0, 0, 0 );
	
    // @modified by zhangwei on 2010.08.21
    // @attention: for all hardware components, you should construct and open them 
    // in the caller function to avoid potential conflictions. so we don't recommend
    // initialize the timer component here. You should construct timer before constructing
    // maca component, and also open the timer component before open the maca too.
    //
	// timer = timer_construct( (void *)&g_timer, sizeof(g_timer) );
    // timer_open( timer, id, NULL, NULL, 0x00 ); 
    hal_assert( mac->timer != NULL );
   
    // initialize the frame transceiver component
	// attention enable ACK support for aloha protocol. the current implementation depends
    // on low level transceiver component to provide ACK mechanism. 

    provider = rxtx->provider;
	rxtx->setchannel( provider, chn );
	rxtx->setpanid( provider, panid );
	rxtx->setshortaddress( provider, address );
    rxtx->enable_addrdecode( provider );
	rxtx->enable_autoack( provider );
   
    _maca_init_rts( mac );
    _maca_init_cts( mac );
    

    ieee802frame154_open( &(mac->desc) );
    // initialize the random number generator with a random seed. you can replace 
    // the seed with other values.
    //
    // @attention: generally, the random generator can be initialized when the whole
    // application started. if so, you needn't call rand_open() to initialize it 
    // again here.

    rand_open( 78 );  

    return mac;
}

/* TiMACA doesn't call transceiver object's close. so you need to close the 
 * transceiver object manually yourself. 
 */
void maca_close( TiMACA * mac )
{
	timer_stop( mac->timer );
    mac->state = MACA_STATE_SHUTDOWN;
}

/* this function will try to accept the input data and copy them into internal buffer. 
 * the real data sending is done in maca_evolve(). the caller should 
 * repeatedly call evolve() to drive the MAC component to run. 
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx maca_send( TiMACA * mac, TiFrame * frame, uint8 option )
{
	TiIEEE802Frame154Descriptor * desc;
    uintx ret=0;

	// @modified by zhangwei on 2010.05.09
    // in the past, the mac is automatically wakeup by the following source code
    // if there's a frame for sending. however, according to the low low power design
    // principal, we'd better do this manually to avoid uncessary wakeup and 
    // forgotten sleepings. so i giveup automatically wakeup in this revision.
    //
    // csma_wakeup( mac );

    switch (mac->state)
    {
    case MACA_STATE_IDLE:
        frame_totalcopyfrom( mac->txbuf, frame );

        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
        frame_skipouter( mac->txbuf, 12, 2 );
        desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), FRAME154_DEF_FRAMECONTROL_DATA );
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, mac->seqid );
        ieee802frame154_set_panto( desc, mac->panto );
        ieee802frame154_set_shortaddrto( desc, mac->shortaddrto );
        ieee802frame154_set_panfrom( desc, mac->panfrom );
        ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );

        mac->sendoption = option;

        #ifdef CONFIG_MACA_STANDARD
        _maca_trysend( mac, mac->txbuf, option );
        #endif

        #ifndef CONFIG_MACA_STANDARD
        if (_maca_ischannelclear(mac))
        {
            _maca_trysend( mac, mac->txbuf, option );
        }
        else{
            // if the channel is busy, then wait for a random period before really 
            // start the transmission
            mac->backoff = rand_uint8( CONFIG_MACA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = MACA_STATE_BACKOFF;
        }
        #endif

        ret = frame_capacity( mac->txbuf );

        maca_evolve( mac, NULL );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code, but the following has side effect because
        // it will block the execution of other source code.
        //
        // do {
        //      maca_evolve( mac, NULL );
        // }while (mac->state != MACA_STATE_IDLE);

        break;

    case MACA_STATE_BACKOFF:
        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //
        maca_evolve( mac, NULL );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code, but the following has side effect because
        // it will block the execution of other source code.
        //
        // do {
        //      maca_evolve( mac, NULL );
        // }while (mac->state != MACA_STATE_IDLE);

        ret = 0;
        break;

    case MACA_STATE_SLEEPING:
    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry maca_send() again.

        // @todo

        ret = 0;
        break;
    }

    return ret;
}

/** 
 * broadcast a frame out. the difference between broadcast() and send() is that the 
 * broadcast function will fill the destination address field with CONFIG_MACA_BROADCAST_ADDRESS
 * to indicate the frame should be broadcasted to all neighbor nodes.
 */
uintx maca_broadcast( TiMACA * mac, TiFrame * frame, uint8 option )
{
    uintx ret=0;
    TiIEEE802Frame154Descriptor * desc;

    switch (mac->state)
    {
    case MACA_STATE_IDLE:
        frame_totalcopyfrom( mac->txbuf, frame );

        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
        frame_skipouter( mac->txbuf, 12, 2 );
        desc = ieee802frame154_format( &(mac->desc), frame_startptr(frame), frame_capacity(frame), FRAME154_DEF_FRAMECONTROL_DATA_NOACK );
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, mac->seqid );
	    ieee802frame154_set_panto( desc, mac->panto );
	    ieee802frame154_set_shortaddrto( desc, FRAME154_BROADCAST_ADDRESS );
	    ieee802frame154_set_panfrom( desc, mac->panfrom );
	    ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );

        mac->sendoption = option;

        #ifdef CONFIG_MACA_STANDARD
        _maca_trysend( mac, mac->txbuf, option );
        #endif

        #ifndef CONFIG_MACA_STANDARD
        if (_maca_ischannelclear(mac))
        {
            _maca_trysend( mac, mac->txbuf, option );
        }
        else{
            mac->backoff = rand_uint8( CONFIG_MACA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = MACA_STATE_BACKOFF;
        }
        #endif

        ret = frame_capacity( mac->txbuf );
        maca_evolve( mac, NULL );
        break;

    case MACA_STATE_BACKOFF:
        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //
        maca_evolve( mac, NULL );
        ret = 0;
        break;

    case MACA_STATE_SLEEPING:
    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry aloha_send() again.

        // @todo

        ret = 0;
        break;
    }

    return ret;
}

uintx maca_recv( TiMACA * mac, TiFrame * frame, uint8 option )
{
	uint8 count;

    _maca_tryrecv( mac, mac->rxbuf, option );

    // if there're frame inside mac->rxbuf, then copy it to the parameter "frame"
    // for output and reset mac->rxbuf for future arriving frames.

    if (!frame_empty(mac->rxbuf))
    {
        frame_totalcopyfrom( frame, mac->rxbuf );
        count = frame_capacity( frame );

        // clear the frame buffer, and prepare to accepting next frame.
        frame_reset( mac->rxbuf, MACA_HEADER_SIZE+1, 2, 0 );
    }
    else
        count = 0;

    maca_evolve( mac, NULL );
    return count;
}

/* Send the frame out. This function will operate the transceiver hardware to perform
 * transmission. It will not affect the state and content of the TiFrame object.
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
#ifdef CONFIG_MACA_TRX_SUPPORT_ACK
uintx _maca_trysend( TiMACA * mac, TiFrame * frame, uint8 option )
{
	uintx count=0;

    // @modified by openwsn on 2010.08.24
    // the MACA protocol needn't to check whether the channel is clear here, because 
    // the RTS/CTS mechanism and the state machine can guarantee the channel is often
    // almost clear. this is different from the CSMA protocol.
    //
	// while (!csma_ischannelclear(mac->rxtx))
	//    continue;

    // attention whether the sending process will wait for ACK or not depends on 
    // the "option" parameter.
    
    // assume: the RTS frame has already been set with correct network pan and addresses.
    _maca_set_rts( mac, mac->seqid, mac->panto, mac->shortaddrto, mac->shortaddrfrom, CONFIG_MACA_DURATION );
    _maca_broadcast_rts( mac );

    // wait for CTS frame in a specified time duration
    // assert( mac->timer is stopped ) now
    timer_restart( mac->timer, CONFIG_MACA_WAIT_CTS_DURATION, 0 );
    count = 0;
    while (!timer_expired(mac->timer))
    {
        count = mac->rxtx->recv( mac->rxtx->provider, &m_cts[0], MACA_CTS_SIZE, option );
        if (count > 0)
        {
            if (_maca_is_cts(mac, &m_cts[0], count))
            {
                timer_stop( mac->timer );
                break;
            }

            count = 0;
        }
    }

    // if received the CTS frame successfully in fixed time duration
    if (count > 0)
    {
        // attention whether the sending process will wait for ACK or not depends on 
        // "option" parameter.

        count = mac->rxtx->send( mac->rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );

        // count > 0 indicates sending successfully, otherwise sending failure
        if (count > 0)
        {
            mac->seqid ++;
	        mac->retry = 0;
            mac->state = MACA_STATE_IDLE;
        }
        else if (mac->retry >= CONFIG_MACA_MAX_RETRY)
        {    
	        mac->retry = 0;
            mac->state = MACA_STATE_IDLE;
            mac->sendfailed ++;
        }
        else{
            mac->retry++;
            mac->backoff = _maca_get_backoff( mac );
		    timer_restart( mac->timer, mac->backoff, 0 );
            mac->state = MACA_STATE_BACKOFF;
        }
    }
    else{
        // check for CTS frame failed
        mac->backoff = _maca_get_backoff( mac );
		timer_restart( mac->timer, mac->backoff, 0 );
        mac->state = MACA_STATE_BACKOFF;
    }

	return count;
}
#endif

/* Send the frame out. This function will operate the transceiver hardware to perform
 * transmission. It will not affect the state and content of the TiFrame object.
 *
 * call the PHY layer functions and try to send the frame immdietely. if ACK frame 
 * required, then wait for ACK. 
 * 
 * attention: this function is used internally. it doesn't care what kind of state
 * the TiMACA object is in now. 
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
	hal_assert(false);
    // todo
	return 0;
}
#endif


/**
 * Check the transceiver for possible arrival frame. 
 * 
 * @attention: assume the "frame" in the paramter list has already prepared to accepting
 * new arrival frame.
 *  
 * @attention: This function should be called frequently in order to catching the
 * RTS frame arrived. if the checking process is too slow, then it may cause RTS
 * lossing.
 *
 * @param frame an empty frame object prepared to accept new arrival frame.
 * @param option control the transceiver's recving behavior. it will be explained 
 *   by the transceiver.
 */
uintx _maca_tryrecv( TiMACA * mac, TiFrame * frame, uint8 option )
{
    TiFrameTxRxInterface  * rxtx = mac->rxtx;
	uintx count;
    char * ptr;

    if (mac->state == MACA_STATE_BACKOFF)
    {
        // to avoid conflications with the sending process, because in this implemention,
        // the sending process and receiving process shares one timer only. so we 
        // give up receiving while sending is active.
        return 0;
    }

    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
    // assert: the following function call must be success and correct!
    //
    frame_skipouter( frame, MACA_HEADER_SIZE, MACA_TAIL_SIZE );

    // check for the transceiver to retrieve possible arrival frame. attention here
    // the frame can be any type. you should distinguish the each type and branching
    // to appropriate processing.
    //
    count = rxtx->recv( rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );

	if (count > 0)
    {
        // the first byte in the frame buffer is the length byte. it represents the 
        // MPDU length. after received the frame, we first check whether this is an
        // incomplete frame or not. if it's an bad frame, then we should ignore it.
        //
        ptr = frame_startptr(frame);
        if (*ptr == count-1)
        {
            // get the pointer to the frame control field according to 802.15.4 frame format
            // we need to check whether the current frame is a DATA type frame.
			// only the DATA type frame will be transfered to upper layers. The other types, 
            // such as COMMAND, BEACON and ACK will be ignored here.
			// buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
            ptr ++;
			if (FCF_FRAMETYPE(*(ptr)) != FCF_FRAMETYPE_DATA)
			{
				count = 0;
			}
			else{
                frame_setlength( frame, count );
                frame_setcapacity( frame, count );
			}

            // "count" is 0 if this is an invalid frame
        }

        if (count > 0)
        {
            // if the frame received is an RTS frame, then we should continue to send the 
            // CTS frame back and then wait for the DATA frame.
            //
			dbc_putchar(0x22);
            if (_maca_is_rts(mac, frame_startptr(frame), frame_capacity(frame)))
            {
                _maca_set_cts_from_rts( mac, m_cts, frame_startptr(frame) );
                _maca_broadcast_cts(mac);

                // RTS duration is decided by the sender. it's actually the channel request
                // period. in this period, all the other nodes should keep silence and the
                // receiver can wait for the frame. however, if the receiver cannot receive
                // an valid data frame during this process, then this function should be
                // failed.
                timer_setinterval( mac->timer, get_rts_duration(mac, &m_rts[0]), 0 );
                timer_start( mac->timer );

                // try to receive a data frame in the specified time duration. RTS/CTS 
                // frame received in this stage will be ignored.
                count = 0;
                while (!timer_expired(mac->timer))
                {
                    count = rxtx->recv( rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );
                    if (count > 0)
                    {
                        if (!_maca_is_data(mac, frame_startptr(frame), frame_capacity(frame)))
                            continue;

                        timer_stop( mac->timer );
                        break;
                    }
                }
                // "count" is 0 is timer expired. this indicates there'no frame received.
            }
            else if (_maca_is_cts(mac, frame_startptr(frame), frame_capacity(frame)))
            {
                count = 0;
            }
            else if (_maca_is_data(mac, frame_startptr(frame), frame_capacity(frame)))
            {
                // do nothing;
            }
            else{
                // other non-support frame types should be better ignored.
                count = 0;
            }

            // count equals 0 means the current frame received should be dropped.
        }
    }

    frame_moveinner( frame );

    if (count > 0)
    {
        frame_setlength( frame, count - MACA_HEADER_SIZE - MACA_TAIL_SIZE );
        frame_setcapacity( frame, count - MACA_HEADER_SIZE - MACA_TAIL_SIZE );
    }
    else{
        frame_setlength( frame, 0 );
    }

    return count;
}

/* currently, the duration is configured as:
 * loadfactor => duration
 *      0           1
 *      1           1*8
 *      2           2*8
 *      3           3*8
 *      4           4*8
 *      ...
 * 
 * @attention: the current delay duration algorithm is for demonstration only. you
 * should adjust it according to your own designs.
 */
inline uint16 _maca_get_backoff( TiMACA * mac )
{
    // algorithm 1
    // return rand_uint16(((uint16)(mac->loadfactor))  << 3);

    // algorithm 2
    uint16 backoff = CONFIG_MACA_MIN_BACKOFF + rand_uint16( mac->backoff << 1 );
    if (backoff > CONFIG_MACA_MAX_BACKOFF)
            backoff = CONFIG_MACA_MAX_BACKOFF;
    return backoff;
}

/* this function can be used as TiFrameTxRxInterface's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using maca_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void maca_evolve( void * macptr, TiEvent * e )
{
	TiMACA * mac = (TiMACA *)macptr;
    TiFrameTxRxInterface * rxtx = mac->rxtx;

    if (mac->state == MACA_STATE_SHUTDOWN)
        return;

    switch (mac->state)
    {
	case MACA_STATE_IDLE:   
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
            // todo
            // Q: shall we need to sleep the timer here? if we sleep it, when can 
            // we wakeup it?
            // timer_stop( mac->timer );
            rxtx->switchtomode( mac->rxtx->provider, FTRX_MODE_SLEEP );
            bit8_clr( mac->request, MACA_SLEEP_REQUEST );
            mac->state = MACA_STATE_SLEEPING;
        }
        break;

    case MACA_STATE_BACKOFF:
        // backoff state is essentially the state that waiting for the physical channel
        // for later sending.
        // if sending random delay timer expired, then call PHY layer functions to 
        // send the frame immediately.
        //
        if (timer_expired(mac->timer))
        {
            // @attention
            // the channel should be clear now. however, you shouldn't add the assertion
            // here because it may failed in some very rare cases. however, the application
            // can recover from these cases. so we needn't introduce more unstable behavior
            // by enabling this assertion.
            // hal_assert( _csma_ischannelclear(mac) );

            // while (1) 
            // {
            //     if (_maca_ischannelclear(mac)) 
            //         break;
            // }

            _maca_trysend( mac, mac->txbuf, mac->option );
        }
        maca_evolve( mac, NULL );
        break;

    case MACA_STATE_SLEEPING:
        if (bit8_get(mac->request, MACA_WAKEUP_REQUEST))
        {
            rxtx->switchtomode( mac->rxtx->provider, FTRX_MODE_RX );
            bit8_clr( mac->request, MACA_WAKEUP_REQUEST );
            mac->state = MACA_STATE_IDLE;
        }
        break;

    default:
        // this should never happen.  the following source code can pull the state 
        // to IDLE and let the state machine to continue in case of state disorders.
        hal_assert( false );
        rxtx->switchtomode( mac->rxtx->provider, FTRX_MODE_RX );
        timer_stop( mac->timer );
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
            frame_reset( mac->rxbuf, MACA_HEADER_SIZE+1, 2, 0 );
            frame_reset( mac->txbuf, MACA_HEADER_SIZE+1, 2, 0 );

            rxtx->switchtomode( rxtx->provider, FTRX_MODE_POWERDOWN );
            bit8_clr( mac->request, MACA_SHUTDOWN_REQUEST );
            mac->state = MACA_STATE_SHUTDOWN;
        }

        if (bit8_get(mac->request, MACA_STARTUP_REQUEST))
        {
            if (mac->state == MACA_STATE_SHUTDOWN)
            {
                rxtx->switchtomode( rxtx->provider, FTRX_MODE_RX );
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

            // call _maca_tryrecv() to move the frame from transceiver's internal
            // buffer to mac's local inside buffer "rxbuf".
            //
            _maca_tryrecv( mac, mac->rxbuf, 0x00 );

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
    uint16 ctrl = FRAME154_DEF_FRAMECONTROL_DATA_NOACK;

    hal_assert( sizeof(m_cts) == 15 );
    memset( &m_rts, 0x00, sizeof(m_rts) );
    m_cts[0] = 12;                      // frame length. not including the length byte itself
    m_cts[1] = (char)(ctrl >> 8);       // frame control
    m_cts[2] = (char)(ctrl & 0xFF);
    m_cts[12] = 0x01;                   // command 0x01 indicates this is a RTS frame
}

/*
 *  +------------------+-------------+-------+-------+------------+
 *  | Frame Control 2B | Sequence 1B | RA 4B | TA 4B | Command 1B |
 *  +------------------+-------------+-------+-------+------------+
 */
void _maca_init_rts( TiMACA * mac )
{
    uint16 ctrl = FRAME154_DEF_FRAMECONTROL_DATA_NOACK;

    hal_assert( sizeof(m_rts) == 14 );
    memset( &m_rts, 0x00, sizeof(m_rts) );
    m_rts[0] = 11;                      // frame length. not including the length byte itself
    m_rts[1] = HIGH_BYTE(ctrl);         // frame control
    m_rts[2] = LOW_BYTE(ctrl);
    m_rts[11] = 0x02;                   // command 0x02 indicates this is a RTS frame
}

void _maca_set_rts( TiMACA * mac, uint8 seqid, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom, uint8 duration )
{
    m_rts[3]  = seqid;
    m_rts[4]  = HIGH_BYTE(pan);
    m_rts[5]  = LOW_BYTE(pan);
    m_rts[6]  = HIGH_BYTE(shortaddrto);
    m_rts[7]  = LOW_BYTE(shortaddrto);
    m_rts[8]  = HIGH_BYTE(pan);
    m_rts[9]  = LOW_BYTE(pan);
    m_rts[10] = HIGH_BYTE(shortaddrfrom);
    m_rts[11] = LOW_BYTE(shortaddrfrom);
 //   m_rts[13] = duration;
}

/*
void _maca_set_cts( TiMACA * mac, uint8 seqid, uint16 pan, uint16 shortaddrto, uint16 shortaddrfrom )
{
    m_cts[3]  = seqid;
    m_cts[4]  = HIGH_BYTE(pan);
    m_cts[5]  = LOW_BYTE(pan);
    m_cts[6]  = HIGH_BYTE(shortaddrto);
    m_cts[7]  = LOW_BYTE(shortaddrto);
    m_cts[8]  = HIGH_BYTE(pan);
    m_cts[9]  = LOW_BYTE(pan);
    m_cts[10] = HIGH_BYTE(shortaddrfrom);
    m_cts[11] = LOW_BYTE(shortaddrfrom);
}
*/

/**
 * set the CTS frame buffer content from the RTS frame received.
 * 
 * @param cts CTS frame memory buffer
 * @param rts RTS frame memory buffer
 */
void _maca_set_cts_from_rts( TiMACA * mac, char * cts, char * rts )
{
    // cts[3] is the sequence id of this frame. it will be set to the rts sequence id.
    // and then switch RTS's pan/address information as the new CTS's pan/address.
    cts[3]  = rts[3];
    cts[4]  = rts[8];
    cts[5]  = rts[9];
    cts[6]  = rts[10];
    cts[7]  = rts[11];
    cts[8]  = rts[4];
    cts[9]  = rts[5];
    cts[10] = rts[6];;
    cts[11] = rts[7];
}

/**
 * return the RTS duration value in the rts buffer. the RTS duration value is the 
 * channel request duration set by the sender node in the RTS frame.
 */
uint8 get_rts_duration( TiMACA * mac, char * rts )
{
    return rts[13];
}

bool _maca_is_rts( TiMACA * mac, char * buf, uint8 len )
{
    return ((len == MACA_RTS_SIZE) && (buf[12] == 0x01));
}

bool _maca_is_cts( TiMACA * mac, char * buf, uint8 len )
{
    return ((len == MACA_CTS_SIZE) && (buf[12] == 0x02));
}

bool _maca_is_data( TiMACA * mac, char * buf, uint8 len )
{
    return ((len == MACA_CTS_SIZE) && (buf[12] == 0x00));
}

/** 
 * this function will send an RTS frame out immediately by calling the send() function
 * in the lower level transceiver's interface. 
 */
uintx _maca_broadcast_rts( TiMACA * mac )
{
    TiFrameTxRxInterface * rxtx = mac->rxtx;

    while (1)
    {
        if (rxtx->send( rxtx->provider, &m_rts[0], sizeof(m_rts), 0x00) > 0)
            break;

        rxtx->evolve( rxtx->provider, NULL );
    }

    return 1;
}

/** 
 * this function will broadcast the CTS frame out immediately by calling the send() function
 * in the lower level transceiver's interface. 
 */
uintx _maca_broadcast_cts( TiMACA * mac )
{
    TiFrameTxRxInterface * rxtx = mac->rxtx;

    while (1)
    {
        if (rxtx->send( rxtx->provider, &m_cts[0], sizeof(m_cts), 0x00) > 0)
            break;

        rxtx->evolve( rxtx->provider, NULL );
    }

    return 1;
}
