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
#include "../hal/opennode2010/hal_mcu.h"
#include "svc_configall.h"  
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_frame.h"
#include "../hal/opennode2010/hal_debugio.h"
#include "../hal/opennode2010/hal_uart.h"
#include "../hal/opennode2010/hal_led.h"

#include "svc_foundation.h"
#include "svc_nio_aloha.h"
#include "../hal/opennode2010/hal_timer.h"

#include "svc_nodebase.h"

#ifndef NeiNum
#define NeiNum  4//the number of the node
#endif


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

#define NHB_SEQUENCEID(pkt) ((pkt)[0])
#define NHB_SHORTADDRTO(pkt) NHB_MAKEWORD((pkt)[2],(pkt)[1])
#define NHB_PANTO(pkt) NHB_MAKEWORD((pkt)[4],(pkt)[3])
#define NHB_SHORTADDRFROM(pkt) NHB_MAKEWORD((pkt)[6],(pkt)[5])
#define NHB_PANFROM(pkt) NHB_MAKEWORD((pkt)[8],(pkt)[7])

#define NHB_PAYLOAD_PTR(pkt) ((char*)(pkt)+9)

#define NHB_SET_SEQUENCEID(pkt,value) (pkt)[0]=(value)
#define NHB_SET_SHORTADDRTO(pkt,addr) {(pkt)[1]=((uint8)(addr&0xFF)); (pkt)[2]=((uint8)(addr>>8));}
#define NHB_SET_PANTO(pkt,pan) {(pkt)[3]=((uint8)(pan&0xFF)); (pkt)[4]=((uint8)(pan>>8));}
#define NHB_SET_SHORTADDRFROM(pkt,addr) {(pkt)[5]=((uint8)(addr&0xFF)); (pkt)[6]=((uint8)(addr>>8));}
#define NHB_SET_PANFROME(pkt,pan) {(pkt)[7]=((uint8)(pan&0xFF)); (pkt)[8]=((uint8)(pan>>8));}


typedef struct{
    uint8 state;
    TiNodeBase * nbase;
    uint8 seqid;
    //TiAloha * mac;
}TiNioNeighborDiscover;

nio_ndp_open()
{
    time axis: put some thing
    or start a timer;
}

nio_ndp_rxhandler( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
    payload = frame_startptr;
    extrace frame payload
    save information into nbase;
    frame_clear( output );
    return 0;
}

nio_ndp_txhandler( void * object, TiFrame * input, TiFrame * output, uint8 option );
{
}

nio_ndp_evolve( void * object, TiEvent * e);
{
    //if timer expired
    if e == NDP_REQUEST_INITIATE
        assemble a frame
        if aloha_send( request ) failed
            task create( nio_ndp_initiate_task, delay time );
        endif
    endif
}

nio_ndp_initiate_task()
{
    TiEvent e;
    e.id = NDP_REQUEST_INITIATE
    ndo_ndp_evolve( object, e)
}


typedef struct{
    uint8 state;
    void * object;
    TiFunRxHandler rxhandler;
    TiFunTxHandler txhandler;
    TiFunEventHandler evolve;
}_TiNioNetLayerDispatcherItem;




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

TiNeighbourNode * neighbournode_construct( void * mem, uint16 memsize );

void neighbournode_destroy( TiNeighbourNode * nei );

TiNeighbourNode * neighbournode_open( TiNeighbourNode * nei, TiAloha * mac );

void neighbournode_close( TiNeighbourNode * nei );

void neighbournode_evolve( void * svcptr, TiEvent * e );

uint8 neighbournode_send( TiNeighbourNode * svc,uint16 addr,TiFrame * frame, uint8 option );

uint8 neighbournode_broadcast( TiNeighbourNode * svc, TiFrame * buf, uint8 option );

uint8 neighbournode_recv( TiNeighbourNode * svc,TiFrame * buf, uint8 option );

uint8 neighbournode_found( TiNeighbourNode * svc );

uint8 neighbournode_getrssi( TiNeighbourNode *svc,uint8 id,uint8 rssi);

uint8 neighbournode_getaddr( TiNeighbourNode *svc,uint8 id,uint16 addr);

uint8 neighbournode_getpan( TiNeighbourNode *svc,uint8 id,uint16 pan);

uint8 neighbournode_getstate( TiNeighbourNode *svc,uint8 id);

bool neighbournode_empty( TiNeighbourNode *svc);

bool neighbournode_full( TiNeighbourNode *svc);

void neighbournode_clear( TiNeighbourNode * svc);
void neighbournode_delete( TiNeighbourNode * svc,uint8 id);
void dump_nodeinf( TiNeighbourNode *svc,uint8 id);

#endif 
