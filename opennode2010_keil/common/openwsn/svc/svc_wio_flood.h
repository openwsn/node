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

#ifndef _SVC_NIO_FLOOD_H_4764_
#define _SVC_NIO_FLOOD_H_4764_

/*******************************************************************************
 * svc_nio_flood
 * This module implements a simple flooding mechanism in a multi-hop wireless
 * network. 
 *
 * @author zhangwei in 2007.06
 *	- first created
 * @modified by zhangwei in 2009.07
 *	- revision. done
 *
 ******************************************************************************/

/* The following two macros should be defined before including "rtl_cache" in order
 * to be effective in rtl_cache module. Or else the rtl_cache will use its internal
 * configurations.
 */
#define CONFIG_FLOOD_CACHE_CAPACITY 1     //#define CONFIG_FLOOD_CACHE_CAPACITY 8

#define CONFIG_FLOOD_CACHE_MAX_LIFETIME 8

#ifndef CONFIG_FLOOD_MAX_COUNT 
  #define CONFIG_FLOOD_MAX_COUNT 5
#endif

#ifndef CONFIG_FLOOD_CACHE_SIZE 
  #define CONFIG_FLOOD_CACHE_SIZE 8
#endif

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_cache.h"
#include "../rtl/rtl_framequeue.h"
#include "../hal/hal_foundation.h"
//#include "../hal/hal_cc2520.h"
//#include "../hal/hal_cc2520.h"
#include "../hal/hal_debugio.h"
#include "svc_foundation.h"
#include "svc_nio_aloha.h"

#define FLOOD_FRAMEOBJECT_SIZE FRAMEQUEUE_ITEMSIZE

#define FLOOD_STATE_IDLE 0
#define FLOOD_STATE_WAITFOR_TXREPLY 1

#ifdef __cplusplus
extern "C"{
#endif

/** 
 * Flood Protocol Packet Format:
 * [Current Hop Count 1B] [Maximum Hop Count 1B] [Sequence Id 1B] 
 * 
 * @todo
 * Suggested flooding protocol format is:
 * [Protocol Idenfier 1B] [Command 1B] [Sequence Id 1B] [Current Hop Count 1B] 
 * [Maximum Hop Count 1B]{Destination Address 2B} {Source Address 2B} 
 */
 
/**
 * _TiFloodCacheItem and _TiFloodCache
 * The cache is used inside flood component to improve the flood performance. In 
 * order to shrink the memory used by the cache, it does only put the meta information 
 * of a frame into the cache instead of put all the frame content into the cache. 
 * 
 * _TiFloodCache is implemented as some macros based on the TiCache component, which 
 * is already provided in module "rtl_cache".
 * 
 * _TiFloodCacheItem is used to save the meta information of a frame.
 * 
 * @attention
 * CONFIG_FLOOD_CACHE_CAPACITY 
 * This macro is used to configure the cache capacity. Suggested values are 4-8. 
 * Attention don't consume all the SRAM inside the MCU. Larger value is better to 
 * improve the performance of the flood component, but the MCU may not have enough 
 * memory as you hoped.
 */

/* lifetime = 0 means this is an empty item in the cache. The bigger the lifetime, 
 * the newer the lifetime.
 */
/* 
typedef struct{
	uint16				panto;
	uint16				shortaddrto;
	uint16				panfrom;
	uint16				shortaddrfrom;
	uint8				seqid;
}_TiFloodCacheItem;
*/

/* The cache item searching key contains 4 bytes, which are the flooding protocol
 * header.
 */
#define FLOOD_CACHE_ITEMSIZE 5
#define FLOOD_CACHE_HOPESIZE CACHE_HOPESIZE(FLOOD_CACHE_ITEMSIZE,CONFIG_FLOOD_CACHE_CAPACITY)

#define _TiFloodCache TiCache
#define flood_cache_open(mem,memsize) cache_open(mem,memsize,FLOOD_CACHE_ITEMSIZE,CONFIG_FLOOD_CACHE_CAPACITY)
#define flood_cache_close(ca) cache_close(ca)
#define flood_cache_hit(ca,item,pidx) cache_hit(ca,item,FLOOD_CACHE_ITEMSIZE,pidx)
#define flood_cache_visit(ca,item) cache_visit(ca,item,FLOOD_CACHE_ITEMSIZE)


/* TiFlootNetwork
 *	distance		The distance between current node and the tree root node. It's
 *                  measured by the minimal hopcount. For root node, the value is 0.
 */
#pragma pack(1) 
typedef struct{
	uint8               state;
	TiAloha *			mac;
	uint8				distance;
	uint16              panto;
	uint16              panfrom;
	uint16              localaddress;
	uint8               seqid;
	TiFrame * 			txque;
	TiFrame *			rxque;
	TiFrame *       	rxbuf;
	TiFunEventHandler   listener;
	void *              lisowner;
	char                txque_mem[ FLOOD_FRAMEOBJECT_SIZE ];
	char                rxque_mem[ FLOOD_FRAMEOBJECT_SIZE ];
	char                rxbuf_mem[ FLOOD_FRAMEOBJECT_SIZE ];
	_TiFloodCache *	    cache;
	char                cache_mem[ FLOOD_CACHE_HOPESIZE ];
}TiFloodNetwork;

TiFloodNetwork * flood_construct( void * mem, uint16 size );
void flood_destroy( TiFloodNetwork * net );
TiFloodNetwork * flood_open( TiFloodNetwork * net, TiAloha * mac, TiFunEventHandler listener, 
	void * lisowner, uint16 pan, uint16 localaddress );
void flood_close( TiFloodNetwork * net );

uintx flood_broadcast( TiFloodNetwork * net, TiFrame * frame, uint8 option );
uintx flood_send( TiFloodNetwork * net, uint16 shortaddrto, TiFrame * frame, uint8 option );
uintx flood_recv( TiFloodNetwork * net, TiFrame * frame, uint8 option );
void flood_set_listener( TiFloodNetwork * net, TiFunEventHandler listener, void * owner );
void flood_evolve( void * netptr, TiEvent * e );

#ifdef __cplusplus
}
#endif

#endif  /* _SVC_NIO_FLOOD_H_4764_ */
