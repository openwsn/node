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

#ifndef _SVC_NIO_ONE2MANY_H_7244_
#define _SVC_NIO_ONE2MANY_H_7244_

/* 程序设想：
 * TiOne2Many有两种类型，一为GATE，另一为SENSOR。
 * 对于GATE来说，其任务为定时发送DATA_REQUEST frame（用到svc_timer、mac、和特定
 * 格式的frame对象），然后等待并接受S节点数据（用到mac 和 frame 对象和TiIoBuf对象 ），
 * 等待一定时间后统计好，然后发到串口（用到UART对象）
 * 所以状态有四个：IDLE、WIO_SENDING、WIO_RECEIVING、SIO_SENDING
 *
 * 对于SENSOR, 其任务为等待，然后收到DATA_REQUEST frame（opf和mac）时，借鉴hal_remoteled
 * 模块的代码读取数据，然后发送回G。
 * 所以状态有两个：WIO_RECEIVING、DATA_COLLECT_AND_SEND
 */
#include "../hal/hal_mcu.h"
#include "svc_configall.h"  
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_led.h"

#include "svc_foundation.h"
#include "svc_nio_aloha.h"
#include "../hal/hal_timer.h"

#include "svc_nodebase.h"
#include "svc_nio_dispatcher.h"
#include "../osx/osx_tlsche.h"

#ifndef NeiNum
#define NeiNum  4//the number of the node
#endif

#define  INIT_STATE 0
#define  WAIT_RESPONSE_STATE 1
#define  RESEND_STATE 2

#define GATEWAYTYPE 1
#define SENSORTYPE 0

#define TIMEISUP 1
#define TIMEISNOTUP 0

#define CONFIG_PANTO 0x01

#define CONFIG_SIOBUF_SIZE 88
#define MAX_IEEE802FRAME154_SIZE                128

#define MAC_GATE_PANID 0xFF
#define MAC_GATE_LOCAL 0x99
#define MAC_GATE_REMOTE 0xFF

#define NHB_MAKEWORD(high,low) (((uint16)high<<8) | ((uint8)low))
//
//protocalid: 0x03->neibournode_discove response. 0x02->neibournode_discover request.  
//            when the second bit is one means the protocal is ndp.
//
#define NHB_PROTOID(pkt) ((pkt)[0])
#define NHB_SEQUENCEID(pkt) ((pkt)[1])
#define NHB_SHORTADDRTO(pkt) NHB_MAKEWORD((pkt)[3],(pkt)[2])
#define NHB_PANTO(pkt) NHB_MAKEWORD((pkt)[5],(pkt)[4])
#define NHB_SHORTADDRFROM(pkt) NHB_MAKEWORD((pkt)[7],(pkt)[6])
#define NHB_PANFROM(pkt) NHB_MAKEWORD((pkt)[9],(pkt)[8])

#define NHB_PAYLOAD_PTR(pkt) ((char*)(pkt)+10)

#define NHB_SET_PROTOID(pkt,value) (pkt)[0]=(value)
#define NHB_SET_SEQUENCEID(pkt,value) (pkt)[1]=(value)
#define NHB_SET_SHORTADDRTO(pkt,addr) {(pkt)[2]=((uint8)(addr&0xFF)); (pkt)[3]=((uint8)(addr>>8));}
#define NHB_SET_PANTO(pkt,pan) {(pkt)[4]=((uint8)(pan&0xFF)); (pkt)[5]=((uint8)(pan>>8));}
#define NHB_SET_SHORTADDRFROM(pkt,addr) {(pkt)[6]=((uint8)(addr&0xFF)); (pkt)[7]=((uint8)(addr>>8));}
#define NHB_SET_PANFROME(pkt,pan) {(pkt)[8]=((uint8)(pan&0xFF)); (pkt)[9]=((uint8)(pan>>8));}


typedef struct{
    uint8 state;
    TiNodeBase * nbase;
    uint8 seqid;
    TiTimerAdapter *timer;
    TiOsxTimeLineScheduler * scheduler;
    TiNioNetLayerDispatcher *dispatcher;
}TiNioNeighborDiscover;

/*
nio_ndp_open()
{
    time axis: put some thing
    or start a timer;
}
*/



intx nio_ndp_rxhandler( void * object, TiFrame * input, TiFrame * output, uint8 option );
intx nio_ndp_txhandler( void * object, TiFrame * input, TiFrame * output, uint8 option );
void ndp_evolve( void * svcptr, TiEvent * e );

void nio_ndp_request_evolve( void * object, TiEvent * e);

void nio_ndp_response_evolve( void * object, TiEvent * e);

void nio_ndp_initiate_task();




/*

typedef struct{
	uint8 state;
	uint8 seqid;
    uint8 neighbourmaxcount;
    uint8 neighbourcount;
	TiAloha * mac;
	TiFrame * txbuf;
	TiFrame * rxbuf;
	TiFrame * tmpbuf;
	char txbuf_memory[FRAME_HOPESIZE(CONFIG_ALOHA_MAX_FRAME_SIZE)];
	char rxbuf_memory[FRAME_HOPESIZE(CONFIG_ALOHA_MAX_FRAME_SIZE)];
	char tmpbuf_memory[FRAME_HOPESIZE(CONFIG_ALOHA_MAX_FRAME_SIZE)];
	TiIEEE802Frame154Descriptor desc;
    TiNeiInf nodeinf[NeiNum];
}TiNioNeighbourNode;

*/

TiNioNeighborDiscover * ndp_construct( void * mem, uint16 memsize );

void ndp_destroy( TiNioNeighborDiscover * nei );

TiNioNeighborDiscover * ndp_open( TiNioNeighborDiscover * nei, TiNioNetLayerDispatcher *dispatcher, TiNodeBase * nbase ,TiOsxTimeLineScheduler * scheduler,TiTimerAdapter *timer);

void ndp_close( TiNioNeighborDiscover * nei );

void ndp_evolve( void * svcptr, TiEvent * e );

uint8 ndp_send( TiNioNeighborDiscover * svc,uint16 addr,TiFrame * frame, uint8 option );

uint8 ndp_response( TiNioNeighborDiscover *svc,uint16 addr,TiFrame * frame,uint8 option);

uint8 ndp_broadcast( TiNioNeighborDiscover * svc, TiFrame * frame, uint8 option );

uint8 ndp_request( TiNioNeighborDiscover * svc,TiFrame * frame,uint8 option);

uint8 ndp_recv( TiNioNeighborDiscover * svc,TiFrame * buf, uint8 option );

uint8 ndp_found( TiNioNeighborDiscover * svc );

uint8 ndp_getrssi( TiNioNeighborDiscover *svc,uint8 id,uint8 rssi);

uint8 ndp_getaddr( TiNioNeighborDiscover *svc,uint8 id,uint16 addr);

uint8 ndp_getpan( TiNioNeighborDiscover *svc,uint8 id,uint16 pan);

uint8 ndp_getstate( TiNioNeighborDiscover *svc,uint8 id);

bool ndp_empty( TiNioNeighborDiscover *svc);

bool ndp_full( TiNioNeighborDiscover *svc);

void ndp_clear( TiNioNeighborDiscover * svc);
void ndp_delete( TiNioNeighborDiscover * svc,uint8 id);
void dump_nodeinf(TiNioNeighborDiscover *svc,uint8 id);

#endif 
