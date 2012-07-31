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

/******************************************************************************* 
 * svc_tinymac
 * TiTinyMAC is based on the TiFrameTxRxInterface interface directly. It transforms 
 * the memory buffer based transceiver interface into a TiFrame based interface and
 * also support the Medium Access Control (MAC) interface.
 * 
 * @status
 *	- developing
 *
 ******************************************************************************/

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "svc_tinymac.h"

//static uintx _tinymac_trysend( TiTinyMAC * mac, char * buf, uint8 len, uint8 option );
static intx _tinymac_trysend( TiTinyMAC * mac, char * buf, uint8 len, uint8 option );

TiTinyMAC * tinymac_construct( char * buf, uintx size )
{
	memset( buf, 0x00, size );
	hal_assert( sizeof(TiTinyMAC) <= size );
	return (TiTinyMAC *)buf;
}

void tinymac_destroy( TiTinyMAC * mac )
{
	return;
}

TiTinyMAC * tinymac_open( TiTinyMAC * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid, 
	uint16 address, TiFunEventHandler listener, void * lisowner, uint8 option )
{
    void * provider;

	rtl_assert((rxtx != NULL) );

    // assume: the rxtx driver has already been opened.

    mac->rxtx = rxtx;
	mac->listener = listener;
	mac->lisowner = lisowner;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;
	mac->option = option;


	// we enable ACK mechanism by default
    provider = rxtx->provider;
	rxtx->setchannel( provider, chn );
	rxtx->setpanid( provider, panid );
	rxtx->setshortaddress( provider, address );
    rxtx->enable_addrdecode( provider );
	rxtx->enable_autoack( provider );

    ieee802frame154_open( &(mac->desc) );

    return mac;
}

void tinymac_close( TiTinyMAC * mac )
{
	// timer_close( mac->timer );
}

uintx tinymac_send( TiTinyMAC * mac, TiFrame * frame, uint8 option )
{
	TiIEEE802Frame154Descriptor * desc;
    uintx ret;

    // according to 802.15.4 specification:
    // header 12 B = 1B frame length (required by the transceiver driver currently) 
    //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
    //  + 2B source pan + 2B source address
    //
	frame_skipouter( frame, 12, 2 );
   
    desc = ieee802frame154_format( &(mac->desc), frame_startptr(frame), frame_capacity(frame), FRAME154_DEF_FRAMECONTROL_DATA  );
    rtl_assert( desc != NULL );

    ieee802frame154_set_sequence( desc, mac->seqid );
	ieee802frame154_set_panto( desc, mac->panto );
	ieee802frame154_set_shortaddrto( desc, mac->shortaddrto );
	ieee802frame154_set_panfrom( desc, mac->panfrom );
	ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );
  
	ret = _tinymac_trysend( mac, frame_startptr(frame), frame_capacity(frame), option ); 
    frame_moveinner( frame );
    return ret;
}

uintx tinymac_broadcast( TiTinyMAC * mac, TiFrame * frame, uint8 option )
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

    ret = _tinymac_trysend( mac, frame_startptr(frame), frame_capacity(frame), option ); 
    frame_moveinner( frame );
    return ret;
}

uintx tinymac_recv( TiTinyMAC * mac, TiFrame * frame, uint8 option )
{
    const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	uint8 count;
    char * ptr = NULL;
    
    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
    frame_skipouter( frame, HEADER_SIZE, TAIL_SIZE );

    // assert: the skipouter must be success
    count = mac->rxtx->recv( mac->rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );


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
        // dbc_putchar( 0x44 );
    }
    else{
        frame_setlength( frame, 0 );
		// dbc_putchar( 0x55 );
    }
    
	return count;
}

/** 
 * try send a frame out through the transceiver. the frame should be already placed 
 * inside buffer. 
 * 
 * Suggest delay a random little period between two adjancent transmissions.
 *
 * this function is shared by tinymac_send() and tinymac_broadcast().
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx _tinymac_trysend( TiTinyMAC * mac, char * buf, uint8 len, uint8 option )
{    
	uintx count=0;

    if (len == 0)
		return 0;

	count = mac->rxtx->send( mac->rxtx->provider, buf, len, option );
	if (count > 0)
	{
        mac->seqid ++;
    }

	return count;	
}

/* this function can be used as TiCc2420Adapter's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using tinymac_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void tinymac_evolve( void * macptr, TiEvent * e )
{
	TiTinyMAC * mac = (TiTinyMAC *)macptr;

    mac->rxtx->evolve( mac->rxtx, NULL );

	if (e)
	{
		switch (e->id)
		{
		case EVENT_DATA_ARRIVAL:
			// If the AOHA listener isn't NULL, then query the transceiver to get possible 
			// incomming frames. If has, then report to the listener owner.

			// todo
			if (mac->listener != NULL)
			{
				//mac->rxlen = tinymac_recv(mac, &(mac->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00);
				//if (mac->rxlen > 0)
					mac->listener( mac->lisowner, e );
			}

		case 1:
			break;
		}
	}

	return;
}

