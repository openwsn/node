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
 * svc_simplealoha.c
 * simple ALOHA medium access control (MAC) protocol 
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
 *  - correct bug in _aloha_recv(). The former byte order to create the frame control
 *    word is wrong. 
 * @modified by Shimiaojing on 20091025
 *  -add macro_alohachannelclear(mac) HAL_READ_CC_CCA_PIN()and delete line141 142 and match the frame construct of 
 * MAC opf_buffer &opf_datalen id different
 * line 89 147 291 who modified it  
 *  solved ok    2009 .11.17
 * 
 * @modified by zhangwei on 20091201
 *	- add random backoff time support to delay if confliction encountered.
 * @modified by zhangwei on 2010.06.12
 *  - rename the module from svc_aloha to svc_simplealoha
 *  - revision. 
 * 
 * @modified by zhangwei on 2010.07.18
 *  - replace TiSimpleAloha with TiSimpleAloha, and "aloha_" with "saloha_". this is to 
 *    avoid conflictions with module "svc_aloha"
 *
 * @modified by zhangwei on 2010.08.07
 *  - revised. bug fixed.
 *
 * @modified by zhangwei on 2010.08.13
 *  - correct bugs in changing mac->seqid. the sequence id should be increased after
 *    sending successfully. (is this right?)
 *
 *****************************************************************************/

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "svc_simplealoha.h"

static uint16 _saloha_get_backoff( TiSimpleAloha * mac );
static uintx _saloha_trysend( TiSimpleAloha * mac, char * buf, uint8 len, uint8 option );
static uintx _saloha_tryrecv( TiSimpleAloha * mac, char * buf, uint8 len, uint8 option );

TiSimpleAloha * saloha_construct( char * buf, uintx size )
{
	memset( buf, 0x00, size );
	hal_assert( sizeof(TiSimpleAloha) <= size );
	return (TiSimpleAloha *)buf;
}

void saloha_destroy( TiSimpleAloha * mac )
{
	return;
}

TiSimpleAloha * saloha_open( TiSimpleAloha * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option )
{
    void * provider;

	rtl_assert((rxtx != NULL) && (timer != NULL));
    timer = timer;

    // assume: the rxtx driver has already been opened.

    mac->rxtx = rxtx;
	//mac->timer = timer;
	mac->listener = listener;
	mac->lisowner = lisowner;
	mac->retry = 0;
	mac->backoff = CONFIG_ALOHA_BACKOFF;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;
	mac->option = option;

	// timer_open( timer, 0, NULL, NULL, 0x00 );
	/* we needn't repeatedly triggered timer */
	// timer_setinterval( mac->timer, CONFIG_ALOHA_ACK_RESPONSE_TIME, 0 );

	// for the aloha module, we should enable ACK mechanism
    provider = rxtx->provider;
	rxtx->setchannel( provider, chn );
	rxtx->setpanid( provider, panid );
	rxtx->setshortaddress( provider, address );
	//rxtx->setrxmode( provider );					
    rxtx->enable_addrdecode( provider );
	rxtx->enable_autoack( provider );

    ieee802frame154_open( &(mac->desc) );

    return mac;
}

void saloha_close( TiSimpleAloha * mac )
{
	// timer_close( mac->timer );
}

uintx saloha_send( TiSimpleAloha * mac, TiFrame * frame, uint8 option )
{
	TiIEEE802Frame154Descriptor * desc;
    uintx ret;

    // according to 802.15.4 specification:
    // header 12 B = 1B frame length (required by the transceiver driver currently) 
    //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
    //  + 2B source pan + 2B source address
    //
	frame_skipouter( frame, 12, 2 );
   
    desc = ieee802frame154_format( &(mac->desc), frame_startptr(frame), frame_capacity(frame), FRAME154_DEF_FRAMECONTROL_DATA );
    rtl_assert( desc != NULL );

    ieee802frame154_set_sequence( desc, mac->seqid );
	ieee802frame154_set_panto( desc, mac->panto );
	ieee802frame154_set_shortaddrto( desc, mac->shortaddrto );
	ieee802frame154_set_panfrom( desc, mac->panfrom );
	ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );
  
	ret = _saloha_trysend( mac, frame_startptr(frame), frame_capacity(frame), option ); 
    frame_moveinner( frame );
    return ret;
}

uintx saloha_broadcast( TiSimpleAloha * mac, TiFrame * frame, uint8 option )
{
    TiIEEE802Frame154Descriptor * desc;
    uintx ret;

    frame_skipouter( frame, 12, 2 );

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

    ret = _saloha_trysend( mac, frame_startptr(frame), frame_capacity(frame), option ); 
    frame_moveinner( frame );
    return ret;
}

uintx saloha_recv( TiSimpleAloha * mac, TiFrame * frame, uint8 option )
{
    const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	uint8 count;
    char * ptr = frame_startptr(frame);;
    
    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
    frame_skipouter( frame, HEADER_SIZE, TAIL_SIZE );
    // assert: the skipouter must be success

	count = _saloha_tryrecv( mac, frame_startptr(frame), frame_capacity(frame), option );

	if (count > 0)
	{
        // the first byte in the frame buffer is the length byte. it represents the 
        // MPDU length. after received the frame, we first check whether this is an
        // incomplete frame or not. if it's an bad frame, then we should ignore it.
        //
        ptr = frame_startptr(frame);
		//dbc_putchar( frame_capacity( frame ) );
		
        if (*ptr == count-1 )
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
    
	return count;
}

uint16 _saloha_get_backoff( TiSimpleAloha * mac )
{
    return 20 + rand_uint8( mac->backoff << 1 );
}

/** 
 * try send a frame out through the transceiver. the frame should be already placed 
 * inside buffer. 
 * 
 * this function is shared by saloha_send() and saloha_broadcast().
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx _saloha_trysend( TiSimpleAloha * mac, char * buf, uint8 len, uint8 option )
{    
    TiFrameTxRxInterface * rxtx = mac->rxtx;
	uintx count=0;

    dbo_putchar( 0x99 );
	if (len == 0)
		return 0;
  
	while (mac->retry < CONFIG_ALOHA_MAX_RETRY )
	{  
        dbo_putchar( 0xF0 );

        //while (!saloha_ischannelclear( mac->transceiver))
	    //continue;
    	
        // attention: you needn't set the the transceiver to TX mode manually. according
        // to the transceiver design, the transceiver will automatically change to TX mode
        // when you call send() function.

	    count = rxtx->send( rxtx->provider, buf, len, option );

        if (count > 0)
		{
		    //// todo : check whether this frame needs ACK

		    ///* If the frame has been successfully sent by the transceiver, the program will
		    // * start to wait for the ACK frame replied from the receiver. The maximum waiting
		    // * duration is configured by macro CONFIG_ALOHA_ACK_RESPONSE_TIME. The time unit
		    // * is milli-seconds.
		    // */
		    //cc2420_setrxmode( mac->transceiver );
		    //// todo
		    //// whether the cc2420 can be stable enough for receiving 
		    //// you may need to check the cc2420 status to guarantee this

		    //timer_start( mac->timer );
		    //expired = true;
		    //while (!timer_expired( mac->timer ))
		    //{
		    //	if (cc2420_read( mac->transceiver, &(mac->rxbuf[0]), CONFIG_CC2420_RXBUFFER_SIZE, 0x00 ) > 0)
		    //	{
		    //		/* 2 bytes for frame control */
		    //		ctrl = ((uint16)(mac->rxbuf[1]) << 8) | (mac->rxbuf[2]);

		    //		/* @attention
		    //		 * If the incoming frame is ACK frame, then we can return successfully. 
		    //		 * Other type frames will be ignored. 
		    //		 * -> This should be improved in the future. Theoretically speaking, the 
		    //		 * current frame arrived may possibly be the data type from other nodes. 
		    //		 * You should not ignore such types or else encounter unnecessary frame loss.
		    //		 */
		    //		if (FCF_FRAMETYPE(ctrl) == FCF_FRAMETYPE_ACK)
		    //		{
		    //			timer_stop( mac->timer );
		    //			mac->retry = 0;
		    //			expired = false;
		    //			break;
		    //		}

		    //		/* We needn't this frame. Simply ignore it */
		    //		mac->rxlen = 0;
		    //	}
		    //}
		    //
		    //if (expired)
		    //{
		    //	mac->retry ++;
		    //	count = 0;
		    //}

            mac->seqid ++;
            dbo_putchar( 0xF1 );
			mac->retry = 0;
			break;
		}

		/* Suggest delay a random little period and restart the sending process again 
		 * However, it's not easy to implement the random delay in a simple method. So
		 * we simply wait 300 milli-seconds here.
		 */

        dbo_putchar( 0xF2 );
		mac->backoff = _saloha_get_backoff( mac );
		hal_delay( mac->backoff );
		mac->retry++;
	}

	/* This function failed to send the frame for at least CONFIG_ALOHA_MAX_RETRY times. 
	 * we had no other good solution except reporting failure to the master function. */
	if (mac->retry >= CONFIG_ALOHA_MAX_RETRY)
	{    
		mac->retry = 0;
		count = 0;
	}

	return count;	
}

uintx _saloha_tryrecv( TiSimpleAloha * mac, char * buf, uint8 len, uint8 option )
{
    TiFrameTxRxInterface  * rxtx = mac->rxtx;
	uintx count;
  //  uint16 ctrl=0;

	/* According to ALOHA protocol, the program should send ACK/NAK after receiving
	 * a data frame. however, this is done by cc2420 transceiver. so nothing to 
	 * do here 
	 */
	count = rxtx->recv( rxtx->provider, buf, len, option );

	//if (count > 0) 
	//{   
        // @todo: you should judge whether the current frame is 802.15.4 DATA type
        // by checking the ctrl field in the frame. Only the 802.15.4 DATA frame
        // can be passed onto the higher layer.
		
		/* 2 bytes for frame control
		 * only the DATA type frame will be accepted here. The other types, such as COMMAND,
		 * BEACON or ACK will be ignored here.
         * buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
         */		 
		//ctrl = (((uint16)buf[2]) << 8) | (buf[1]);

		//if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
	//	{
		//	count = 0;
	//	}
	//}
    
	return count;
	
}

/* this function can be used as TiCc2420Adapter's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using saloha_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void saloha_evolve( void * macptr, TiEvent * e )
{
	TiSimpleAloha * mac = (TiSimpleAloha *)macptr;

    mac->rxtx->evolve( mac->rxtx, NULL );

	if (e)
	{
		switch (e->id)
		{
		case EVENT_DATA_ARRIVAL:
			/* If the AOHA listener isn't NULL, then query the transceiver to get possible 
			 * incomming frames. If has, then report to the listener owner. */

			// todo
			if (mac->listener != NULL)
			{
				//mac->rxlen = saloha_recv(mac, &(mac->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00);
				//if (mac->rxlen > 0)
					mac->listener( mac->lisowner, e );
			}

		case 1:
        default:
			break;
		}
	}

	return;
}

