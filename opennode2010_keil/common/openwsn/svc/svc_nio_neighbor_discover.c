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
#include "../hal/hal_mcu.h"
#include "svc_configall.h"  
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_led.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_aloha.h"
#include "svc_nodebase.h"
#include "svc_nio_dispatcher.h"
#include "svc_nio_neighbor_discover.h"
#include "../osx/osx_tlsche.h"

void _init_request( TiFrame *txbuf, uint16 localaddr, uint16 remotaddr);
void _init_response(TiFrame *txbuf,uint16 localaddr,uint16 remotaddr);

TiNioNeighborDiscover * ndp_construct( void * mem, uint16 memsize )
{
    memset( mem, 0x00, memsize );
    return (TiNioNeighborDiscover*)mem;
}

void ndp_destroy( TiNioNeighborDiscover * nei )
{

}

TiNioNeighborDiscover * ndp_open( TiNioNeighborDiscover * nei, TiNioNetLayerDispatcher *dispatcher, TiNodeBase * nbase ,TiOsxTimeLineScheduler * scheduler,TiTimerAdapter *timer)
{
    nei->dispatcher = dispatcher;
    nei->nbase = nbase;
    nei->timer = timer;
    nei->seqid =0;
    nei->state =INIT_STATE;
    nei->scheduler = scheduler;
}

void ndp_close( TiNioNeighborDiscover * nei )
{

}

void ndp_evolve( void * svcptr, TiEvent * e )
{

}

uint8 ndp_response( void * object, uint16 addr, TiFrame * frame, uint8 option)
{
    uint8 count=0;
    char * response;
    TiFrame * nei_frame;
    char * nei;
    char * pkt;
    uint8 legth;
    uint8 i;
    uint8 seqid;
    char neiframe_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    TiNioNeighborDiscover * svc = (TiNioNeighborDiscover *)object;

    response = frame_startptr(frame);
    legth = frame_length( frame);
    nei_frame = frame_open( (char*)(&neiframe_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    frame_reset( nei_frame,3,20,0);
    nei = frame_startptr( nei_frame);//nei = frame_startptr( s_frame);

    //response[0] stores the protocal information.
    //0x03 neighbornode discover response,0x02 neibournode discover request.
    NHB_SET_PROTOID( nei,0x03);
    NHB_SET_SEQUENCEID( nei,svc->seqid);
    NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( response[1],response[2]));
    NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
    NHB_SET_PANTO(nei,CONFIG_PANTO);
    NHB_SET_PANFROME(nei,CONFIG_PANTO);
    pkt = NHB_PAYLOAD_PTR( nei);
    for (i=0; i<legth; i++)
        pkt[i] = response[i];
    frame_setlength( nei_frame,( legth+8));//frame_setlength( s_frame,( legth+8));

    count = net_disp_send( svc->dispatcher,nei_frame,addr,0x00);
    if (count > 0)
        svc->seqid ++;

    return count;
}

uint8 ndp_request( TiNioNeighborDiscover * svc,TiFrame * frame,uint8 option)
{
    uint8 count=0;
    char * response;
    TiFrame * nei_frame;
    char * nei;
    char * pkt;
    uint8 legth;
    uint8 i;
    uint8 sqid;
    char neiframe_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    response = frame_startptr(frame);
    legth = frame_length( frame);
    nei_frame = frame_open( (char*)(&neiframe_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    frame_reset( nei_frame,3,20,0);
    nei = frame_startptr( nei_frame);//nei = frame_startptr( s_frame);
    //response[0] stores the protocal information.
    //0x03 neighbornode discover response,0x02 neibournode discover request.
    NHB_SET_PROTOID( nei,0x02);
    NHB_SET_SEQUENCEID( nei,svc->seqid);
    NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( response[1],response[2]));
    NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
    NHB_SET_PANTO(nei,CONFIG_PANTO);
    NHB_SET_PANFROME(nei,CONFIG_PANTO);
    pkt = NHB_PAYLOAD_PTR( nei);
    for (i=0; i<legth; i++)
        pkt[i] = response[i];
    frame_setlength( nei_frame,( legth+8));//frame_setlength( s_frame,( legth+8));

    count = net_disp_broadcast( svc->dispatcher,nei_frame,0x00);

    if (count > 0)
        svc->seqid ++;
    return count;
}

uint8 ndp_send( TiNioNeighborDiscover * svc,uint16 addr,TiFrame * frame, uint8 option )
{
    uint8 count=0;
    char * response;
    TiFrame * nei_frame;
    char * nei;
    char * pkt;
    uint8 legth;
    uint8 i;
    char neiframe_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

    response = frame_startptr(frame);
    legth = frame_length( frame);
    nei_frame = frame_open( (char*)(&neiframe_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    frame_reset( nei_frame,3,20,0);
    nei = frame_startptr( nei_frame);//nei = frame_startptr( s_frame);

    //response[0] stores the protocal information.
    //0x03 neighbornode discover response,0x02 neibournode discover request.
    //NHB_SET_PROTOID( nei,response[0]);
    NHB_SET_SEQUENCEID( nei,svc->seqid);
    NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( response[1],response[2]));
    NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
    NHB_SET_PANTO(nei,CONFIG_PANTO);
    NHB_SET_PANFROME(nei,CONFIG_PANTO);
    pkt = NHB_PAYLOAD_PTR( nei);
    for (i=0; i<legth; i++)
        pkt[i] = response[i];
    frame_setlength( nei_frame,( legth+8));//frame_setlength( s_frame,( legth+8));

    count = net_disp_send( svc->dispatcher,nei_frame,addr,0x00);
    if (count > 0)
        svc->seqid ++;

    return count;
}

uint8 ndp_broadcast( TiNioNeighborDiscover * svc, TiFrame * frame, uint8 option )
{
    uint8 count=0;
    char * response;
    TiFrame * nei_frame;
    char * nei;
    char * pkt;
    uint8 legth;
    uint8 i;
    char neiframe_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    response = frame_startptr(frame);
    legth = frame_length( frame);
    nei_frame = frame_open( (char*)(&neiframe_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    frame_reset( nei_frame,3,20,0);
    nei = frame_startptr( nei_frame);//nei = frame_startptr( s_frame);
    //response[0] stores the protocal information.
    //0x03 neighbornode discover response,0x02 neibournode discover request.
    //NHB_SET_PROTOID( nei,response[0]);
    NHB_SET_SEQUENCEID( nei,svc->seqid);
    NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( response[1],response[2]));
    NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
    NHB_SET_PANTO(nei,CONFIG_PANTO);
    NHB_SET_PANFROME(nei,CONFIG_PANTO);
    pkt = NHB_PAYLOAD_PTR( nei);
    for (i=0; i<legth; i++)
        pkt[i] = response[i];
    frame_setlength( nei_frame,( legth+8));//frame_setlength( s_frame,( legth+8));

    count = net_disp_broadcast( svc->dispatcher,nei_frame,0x00);

    if (count > 0)
        svc->seqid ++;
    return count;
}

uint8 ndp_recv( TiNioNeighborDiscover * svc,TiFrame * buf, uint8 option )
{
    TiFrame * rxbuf;
    uint8 rssi;
    char rx_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
    uint8 len=0;
    uint8 count = 0;
    char * response = frame_startptr( buf );
    char * nei;
    char * pkt;
    char *pc;
    uint8 i;
    rxbuf = frame_open( (char*)(&rx_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    nei = frame_startptr( buf);
    frame_reset( rxbuf,3,20,0);
    frame_reset(buf,3,20,0);
    len = net_disp_recv( svc->dispatcher,rxbuf);
    if (len > 0)
    {
        nei = frame_startptr( rxbuf);
        pkt = NHB_PAYLOAD_PTR(nei);
        count = min( frame_capacity(buf), (len-10) );
        memmove( response, pkt, count );
        frame_setlength( buf, count );
    }
    return count;
}


intx nio_ndp_rxhandler( TiNioNeighborDiscover * svc, TiFrame * input, TiFrame * output, uint8 option )
{
    char * payload;
    TiNodeDescriptor node;
    uint8 rssi;
    uint8 i;
    payload = frame_startptr(input);

    if ( !frame_empty( input))
    {
        frame_totalcopyfrom( output,input);

        if ( payload[0]==0x03)//if the frame is ndp response frame.then store the information else upload.
        {
            frame_movelower( input);
            payload = frame_startptr( input);
            rssi = payload[ frame_length(input)-2];
            frame_movehigher(input);
            payload = frame_startptr( input);

            node.state = 1;
            node.address = NHB_SHORTADDRFROM(payload);
            node.pan =  NHB_PANFROM(payload);
            node.rssi = rssi;
            nbase_setnode( svc->nbase,node.address,&node);
            frame_clear( input);
            frame_clear( output);
        }
        
    } 

  
}

intx nio_ndp_txhandler( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
    //do nothing
}

void nio_ndp_request_evolve( void * object, TiEvent * e)
{
    TiNioNeighborDiscover * svc = (TiNioNeighborDiscover *)object;

    _init_request( svc->dispatcher->txbuf, svc->nbase->shortaddress,0xffff );
    if (ndp_request( svc, svc->dispatcher->txbuf, 0x00) > 0)
    {  

        svc->state = INIT_STATE;
        svc->seqid ++;
    }

    osx_tlsche_taskspawn(svc->scheduler, net_disp_evolve,NULL,0,0,0);
    osx_tlsche_taskspawn(svc->scheduler, nio_ndp_response_evolve,NULL,0,0,0);
    osx_tlsche_taskspawn(svc->scheduler,nio_ndp_request_evolve,NULL,2000,0,0);

    /*

    switch ( svc->state)  
    {
    case INIT_STATE:
        if (timer_expired(svc->timer))
        { 
            timer_start( svc->timer );
            _init_request( svc->dispatcher->txbuf, svc->nbase->shortaddress,0xffff );
            if (ndp_request( svc, svc->dispatcher->txbuf, 0x00) > 0)
            {  
                
                svc->state = INIT_STATE;
                svc->seqid ++;
            }
            else
            {
                timer_setinterval( svc->timer,1000,7999);
                timer_start( svc->timer);
                svc->state = RESEND_STATE;
            }

        }
        break;
    case RESEND_STATE:
        if ( timer_expired( svc->timer))
        {
            if (ndp_request( svc, svc->dispatcher->txbuf, 0x00) > 0)
            {  
                svc->state = INIT_STATE;
                svc->seqid ++;
            }
            else
            {
                timer_setinterval( svc->timer,1000,7999);
                timer_start( svc->timer);
                svc->state = RESEND_STATE;
            }
        } 
        break;
    default: 
        svc->state = INIT_STATE;
        break;
       
    }
    */

}

void nio_ndp_response_evolve( void * object, TiEvent * e)
{
    TiNioNeighborDiscover * svc = (TiNioNeighborDiscover *)object;
    char        m_rtxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
    TiFrame     * rtxbuf;
    uint8       len;
    char        *request;
    char        *response;
    uint16      addr;
    rtxbuf = frame_open( (char*)(&m_rtxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    len = ndp_recv(svc,rtxbuf,0);
    if ( len)
    {
        request = frame_startptr( rtxbuf );
        addr = NHB_MAKEWORD( request[3],request[4]);
        if (request[0] == 0x02)
        {
            _init_response( svc->dispatcher->txbuf,svc->nbase->shortaddress,addr);
            len = ndp_response( svc,addr,svc->dispatcher->txbuf,0x00);
        }
    }
}

void nio_ndp_initiate_task()
{
    /*
    TiEvent e;
    e.id = NDP_REQUEST_INITIATE
        ndo_ndp_evolve( object, e)
        */
}

void _init_request( TiFrame *txbuf, uint16 localaddr, uint16 remotaddr)
{
    char * request;
    frame_reset( txbuf ,3,20,0);
    request = frame_startptr(txbuf);
    request[0] = 0x02;              // request type
    request[1] = 0xFF;              // set destination address
    request[2] = 0xFF;              // 0xFFFF is the broadcast address
    request[3] = (char)(localaddr >> 8);       
    // set source address, 
    request[4] = (char)(localaddr & 0xFF);   
    // namely local address
    request[5] = 0x06;
    request[6] = 0x07;
    frame_setlength( txbuf, 7 );
}

void _init_response(TiFrame *txbuf,uint16 localaddr,uint16 remotaddr)
{
    char * response;
    uint16 value;
    value = 0x9999;
    frame_reset( txbuf,3,20,0);
    response = frame_startptr( txbuf );
    response[0] = 0x03;                                   // set response type
    response[1] = (char)(remotaddr>> 8);                             // set destination address, 
    response[2] = (char)(remotaddr & 0x00FF);                             // namely the gateway address

    response[3] = (char)(localaddr >> 8);       // set source address, 
    response[4] = (char)(localaddr & 0x00FF);   // namely local address
    response[5] = (uint8)(value >> 8);
    response[6] = (uint8)(value & 0xfF);
    frame_setlength( txbuf, 7 );    

}


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


