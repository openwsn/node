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
 * svc_nio_csma.c
 * standard CSMA medium access control (MAC) protocol 
 *  
 * @attention
 *	- This version will wait for the ACK frame after sending a DATA frame immediately.
 * If another DATA frame arrives during this period, it will be ignored. This can be 
 * improved in the future. 
 *
 *	- Different to the basic CSMA protocol, this version will check whether the
 * communication channel is clear or not. It will only send the frame when the channel
 * is clear. This clear channel assesement (CCA) functionality is provided by 
 * the cc2420 transceiver.
 *
 *  - When encounting sending conflicts, this version of CSMA will retry for some 
 * times. The retry count is configures by macro CONFIG_CSMA_MAX_RETRY. However, 
 * there's no backoff when retry the sending, so there's still large chance to 
 * be conflicted again. This should be improved before this version of CSMA can 
 * be used in real applications. 
 *
 *  - This version doesn't depend on "osx" functionalities. So it's easy to be used.
 * The enhanced version of this simple CSMA is in svc_openmac and svc_adaptivecsma
 *
 *	In Summary, this version of CSMA is mainly for demonstration and experiments. 
 * It's a good start to be used for teaching than in real applications. 
 *
 * @author zhangwei and shan-lei on 20050710
 *	- first version on PIC16F876. This version is fully discarded.
 * @modified by zhangwei on 20090610
 *	- fully revised for ICT GAINZ platform
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * @modified by zhangwei on 20090802
 *  - correct bug in _csma_tryrecv(). The former byte order to create the frame control
 *    word is wrong. 
 * @modified by Shimiaojing on 20091025
 *  -add macro_csmachannelclear(mac) HAL_READ_CC_CCA_PIN()and delete line141 142 and match the frame construct of 
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
 * @modified by Shi Zhirong on 2012.07.24
 *  - Revised and tested
 * @modified by ZhangWei on 2012.07.30
 *  - Using "nac->ackbuf" to replace "fmque_rear(nac->rxque)".
 ******************************************************************************/
 
#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_mcu.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_random.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
//#include "svc_timer.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_csma.h"
#include "svc_nio_mac.h"

static intx _csma_wait_channelclear( TiCsma * mac, int interval );
static intx _csma_trysend( TiCsma * mac, TiFrame * frame, uint8 option );
static intx _csma_tryrecv( TiCsma * mac, TiFrame * frame, uint8 option );
static void _csma_on_ack_frame(TiCsma * mac, TiFrame * frame);
static void _csma_on_command_frame(TiCsma * mac, TiFrame * frame);
static void _csma_on_beacon_frame(TiCsma * mac, TiFrame * frame);

TiCsma * csma_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiCsma) <= size );
	memset( buf, 0x00, size );
	return (TiCsma *)buf;
}

void csma_destroy( TiCsma * mac )
{
	timer_stop( mac->timer );
	return;
}

/**
 * @pre the rxtx adapter has already been opened
 * @pre mac->timer is already opened but not start yet
 * @pre network acceptor is opened successfully
 */
TiCsma * csma_open( TiCsma * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, uint8 option )
{
    void * provider;
    
    // assert( the rxtx driver has already been opened );
    // assert( mac->timer is already opened but not start yet );
	// assert( network acceptor is opened successfully );
    
	rtl_assert((rxtx != NULL) && (nac != NULL) && (timer != NULL));

    mac->rxtx = rxtx;
	mac->nac = nac;
	mac->timer = timer;
	mac->listener = NULL;
	mac->lisowner = NULL;
	mac->retry = 0;
	mac->backoff = CONFIG_CSMA_MIN_BACKOFF_TIME;
    mac->panto = panid;
    mac->shortaddrto = FRAME154_BROADCAST_ADDRESS;
    mac->panfrom = panid;
    mac->shortaddrfrom = address;
    mac->seqid = 0;	
    mac->sendprob = CSMA_P_INSIST_INDICATOR;
    
    memset( &mac->stat, 0x00, sizeof(TiCsmaStatistics) );

	mac->option = option;
    mac->txbuf = frame_open( (char *)(&mac->txbuf_memory[0]), sizeof(mac->txbuf_memory), 0, 0, 0 );
    
	mac->success = 0;//@todo added by Jiang Ridong on 2011.04
	
    // @modified by zhangwei on 2010.08.21
    // @attention: for all hardware components, you should construct and open them 
    // in the caller function to avoid potential conflictions. so we don't recommend
    // initialize the timer component here.

	// timer = vti_construct( (void *)&g_timer, sizeof(g_timer) );
    // vti_open( timer, id, NULL, NULL, 0x00 ); 
    hal_assert( mac->timer != NULL );

    // initialize the frame transceiver component
	// attention enable ACK support for csma protocol. the current implementation depends
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

	mac->state = CSMA_STATE_IDLE;
    return mac;
}

void csma_close( TiCsma * mac )
{
	timer_close( mac->timer );
	mac->state = CSMA_STATE_NULL;
}

/**
 * Send a frame out
 * 
 * @param option
 *  option & CSMA_OPTION_ACK > 0 means this frame require the receiver to send ACK back.
 * 
 * @return
 *	> 0			successfully send the data. This includes two cases: 
 *              - when no ack required: return positive value if successfully send it out 
 *                through the transceiver
 *              - when ack required: retuen positive value only when received the ack
 *                frame from the receiver successfully.                 
 *	CSMA_IORET_NOACTION(0): do nothing.
 *  CSMA_IORET_ERROR_NOACK: when ack not received if the frame requires ack.
 *  CSMA_IORET_ERROR_ACCEPTED_AND_BUSY: when the channel is busy.
 *
 * @attention: This function assumes you should set the "destination address" field 
 * of the frame object.
 */
intx csma_send( TiCsma * mac,  uint16 shortaddrto, TiFrame * frame, uint8 option )
{   
	TiIEEE802Frame154Descriptor * desc;
    intx count=0;
    intx retval=0;

	bool failed = true;
   
    switch (mac->state)
    {
    case CSMA_STATE_IDLE:
		// make a copy of the original frame inside the MAC component
        frame_totalcopyfrom( mac->txbuf, frame );
		frame_setlength( mac->txbuf, frame_length(frame) );
		count = frame_length( mac->txbuf );
        
        // according to 802.15.4 specification:
        // header 11 B = 
        //  + 2B frame control + 1B sequence number + 2B destination pan + 2B destination address
        //  + 2B source pan + 2B source address
        // however, we should leave 12 bytes in the front becacuse there's an 
        // additional frame length occupied 1B.
        
        //frame_dump(mac->txbuf);
		
		mac->stat.sendcount ++;
		
        // allocate enough space for the MAC header and PHY header. The PHY header
        // currently only contains 1 byte which is the frame length.
        // The last two bytes are for checksum.
        frame_skipouter( mac->txbuf, CSMA_HEADER_SIZE+1, CSMA_TAIL_SIZE );
        
        desc = ieee802frame154_format( &(mac->desc), frame_startptr(mac->txbuf), frame_capacity(mac->txbuf ), 
			FRAME154_DEF_FRAMECONTROL_DATA );
        rtl_assert( desc != NULL );
        ieee802frame154_set_ack_request(desc, (option & CSMA_OPTION_ACK));
        ieee802frame154_set_sequence( desc, mac->seqid ); 
        ieee802frame154_set_panto( desc, mac->panto );
        ieee802frame154_set_shortaddrto( desc, shortaddrto );
        ieee802frame154_set_panfrom( desc, mac->panfrom );
        ieee802frame154_set_shortaddrfrom( desc, mac->shortaddrfrom );
        
        mac->txbuf->option = option;
        frame_setlength(mac->txbuf, (CSMA_HEADER_SIZE + 1 + count + CSMA_TAIL_SIZE));

        // standard csma protocol behavior
		// You should define macro CONFIG_CSMA_STANDARD before this module to choose
		// this branch.

        #ifdef CONFIG_CSMA_STANDARD
		failed = true;
 
		retval = _csma_trysend( mac, mac->txbuf, option );
		if (CSMA_IORET_SUCCESS(retval))
			failed = false;
		
		// If the channel currently is busy or the sending is failed, then we should
		// backoff and start the retry sending process.
		if (failed)
		{
            mac->backoff = CONFIG_CSMA_MIN_BACKOFF_TIME + (uint16)hal_rand_uint8(CONFIG_CSMA_MAX_BACKOFF_TIME);	
            mac->retry = 0;
            
            // @todo: should check whether the interval value is really support
            // by the low level timer hardware. Some hardware timer may not support 
            // long interval time due to hardware restrictions.
            // 
            // @todo: Recommend to use an time-indepedent timer such as the timer component
            // in the service layer, because the interval may depends on the system
            // frequency if you use hardware timer directly. 
            //
            timer_setinterval( mac->timer, mac->backoff, 0 );
            timer_start( mac->timer );
            mac->state = CSMA_STATE_BACKOFF;
        }
        #endif

        // Optimized csma protocol behavior. it will insert a random delay before
        // sending to decrease the probability of conflictions.
        // wait for a random period before really start the transmission
		//
		// You should undef macro CONFIG_CSMA_STANDARD before this module to choose
		// this branch.
		
        #ifndef CONFIG_CSMA_STANDARD
        mac->backoff = CONFIG_CSMA_MIN_BACKOFF_TIME + (uint16)hal_rand_uint8( CONFIG_CSMA_MAX_BACKOFF_TIME );	
        mac->retry = 0;
        timer_setinterval( mac->timer, mac->backoff, 0 );
        timer_start( mac->timer );
        mac->state = CSMA_STATE_BACKOFF;
        #endif

        /* @modified by openwsn on 2010.08.30
         * - bug fix. you should place the frame_length() call before csma_evolve() 
         * because csma_evolve() may send mac->txbuf and clear it. this will cause
         * frame_length() returns 0 instead of the real frame length.
         * 
         * @modified by XuFuzhen on 2010.09.29
         * - bug fix. should replace frame_length() here with frame_capacity()
         */
        mac->success = CSMA_IORET_NOACTION;
        csma_evolve( mac, NULL );

		//如果没有下面这一句，即使在evolve中发送成功，send的返回值还是不为真，同一个帧可能被连续发送多次。
		if (failed)//todo added by Jiang Ridong on 2011.04
		{
			retval = mac->success;
		}
        break;

    case CSMA_STATE_BACKOFF:
        // in this state, there's already a frame pending inside the csma object. 
        // you have no better choice but wait for this frame to be processed.
        //
        csma_evolve( mac, NULL );
        retval = CSMA_IORET_NOACTION;
        break;

    case CSMA_STATE_SLEEPING:

    default:
        // currently, this version implementation will ignore any frame sending request
        // if the mac component is still in sleeping state. you should wakeup it and
        // then retry csma_send() again.
        retval = CSMA_IORET_NOACTION;
        break;
    }

    return retval;
}

intx csma_broadcast( TiCsma * mac, TiFrame * frame, uint8 option )
{
 	// clear the ACK REQUEST bit
	// option &= 0xFE;
    option &= (~CSMA_OPTION_ACK);
    
    return csma_send(mac, FRAME154_BROADCAST_ADDRESS, frame, option );
}

intx _csma_wait_channelclear( TiCsma * mac, int interval )
{
/*
    // @todo
	while (!csma_ischannelclear(mac->rxtx))
    {
        continue;
	}
*/
    return 0;
}

/** 
 * try send a frame out through the transceiver. the frame should be already placed 
 * inside buffer. 
 * 
 * this function is shared by csma_send() and csma_broadcast().
 *
 * @return
 *	> 0			successfully send the data. This includes two cases: 
 *              - when no ack required: return positive value if successfully send it out 
 *                through the transceiver
 *              - when ack required: retuen positive value only when received the ack
 *                frame from the receiver successfully.                 
 *	CSMA_IORET_NOACTION(0): do nothing.
 *  CSMA_IORET_ERROR_NOACK: when ack not received if the frame requires ack.
 *  CSMA_IORET_ERROR_ACCEPTED_AND_BUSY: when the channel is busy.
 */
intx _csma_trysend( TiCsma * mac, TiFrame * frame, uint8 option )
{   
	uintx count=0, len;
	char * buf;
	bool ack_success = false;
	uint16 fcf; 
    TiFrame * rxf;
    intx retval = CSMA_IORET_NOACTION;

	// @todo: frame_length is better, but the frame length property should be 
	// assigned correct value first by the upper layer component.
    //
	// len = frame_capacity(frame);
	len = frame_length(frame);
	hal_assert((frame != NULL) && (len > 0));
	/*
    // @modified by openwsn on 2010.08.24
    // - needn't wait for channel clear here. because the caller can guarantee the 
    // channel is clear enough before calling this function.
	// @modified by openwsn on 2011.04.05
	// - I cannot understand my comment above now :) Maybe in 2010, I hope the higher
	// level program can guarantee the channel is clear. 
	// - I do think the csma protocol doesn't have channel assesement capability. 
	// In order to simulates the behavior of an pure csma protocol, I give up
	// channel clear assesement here. 
	// - Furthermore, considering the undetermined waiting time for channel clear, 
	// We'd better make the WAIT_CHN_CLEAR as a separate state rather than while
	// loop here. 
	
	// The standard csma protocol doesn't check whether the carrier channel is 
	// avaiable or not. If you want to add cariier sense (CA) capability to the channel, 
	// you can uncomment the following code. 
	//
	// if (option & 0x02)
	// {
	// 	while (!csma_ischannelclear(mac->rxtx))
	//    	continue;
	// }
	*/
    _csma_wait_channelclear(mac, 5);
    
    // attention whether the sending process will wait for ACK or not depends on 
    // "option" parameter.

    // If this frame requires the ACK response, then wait for the ACK
    if (option & CSMA_OPTION_ACK)
    {
        if (mac->rxtx->ischnclear(mac) && (hal_random_uint8() < mac->sendprob))
        {
            count = nac_send(mac->nac, frame, option);
            
            if (CONFIG_CSMA_MIN_ACK_TIME > 0)
            {
                timer_stop(mac->timer);
                timer_setinterval(mac->timer, CONFIG_CSMA_MIN_ACK_TIME, 0);
                timer_start(mac->timer);
                while (!timer_expired(mac->timer)) { NULL; }
            }

            // @attention: For this timer, the maximum duration input is 8 due to 
            // the limited width of the timer hardware (8 bits only). So I add the
            // following assertion to do the check. This is only on GAINZ node.
            //
            // hal_assert( CONFIG_CSMA_MAX_ACK_TIME - CONFIG_CSMA_MIN_ACK_TIME <= 80 );
            // timer_setinterval( mac->timer, CONFIG_CSMA_MAX_ACK_TIME - CONFIG_CSMA_MIN_ACK_TIME, 7999 );
            
            timer_setinterval(mac->timer, CONFIG_CSMA_MAX_ACK_TIME - CONFIG_CSMA_MIN_ACK_TIME, 0);
            timer_start(mac->timer);
            
            /*            
            ack_success = false;
            while (!timer_expired(mac->timer)) 
            { 
                nac_evolve(mac->nac, NULL);
                rxf = fmque_rear(nac_rxque(mac->nac));
                if (rxf != NULL)
                {
                    buf = frame_startptr(rxf);
                    fcf = FRAME154_MAKEWORD( buf[2], buf[1] );
                    if (FCF_FRAMETYPE(fcf) == FCF_FRAMETYPE_ACK)
                    {
                        if (buf[3] == mac->seqid)
                        {   
                            ack_success = true;
							fmque_poprear(nac_rxque(mac->nac));//JOE 0726 bug
                            break;
                        }
						else//todo : is this operation safe? //JOE 0726 bug
						{
							fmque_poprear(nac_rxque(mac->nac));//JOE 0726 bug
						}
                    }
                    fmque_poprear(nac_rxque(mac->nac));
                }            
            }
            */
            ack_success = false;
            while (!timer_expired(mac->timer)) 
            { 
                nac_evolve(mac->nac, NULL);
                rxf = nac->ackbuf;
                if (!frame_empty(rxf))
                {
                    buf = frame_startptr(rxf);
                    // fcf = FRAME154_MAKEWORD( buf[2], buf[1] );		//JOE  0801
                    // if (FCF_FRAMETYPE(fcf) == FCF_FRAMETYPE_ACK)
                    // {
                        if (buf[3] == mac->seqid)
                        {   
                            ack_success = true;
                            break;
                        }
                    // }
                    //frame_clear(nac->ackbuf);		//JOE 0801
					frame_totalclear(nac->ackbuf);
                }            
            }           
            
            retval = (ack_success) ? count : CSMA_IORET_ERROR_NOACK;
        } 
        else
		{
            retval = CSMA_IORET_ERROR_ACCEPTED_AND_BUSY;
        }
        
        if (CSMA_IORET_SUCCESS(retval))
        {   
            mac->seqid ++;
            mac->retry = 0;
            mac->state = CSMA_STATE_IDLE;
        }
        else
		{
            if (mac->retry >= CONFIG_CSMA_MAX_RETRY)		
            {    
                mac->retry = 0;
                mac->state = CSMA_STATE_IDLE;
                mac->stat.sendfailed ++;
            }
            else
			{											
                mac->retry++;
                mac->backoff = CONFIG_CSMA_MIN_BACKOFF_TIME + (uint16)hal_rand_uint8( mac->backoff << 1 );	
                if (mac->backoff > CONFIG_CSMA_MAX_BACKOFF_TIME)
                    mac->backoff = CONFIG_CSMA_MAX_BACKOFF_TIME;
                timer_setinterval( mac->timer, mac->backoff, 0 );
                timer_start( mac->timer );
                mac->state = CSMA_STATE_BACKOFF;			
            }
        }
    } /* (option & CSMA_OPTION_ACK) */
    else
	{
        if (mac->rxtx->ischnclear(mac) && ((uint16)hal_random_uint8() < mac->sendprob))
        {
            count = nac_send(mac->nac, frame, option);
            if (count > 0)
                retval = count;
            //else if (count == CSMA_IORET_
            retval = (count >= 0) ? count : CSMA_IORET_ERROR_UNKNOWN;  
			if(CSMA_IORET_SUCCESS(retval))
			{
				mac->seqid ++;	//JOE 0725
			}
        }
        else
            retval = CSMA_IORET_ERROR_ACCEPTED_AND_BUSY;
	}
     
    return retval; 
}
    
/**
 * Check whether there's some frame arrivaled. If there's a frame arrived, then place
 * it into "frame" parameter. This function can be called anytime. 
 * 
 * @attention: If csma_recv() doesn't got an frame, the input buffer "frame" may
 * still be changed. The upper layer should consider this behavior. This is different 
 * to csma_send(). 
 * 
 * @param frame Containing the frame received.
 * @param option Reseved. Should be always 0x00 now.
 * @return > 0 if frame received. 0 if none. -1 if failed.
 */
intx  csma_recv( TiCsma * mac, TiFrame * frame, uint8 option )
{
	uintx retval;
	uint16 fcf;
    char * ptr = NULL;
    uint8 cur;

	// @modified by zhangwei on 2011.03.14
	// - Since the network acceptor will reset the frame inside, we needn't call
	// frame_slipouter() and frame_moveinner() in this function. The following 
	// are deprecated:
	//
    // move the frame current layer to mac protocol related layer. the default layer
    // only contains the data (mac payload) rather than mac frame.
	// 
    // frame_skipouter( frame, HEADER_SIZE+1, TAIL_SIZE );
	
    // assert: the skipouter must be success
	// attention the network acceptor requirement of the frame layer
	retval = _csma_tryrecv( mac, frame, option );
	if (retval > 0)
	{
        // the first byte in the frame buffer is the length byte. it represents the 
        // MPDU length. after received the frame, we first check whether this is an
        // incomplete frame or not. if it's an bad frame, then we should ignore it.
        //
        ptr = frame_startptr(frame);

        if (*ptr == retval-1)
        {
            // get the pointer to the frame control field according to 802.15.4 frame format
            // we need to check whether the current frame is a DATA type frame.
			// only the DATA type frame will be transfered to upper layers. The other types, 
            // such as COMMAND, BEACON and ACK will be ignored here.
			// buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
            //
			fcf = I802F154_MAKEWORD(ptr[2], ptr[1]);
			switch (FCF_FRAMETYPE(fcf))
            {
            case FCF_FRAMETYPE_DATA:
                frame_setlength(frame, retval);
                //frame_setcapacity(frame, retval);   
                break;
                
            case FCF_FRAMETYPE_ACK:
            case FCF_FRAMETYPE_COMMAND:
            case FCF_FRAMETYPE_BEACON:
            default:
				retval = CSMA_IORET_NOACTION;
                break;
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

    // The value of retval equal to the total data length, which equals to framelength+1.
    if (retval > 0)
	{
        cur = frame_curlayer(frame);
        svc_assert(frame->layercapacity[cur] >= (CSMA_HEADER_SIZE  + CSMA_TAIL_SIZE));
        if (frame_skipinner(frame, CSMA_HEADER_SIZE+1, CSMA_TAIL_SIZE))
        {
            // substract 1 byte occupied by the frame length
            retval --;    
            retval -= CSMA_HEADER_SIZE;
            retval -= CSMA_TAIL_SIZE;
            frame_setlength(frame, retval);
        }
        else
            retval = 0;
		
		// - bug fix: since frame_skipinner can calculate correct layer capacity, 
		// the following line is unecessary now
        // frame_setcapacity( frame, retval - HEADER_SIZE - TAIL_SIZE-1 );
    }
    else
	{
        frame_setlength(frame, 0);
    }

	csma_evolve( mac, NULL );
	return retval;
}

/**
 * Try to receive an frame from low level component.
 * 
 * @param frame Containing the frame received.
 * @param option Reseved. Should be always 0x00 now.
 * @return > 0 if frame received. 0 if none. -1 if failed.
 */
intx _csma_tryrecv( TiCsma * mac, TiFrame * frame, uint8 option )
{   
	//TiFrameTxRxInterface  * rxtx = mac->rxtx;
	intx retval;

	// @attention: According to CSMA protocol, the program should send ACK/NAK after 
	// receiving a data frame. however, this is done by the low level transceiver's
	// adapter component (TiCc2420Adapter), so we needn't to send ACK manually here.

	// retval = rxtx->recv( rxtx->provider, frame_startptr(frame), frame_capacity(frame), option );
	retval = nac_recv( mac->nac, frame, option);
	return retval;
}

/**
 * This function is called when a ACK frame received 
 */

void _csma_on_ack_frame(TiCsma * mac, TiFrame * frame)
{
}

void _csma_on_command_frame(TiCsma * mac, TiFrame * frame)
{
}

void _csma_on_beacon_frame(TiCsma * mac, TiFrame * frame)
{
}

/* this function can be used as TiCc2420Adapter's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using csma_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void csma_evolve( void * macptr, TiEvent * e )
{  
	TiCsma * mac = (TiCsma *)macptr;
	uintx retval;
    TiEvent tmpe;


	mac->rxtx->evolve( mac->rxtx, NULL );

    switch (mac->state)
    {
    case CSMA_STATE_IDLE:
        break;

    case CSMA_STATE_BACKOFF:
        
		// Retry sending the frame inside mac->txbuf. if sending successfully, then 
		// transfer to IDLE state. if failed, then still in WAITFOR_SENDING state.
        
        //while ( !timer_expired( mac->timer))//我觉得这一句应该加上去 201108
        //{
        //}
        if (timer_expired(mac->timer))
		{
			retval = 0;
			// Try to send the frame in mac->txbuf again. The _csma_trysend() function
			// will deal with necessary ACK processing, sequence processing and 
			// try limitation checking. 
			//
			// After _csma_trysend() call, the mac component will stay in CSMA_STATE_IDLE
			// or CSMA_STATE_BACKOFF state. If the sending is successfully or the 
			// retry reaches its maximum limitation count, then _csma_trysend() 
			// will transfer the state to CSMA_STATE_IDLE automatically. If the 
			// retry failed and not exceed the maximum sending limitation, then 
			// the state will still in CSMA_STATE_BACKOFF state. The backoff timer
			// will also be started by _csma_trysend() function.
			

			retval = _csma_trysend(  mac, mac->txbuf, mac->txbuf->option );
			if (CSMA_IORET_SUCCESS(retval) )
			{                
				mac->state = CSMA_STATE_IDLE;
				mac->retry = 0;//JOE 0726
//                if (mac->listener != NULL)
//                {
//                    memset(&tmpe, 0x00, sizeof(TiEvent));
//                    e.id = EVENT_SEND_COMPLETE;
//                    mac->listener( mac->lisowner, e );
//                }
			}
			mac->success = retval;
		}
		break;
    
    case CSMA_STATE_SLEEPING:
		mac->state = CSMA_STATE_IDLE;
        //todo addded by Jiang Ridong on 2011.04.16
        /* 
        if (e->id = wakeuprequest)
        {
            phy_wakeup();
            mac->state = ADTCSMA_IDLE;
        }
        */

    default:
		mac->state = CSMA_STATE_IDLE;
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
        case ADTCSMA_EVENT_SHUTDOWN_REQUEST:
            // no matter what the current state is, then you can do shutdown
            vti_stop(); 
            phy_shutdown();
            mac->state = SHUTDOWN;
            break;    

        case ADTCSMA_EVENT_STARTUP_REQUEST: 
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

void csma_statistics( TiCsma * mac, TiCsmaStatistics * stat )
{
    stat->sendcount = mac->stat.sendcount;
    stat->sendfailed = mac->stat.sendfailed;
	stat->recvcount = mac->stat.recvcount;
	
    mac->stat.sendfailed = 0;
	mac->stat.sendcount = 0;
	mac->stat.recvcount = 0;
}
