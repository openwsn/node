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

/****************************************************************************** 
 * svc_aloha.c
 * standard ALOHA medium access control (MAC) protocol 
 *  
 * @attention
 *	- This version will wait for the ACK frame after sending a DATA frame immediately.
 * If another DATA frame arrives during this period, it will be ignored. This can be 
 * improved in the future. 
 *
 *	- Different to the basic ALOHA protocol, this version will check whether the
 * communication channel is clear or not. It will only send the frame when the channel
 * is clear. This clear channel assesement (CCA) functionality is provided by 
 * the cc2420 transceiver.
 *
 *  - When encounting sending conflicts, this version of ALOHA will retry for some 
 * times. The retry count is configures by macro CONFIG_ALOHA_MAX_RETRY. However, 
 * there's no backoff when retry the sending, so there's still large chance to 
 * be conflicted again. This should be improved before this version of ALOHA can 
 * be used in real applications. 
 *
 *  - This version doesn't depend on "osx" functionalities. So it's easy to be used.
 * The enhanced version of this simple aloha is in svc_openmac and svc_adaptivealoha
 *
 *	In Summary, this version of ALOHA is mainly for demonstration and experiments. 
 * It's a good start to be used for teaching than in real applications. 
 *
 * @author zhangwei and shan-lei on 20050710
 *	- first version on PIC16F876. This version is fully discarded.
 * @modified by zhangwei on 20090610
 *	- fully revised for ICT GAINZ platform
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * @modified by zhangwei on 20090802
 *  - correct bug in _aloha_tryrecv(). The former byte order to create the frame control
 *    word is wrong. 
 * @modified by Shimiaojing on 20091025
 *  -add macro_alohachannelclear(mac) HAL_READ_CC_CCA_PIN()and delete line141 142 and match the frame construct of 
 * MAC opf_buffer &opf_datalen id different
 * line 89 147 291 who modified it  
 *  solved ok    2009 .11.17
 * 
 * @modified by zhangwei on 20091201
 *	- add random backoff time support to delay if confliction encountered.
 *
 *****************************************************************************/

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "svc_aloha.h"

static uintx _aloha_trysend( TiAloha * mac, char * buf, uint8 len, uint8 option );
static uintx _aloha_tryrecv( TiAloha * mac, char * buf, uint8 len, uint8 option );

TiAloha * aloha_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiAloha) <= size );
	memset( buf, 0x00, size );
	return (TiAloha *)buf;
}

void aloha_destroy( TiAloha * mac )
{
    timer_stop( mac->timer );
	return;
}

TiAloha * aloha_open( TiAloha * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option )
{
    void * provider;

    // assert( the rxtx driver has already been opened );
    // assert( mac->timer is already opened but not start yet );

	rtl_assert((rxtx != NULL) && (timer != NULL));

	mac->state = ALOHA_STATE_IDLE;
    mac->rxtx = rxtx;
	mac->timer = timer;
	mac->listener = listener;
	mac->lisowner = lisowner;
	mac->retry = 0;
	mac->backoff = CONFIG_ALOHA_MIN_BACKOFF;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;
    mac->sendoption = 0x00;
    mac->sendfailed = 0;
	mac->option = option;
    mac->txbuf = frame_open( &(mac->txbuf_memory[0]), FRAME_HOPESIZE(CONFIG_ALOHA_MAX_FRAME_SIZE), 0, 0, 0 );
	
    // @modified by zhangwei on 2010.08.21
    // @attention: for all hardware components, you should construct and open them 
    // in the caller function to avoid potential conflictions. so we don't recommend
    // initialize the timer component here.

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

    ieee802frame154_open( &(mac->desc) );

    // initialize the random number generator with a random seed. you can replace 
    // the seed with other values.
    //
    // @attention: generally, the random generator can be initialized when the whole
    // application started. if so, you needn't call rand_open() to initialize it 
    // again here.

    rand_open( 0x3212 );

    return mac;
}

void aloha_close( TiAloha * mac )
{
	timer_stop( mac->timer );
	mac->state = ALOHA_STATE_NULL;
}

uintx aloha_send( TiAloha * mac, TiFrame * frame, uint8 option )
{
	TiIEEE802Frame154Descriptor * desc;
    uintx ret=0;
    switch (mac->state)
    {
    case ALOHA_STATE_IDLE:

        frame_totalcopyfrom( mac->txbuf, frame );

        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
        //frame_dump(mac->txbuf);
        frame_skipouter( mac->txbuf, 12, 2 );
        desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), FRAME154_DEF_FRAMECONTROL_DATA );
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, mac->seqid ); 
        ieee802frame154_set_panto( desc, mac->panto );
        ieee802frame154_set_shortaddrto( desc, mac->shortaddrto );
        ieee802frame154_set_panfrom( desc, mac->panfrom );
        ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );
        
        //frame_moveinner( mac->txbuf );
        mac->sendoption = option;

        // standard aloha protocol behavior

        #ifdef CONFIG_ALOHA_STANDARD
        /*
         * @modified by XuFuzhen on 2010.09.29
         * - bug fix. should replace frame_length() here with frame_capacity()
         */
        if (mac->rxtx->ischnclear(mac))
        {
			_aloha_trysend( mac, frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), option );
        }
        else{
            mac->backoff = rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = ALOHA_STATE_BACKOFF;
        }
        #endif

        // optimized aloha protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
        #ifndef CONFIG_ALOHA_STANDARD
        mac->backoff = rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 0 );
        timer_start( mac->timer );
        mac->state = ALOHA_STATE_BACKOFF;
        #endif

        /* @modified by openwsn on 2010.08.30
         * - bug fix. you should place the frame_length() call before aloha_evolve() 
         * because aloha_evolve() may send mac->txbuf and clear it. this will cause
         * frame_length() returns 0 instead of the real frame length.
         * 
         * @modified by XuFuzhen on 2010.09.29
         * - bug fix. should replace frame_length() here with frame_capacity()
         */
        ret = frame_capacity( mac->txbuf );
        aloha_evolve( mac, NULL );
        break;

    case ALOHA_STATE_BACKOFF:

        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //
        aloha_evolve( mac, NULL );
        ret = 0;
        break;

    case ALOHA_STATE_SLEEPING:

    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry aloha_send() again.
        ret = 0;
        break;
    }

    return ret;
}

uintx aloha_broadcast( TiAloha * mac, TiFrame * frame, uint8 option )
{
    uintx ret=0;
    TiIEEE802Frame154Descriptor * desc;

    switch (mac->state)
    {
    case ALOHA_STATE_IDLE:

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

        // char * fcf;
        // char * shortaddrto;

        // fcf = frame_startptr(frame);
        // shortaddrto = (char*)(frame_startptr(frame)) + 3;  // todo: according to IEEE 802.15.4 format, 加几？请参考15.4文档确认


        // for broadcasting frames, we don't need acknowledgements. so we clear the ACK 
        // REQUEST bit in the frame control field. 
        // refer to 802.15.4 protocol format
        //
        // fcf ++;
        // (*fcf) &= 0xFA; // TODO: this value should changed according to 802.15.4 format 

        // 0xFFFFFFFF the broadcast address according to 802.15.4 protocol format
        // attention: we only set the destination short address field to broadcast address.
        // the destination pan keeps unchanged.
        //
        // *shortaddrto ++ = 0xFF;
        // *shortaddrto ++ = 0xFF;

        //frame_moveinner( mac->txbuf );
        mac->sendoption = option;

        // standard aloha protocol behavior
        #ifdef CONFIG_ALOHA_STANDARD
        if (mac->rxtx->ischnclear(mac))
        {
            _aloha_trysend( mac, frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), option );
        }
        else{
            mac->backoff = rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = ALOHA_STATE_BACKOFF;
        }
        #endif

        // optimized aloha protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
        #ifdef CONFIG_ALOHA_STANDARD
        mac->backoff = rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 0 );
        timer_start( mac->timer );
        mac->state = ALOHA_STATE_BACKOFF;
        #endif

        ret = frame_capacity( mac->txbuf );
        aloha_evolve( mac, NULL );
        break;

    case ALOHA_STATE_BACKOFF:

        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //
        aloha_evolve( mac, NULL );
        ret = 0;
        break;

    case ALOHA_STATE_SLEEPING:

    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry aloha_send() again.
        ret = 0;
        break;
    }

    return ret;
}

/**
 * Check whether there's some frame arrivaled. This function can be called anytime. 
 */
uintx aloha_recv( TiAloha * mac, TiFrame * frame, uint8 option )
{
    const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	uint8 count;
    char * ptr=NULL;

    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.

    frame_skipouter( frame, HEADER_SIZE, TAIL_SIZE );
    // assert: the skipouter must be success
	count = _aloha_tryrecv( mac, frame_startptr(frame), frame_capacity(frame), option );
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
        }
    }
    frame_moveinner( frame );

    if (count > 0)
    {
        frame_setlength( frame, count - HEADER_SIZE - TAIL_SIZE );
        frame_setcapacity( frame, count - HEADER_SIZE - TAIL_SIZE );
    }
    else{
        frame_setlength( frame, 0 );
    }

	aloha_evolve( mac, NULL );
	return count;
}

/** 
 * try send a frame out through the transceiver. the frame should be already placed 
 * inside buffer. 
 * 
 * this function is shared by aloha_send() and aloha_broadcast().
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. need call this function to retry.
 *  - 1:  retry reach maximum count. failed the whole sending 
 */
uintx _aloha_trysend( TiAloha * mac, char * buf, uint8 len, uint8 option )
{    
	uintx count=0;

	// while (!csma_ischannelclear(mac->rxtx))
	//    continue;

    // attention whether the sending process will wait for ACK or not depends on 
    // "option" parameter.

    count = mac->rxtx->send( mac->rxtx->provider, buf, len, option );
    if (count > 0)
    {       
		mac->seqid ++;
        mac->retry = 0;
        mac->state = ALOHA_STATE_IDLE;
    }
    else{
        if (mac->retry >= CONFIG_ALOHA_MAX_RETRY)
        {    
            mac->retry = 0;
            mac->state = ALOHA_STATE_IDLE;
            mac->sendfailed ++;
        }
        else{
            mac->retry++;
            mac->backoff = CONFIG_ALOHA_MIN_BACKOFF + rand_uint8( mac->backoff << 1 );
            if (mac->backoff > CONFIG_ALOHA_MAX_BACKOFF)
                mac->backoff = CONFIG_ALOHA_MAX_BACKOFF;
            timer_restart( mac->timer, mac->backoff, 0 );
            mac->state = ALOHA_STATE_BACKOFF;
        }
    }

    return count;
}

uintx _aloha_tryrecv( TiAloha * mac, char * buf, uint8 len, uint8 option )
{
    TiFrameTxRxInterface  * rxtx = mac->rxtx;
	uintx count;

    // @attention: According to ALOHA protocol, the program should send ACK/NAK after 
    // receiving a data frame. however, this is done by the low level transceiver's
    // adapter component (TiCc2420Adapter), so we needn't to send ACK manually here.

    count = rxtx->recv( rxtx->provider, buf, len, option );
	if (count > 0)
	{   
        // possible DATA frame check and ACK processing here.
	}

	return count;
}

/* this function can be used as TiCc2420Adapter's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using aloha_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void aloha_evolve( void * macptr, TiEvent * e )
{
	TiAloha * mac = (TiAloha *)macptr;

    mac->rxtx->evolve( mac->rxtx, NULL );

    // try sending the frame in mac->txbuf. if sending successfully, then transfer
    // to IDLE state. if failed, then still in WAITFOR_SENDING state.

    switch (mac->state)
    {
    case ALOHA_STATE_BACKOFF:
        if (timer_expired(mac->timer))
        {
            // if _aloha_trysend() returns positive value, then it means the frame
            // has been sent successfully. if it returns negative value, then it means
            // the frame has reached its maximum retry count. 
            // 
            // in the above two cases, the state should transfer to IDLE state. 
            // if the frame is failed sending and need retry, then _alohs_trysend() will 
            // return 0;
            
            // @modified by zhangwei on 2010.08.20
            // there're three cases for the result of _aloha_trysend(). if it return 
            // positive value, then it means the frame has been sent successfully. 
            // if it return 0, then means it sends nothing and you should retry the
            // sending again. if it returns negtive value, then it means this frame
            // has reached its maximum number retry count and should report sending
            // failure.
            //
            _aloha_trysend(mac, frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), mac->sendoption );
        }
        break;
    
    case ALOHA_STATE_IDLE:
        break;

    case ALOHA_STATE_SLEEPING:
        /* 
        if (e->id = wakeuprequest)
        {
            phy_wakeup();
            mac->state = ADTALOHA_IDLE;
        }
        */

    default:
        break;
	}

    // attention the following process can occur in any state.
	if (e != NULL)
	{
		switch (e->id)
		{
		case EVENT_DATA_ARRIVAL:
			if (mac->listener != NULL)
			{
				mac->listener( mac->lisowner, e );
			}
            break;
        /*
        case ADTALOHA_EVENT_SHUTDOWN_REQUEST:
            // no matter what the current state is, then you can do shutdown
            timer_stop(); 
            phy_shutdown();
            mac->state = SHUTDOWN;
            break;    

        case ADTALOHA_EVENT_STARTUP_REQUEST: 
            if (mac->state == SHUTDOWN)
            {
                phy_startup();
                mac->state = IDLE;
            }
        */
		default:
			break;
		}
	}

	return;
}

void aloha_statistics( TiAloha * mac, uint16 * sendcount, uint16 * sendfailed )
{
    *sendcount = 0;
    *sendfailed = mac->sendfailed;
    mac->sendfailed = 0;
}
