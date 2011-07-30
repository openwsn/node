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

#ifndef _SVC_FLOOD_H_4764_
#define _SVC_FLOOD_H_4764_

/******************************************************************************
 * svc_flood
 * This module implements a simple flooding mechanism in a multi-hop wireless
 * network. 
 *
 * @author zhangwei in 2007.06
 *	- first created
 * @modified by zhangwei in 2009.07
 *	- revision. done
 *
 *****************************************************************************/

/* todo: flood is NET layer's protocol. flood's seqid is essentially different from
 * the MAC's seqid. However, in our current implementation, they two are the same.
 * attention the aloha MAC doesn't manage the seqid now.
 */

/* @attention: you can also replace module with "svc_adaptaloha.h" to enable the 
 * Adaptive ALOHA protocol 
 */

#include "svc_configall.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_target.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_debugio.h"
#include "svc_foundation.h"
//#include "svc_aloha.h"  //todo
#include "svc_nio_aloha.h"

#ifndef CONFIG_MAX_FLOOD_COUNT 
  #define CONFIG_MAX_FLOOD_COUNT 5
#endif

#ifndef CONFIG_FLOOD_CACHE_SIZE 
  #define CONFIG_FLOOD_CACHE_SIZE 8
#endif

#define CONFIG_FLOOD_CACHE_MAX_LIFETIME 8

/* Network layer packet format used in this flooding module:
 *  [1B Hopcount] [1B Maximum Hopcount] [1B LED Control]
 */

#define PACKET_CUR_HOPCOUNT(msdu) ((msdu)[0])
#define PACKET_MAX_HOPCOUNT(msdu) ((msdu)[1])
#define PACKET_SET_HOPCOUNT(msdu,hop) ((msdu)[0] = hop)
#define PACKET_SET_MAX_HOPCOUNT(msdu,hop) ((msdu)[1] = hop)

#define FLOOD_STATE_IDLE 0
#define FLOOD_STATE_WAITFOR_TXREPLY 1

#ifdef __cplusplus
extern "C"{
#endif
/* lifetime = 0 means this is an empty item in the cache. The bigger the lifetime, 
 * the newer the lifetime.
 */
typedef struct{
	uint16				lifetime;
	uint16				panto;
	uint16				shortaddrto;
	uint16				panfrom;
	uint16				shortaddrfrom;
	uint8				seqid;
}_TiFloodCacheItem;

/* TiFlootNetwork
 *	distance		The distance between current node and the tree root node. It's
 *                  measured by the minimal hopcount. For root node, the value is 0.
 */
typedef struct{
	uint8               state;
	TiAloha *			mac;
	uint8				distance;
	uint16              panto;
	uint16              panfrom;
	uint16              localaddress;
	TiOpenFrame * 		txque;
	TiOpenFrame *		rxque;
	TiOpenFrame *       rxbuf;
	TiFunEventHandler   listener;
	void *              lisowner;
	char                opf1[ OPF_SUGGEST_SIZE ];
	char                opf2[ OPF_SUGGEST_SIZE ];
	char                opf3[ OPF_SUGGEST_SIZE ];
	_TiFloodCacheItem	cache[CONFIG_FLOOD_CACHE_SIZE]; 
}TiFloodNetwork;

TiFloodNetwork * flood_construct( void * mem, uint16 size );
void flood_destroy( TiFloodNetwork * net );
TiFloodNetwork * flood_open( TiFloodNetwork * net, TiAloha * mac, TiFunEventHandler listener, 
	void * lisowner, uint16 pan, uint16 localaddress );
void flood_close( TiFloodNetwork * net );
uint8 flood_broadcast( TiFloodNetwork * net, TiOpenFrame * opf, uint8 option );
uint8 flood_send( TiFloodNetwork * net, TiOpenFrame * opf, uint8 option );
uint8 flood_recv( TiFloodNetwork * net, TiOpenFrame * opf, uint8 option );
void flood_set_listener( TiFloodNetwork * net, TiFunEventHandler listener, void * owner );
void flood_evolve( void * netptr, TiEvent * e );
#ifdef __cplusplus
}
#endif
#endif  /* _SVC_FLOOD_H_4764_ */
