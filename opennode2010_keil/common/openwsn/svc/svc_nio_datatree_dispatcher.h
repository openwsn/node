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

#ifndef _SVC_DATATREE_H_4576_
#define _SVC_DATATREE_H_4576_
/*******************************************************************************
 * svc_datatree
 * This module implements a tree based data collection multi-hop protocol and service
 * for ad-hoc network. The module mainly includes three parts:
 *	- Tree generation and maintainance based on flooding.
 *	- Request broadcast, multicast(not support now) or unicast
 *  - Response collection. 
 *
 * The tree generation process is almost the same as the svc_flood module. 
 *
 * @author zhangwei in 2007.06
 *	- first created
 * @modified by zhangwei in 2008.08 
 *	- revision developing.
 * @modified by zhangwei in 2008.08 
 *  - fully revision including interface modifications. 
 *  - tested by yan-shixing, shi-miaojing.
 * @modified by zhangwei in 2008.11.25
 *	- add sequence id in the DTP header. It's the sequence id in NET layer.
 *    It's different from the sequence id in MAC layer's 802.15.4 MAC frame format.
 *    The sequence id in DTP header is actually an identifier to distinguish 
 *    different request and response packets.
 ******************************************************************************/
 
/*
 * reference
 * - The Collection Tree Protocol (CTP), http://www.tinyos.net/tinyos-2.x/doc/html/tep123.html
 * - The Collection Tree Protocol (CTP), http://www.tinyos.net/tinyos-2.x/doc/txt/tep123.txt
 * - Collection, http://www.tinyos.net/tinyos-2.x/doc/html/tep119.html
 * - Network Protocols, http://docs.tinyos.net/index.php/Network_Protocols
 */
 
/* The following two macros should be defined before including "rtl_cache" in order
 * to be effective in rtl_cache module. Or else the rtl_cache will use its internal
 * configurations.
 */
#ifndef CONFIG_DTP_CACHE_CAPACITY
  #define CONFIG_DTP_CACHE_CAPACITY 2
#endif

#define CONFIG_DTP_CACHE_MAX_LIFETIME 8

#ifndef CONFIG_DTP_MAX_COUNT 
  #define CONFIG_DTP_MAX_COUNT 5
#endif

 


#include "../hal/hal_mcu.h"
#include "svc_configall.h"
#include "../rtl/rtl_cache.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_cc2520.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_debugio.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_aloha.h"
#include "../osx/osx_tlsche.h"
#include "svc_nodebase.h"
#include "svc_nio_dispatcher.h"

/* This macro only provides a default value to the max hopcount field in the frame.
 * Actually, the application layer program can freely assign values to the max hopcount
 * field in the packet.
 */

/*
#ifndef CONFIG_DTP_DEF_MAX_HOPCOUNT 
  #define CONFIG_DTP_DEF_MAX_HOPCOUNT 5
#endif

#ifndef CONFIG_DTP_CACHE_CAPACITY 
  #define CONFIG_DTP_CACHE_CAPACITY 8
#endif

#define CONFIG_DTP_CACHE_MAX_LIFETIME 16
*/


/* @attention
 * Though the data tree(DTP) design includes DTP_MAINTAIN_RESPONSE, this part hasn't
 * been implemented. This is mainly because current DTP is simple enough and it 
 * can run without DTP_MAINTAIN_RESPONSE.
 */ 

/*******************************************************************************
 * Packet type of DTP(datatree protocol)
 * 
 * MAC Frame := [MAC Header mB] [DTP Packet nB]
 * DTP content is the payload of MAC frame now.
 * 
 * DTP Packet := [DTP Header 8B+mB] [DTP Payload nB]
 * 
 * DTP Header := [DTP Protocol Identifier 1B][Command and Control 1B][Sequence Id 1B][Destination Address 2B][Source Address 2B]
 *               [Hop Count 1B][Maximum Hop Count 1B] [Path Descriptor Count 1B] 
 *               [Path Description m byte]
 *               The length of path description = value of [Path Description Count] * 2
 *
 * DTP Header := [DTP Protocol Control 1B][Sequence Id 1B][Destination Address 2B][Source Address 2B]
 *               [Hop Count 1B][Maximum Hop Count 1B] [Path Description Count 1B] 
 *               [Path Description  m byte]
 *               The length of path description = value of [Path Description Count] * 2
 * DTP Payload := [char based array]
 * 
 * Detail Description about the DTP Header:
 * DTP Protocol Identifier: always 0x33
 * DTP Command and Control Byte := [b7,...b0]
 *   b1 b0  packet command type. 
 *      0x00 MAINTAIN REQUEST
 *      0x01 MAINTAIN RESPONSE 
 *      0x02 DATA REQUEST 
 *      0x03 DATA RESPONSE
 *   b7 b6  transportation method. 
 *      0x00 broadcast
 *      0x40 multicast 
 *      0x80 unicast from root to leaf node 
 *      0xC0 unicast from leaf to root node
 *   b5,...,b2  default to zero now. for future use
 * 
 ******************************************************************************/  

/* If the destination of a packet is 0xFFFF, then this frame will be broadcasted
 * among the whole PAN network.
 */
#define DTP_BROADCAST_ADDRESS           0xFFFF

//#define DTP_DEFAULT_MAX_HOPCOUNT		0x05
#define DTP_MAINTAIN_TIME				10
//#define DTP_MAINTAIN_TIME				2000

/* The transportation type of a frame is very important in the routing. They're 
 * saved as bit7 and bit6 in the packet control field. There're only the following
 * four kinds of transportation method in DTP.
 */ 
#define DTP_BROADCAST                   0x00
#define DTP_MULTICAST                   0x40
#define DTP_UNICAST_ROOT2LEAF           0x80
#define DTP_UNICAST_LEAF2ROOT           0xC0

/* The command inside the DTP packet. In the future, new commands may be added into
 * DTP. They're saved as bit1 and bit 0 in the packet control field.
 * 
 * Attention, the DTP service can recognize some commands such as maintain request
 * and maintain response.
 */
#define DTP_MAINTAIN_REQUEST            0x00
#define DTP_MAINTAIN_RESPONSE           0x01 
#define DTP_DATA_REQUEST                0x02 
#define DTP_DATA_RESPONSE               0x03

#define DTP_MAX_FRAME_SIZE 128

//todo #define DTP_MAX_TX_TRYTIME              0x1FF

#define DTP_MAX_TX_TRYTIME            0x04

//#define DTP_HEADER_SIZE(maxhopcount) (9+(maxhopcount-1)*2)//(10+(maxhopcount-1)*2)//(9+(maxhopcount-1)*2)
//JOE 0713
#define DTP_HEADER_SIZE(maxhopcount) (10+(maxhopcount-1)*2)    
#define DTP_MAKEWORD(high,low) (((uint16)high<<8) | ((uint8)low))

#define DTP_PACKETCONTROL(pkt) ((pkt)[1])
#define DTP_SEQUENCEID(pkt) ((pkt)[2])
#define DTP_SHORTADDRTO(pkt) DTP_MAKEWORD((pkt)[4],(pkt)[3])
#define DTP_SHORTADDRFROM(pkt) DTP_MAKEWORD((pkt)[6],(pkt)[5])
#define DTP_HOPCOUNT(pkt) ((pkt)[7])
#define DTP_MAX_HOPCOUNT(pkt) ((pkt)[8])
#define DTP_PATHDESC_COUNT(pkt) ((pkt)[9])
#define DTP_PATHDESC_PTR(pkt) (&(pkt[9]))
#define DTP_PAYLOAD_PTR(pkt) ((char*)(pkt)+DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT((pkt))))

#define DTP_SET_PROTOCAL_IDENTIFIER(pkt,value) (pkt)[0]=(value)
#define DTP_SET_PACKETCONTROL(pkt,value) (pkt)[1]=(value)//b1 b0  packet command type;b7 b6  transportation method.
#define DTP_SET_SEQUENCEID(pkt,value) (pkt)[2]=(value)
#define DTP_SET_SHORTADDRTO(pkt,addr) {(pkt)[3]=((uint8)(addr&0xFF)); (pkt)[4]=((uint8)(addr>>8));}
#define DTP_SET_SHORTADDRFROM(pkt,addr) {(pkt)[5]=((uint8)(addr&0xFF)); (pkt)[6]=((uint8)(addr>>8));}
#define DTP_SET_HOPCOUNT(pkt,value) (pkt)[7]=(value)
#define DTP_SET_MAX_HOPCOUNT(pkt,value) (pkt)[8]=(value)
#define DTP_SET_PATHDESC_COUNT(pkt,value) (pkt)[9]=(value)  // path descriptor count

//pkt[0]:protocal_identifier
//pkt[1]:command type and transportation method.
#define DTP_TRANTYPE(pkt) (pkt[1] & 0xC0)
#define DTP_SET_TRANTYPE(pkt,newtype) (pkt[1] = (pkt[1] & 0x3F) | (newtype))
#define DTP_CMDTYPE(pkt) (pkt[1] & 0x03)
#define DTP_SET_CMDTYPE(pkt,newtype) (pkt[1] = (pkt[1] & 0xFC) | (newtype))

/* DTP protocol state
 * STARTUP state: when the node first powered on, it's in STARTUP state. In this state, 
 *		it will inspect the incoming frames unless it can find an MAINTAIN_REQUEST
 *		frame. All other frames will be ignored. After it received the MAINTAIN_REQUEST 
 *      frame, it will join the network and set appropriate parent node, and switch 
 *      to IDLE state.
 */
#define DTP_STATE_STARTUP 1
#define DTP_STATE_SHUTDOWN 2
#define DTP_STATE_IDLE 3
#define DTP_STATE_WAITFOR_TXREPLY 4
#define DTP_STATE_RECVING 5
#define DTP_STATE_SENDING 6

#define TiDTP TiDataTreeNetwork
#define DTP_PROTOCOL_IDENTIFER 0x33

#define dtp_send(net,opf,option) dtp_unicast((net),(opf),(option))


/**
 * _TiDtpCacheItem and _TiDtpCache
 * The cache is used inside flood component to improve the flood performance. In 
 * order to shrink the memory used by the cache, it does only put the meta information 
 * of a frame into the cache instead of put all the frame content into the cache. 
 * 
 * _TiDtpCache is implemented as some macros based on the TiCache component, which 
 * is already provided in module "rtl_cache".
 * 
 * _TiDtpCacheItem is used to save the meta information of a frame.
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
#pragma pack(1) 
typedef struct{
	uint16				lifetime;
	uint16				panto;
	uint16				shortaddrto;
	uint16				panfrom;
	uint16				shortaddrfrom;
	uint8				seqid;
}_TiDtpCacheItem;

#define DTP_CACHE_ITEMSIZE sizeof(_TiDtpCacheItem)
#define DTP_CACHE_HOPESIZE CACHE_HOPESIZE(DTP_CACHE_ITEMSIZE,CONFIG_DTP_CACHE_CAPACITY)

#define _TiDtpCache TiCache
#define dtp_cache_open(mem,memsize) cache_open(mem,memsize,DTP_CACHE_ITEMSIZE,CONFIG_DTP_CACHE_CAPACITY)
#define dtp_cache_close(ca) cache_close(ca)
#define dtp_cache_hit(ca,item,pidx) cache_hit(ca,item,DTP_CACHE_ITEMSIZE,pidx)
#define dtp_cache_visit(ca,item) cache_visit(ca,item,DTP_CACHE_ITEMSIZE)


/* @todo 
 * Q: how the DTP service behave when it received more than 2 request at the same time?
 * R: the response_id only saves the last response id. So the DTP will confused in 
 * this case. All the response packet will be assigned response_id of the last request 
 * received. This will cause the request initiator drop the response packet arrived.
 */

/* TiDataTreeNetwork
 *  root                root node address
 *  parent              parent node in the tree towarding to the root node
 *	distance		    The distance between current node and the tree root 
 *                      node. It's measured by hopcount. For root node, the value is 0.
 *  depth               Depth of the current node. It's actually the hopcount between
 *                      tree root node and the current node. 
 *
 *  request_id          Used to identify the request packet. Each request should 
 *                      have a unique identifier. That's what the request_id does.
 *                      it's different from the sequence id in the MAC header's. 
 *
 *                      - MAC sequence id should be different from NET/datatree's 
 *                        sequence id. 
 *                      - The router node shouldn't change the sequence id field
 *                        of a frame, because it's the sequence id of the request
 *                        or response among the whole network. It quite different
 *                        from MAC layers sequence id in window based flow control
 *                        algorithm. 
 *                      - Only the gateway node can increase sequence id.
 *
 *  response_id         ??? 
 *                      Currently, This member is used to save the request id just received.
 *
 *  txtrytime           how many times we had wait to send the current frame. 
 *                      Since the current data tree doesn't use timer object, we use
 *                      counter variable to simulate it. when txtrytime == 0, then the
 *                      frame inside txbuf will be cleared even though it's failed sending.
 */
#pragma pack(1) 
typedef struct{
	uint8               state;
	uint8               count;//todo for tesitng
	uint8               option;
	TiNioAcceptor *     nac;
	uint16              pan;
	uint16              root;
	uint16		        parent;
	uint8               rssi;		// rssi extracted from the parent frame
	uint8		        depth;    
	uint8				distance;

	TiNodeBase * 		nbase;//JOE
	TiOsxTimeLineScheduler * scheduler;
    TiNioNetLayerDispatcher *dispatcher;

	TiAloha *			mac;
//	uint16              txtrytime;
	uint8               request_id; 
	uint8               response_id;
	TiFunEventHandler   listener;
	void *              lisowner;
	char                frame_feature[6];
	_TiDtpCache *	    cache;
	char                cache_mem[ DTP_CACHE_HOPESIZE ];
}TiDTP;

TiDataTreeNetwork * dtp_construct( void * mem, uint16 size );

TiDataTreeNetwork * dtp_open_node( TiDataTreeNetwork * net, TiAloha * mac, TiNodeBase * nbase, 	TiOsxTimeLineScheduler * scheduler,
    TiNioNetLayerDispatcher *dispatcher, uint8 option );
	
TiDataTreeNetwork * dtp_open_sink( TiDataTreeNetwork * net, TiAloha * mac, TiNodeBase * nbase, 	TiOsxTimeLineScheduler * scheduler,
    TiNioNetLayerDispatcher *dispatcher, uint8 option );

void dtp_setlistener( TiDataTreeNetwork * net, TiFunEventHandler listener, void * object );
	
void dtp_destroy( TiDataTreeNetwork * net );
void dtp_close( TiDataTreeNetwork * net );

void dtp_evolve( void * object, TiEvent * e );
void dtp_maintain_evolve( void * object, TiEvent * e);

intx nio_dtp_rxhandler_node( void * object, TiFrame * input, TiFrame * output, uint8 option );
intx nio_dtp_rxhandler_sink( void * object, TiFrame * input, TiFrame * output, uint8 option );
intx nio_dtp_txhandler_node( void * object, TiFrame * input, TiFrame * output, uint8 option );
intx nio_dtp_txhandler_sink( void * object, TiFrame * input, TiFrame * output, uint8 option );


void dtp_test_recv_evolve( void * object, TiEvent * e);	 //for recv



#endif /* _SVC_DATATREE_H_4576_ */
