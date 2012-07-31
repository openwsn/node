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

#ifndef _SVC_NIO_CSMA_H_4829_
#define _SVC_NIO_CSMA_H_4829_

/******************************************************************************* 
 * svc_csma.c
 * Standard CSMA medium access control (MAC) protocol.
 * Thought it's called standard, this implementation has an special feature comparing 
 * to standard CSMA protocol on that: it will insert a random delay before each 
 * sending in order to decrease wakeup sending conflictions.
 * 
 * @todo
 * you should replace the infinite delay when waiting for ACK frame with timer
 * driven delay in the future!!
 *
 * you may need to add seqid management in MAC layer!
 * 
 * @status
 *	- released.
 * 
 * @modified by zhangwei on 2010.05.10
 *  - the original csma module was divided into two branches: simple csma and 
 *    standard csma. the simple csma doesn't use timer so it's easy for demonstration. 
 *    and the standard csma use timer to control the delay time and waiting time,
 *    thus it's more reasonable and practical. 
 *  - simple csma and standard csma are designed to share the same interface.
 * @modified by zhangwei on 2010.08.23
 *  - revision
 * @modified by zhangwei on 2010.08.24
 *  - correct bug in csma_ischannelclear(). the return value should be bool type
 * @modified by Jiang Ridong in 2011.08
 *  - tested Ok.
 * @modified by zhangwei on 2012.07.20
 *
 ******************************************************************************/  

/**
 * @reference
 * - CSMAnet, http://en.wikipedia.org/wiki/Aloha_protocol#The_CSMA_protocol;
 */
 
#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_mcu.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
 
/**
 * CONFIG_CSMA_MAX_BACKOFF
 * Maximum backoff delay time. the really backoff time is a random number between 
 * 0 and CONFIG_CSMA_MAX_BACKOFF. Currently, it's set to 100 milliseconds. You should 
 * optimize it according to your own network parameters.
 * 
 * CONFIG_CMSA_MIN_ACK_TIME
 * CONFIG_CSMA_MAX_ACK_TIME
 * These two macros defines the range for ACK waiting. You should tune it according
 * to the transceiver parameters.
 */

#ifndef CONFIG_CSMA_DEFAULT_PANID
#define CONFIG_CSMA_DEFAULT_PANID              0x0001
#endif

#define CONFIG_CSMA_DEFAULT_CHANNEL            11

#define CONFIG_CSMA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_CSMA_MAX_RETRY                  3
#define CONFIG_CSMA_ACK_RESPONSE_TIME          10

#define CONFIG_CSMA_MAX_FRAME_SIZE             127

/* Time unit is milliseconds?
 * @todo
 */
#define CONFIG_CSMA_MIN_ACK_TIME               1
#define CONFIG_CSMA_MAX_ACK_TIME               50

#define CONFIG_CSMA_MAX_BACKOFF_TIME           100
#define CONFIG_CSMA_MIN_BACKOFF_TIME           2    //�ú���������0������timer�������쳣

/* Set the module work in standard CSMA behavior without any optimization */
#define CONFIG_CSMA_STANDARD

/* The csma header here includes: 
 *  - 2B for frame control
 *  - 1B for sequence number
 *  - 4B for destination address
 *  - 4B for source address
 * So the total header length is 11.
 * 
 * @attention
 * - The first byte, which includes the total length of the frame is actually
 *   not belong to the MAC layer. So this csma header doesn't include this byte.
 * - For 802.15.4 compatible transcevers, the MAC header is determined by the 802.15.4
 *   protocol. The above header definition is actually only one possible format
 *   according to 802.15.4 specification.
 */
#define CSMA_HEADER_SIZE   11
#define CSMA_TAIL_SIZE     2

/* The p-insist sending probability = CSMA_P_INSIST_INDICATOR / 255 */
#define CSMA_P_INSIST_INDICATOR 200

#define CSMA_OPTION_ACK                    0x01
#define CSMA_OPTION_NOACK                  0x00
#define CSMA_DEF_OPTION                    CSMA_OPTION_NOACK

#define CSMA_IORET_SUCCESS(ret)             ((ret)>0)
#define CSMA_IORET_NOACTION                 0
#define CSMA_IORET_ERROR_NOACK              -1
#define CSMA_IORET_ERROR_ACCEPTED_AND_BUSY  -2
#define CSMA_IORET_ERROR_BUSY               -3
#define CSMA_IORET_ERROR_UNKNOWN            -4

/* standard csma protocol state:
 * - IDLE: wait for sending and receiving. since the receiving is a fast process
 *         then i give up design a special state for receiving. 
 *         receiving process can be occur in any state.
 * - WAITFOR_SENDING: wait for sending 
 * - BACKOFF:
 * - SLEEP: sleep mode.
 */

#define CSMA_STATE_NULL                    0
#define CSMA_STATE_IDLE                    1
#define CSMA_STATE_BACKOFF                 2
#define CSMA_STATE_SLEEPING                3

typedef struct{
    uint16 sendcount;
    uint16 sendfailed;
    uint16 recvcount;
    uint16 dropped;
}TiCsmaStatistics;

#pragma pack(1) 
typedef struct{
	uint8 state;
    TiFrameTxRxInterface * rxtx;
	TiNioAcceptor *nac;
	TiTimerAdapter * timer;
	TiFrame * txbuf;
    uint8 retry;
	uint16 backoff;
    uint16 panto;
    uint16 shortaddrto;
    uint16 panfrom;
    uint16 shortaddrfrom;
    uint8 seqid;
    uint8 sendprob; 
    TiCsmaStatistics stat;
    TiIEEE802Frame154Descriptor desc;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
    char txbuf_memory[FRAME_HOPESIZE(CONFIG_CSMA_MAX_FRAME_SIZE+1)];
	uintx success;
}TiCsma;

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Construct an TiNioAloha object inside the specified memory block. Attention the memory 
 * block must be large enough to hold the object.
 */
TiCsma * csma_construct( char * buf, uint16 size );

/** 
 * Destroy the TiNioAloha object.
 */
void csma_destroy( TiCsma * mac );

/**
 * Open the TiNioAloha object for sending and receiving. 
 */
TiCsma * csma_open( TiCsma * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, uint8 option );

/**
 * Close the TiNioAloha object. The closing process will release resource applied by 
 * the csma object. 
 */
void csma_close( TiCsma * mac );


/**
 * \brief csma_send() will send the frame object with the option. This function will
 * duplicate a frame object inside the MAC object, so the original input frame keeps
 * unchanged.
 *
 * @param mac mac component
 * @param frame the frame object to be sent
 * @param option sending option. controls the ACK behavior currently. please refer to 
 *   the transceiver's driver interface. 
 *      current settings: if bit 0 of option is 1, then this function will request ACK 
 *   from the receiver. the ack is handled inside mac or transceiver object.
 * 
 * @return
 *  if the frame can be accepted successfully, then this function will return positive
 * value. however, theoretically speaking, the hardware may haven't really send the 
 * frame out. you should guarantee to repeated call csma_evolve() to try to send the
 * frame out if this occurs.
 */
intx csma_send( TiCsma * mac, uint16 shortaddrto, TiFrame * frame, uint8 option );

/**
 * \brief This function broadcast a frame out.
 */
intx csma_broadcast( TiCsma * mac, TiFrame * frame, uint8 option );

/**
 * \brief This function try to receive a frame and place it into parameter "frame".
 * If there's no frame coming, then returns 0.
 */
intx csma_recv( TiCsma * mac, TiFrame * frame, uint8 option );

inline void csma_setlistener( TiCsma * mac, TiFunEventHandler listener, void * lisowner )
{
	mac->listener = listener;
	mac->lisowner = lisowner;
} 


/**
 * \brief Evolve the state machine of csma protocol.
 */
void csma_evolve( void * macptr, TiEvent * e );

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

// todo
inline bool csma_ischannelclear( TiCsma * mac )
{
    // return (mac->rxtx->ischnclear == NULL) ? true : mac->rxtx->ischnclear( mac->rxtx->provider );
    return true;
}

inline void csma_setsendprobability( TiCsma * mac , uint8 probability)
{
	mac->sendprob = probability;
}


void csma_statistics( TiCsma * mac, TiCsmaStatistics * stat );

#ifdef __cplusplus
}
#endif

#endif /* _SVC_NIO_CSMA_H_4829_ */

