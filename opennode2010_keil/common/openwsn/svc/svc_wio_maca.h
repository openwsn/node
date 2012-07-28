#ifndef _SVC_MACA_H_4829_
#define _SVC_MACA_H_4829_
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
 * svc_maca
 * The implementation of MACA protocol (Multiple Access with Collision Avoidance).
 * 
 *
 * @state
 *   finished developing. needs fully testing.
 *
 * @author zhangwei in 200912
 *  - first created
 * @modified by zhangwei on 2010.05.10
 *  - revision. 
 * @modified by zhangwei on 2010.06.14
 *  - revision. compile passed
 * 
 * @modified by openwsn on 2010.08.24
 *  - upgraded TiOpenFrame/TiIoBuf to TiFrame
 *  - fully revised.
 * 
 * @modified by zhangwei(Control Department, TongJi University) on 2010.10.18
 *  - revision.
 * 
 ******************************************************************************/

/**
 * CONFIG_MACA_BROADCAST_ADDRESS
 * This is an const value equals 0xFFFF. You shouldn't change it.
 *
 * CONFIG_MACA_MAX_RETRY
 * Maxium sending retry count when encounter sending failure.
 *
 * CONFIG_MACA_ACK_RESPONSE_TIME
 * Maximum duration to wait for the ACK frame.
 *
 * CONFIG_CSMA_MAX_BACKOFF
 * Maximum backoff delay time. the really backoff time is a random number between 
 * 0 and CONFIG_ALOHA_MAX_BACKOFF. Currently, it's set to 100 milliseconds. You should 
 * optimize it according to your own network parameters.
 *
 * CONFIG_MACA_WAIT_CTS_DURATION
 * The maximum duration to wait the CTS frame after RTS is sent.
 */

#define CONFIG_MACA_DEFAULT_PANID				0x0001
#define CONFIG_MACA_DEFAULT_LOCAL_ADDRESS		0x07
#define CONFIG_MACA_DEFAULT_REMOTE_ADDRESS      0x08
#define CONFIG_MACA_DEFAULT_CHANNEL            11
#define CONFIG_MACA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_MACA_MAX_RETRY                  3
#define CONFIG_MACA_ACK_RESPONSE_TIME          10

#define CONFIG_MACA_MAX_FRAME_SIZE             128

#define CONFIG_MACA_TRX_ACK_SUPPORT
#define CONFIG_MACA_MAX_BACKOFF                100
#define CONFIG_MACA_MIN_BACKOFF                0

#define CONFIG_MACA_WAIT_CTS_DURATION          10
#define CONFIG_MACA_RTS_SLEEPTIME              10

#define CONFIG_MACA_STANDARD 
#define CONFIG_MACA_TRX_SUPPORT_ACK

#define CONFIG_MACA_DURATION                    100


#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"

/* @todo 2010.08
 * this implementation should continue stop sending if the RTS required time silent 
 * duration doesn't expired.
 * however, the current implementation doesn't care about it.
 * => You need at least 2 timer to deal with this. however, we have used only 1 now.
 * so it's a little difficult for us to do.
 */

/* Q: What's MACA/MACAW protocol?
 *  Multiple Access with Collision Avoidance (MACA) is a media access control protocol 
 * used in wireless LAN data transmission to avoid collisions caused by the hidden 
 * station problem and to simplify exposed station problem.
 *  The basic idea of MACA is a wireless network node makes an announcement before 
 * it sends the data frame to inform other nodes to keep silent. When a node wants 
 * to transmit, it sends a signal called Request-To-Send (RTS) with the length of 
 * the data frame to send. If the receiver allows the transmission, it replies the 
 * sender a signal called Clear-To-Send (CTS) with the length of the frame that is 
 * about to receive.
 *  Meanwhile, a node that hears RTS should remain silent to avoid conflict with CTS; 
 * a node that hears CTS should keep silent until the data transmission is complete.
 * WLAN data transmission collisions may still occur, and the MACA for Wireless (MACAW) 
 * is introduced to extend the function of MACA. It requires nodes sending acknowledgements 
 * after each successful frame transmission, as well as the additional function of 
 * Carrier sense.
 * 
 * reference:
 * - Multiple Access with Collision Avoidance, wikipedia, 
 *   http://en.wikipedia.org/wiki/Multiple_Access_with_Collision_Avoidance;
 * - Hidden node problem, http://en.wikipedia.org/wiki/Hidden_station_problem;
 * - Exposed terminal problem, http://en.wikipedia.org/wiki/Exposed_station_problem;
 * - Multiple Access with Collision Avoidance for Wireless, wikipedia,
 *   http://en.wikipedia.org/wiki/MACAW;
 * - Phil Karn: MACA - A New Channel Access Method for Packet Radio (Phil Karn, KA9Q)
 */


#define MACA_SLEEP_REQUEST                     1
#define MACA_WAKEUP_REQUEST                    2
#define MACA_SHUTDOWN_REQUEST                  3
#define MACA_STARTUP_REQUEST                   4

#define MACA_EVENT_FRAME_ARRIVAL               234 // todo

#define MACA_RXBUF_MEMSIZE FRAME_HOPESIZE(CONFIG_CSMA_MAX_FRAME_SIZE)
#define MACA_TXBUF_MEMSIZE FRAME_HOPESIZE(CONFIG_CSMA_MAX_FRAME_SIZE)

#define MACA_HEADER_SIZE 12
#define MACA_TAIL_SIZE 2


/* MACA state:
 * - IDLE: wait for sending and receiving. since the receiving is a fast process
 *      then i give up design a special state for receiving. 
 *      receiving process can be occur in any state.
 * - WAITFOR_SENDING: wait for PHY layer to sending buffer frame (namely, acceess 
 *      the channel)
 * - SLEEPING: sleep mode.
 */
#define MACA_STATE_SHUTDOWN                0
#define MACA_STATE_IDLE                     1
#define MACA_STATE_BACKOFF                  3
#define MACA_STATE_SLEEPING                 4

/**
 * TiMACA component:
 *	- state: state of the maca protocol component.
 *  - rxtx: the transceiver interface
 *  - timer: hardware timer used by the mac protocol
 *  - backoff: backoff duration when sending failed. 
 */
typedef struct{
    uint8 state;
	TiFrameTxRxInterface * rxtx;
	TiTimerAdapter * timer;
	TiFrame * txbuf;
	TiFrame * rxbuf;
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
	char txbuf_memory[FRAME_HOPESIZE(CONFIG_MACA_MAX_FRAME_SIZE)];
	char rxbuf_memory[FRAME_HOPESIZE(CONFIG_MACA_MAX_FRAME_SIZE)];
}TiMACA; 

#ifdef __cplusplus
extern "C"{
#endif

TiMACA * maca_construct( char * buf, uint16 size );
void maca_destroy( TiMACA * mac );
TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid,
    uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner );
void maca_close( TiMACA * mac );

uintx maca_send( TiMACA * mac, TiFrame * frame, uint8 option );
uintx maca_broadcast( TiMACA * mac, TiFrame * frame, uint8 option );
uintx maca_recv( TiMACA * mac, TiFrame * frame, uint8 option );
void  maca_evolve( void * macptr, TiEvent * e );

inline void maca_setlocaladdress( TiMACA * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void maca_setremoteaddress( TiMACA * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void maca_setpanid( TiMACA * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void maca_setchannel( TiMACA * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

inline bool maca_ischannelclear( TiMACA * mac )
{
    // todo
    // return (mac->rxtx->ischnclear == NULL) ? true : mac->rxtx->ischnclear( mac->rxtx->provider );
    return true;
}

void maca_statistics( TiMACA * mac, uint16 * sendcount, uint16 * sendfailed );


#ifdef __cplusplus
}
#endif

#endif
