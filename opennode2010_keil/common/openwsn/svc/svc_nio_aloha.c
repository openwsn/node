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
 * svc_nio_aloha.c
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
 * @modified by Jiang Ridong and Zhang Wei in 2011.04
 *	- Revised and tested.
 * @modified by Jiang Ridong and Zhang Wei in 2011.08
 *  - Revised and tested
 ******************************************************************************/
 
#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_mcu.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
//#include "svc_timer.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_aloha.h"

#define MAC_HEADER_LENGTH 12
#define MAC_TAIL_LENGTH 2

static uintx _aloha_trysend( TiAloha * mac, TiFrame * frame, uint8 option );
static uintx _aloha_tryrecv( TiAloha * mac, TiFrame * frame, uint8 option );

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

/**
 * @pre the rxtx adapter has already been opened
 * @pre mac->timer is already opened but not start yet
 * @pre network acceptor is opened successfully
 */
TiAloha * aloha_open( TiAloha * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option )
{
    void * provider;
    
    // assert( the rxtx driver has already been opened );
    // assert( mac->timer is already opened but not start yet );
	// assert( network acceptor is opened successfully );
    
	rtl_assert((rxtx != NULL) && (nac != NULL) && (timer != NULL));

	mac->state = ALOHA_STATE_IDLE;
    mac->rxtx = rxtx;
	mac->nac = nac;
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
    mac->txbuf = frame_open( (char *)(&mac->txbuf_memory[0]), sizeof(mac->txbuf_memory), 0, 0, 0 );
	mac->success = 0;//todo added by Jiang Ridong on 2011.04
	
    // @modified by zhangwei on 2010.08.21
    // @attention: for all hardware components, you should construct and open them 
    // in the caller function to avoid potential conflictions. so we don't recommend
    // initialize the timer component here.

	// timer = vti_construct( (void *)&g_timer, sizeof(g_timer) );
    // vti_open( timer, id, NULL, NULL, 0x00 ); 
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

/**
 * Send a frame out
 *
 * @attention: This function assumes you should set the "destination address" field 
 * of the frame object.
 */
uintx aloha_send( TiAloha * mac,  uint16 shortaddrto, TiFrame * frame, uint8 option )
{   
	
	TiIEEE802Frame154Descriptor * desc;
    uintx ret=0;

	bool failed = true;
   
    switch (mac->state)
    {
    case ALOHA_STATE_IDLE:
		// make a copy of the original frame inside the MAC component
        frame_totalcopyfrom( mac->txbuf, frame );
		frame_setlength( mac->txbuf, frame_length(frame) );
		ret = frame_length( mac->txbuf );
        
        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
        //frame_dump(mac->txbuf);
		
        frame_skipouter( mac->txbuf, MAC_HEADER_LENGTH, MAC_TAIL_LENGTH );
        desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf ), 
			FRAME154_DEF_FRAMECONTROL_DATA );
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, mac->seqid ); 
        ieee802frame154_set_panto( desc, mac->panto );
        ieee802frame154_set_shortaddrto( desc, shortaddrto );
        ieee802frame154_set_panfrom( desc, mac->panfrom );
        ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );
        
        //frame_moveinner( mac->txbuf );
        mac->txbuf->option = option;
        mac->sendoption = option;

        // standard aloha protocol behavior
		// You should define macro CONFIG_ALOHA_STANDARD before this module to choose
		// this branch.
        frame_setlength(mac->txbuf, (ret + MAC_HEADER_LENGTH + MAC_TAIL_LENGTH));
        #ifdef CONFIG_ALOHA_STANDARD
		failed = true;
        if (mac->rxtx->ischnclear(mac))
        {   
			ret = _aloha_trysend( mac, mac->txbuf, option );
			if (ret > 0)
				failed = false;
        }
		
		// If the channel currently is busy or the sending is failed, then we should
		// backoff and start the retry sending process.
		if (failed)
		{
            mac->backoff =CONFIG_ALOHA_MIN_BACKOFF+ rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 7999 );//7999时mac->backoff的单位是ms，此时系统时钟是8M
            timer_start( mac->timer );
            mac->state = ALOHA_STATE_BACKOFF;
        }
        #endif

        // Optimized aloha protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
		//
		// You should undef macro CONFIG_ALOHA_STANDARD before this module to choose
		// this branch.
		
        #ifndef CONFIG_ALOHA_STANDARD
        mac->backoff =CONFIG_ALOHA_MIN_BACKOFF+ rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 7999 );
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
        //ret = frame_capacity( mac->txbuf );
        aloha_evolve( mac, NULL );
		//如果没有下面这一句，即使在evolve中发送成功，send的返回值还是不为真，同一个帧可能被连续发送多次。
		if ( failed)//todo added by Jiang Ridong on 2011.04
		{
			ret = mac->success;
		}
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
    TiIEEE802Frame154Descriptor * desc;
	bool failed=true;
    uintx ret=0;

	// clear the ACK REQUEST bit
	option &= 0xFE;
    switch (mac->state)
    {
    case ALOHA_STATE_IDLE:
		// make a copy of the original frame inside the MAC component
        frame_totalcopyfrom( mac->txbuf, frame );
		ret = frame_length( mac->txbuf );
        // according to 802.15.4 specification:
        // header 12 B = 1B frame length (required by the transceiver driver currently) 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        //
        frame_skipouter( mac->txbuf, MAC_HEADER_LENGTH, MAC_TAIL_LENGTH );
        //desc = ieee802frame154_format( &(mac->desc), frame_startptr(frame), frame_capacity(frame), 
			//FRAME154_DEF_FRAMECONTROL_DATA_NOACK );

		desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf), 
			FRAME154_DEF_FRAMECONTROL_DATA );// @todo 如果使用上一句则前12个字节没有赋值成功！2011.04.11
        rtl_assert( desc != NULL );
		
        ieee802frame154_set_sequence( desc, mac->seqid );
	    ieee802frame154_set_panto( desc, mac->panto );
	    ieee802frame154_set_shortaddrto( desc, FRAME154_BROADCAST_ADDRESS );
	    ieee802frame154_set_panfrom( desc, mac->panfrom );
	    ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );

		mac->txbuf->option = option;
        mac->sendoption = option;
		frame_setlength( mac->txbuf,(ret + MAC_HEADER_LENGTH + MAC_TAIL_LENGTH));
        // standard aloha protocol behavior
        #ifdef CONFIG_ALOHA_STANDARD
		failed = true;
        if (true)//if (mac->rxtx->ischnclear(mac))//todo for testing
        {  
			ret = _aloha_trysend( mac, mac->txbuf, option );
			if (ret > 0)
				failed = false;
			
        }
		
		// If the channel currently is busy or the sending is failed, then we should
		// backoff and start the retry sending process.
		if (failed)
		{
            mac->backoff =CONFIG_ALOHA_MIN_BACKOFF+ rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
            mac->retry = 0;
            timer_setinterval( mac->timer, mac->backoff, 7999 );
            timer_start( mac->timer );
            mac->state = ALOHA_STATE_BACKOFF;
        }
        #endif

        // optimized aloha protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
		
        #ifndef CONFIG_ALOHA_STANDARD
        mac->backoff =CONFIG_ALOHA_MIN_BACKOFF+ rand_uint8( CONFIG_ALOHA_MAX_BACKOFF );
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 7999 );
        timer_start( mac->timer );
        mac->state = ALOHA_STATE_BACKOFF;
        #endif

        //ret = frame_capacity( mac->txbuf );
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
		mac->state = ALOHA_STATE_IDLE;
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry aloha_send() again.
        ret = 0;
        break;
    }
   
    return ret;
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
uintx _aloha_trysend( TiAloha * mac, TiFrame * frame, uint8 option )
{   
	uintx count=0;
	char * buf;
	bool ack_success;
	uint16 fcf; 

	// @todo: frame_length is better, but the frame length property should be 
	// assigned correct value first
	//uintx len = frame_capacity(frame);
	
	uintx len = frame_length(frame);
	hal_assert( (frame != NULL) && (len > 0) );


    // @modified by openwsn on 2010.08.24
    // - needn't wait for channel clear here. because the caller can guarantee the 
    // channel is clear enough before calling this function.
	// @modified by openwsn on 2011.04.05
	// - I cannot understand my comment above now :) Maybe in 2010, I hope the higher
	// level program can guarantee the channel is clear. 
	// - I do think the aloha protocol doesn't have channel assesement capability. 
	// In order to simulates the behavior of an pure aloha protocol, I give up
	// channel clear assesement here. 
	// - Furthermore, considering the undetermined waiting time for channel clear, 
	// We'd better make the WAIT_CHN_CLEAR as a separate state rather than while
	// loop here. 
	
	// The standard aloha protocol doesn't check whether the carrier channel is 
	// avaiable or not. If you want to add cariier sense (CA) capability to the channel, 
	// you can uncomment the following code. 
	//
	// if (option & 0x02)
	// {
	// 	while (!csma_ischannelclear(mac->rxtx))
	//    	continue;
	// }

    // attention whether the sending process will wait for ACK or not depends on 
    // "option" parameter.
    
    if (len > 0)
    { 
		count = nac_send( mac->nac, frame, option );
		
		// If this frame requires ACK response

		if (option & 0x01)
		{
            timer_setinterval( mac->timer, CONFIG_ALOHA_MIN_ACK_TIME, 7999 );
            timer_start( mac->timer );
			while (!timer_expired(mac->timer)) 
			{
				
			}

			
			// @attention: For this timer, the maximum duration input is 8 due to 
			// the limited width of the timer hardware (8 bits only). So I add the
			// following assertion to do the check. This is only on GAINZ node.
			//hal_assert( CONFIG_ALOHA_MAX_ACK_TIME - CONFIG_ALOHA_MIN_ACK_TIME <= 80 );
            timer_setinterval( mac->timer, CONFIG_ALOHA_MAX_ACK_TIME - CONFIG_ALOHA_MIN_ACK_TIME, 7999 );
            timer_start( mac->timer );
			ack_success = false;
			while (!timer_expired(mac->timer)) 
			{ 
				buf = &(mac->rxbuf_ack[0]);
				memset( buf, 0x00, FRAME154_ACK_FRAME_SIZE );
				mac->rxtx->recv( mac->rxtx->provider,buf,FRAME154_ACK_FRAME_SIZE, 0x00 );
				//rxtx->recv( rxtx->provider, buf, FRAME154_ACK_FRAME_SIZE, 0x00 );
				if (buf[0] > 1)
				{ 
					// If the received frame is acknowledgement and the sequence 
					// number is idential to the frame just sent, then we can deduce
					// that the frame is accepted successfully by the receiver.
					//
					fcf = FRAME154_MAKEWORD( buf[2], buf[1] );
					if ( FCF_FRAMETYPE(fcf) == FCF_FRAMETYPE_ACK)
					{
						if (*(buf+3) == mac->seqid)
						{   
							ack_success = true;
							break;
						}
					}
				}
			}
			
			if (!ack_success)
				count = 0;
		}
		
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
                timer_setinterval( mac->timer, mac->backoff, 7999 );
                timer_start( mac->timer );
				mac->state = ALOHA_STATE_BACKOFF;
			}
		}
    }    
     
    return count; 
}

/**
 * Check whether there's some frame arrivaled. This function can be called anytime. 
 * 
 * @attention: If aloha_recv() doesn't got an frame, the input buffer "frame" is 
 * still changed. The upper layer should consider this behavior. This is different 
 * to aloha_send(). 
 */
uintx aloha_recv( TiAloha * mac, TiFrame * frame, uint8 option )
{
    const uint8 HEADER_SIZE = 12, TAIL_SIZE = 2;
	uintx count;
	uint16 fcf;
    char * ptr=NULL;

	// @modified by zhangwei on 2011.03.14
	// - Since the network acceptor will reset the frame inside, we needn't call
	// frame_slipouter() and frame_moveinner() in this function. The following 
	// are deprecated:
	//
    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
	// 
    // frame_skipouter( frame, HEADER_SIZE, TAIL_SIZE );
	
    // assert: the skipouter must be success
	// attention the network acceptor requirement of the frame layer
	count = _aloha_tryrecv( mac, frame, option );
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
			fcf = FRAME154_MAKEWORD( ptr[2], ptr[1] );
			if (FCF_FRAMETYPE(fcf) != FCF_FRAMETYPE_DATA)
			{
				count = 0;
			}
			else{
                frame_setlength( frame, count );
                //frame_setcapacity( frame, count );   todo for testing
			}
        }
    }
	
	// @modified by zhangwei on 2011.03.14
	// - Since the network acceptor will reset the frame inside, we needn't call
	// frame_slipouter() and frame_moveinner() in this function. The following 
	// are deprecated:
	//
	// related to frame_skipouter(frame), so that the above process doesn't affect
	// the current layer setting of the frame.
    // frame_moveinner( frame );

    if (count > 0)
	{
		uintx cur;
        cur = frame->curlayer;
		if(frame->layercapacity[cur] >= (HEADER_SIZE + TAIL_SIZE))
		{
			//todo frame_skipinne执行完后会使应用层帧的seqid清零，不知道为什么？
			if(!frame_skipinner( frame,  HEADER_SIZE, TAIL_SIZE ))
			{
				count = 0;
			}
			frame_setlength( frame, count - HEADER_SIZE - TAIL_SIZE );
			count = count - HEADER_SIZE - TAIL_SIZE;//todo for testing
		}
		else
		{
			count = 0;
		}

		
		// - bug fix: since frame_skipinner can calculate correct layer capacity, 
		// the following line is unecessary now
        // frame_setcapacity( frame, count - HEADER_SIZE - TAIL_SIZE );
    }
    else{
        frame_setlength( frame, 0 );
    }

	aloha_evolve( mac, NULL );
	return count;
}

uintx _aloha_tryrecv( TiAloha * mac, TiFrame * frame, uint8 option )
{   
	//TiFrameTxRxInterface  * rxtx = mac->rxtx;
	uintx count;

	// @attention: According to ALOHA protocol, the program should send ACK/NAK after 
	// receiving a data frame. however, this is done by the low level transceiver's
	// adapter component (TiCc2420Adapter), so we needn't to send ACK manually here.

	// count = rxtx->recv( rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );
	count = nac_recv( mac->nac, frame, option);

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
	uintx ret;

    mac->rxtx->evolve( mac->rxtx, NULL );

    switch (mac->state)
    {
    case ALOHA_STATE_IDLE:
        break;

    case ALOHA_STATE_BACKOFF:
        
		// Retry sending the frame inside mac->txbuf. if sending successfully, then 
		// transfer to IDLE state. if failed, then still in WAITFOR_SENDING state.
        while ( !timer_expired( mac->timer))//我觉得这一句应该加上去
        {
        }
		hal_assert( timer_expired( mac->timer));
        if (timer_expired(mac->timer))
		{
			ret = 0;
			// Try to send the frame in mac->txbuf again. The _aloha_trysend() function
			// will deal with necessary ACK processing, sequence processing and 
			// try limitation checking. 
			//
			// After _aloha_trysend() call, the mac component will stay in ALOHA_STATE_IDLE
			// or ALOHA_STATE_BACKOFF state. If the sending is successfully or the 
			// retry reaches its maximum limitation count, then _aloha_trysend() 
			// will transfer the state to ALOHA_STATE_IDLE automatically. If the 
			// retry failed and not exceed the maximum sending limitation, then 
			// the state will still in ALOHA_STATE_BACKOFF state. The backoff timer
			// will also be started by _aloha_trysend() function.
			

			//todo 我觉得这里即使发送成功，状态也不会转变，所以要加上状态装换语句。
			ret = _aloha_trysend(  mac, mac->txbuf, mac->txbuf->option );
			if ( ret>0)
			{
				mac->state = ALOHA_STATE_IDLE;
			}
			mac->success = ret;
		}
		
		/*原代码
        if (vti_expired(mac->timer))
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
			ret = _aloha_trysend(  mac, mac->txbuf, mac->txbuf->option );

			if ( ret)//todo addded by Jiang Ridong on 2011.04.16
			{
				mac->state = ALOHA_STATE_IDLE;
			}
        }*/
        break;
    
    case ALOHA_STATE_SLEEPING:
		mac->state = ALOHA_STATE_IDLE;//todo addded by Jiang Ridong on 2011.04.16
        /* 
        if (e->id = wakeuprequest)
        {
            phy_wakeup();
            mac->state = ADTALOHA_IDLE;
        }
        */

    default:
		mac->state = ALOHA_STATE_IDLE;//todo addded by Jiang Ridong on 2011.04.16
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
            vti_stop(); 
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
