#ifndef _SVC_CSMA_H_4829_
#define _SVC_CSMA_H_4829_
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

/*******************************************************************************
 * svc_csma
 * This module implements the standard carrier sense multi-access (CSMA) protocol. 
 * If you want more Collision Avoidance (CA) features, you can choose MACA protocol 
 * in module svc_maca.
 * 
 * @state
 *  compiled successfully. tested.
 *
 * @author zhangwei in 2009.12
 *  - first created
 * @modified by zhangwei on 2010.05.08
 *  - revision. replace original "cc2420.h" with "hal_frame_transceiver.h". So this
 *    module doesn't depend on the cc2420 from now on. It can run on other device
 *    drivers if the "TiFrameTxRxTransceiver" interface.
 * 
 * @modified by zhangwei on 2010.06.13
 *  - replace TiIoBuf with TiFrame
 * @modified by zhangwei on 2010.08.24
 *  - revised.
 * 
 * @modified by xu-fuzhen(Control Department, TongJi University) on 2010.10.12
 *  - revision. tested ok. 
 * 
 ******************************************************************************/

/**
 * CONFIG_CSMA_BROADCAST_ADDRESS
 * This is an const value equals 0xFFFF. You shouldn't change it.
 *
 * CONFIG_CSMA_MAX_RETRY
 * Maxium sending retry count when encounter sending failure.
 *
 * CONFIG_CSMA_ACK_RESPONSE_TIME
 * Maximum duration to wait for the ACK frame.
 *
 * CONFIG_CSMA_MAX_BACKOFF
 * Maximum backoff delay time. the really backoff time is a random number between 
 * 0 and CONFIG_ALOHA_MAX_BACKOFF. Currently, it's set to 100 milliseconds. You should 
 * optimize it according to your own network parameters.
 */

#define CONFIG_CSMA_DEFAULT_PANID			    0x0001
#define CONFIG_CSMA_DEFAULT_LOCAL_ADDRESS		0x07 
#define CONFIG_CSMA_DEFAULT_REMOTE_ADDRESS		0x08
#define CONFIG_CSMA_DEFAULT_CHANNEL            11
#define CONFIG_CSMA_BROADCAST_ADDRESS          FRAME154_BROADCAST_ADDRESS

#define CONFIG_CSMA_MAX_RETRY                  3
#define CONFIG_CSMA_ACK_RESPONSE_TIME          10

#define CONFIG_CSMA_MAX_FRAME_SIZE             128

#define CONFIG_CSMA_TRX_ACK_SUPPORT
#define CONFIG_CSMA_MAX_BACKOFF                100
#define CONFIG_CSMA_MIN_BACKOFF                0

#define CONFIG_CSMA_STANDATD

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"


/**
 * TiCsma is a enhanced version of the fundamental ALOHA medium access
 * protocol. It has the same interface with TiAloha. 
 */

/**
 * Q: what's CSMA medium access protocol?
 * R: CSMA means carrier sense multiple access. 
 * 
 * Q: what's p-persistent CSMA?
 * R: When the sender is ready to send data, it checks continually if the medium is 
 * busy. If the medium becomes idle, the sender transmits a frame with a probability p. 
 * If the station chooses not to transmit (the probability of this event is 1-p),
 * the sender waits until the next available time slot and transmits again. This 
 * process repeats until the frame is sent or some other sender stops transmitting. 
 * In the latter case the sender monitors the channel, and when idle, transmits with 
 * a probability p, and so on.
 *
 * link: http://en.wikipedia.org/wiki/Carrier_sense_multiple_access
 */

/* reference
 * - 基于短距离无线传输的CSMA/CA协议实现方法, http://www.dzsc.com/data/html/2010-7-5/83921.html;
 * - 载波侦听多路访问协议介绍, http://www.pcdog.com/network/protocol/2005/10/e038098.html;
 * - 无线传感器网络CSMA协议的设计与实现, http://blog.21ic.com/user1/1600/archives/2009/61918.html;
 * - CSMA退避算法, http://book.51cto.com/art/200911/163450.htm;
 * - CSMA, Carrier sense multiple access, http://en.wikipedia.org/wiki/Carrier_sense_multiple_access;
 * - CSMA/CA, Carrier sense multiple access with collision avoidance, http://en.wikipedia.org/wiki/CSMA_CA;
 * - IEEE 802.11 RTS/CTS, http://en.wikipedia.org/wiki/IEEE_802.11_RTS/CTS;
 * - MACA, Multiple Access with Collision Avoidance, 
 *   http://en.wikipedia.org/wiki/Multiple_Access_with_Collision_Avoidance;
 * - Multiple Access with Collision Avoidance for Wireless, http://en.wikipedia.org/wiki/MACAW;
 */


#define CSMA_OPTION_ACK                    0x00
#define CSMA_DEF_OPTION                    0x00

/* In order to support low power implementation, current version CSMA component support 
 * four external request.
 */ 
#define CSMA_STARTUP_REQUEST                1
#define CSMA_SHUTDOWN_REQUEST               2
#define CSMA_SLEEP_REQUEST                  3
#define CSMA_WAKEUP_REQUEST                 4

/* cama state:
 * - IDLE: wait for sending and receiving. since the receiving is a fast process
 *      then i give up design a special state for receiving. 
 *      receiving process can be occur in any state.
 * - WAITFOR_SENDING: wait for PHY layer to sending buffer frame (namely, acceess 
 *      the channel)
 * - SLEEPING: sleep mode.
 */

#define CSMA_STATE_NULL                    0
#define CSMA_STATE_IDLE                    1
//#define CSMA_STATE_WAITFOR_CHANNELCLEAR    2
#define CSMA_STATE_BACKOFF                 3
#define CSMA_STATE_SLEEPING                4
#define CSMA_STATE_POWERDOWN               5


/* event definition */
#define CSMA_EVENT_FRAME_ARRIVAL           7

    
typedef struct{
    uint8 state;
	TiFrameTxRxInterface * rxtx;
	TiTimerAdapter * timer;
	TiFrame * txbuf;
    uint8 sendprob;
    uint8 loadfactor;
    uint8 request;
    uint8 retry;
	uint16 backoff;
    uint16 panto;
    uint16 shortaddrto;
    uint16 panfrom;
    uint16 shortaddrfrom;
    uint8 seqid;
    uint8 sendoption;
    uint8 sendfailed;
    TiIEEE802Frame154Descriptor desc;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
	char txbuf_memory[FRAME_HOPESIZE(CONFIG_CSMA_MAX_FRAME_SIZE)];
}TiCsma; 

#ifdef __cplusplus
extern "C"{
#endif

TiCsma * csma_construct( char * buf, uint16 size );
void csma_destroy( TiCsma * mac );
TiCsma * csma_open( TiCsma * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid, uint16 address, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner );
void csma_close( TiCsma * mac );

uintx csma_send( TiCsma * mac, TiFrame * frame, uint8 option );
uintx csma_broadcast( TiCsma * mac, TiFrame * frame, uint8 option );
uintx csma_recv( TiCsma * mac, TiFrame * frame, uint8 option );
void csma_evolve( void * macptr, TiEvent * e );

//#define csma_setshortaddress(mac,addr) mac->rxtx->setshortaddress((mac->rxtx->provider),(addr))
//#define csma_setpanid(mac,pan) mac->rxtx->setpanid((mac->rxtximpl),(pan))
//#define csma_setchannel(mac,chn) mac->rxtx->setchannel((mac->rxtximpl),(chn))
//#define csma_ischannelclear(mac) mac->rxtx->ischannelclear((mac->rxtx))

inline void csma_setlocaladdress( TiCsma * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void csma_setremoteaddress( TiCsma * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void csma_setpanid( TiCsma * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void csma_setchannel( TiCsma * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

inline bool csma_ischannelclear( TiCsma * mac )
{
    // return (mac->rxtx->ischnclear == NULL) ? true : mac->rxtx->ischnclear( mac->rxtx->provider );
    return true;
}

void csma_statistics( TiCsma * mac, uint16 * sendcount, uint16 * sendfailed );


#ifdef __cplusplus
}
#endif

#endif /* _SVC_CSMA_H_4829_ */
