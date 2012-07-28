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

/* CSMA/CA medium access control (MAC) protocol 
 * This module implements a p-insist carrier sense multi access MAC protocol.
 *
 * @author zhangwei and shan-lei on 20050710
 *	- first version on PIC16F876. This version is fully discarded.
 * @modified by zhangwei on 20060724
 *  - revised
 * @modified by zhangwei on 20090610
 *	- fully revised for ICT GAINZ platform
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - add state transfer machine implementation
 *
 * @modified by zhangwei on 2010.06.14
 *  - change the module name from svc_adaptaloha to svc_csma
 * 
 * @modified by openwsn on 2010.08.24
 *  - upgraded TiOpenFrame with TiFrame. fully revised.
 *
 * @modified by xu-fuzhen(Control Department, TongJi University) on 2010.10.12
 *  - revision. tested ok. 
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
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_csma.h"

/*  
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
 */

#define bit8_set(token,index) ((token) |= (_BV(index)))
#define bit8_get(token,index) ((token) &  (_BV(index)))
#define bit8_clr(token,index) ((token) &= (~_BV(index)))

static uintx _csma_trysend( TiCsma * mac, TiFrame * frame, uint8 option );
static uintx _csma_tryrecv( TiCsma * mac, TiFrame * frame, uint8 option );
static uint16 _csma_get_backoff( TiCsma * mac );

TiCsma * csma_construct( char * buf, uint16 size )  
{
    TiCsma * mac = (TiCsma *)buf;

	hal_assert( sizeof(TiCsma) <= size );
	memset( buf, 0x00, size );
    mac->state = CSMA_STATE_NULL;
	return mac;
}

void csma_destroy( TiCsma * mac )
{
	return;
}

TiCsma * csma_open( TiCsma * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac,uint8 chn, uint16 panid, uint16 address, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner )
{
    void * provider;

    // assert( the rxtx driver has already been opened );
    // assert( mac->timer is already opened but not start yet );

	hal_assert( (rxtx != NULL) && (timer != NULL) && (mac->state == CSMA_STATE_NULL) );

    // sendprob: sending probability

	mac->state = CSMA_STATE_IDLE;
    mac->rxtx = rxtx;
	mac->nac = nac;//todo
    mac->timer = timer;
    mac->sendprob = 60;
    mac->loadfactor = 5;
    mac->request = 0x00;
    mac->retry = 0;
	mac->listener = listener;
	mac->lisowner = lisowner;
	mac->retry = 0;
	mac->backoff = CONFIG_CSMA_MIN_BACKOFF;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;
    mac->sendoption = 0x00;
    mac->sendfailed = 0;
	mac->option = CSMA_DEF_OPTION;
    mac->txbuf = frame_open( &(mac->txbuf_memory[0]), FRAME_HOPESIZE(CONFIG_CSMA_MAX_FRAME_SIZE), 0, 0, 0 );
	
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

    rand_open( 78 );  

    return mac;
}

/* TiCsma doesn't call transceiver object's close. so you need to close the 
 * transceiver object manually yourself. 
 */
void csma_close( TiCsma * mac )
{
	timer_stop( mac->timer );
    mac->state = CSMA_STATE_NULL;
}

/* this function will try to accept the input data and copy them into internal buffer. 
 * the real data sending is done in csma_evolve(). the caller should 
 * repeatedly call evolve() to drive the MAC to run. 
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx csma_send( TiCsma * mac, uint16 shortaddrto,TiFrame * frame, uint8 option )
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
    case CSMA_STATE_IDLE:
        frame_totalcopyfrom( mac->txbuf, frame );
        ret = frame_length( frame);
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
        ieee802frame154_set_shortaddrto( desc, shortaddrto ); //ieee802frame154_set_shortaddrto( desc, mac->shortaddrto );
        ieee802frame154_set_panfrom( desc, mac->panfrom );
        ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );

		// option = [b7,...,b0]
		// b0 = 1: require ACK. 
		// b1 = 1: require carrier sense (CCA) before sending. 
		// The csma protocol needs ACK and CCA.

        //mac->sendoption = (option | 0x03);

		mac->sendoption = (option | 0x02);

		//mac->sendoption = 0x02;//require cca,no ack;

		// This is the standard CSMA protocol behavior. First do carrier sense. 
		// If the node found the channel is empty, then send the frame, or else
		// wait for some time and restart the sending after the timer expires.
		//
        frame_setlength( mac->txbuf,( ret+14));
		
        #ifdef CONFIG_CSMA_STANDATD
        if (mac->rxtx->ischnclear(mac))
        {   
            ret = _csma_trysend( mac, mac->txbuf, mac->sendoption );
        }
        else{
            mac->backoff = rand_uint8( CONFIG_CSMA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = CSMA_STATE_BACKOFF;
        }
        #endif

		// This is another version of CSMA protocol. It will insert a random delay 
        // before sending to decrease the probability of conflictions.
        // Wait for a random period before really start the transmission.
		//
        #ifndef CONFIG_CSMA_STANDATD
        mac->backoff = rand_uint8( CONFIG_CSMA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 0 );
        timer_start( mac->timer );
        mac->state = CSMA_STATE_BACKOFF;
        #endif

		// @todo: for Jiang Ridong: 2011.04
		// I found the following assignment is commented. I think it should be enabled.
		// how do you think about it? but we should think whether frame_length here.
		// 
		// If this line is commented, then this function 

		if ( ret)//todo  直接使用ret = frame_capacity( mac->txbuf );没办法判断是否发送成功。
		{
			ret = frame_capacity( mac->txbuf );
		}
        //ret = frame_capacity( mac->txbuf );

        csma_evolve( mac, NULL );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code. This is only for testing purpose.
        // do {
        //      csma_evolve( mac, NULL );
        // }while (mac->state != CSMA_STATE_IDLE);

        break;

    case CSMA_STATE_BACKOFF:
        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //
        csma_evolve( mac, NULL );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code. This is only for testing purpose.
        // do {
        //      csma_evolve( mac, NULL );
        // }while (mac->state != CSMA_STATE_IDLE);

        ret = 0;
        break;

    case CSMA_STATE_SLEEPING:
    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry csma_send() again.
        ret = 0;
        break;
    }

    return ret;
}

uintx csma_broadcast( TiCsma * mac, TiFrame * frame, uint8 option )
{
    uintx ret=0;
	//uintx cur;//todo
    TiIEEE802Frame154Descriptor * desc;

    switch (mac->state)
    {
    case CSMA_STATE_IDLE:
		
        frame_totalcopyfrom( mac->txbuf, frame );

        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
		//cur = mac->txbuf->curlayer;
		//rtl_assert( 12 <= mac->txbuf->layerstart[cur] );

        frame_skipouter( mac->txbuf, 12, 2 );
        //desc = ieee802frame154_format( &(mac->desc), frame_startptr(frame), frame_capacity(frame), FRAME154_DEF_FRAMECONTROL_DATA_NOACK );
		//desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), FRAME154_DEF_FRAMECONTROL_DATA );
		desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), FRAME154_DEF_FRAMECONTROL_DATA_NOACK );
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, mac->seqid );
	    ieee802frame154_set_panto( desc, mac->panto );
	    ieee802frame154_set_shortaddrto( desc, FRAME154_BROADCAST_ADDRESS );
	    ieee802frame154_set_panfrom( desc, mac->panfrom );
	    ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );

		// @attention
		// The above programming style is clear but less efficient. The following
		// is an efficient but not clear style:
		// 
        // char * fcf;
        // char * shortaddrto;
		//
        // fcf = frame_startptr(frame) + 0;
        // shortaddrto = (char*)(frame_startptr(frame)) + 3;  // todo: according to IEEE 802.15.4 format, 加几？请参考15.4文档确认
		//
        // for broadcasting frames, we don't need acknowledgements. so we clear the ACK 
        // REQUEST bit in the frame control field directly.
        //
        // fcf ++;
        // (*fcf) &= 0xFA; // TODO: this value should changed according to 802.15.4 format 
		//
        // 0xFFFFFFFF the broadcast address according to 802.15.4 protocol format
        // attention: we only set the destination short address field to broadcast address.
        // the destination pan keeps unchanged.
        //
        // *shortaddrto ++ = 0xFF;
        // *shortaddrto ++ = 0xFF;

		// option = [b7,...,b0]
		// b0 = 1: require ACK. 
		// b1 = 1: require carrier sense (CCA) before sending. 
		// The csma protocol needs ACK and CCA.
        mac->sendoption = option & 0x02;

        #ifdef CONFIG_CSMA_STANDATD
        if (mac->rxtx->ischnclear(mac))
        {   
           ret =  _csma_trysend( mac, mac->txbuf, mac->sendoption );
        }
        else{
            mac->backoff = rand_uint8( CONFIG_CSMA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = CSMA_STATE_BACKOFF;
        }
        #endif

        // optimized aloha protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
        #ifndef CONFIG_CSMA_STANDATD
        mac->backoff = rand_uint8( CONFIG_CSMA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 0 );
        timer_start( mac->timer );
        mac->state = CSMA_STATE_BACKOFF;
        #endif

        //ret = frame_capacity( mac->txbuf );
        csma_evolve( mac, NULL );
        break;

    case CSMA_STATE_BACKOFF:
        // in this state, there's already a frame pending inside the aloha object. 
        // you have no better choice but wait for this frame to be processed.
        //

        csma_evolve( mac, NULL );
        ret = 0;
        break;

    case CSMA_STATE_SLEEPING:
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
 * Check whether there's some frame arrivaled. If there're frames arrived, then 
 * this function will place the earliest into parameter "frame". This function can 
 * be called anytime.
 */
uintx csma_recv( TiCsma * mac, TiFrame * frame, uint8 option )
{
/*todo for testing

	const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	uint8 count;
    char * ptr=NULL;

    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
	//
	   frame_skipouter( frame, HEADER_SIZE, TAIL_SIZE );//todo for testing 临时删掉
       // assert: the skipouter must be success

	   count = _csma_tryrecv( mac, frame, option );
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
          //frame_setcapacity( frame, count - HEADER_SIZE - TAIL_SIZE );//todo
       }
       else{
           frame_setlength( frame, 0 );
       }
*/
/************************************************************************************************************/
	   const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	   uint8 count;
	   char * ptr=NULL;

	   //count = nac_recv( mac->nac, frame, option);
	   count = _csma_tryrecv( mac, frame, option );

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

	   if (count > 0)
	   {   
		   frame_skipinner( frame,  HEADER_SIZE, TAIL_SIZE );
		   frame_setlength( frame, count - HEADER_SIZE - TAIL_SIZE );

		   // - bug fix: since frame_skipinner can calculate correct layer capacity, 
		   // the following line is unecessary now
		   // frame_setcapacity( frame, count - HEADER_SIZE - TAIL_SIZE );
	   }
	   else{
		   frame_setlength( frame, 0 );
	   }
/*************************************************************************************************************/
	csma_evolve( mac, NULL );
	return count;
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
#ifdef CONFIG_CSMA_TRX_ACK_SUPPORT     
uintx _csma_trysend( TiCsma * mac, TiFrame * frame, uint8 option )
{
	uintx count=0;
	uint8 i;

    // @attention
    // Needn't do carrier sense and wait for channel clear here. In most cases, 
	// this is guaranteed by the caller. And there's one exception, if the evolve()
	// function finds the timer expired, it will not check for whether the channel 
	// is clear or not now.
	// 
	// @modified by zhangwei in 2011.04
	// I use this "bad" behavior in order to speed the response. But it seems not
	// a good idea now. 
	
	// @attention
	// @todo
	// If you want to perform channel clear checking (carrier sense process) here, 
	// you can uncomment the following code. But you should check it for some time.
	// Remember to avoid such infinite loops. 
    //
	// while (!csma_ischannelclear(mac->rxtx))
	//    continue;
	
	// @modified by zhangwei in 2011.04
	// - improved the channel clear assessment. The following is better:
	// 
	// option & 0x02 is 1 means clear channel assessment (CCA) required.
	if (option & 0x02) 
	{
		for (i=0; i<200; i++)
		{
			if (csma_ischannelclear(mac))
				break;
			hal_delayus(2);
		}
	}
	
	count = frame_capacity(frame);

	if (count > 0)
	{  
		
	   if (rand_uint8(100) <= mac->sendprob)
	   {   

		   // attention whether the sending process will wait for ACK or not depends on 
		   // "option" parameter.
		   //count = mac->rxtx->send( mac->rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );
			count = nac_send( mac->nac, frame, option );
			if (count > 0)
			{  
				mac->seqid ++;
				mac->retry = 0;
				mac->state = CSMA_STATE_IDLE;
			}
			else if (mac->retry >= CONFIG_CSMA_MAX_RETRY)
			{    
				mac->seqid ++;
				mac->retry = 0;
				mac->state = CSMA_STATE_IDLE;
				mac->sendfailed ++;
			}
			else{
				mac->retry++;
				mac->backoff = _csma_get_backoff( mac );
				timer_restart( mac->timer, mac->backoff, 0 );
				mac->state = CSMA_STATE_BACKOFF;
			}
	   }
	   else{
		   // give up this sending try and restart the sending process after backoff duration.
		   mac->backoff = _csma_get_backoff( mac );
		   timer_restart( mac->timer, mac->backoff, 0 );
		   mac->state = CSMA_STATE_BACKOFF;
		   count = 0;
	   }
   }

	return count;
}
#endif

/* call the PHY layer functions and try to send the frame immdietely. if ACK frame 
 * required, then wait for ACK
 * 
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - revision
 */
#ifndef CONFIG_CSMA_TRX_ACK_SUPPORT
uintx _csma_trysend( TiCsma * mac, TiFrame * frame, uint8 option )
{
/*
	// todo
	
	uintx count = 0;
    uint16 ctrl;
    bool expired;
    char * ptr;
	
	while (!mac->rxtx->ischannelclear( mac->rxtximpl ))
		continue;

	mac->rxtx->switchtomode( mac->rxtximpl, FTRX_MODE_TX );
    count = mac->rxtx->send( mac->rxtximpl, frame, option );
	mac->rxtx->switchtomode( mac->rxtximpl, FTRX_MODE_RX );
	
    if (count > 0)
	{
		// If the frame has been successfully sent by the transceiver, the program will
		// start to wait for the ACK frame replied from the receiver. The maximum waiting
		// duration is configured by macro CONFIG_ALOHA_ACK_RESPONSE_TIME. The time unit
		// is milli-seconds.
		//

        // wait for ack frame from the receiver. if we cannot receive the ack frame
        // in time, then we can regard the frame just send is lost during transmission.
        // this duration here is for the receiver to send the ack back. it consists 
        // the processing time inside receiver node and the transmission time in 
        // the air.
        // 
		timer_restart( mac->timer, CONFIG_ADAPT_ALOHA_ACK_RESPONSE_TIME, 0 ); 
		expired = true;
		while (!timer_expired( mac->timer ))
		{
            hal_delay( 1 );
			if (mac->rxtx->recv(mac->rxtximpl, mac->rxbuf, 0x00) > 0)
			{
				// 2 bytes for frame control 
                ptr = frame_ptr( mac->rxbuf );
				ctrl = (((uint16)(ptr[1])) << 8) | (ptr[2]);

				// If the incoming frame is ACK frame, then we can return success. 
				// other type frames will be ignored. 
                // 
                // @attention
				//  - Theoretically speaking, the current frame arrived may possibly be the 
                // data type from other nodes. in the current version, this frame will be 
                // overwrotten, which leads to unnecessary frame loss.
				//
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
*/
	hal_assert(false);
	return 0;
}
#endif

uintx _csma_tryrecv( TiCsma * mac, TiFrame * frame, uint8 option )
{
    // TiFrameTxRxInterface * rxtx = mac->rxtx;
	uintx count;

    // @attention: According to ALOHA protocol, the program should send ACK/NAK after 
    // receiving a data frame. however, this is done by the low level transceiver's
    // adapter component (TiCc2420Adapter), so we needn't to send ACK manually here.

    //todo count = rxtx->recv( rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );
	count = nac_recv(  mac->nac, frame, option );
	if (count > 0)
	{   
    	/* according to ALOHA protocol, the program should send ACK/NAK after receiving
	     * a data frame. acoording to 802.15.4 protocol, there maybe COMMAND, BEACON or 
         * ACK frames in the air. however, these type of frames are all used inside the 
         * MAC layer only. we should filter them and keep the DATA frames only.
         * 
		 * 2 bytes for frame control
		 * only the DATA type frame will be accepted here. The other types, such as COMMAND,
		 * BEACON or ACK will be ignored. 
		 */
		/*
        buf = frame_fullstartptr(frame);
		ctrl = (((uint16)buf[1]) << 8) | (buf[2]);

		if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
        {
			count = 0;
            frame_setlength( frame, 0 );
        }
        else
            frame_setlength( frame, count );
		*/
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
inline uint16 _csma_get_backoff( TiCsma * mac )
{
    // algorithm 1
    // return rand_uint16(((uint16)(mac->loadfactor))  << 3);

    // algorithm 2
    uint16 backoff = CONFIG_CSMA_MIN_BACKOFF + rand_uint16( mac->backoff << 1 );
    if (backoff > CONFIG_CSMA_MAX_BACKOFF)
            backoff = CONFIG_CSMA_MAX_BACKOFF;
    return backoff;
}

/* this function can be used as TiFrameTxRxInterface's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using csma_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void csma_evolve( void * macptr, TiEvent * e )
{
	TiCsma * mac = (TiCsma *)macptr;
    TiFrameTxRxInterface * rxtx = mac->rxtx;

    if (mac->state == CSMA_STATE_NULL)
        return;

    switch (mac->state)
    {
    case CSMA_STATE_IDLE:   
        // only in the idle state, we check for the sleeping request bit. if
        // this bit in the request variable is set by the sleep() function, 
        // then we call transceiver's sleep() function and force it into sleep
        // mode. in the sleeping mode, all the frames in the mac layer are still 
        // pending inside it, so the frames won't lost. different to the POWERDOWN
        // mode, the whole application can recover from SLEEP mode fast and continue
        // previous state.
        //
        if (bit8_get(mac->request, CSMA_SLEEP_REQUEST))
        {
            rxtx->switchtomode( rxtx->provider, FTRX_MODE_SLEEP );
            bit8_clr( mac->request, CSMA_SLEEP_REQUEST );
            mac->state = CSMA_STATE_SLEEPING;
        }
        break;

    case CSMA_STATE_BACKOFF:
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

            //while (!_csma_ischannelclear(mac))
            //    continue;

            _csma_trysend( mac, mac->txbuf, mac->sendoption );
        }
        mac->state = CSMA_STATE_IDLE;//todo for testing
        break;

    case CSMA_STATE_SLEEPING:
        if (bit8_get(mac->request, CSMA_WAKEUP_REQUEST))
        {
            rxtx->switchtomode( rxtx->provider, FTRX_MODE_RX );
            bit8_clr( mac->request, CSMA_WAKEUP_REQUEST );
            mac->state = CSMA_STATE_IDLE;
        }
        break;

    default:
        // this should never happen.  the following source code can pull the state 
        // to IDLE and let the state machine to continue in case of state disorders.
        hal_assert( false );
        mac->state = CSMA_STATE_IDLE;
    }

    if (mac->request > 0)
    {
        if (bit8_get(mac->request, CSMA_SHUTDOWN_REQUEST))
        {
            // no matter what the current state is, you can do shutdown. the power will 
            // lost. so the data inside the SRAM will be lost. 

            // if timer is already running, then we should stop it. 
            timer_stop( mac->timer );
            frame_clear( mac->txbuf );

            rxtx->switchtomode( rxtx->provider, FTRX_MODE_POWERDOWN );
            bit8_clr( mac->request, CSMA_SHUTDOWN_REQUEST );
            mac->state = CSMA_STATE_POWERDOWN;
        }

        if (bit8_get(mac->request, CSMA_STARTUP_REQUEST))
        {
            if (mac->state == CSMA_STATE_POWERDOWN)
            {
                rxtx->switchtomode( rxtx->provider, FTRX_MODE_RX );
                bit8_clr( mac->request, CSMA_STARTUP_REQUEST );
                mac->state = CSMA_STATE_IDLE;
            }
        }
    }

    // attention the following process can occur in any state.
    if (e != NULL)
    {
	    switch (e->id)
	    {
        case CSMA_EVENT_FRAME_ARRIVAL:
		//case EVENT_DATA_ARRIVAL:
            // if the incoming event indicates that the transceiver receives an frame, 
            // then it simply pass the event to listener object. the listener object
            // will call csma_recv() to retrieve the frame out. 
            //
            // @attention
            // - if the caller cann't check for incoming frames fast enough, then the 
            // new frames will overwrite the old ones inside the transceiver object.
            // it's the application developer's responsibility to guarantee there
            // no packet loss due to slow querying and insufficient buffering spaces.
            //
            // - however, for some transceiver such as 802.15.4 compatible chip cc2420, 
            // it may report ACK/BECON/COMMAND frames according to 802.15.4 specification. 
            // however, the upper layer doesn't need these frames. the program will ignore 
            // them.
            //
            // then the following source code is enabled and the 802.15.4's ACK/BECON/COMMAND
            // frames will be filtered. and only the DATA frame is reported to the upper 
            // layer. this can guarantee every time the listener runs, it can read an 
            // valid frame. 
            //            
            //if (_csma_tryrecv(mac, mac->rxbuf, 0x00) > 0)
            //{
            //    if (mac->listener != NULL)
	        //    {
		    //        mac->listener( mac->lisowner, e );
	        //    }
            //}
			if (mac->listener != NULL)
			{
				mac->listener( mac->lisowner, e );
			}
            break;

        /*
        case CSMA_EVENT_SHUTDOWN_REQUEST:
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

void csma_statistics( TiCsma * mac, uint16 * sendcount, uint16 * sendfailed )
{
    *sendcount = 0;
    *sendfailed = mac->sendfailed;
    mac->sendfailed = 0;
}
