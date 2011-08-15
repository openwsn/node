#ifndef _SVC_TINYMAC_H_4829_
#define _SVC_TINYMAC_H_4829_

/* 
 *******************************************************************************
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
 */

/** 
 * svc_tinymac
 * TiTinyMAC is based on the TiFrameTxRxInterface interface directly. It transforms 
 * the memory buffer based transceiver interface into a TiFrame based interface and
 * also support the Medium Access Control (MAC) interface.
 * 
 * @status
 *	- developing
 * 
 * @history
 * @author xu fuzhen and zhang wei on 2010.08.22
 *  - first developed.
 *  
 * @modified by XXX on 2010.08.23
 *  - compile passed.
 */

#define CONFIG_TINYMAC_DEFAULT_PANID		 0x0001
#define CONFIG_TINYMAC_DEFAULT_CHANNEL       11
#define CONFIG_TINYMAC_BROADCAST_ADDRESS     0xFFFF

#define CONFIG_TINYMAC_ACK_RESPONSE_TIME     10

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

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "svc_nio_acceptor.h"
#include "svc_foundation.h"

#define TINYMAC_OPTION_ACK 0x00
#define TINYMAC_DEF_OPTION 0x00

#ifdef __cplusplus
extern "C"{
#endif

/* TiTinyMAC:
 * Sending/receiving frames directly. It's just a simple encapsulation of the basic
 * transceiver interface into TiFrame based interface.
 * 
 * You can regard it as a ultra simple medium access control (MAC) component. But
 * it's more simpler than TiSimplaAloha and TiAloha.
 */
typedef struct{
	uint8 state;
    TiFrameTxRxInterface * rxtx;
	TiNioAcceptor * nac;
    uint16 panto;
    uint16 shortaddrto;
    uint16 panfrom;
    uint16 shortaddrfrom;
    uint8 seqid;
    TiIEEE802Frame154Descriptor desc;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
}TiTinyMAC; 

TiTinyMAC *	tinymac_construct( char * buf, uintx size );
void tinymac_destroy( TiTinyMAC * mac );

/**
 * open the tiny mac service component for sending and recving. 
 *
 * attention: you should open the transceiver component successfully before calling
 * tinymac_open().
 */

TiTinyMAC *	tinymac_open( TiTinyMAC * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
			uint16 address, TiFunEventHandler listener, void * lisowner, uint8 option );
void        tinymac_close( TiTinyMAC * mac );

/** if bit 0 of option is 1, then this function will request ACK from the receiver.
 */
intx tinymac_send( TiTinyMAC * mac,TiFrame * frame, uint8 option);
intx tinymac_broadcast( TiTinyMAC * mac, TiFrame * frame, uint8 option );
intx tinymac_recv( TiTinyMAC * mac, TiFrame * frame, uint8 option );

void tinymac_evolve( void * macptr, TiEvent * e );

inline void tinymac_setlocaladdress( TiTinyMAC * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void tinymac_setremoteaddress( TiTinyMAC * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void tinymac_setpanid( TiTinyMAC * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void tinymac_setchannel( TiTinyMAC * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

inline void tinymac_ischannelclear( TiTinyMAC * mac )
{
    mac->rxtx->ischnclear( mac->rxtx->provider );
}

#ifdef __cplusplus
}
#endif

#endif /* _SVC_TINYMAC_H_4829_ */
