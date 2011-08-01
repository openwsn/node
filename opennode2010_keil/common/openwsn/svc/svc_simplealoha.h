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

#ifndef _SVC_SIMPLEALOHA_H_4829_
#define _SVC_SIMPLEALOHA_H_4829_

/****************************************************************************** 
 * svc_aloha.c
 * ALOHA medium access control (MAC) protocol 
 * 
 * @todo
 * you should replace the infinite delay when waiting for ACK frame with timer
 * driven delay in the future!! 
 *
 * you may need to add seqid management in MAC layer!
 * 
 * @status
 *	- tested
 * 
 * @history
 * @modified by zhangwei on 2010.07.08
 *  - use transceiver interface instead of call TiCc2420Transceiver directly;
 *  - replace TiOpenFrame with TiFrame in the send()/recv() function.
 * 
 * @modified by zhangwei on 2010.07.18
 *  - replace TiAloha with TiSimpleAloha, and "aloha_" with "saloha_". this is to 
 *    avoid conflictions with module "svc_aloha"
 *
 *****************************************************************************/  

/* Q: what's simple aloha?
 * R: simple aloha is the aloha protocol without randome delay.
 *  - start sending whenever the frame is ready
 *  - ack frame 
 *  - fixed delay duration when sending failed. this is because the simplealoha
 *    doesn't use timer for simplicity reason.
 * 
 * due to the convenience of cc2420 transceiver, the current simple aloha supports:
 *  - csma feature
 * though this isn't necessary in aloha theory.
 * 
 * Q: the difference between simple aloha and standard aloha?
 * R: the standard aloha introduce random delay when sending failed
 */

/* The "aloha" MAC protocol implementation requires the following interface functions 
 * of the wireless transceiver:
 *	- setlocaladdress;
 *	- setremoteaddress;
 *	- setpanid;
 *  - setchannel;
 *  - ischannelclear;
 *  - send;
 *  - write;
 *  - sleep;
 *  - wakeup;
 */

#define CONFIG_ALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ALOHA_DEFAULT_CHANNEL            11
#define CONFIG_ALOHA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_ALOHA_MAX_RETRY                  3
#define CONFIG_ALOHA_ACK_RESPONSE_TIME          10

/* initial backoff time if encountered confliction */

#define CONFIG_ALOHA_BACKOFF                    10

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "svc_foundation.h"

#define ALOHA_OPTION_ACK 0x00
#define ALOHA_DEF_OPTION 0x00

#ifdef __cplusplus
extern "C"{
#endif

/* TiSimpleAloha:
 * A simplied version of ALOHA protocol implementation. Comparing to the standard
 * implementation, the simple version:
 * - using hal_delay() to generate delay instead of timer
 * - the CPU will continue wait during back off stage due to the hal_delay() call.
 *   so the efficiency is low. This is improved in the standard version.
 */
typedef struct{
	uint8 state;
    TiFrameTxRxInterface * rxtx;
	//TiTimerAdapter * timer;
    uint8 retry;
	uint16 backoff;
    uint16 panto;
    uint16 shortaddrto;
    uint16 panfrom;
    uint16 shortaddrfrom;
    uint8 seqid;
    TiIEEE802Frame154Descriptor desc;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
}TiSimpleAloha;


TiSimpleAloha *	saloha_construct( char * buf, uintx size );
void saloha_destroy( TiSimpleAloha * mac );

/**
 * open the simple aloha service component for sending and recving. 
 *
 * attention: you should open the transceiver component successfully before calling
 * saloha_open().
 */

TiSimpleAloha *	saloha_open( TiSimpleAloha * mac, TiFrameTxRxInterface * rxtx, uint8 chn, uint16 panid, 
			uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option );
void        saloha_close( TiSimpleAloha * mac );

/** if bit 0 of option is 1, then this function will request ACK from the receiver.
 */
uintx       saloha_send( TiSimpleAloha * mac, TiFrame * frame, uint8 option );
uintx       saloha_broadcast( TiSimpleAloha * mac, TiFrame * frame, uint8 option );
uintx       saloha_recv( TiSimpleAloha * mac, TiFrame * frame, uint8 option );
void        saloha_evolve( void * macptr, TiEvent * e );

inline void saloha_setlocaladdress( TiSimpleAloha * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void saloha_setremoteaddress( TiSimpleAloha * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void saloha_setpanid( TiSimpleAloha * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void saloha_setchannel( TiSimpleAloha * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

inline void saloha_ischannelclear( TiSimpleAloha * mac )
{
    mac->rxtx->ischnclear( mac->rxtx->provider );
}

#ifdef __cplusplus
}
#endif

#endif /* _SVC_SIMPLEALOHA_H_4829_ */
