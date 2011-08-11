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
#include "../hal/opennode2010/hal_mcu.h"
#include "svc_configall.h"  
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_frame.h"
#include "../hal/opennode2010/hal_foundation.h"
#include "../hal/opennode2010/hal_cpu.h"
#include "../hal/opennode2010/hal_timer.h"
#include "../hal/opennode2010/hal_debugio.h"
#include "../hal/opennode2010/hal_uart.h"
#include "../hal/opennode2010/hal_led.h"
#include "../hal/opennode2010/hal_assert.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_aloha.h"
#include "svc_nio_neighbournode_found.h"

TiNeighbourNode * neighbournode_construct( void * mem, uint16 memsize )
{
	memset( mem, 0x00, memsize );
	return (TiNeighbourNode *)mem;
}

void neighbournode_destroy( TiNeighbourNode * nei )
{
	nei = nei;
	return;
}

TiNeighbourNode * neighbournode_open( TiNeighbourNode * nei, TiAloha * mac )
{
    int i;
    TiIEEE802Frame154Descriptor * desc;
    char *buf;
	desc = &(nei->desc);
	
	nei->seqid = 0;
	nei->mac = mac;

    nei->neighbourmaxcount = NeiNum;

    nei->rxbuf = frame_open( (char*)(&nei->rxbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    nei->txbuf = frame_open( (char*)(&nei->txbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    nei->tmpbuf = frame_open( (char*)(&nei->tmpbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

	/*
    desc = ieee802frame154_format( desc, frame_startptr(nei->txbuf), 20, 
		FRAME154_DEF_FRAMECONTROL_DATA );
    rtl_assert( desc != NULL );
    ieee802frame154_set_sequence( desc, 0xAA );
    ieee802frame154_set_panto( desc, MAC_GATE_PANID );
    ieee802frame154_set_shortaddrto( desc, MAC_GATE_REMOTE );
    ieee802frame154_set_panfrom( desc, MAC_GATE_PANID );
    ieee802frame154_set_shortaddrfrom( desc, MAC_GATE_LOCAL );
	
	buf = ieee802frame154_msdu( desc );
	for (i=0; i<ieee802frame154_msdu_len(desc); i++)
		buf[i] = i;
        */


	return nei;
}


void neighbournode_close( TiNeighbourNode * nei )
{
	nei = nei;
	return;
}

void neighbournode_evolve( void * svcptr, TiEvent * e )
{
	
}

/* option = 0x00  No ACK support in the MAC 
 * option = 0x01 ACK is used in MAC (default value)
 */
uint8 neighbournode_send(TiNeighbourNode * svc,uint16 addr, TiFrame * frame, uint8 option )
{
	//TiFrame * s_frame;
	//char sframe_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	char * response;
	char * nei;
	char * pkt;
    uint8 legth;
	uint16 fcf;
	uint8 size, len;
	uint8 i;

    response = frame_startptr(frame);
    legth = frame_length( frame);
	//s_frame = frame_open( (char*)(&sframe_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    frame_reset( svc->txbuf,3,20,0);
    nei = frame_startptr( svc->txbuf);//nei = frame_startptr( s_frame);
    
	NHB_SET_SEQUENCEID( nei,svc->seqid);
	NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( response[1],response[2]));
	NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
	NHB_SET_PANTO(nei,CONFIG_PANTO);
	NHB_SET_PANFROME(nei,CONFIG_PANTO);
    pkt = NHB_PAYLOAD_PTR( nei);
	for (i=0; i<legth; i++)
		pkt[i] = response[i];
    frame_setlength( svc->txbuf,( legth+8));//frame_setlength( s_frame,( legth+8));
	len = aloha_send( svc->mac,addr, svc->txbuf, 0x01 );//len = aloha_send( svc->mac,addr, s_frame, option );
	if (len > 0)
		svc->seqid ++;

	return len;
}


uint8 neighbournode_broadcast( TiNeighbourNode * svc, TiFrame * buf, uint8 option )
{
	//TiFrame * frame;
	//char frame_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	char * response;
	uint16 fcf;
	uint8 size, len;
    uint8 legth;
	uint8 i;
	char * nei;
	char * pkt;

    response = frame_startptr( buf );
    legth = frame_length( buf);
	//frame = frame_open( (char*)(&frame_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );


	//frame_reset( frame,3,20,0);
    frame_reset( svc->txbuf,3,20,0);
    nei = frame_startptr(svc->txbuf);
	

	NHB_SET_SEQUENCEID( nei,0x02);
	NHB_SET_SHORTADDRTO( nei,NHB_MAKEWORD( 0xff,0xff));
	NHB_SET_SHORTADDRFROM( nei,NHB_MAKEWORD( response[3],response[4]));
	NHB_SET_PANTO(nei,CONFIG_PANTO);
	NHB_SET_PANFROME(nei,CONFIG_PANTO);

	pkt = NHB_PAYLOAD_PTR( nei);

	for (i=0; i<legth; i++)
		pkt[i] = response[i];

     frame_setlength( svc->txbuf,( legth+8));
	
	len = aloha_broadcast( svc->mac, svc->txbuf, 0x00 );


	if (len > 0)
		svc->seqid ++;

	return len;
}



uint8 neighbournode_recv( TiNeighbourNode * svc, TiFrame * buf, uint8 option )
{
	//TiFrame * rxbuf;
    uint8 rssi;
	//char rx_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	uint8 len=0;
	uint8 count = 0;
	char * response = frame_startptr( buf );
	char * nei;
	char * pkt;

    char *pc;
    uint8 i;
	//rxbuf = frame_open( (char*)(&rx_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );

    nei = frame_startptr( buf);

    frame_reset( svc->rxbuf,3,20,0);
    frame_reset(buf,3,20,0);
	len = aloha_recv( svc->mac, svc->rxbuf, option );

    if (len > 0)
    {
        nei = frame_startptr( svc->rxbuf);
        pkt = NHB_PAYLOAD_PTR(nei);

        count = min( frame_capacity(buf), (len-9) );
        memmove( response, pkt, count );
        frame_setlength( buf, count );
    }

   // return count;
    return count;
	
}

uint8 neighbournode_found( TiNeighbourNode * svc )
{
    //TiFrame * rxbuf;
    uint8 rssi;
    //char rx_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
    uint8 len;
    uint8 count = 0;
    //char * response = frame_startptr( buf );
    char * nei;
    char * pkt;

    char *pc;
    uint8 i;
    //rxbuf = frame_open( (char*)(&rx_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );

    frame_reset( svc->rxbuf,3,20,0);

    len = aloha_recv( svc->mac, svc->rxbuf, 0x01 );


    if (len > 0)
    {
        
        frame_movelower(svc->rxbuf);
        nei = frame_startptr( svc->rxbuf);
        /*
        pc = frame_startptr( svc->rxbuf);//todo for testing
        for ( i=0;i<frame_length( svc->rxbuf);i++)//todo for testing
        {
            USART_Send( pc[i]);//todo for testing
        }
        */
        rssi = nei[ frame_length(svc->rxbuf)-2];
        //USART_Send( rssi);//todo for testing
        frame_movehigher(svc->rxbuf);
       
        nei = frame_startptr( svc->rxbuf);

        for ( i=0;i<NeiNum;i++)
        {
            if ( svc->nodeinf[i].state==0)
            {
                svc->nodeinf[i].seqid = NHB_SEQUENCEID( nei);
                svc->nodeinf[i].addr = NHB_SHORTADDRFROM(nei);
                svc->nodeinf[i].pan = NHB_PANFROM(nei);
                svc->nodeinf[i].rssi = rssi;
                svc->nodeinf[i].state = 1;
                count = 1;
                break;
            }
        }
    }

    return count;
}

uint8 neighbournode_getrssi( TiNeighbourNode *svc,uint8 id,uint8 rssi)
{
    uint8 count;
    if ( svc->nodeinf[id].state)
    {
        rssi = svc->nodeinf[id].rssi;
        count =1;
    } 
    else
    {
        count = 0;
    }

    return count;
}

uint8 neighbournode_getaddr( TiNeighbourNode *svc,uint8 id,uint16 addr)
{
    uint8 count;
    if ( svc->nodeinf[id].state)
    {
        addr = svc->nodeinf[id].addr;
        count = 1;
    } 
    else
    {
        count = 0;
    }
    return count;
}

uint8 neighbournode_getpan( TiNeighbourNode *svc,uint8 id,uint16 pan)
{
    uint8 count;
    if ( svc->nodeinf[id].state)
    {
        pan = svc->nodeinf[id].pan;
        count = 1;
    } 
    else
    {
        count = 0;
    }
}

uint8 neighbournode_getstate( TiNeighbourNode *svc,uint8 id)
{
    return svc->nodeinf[id].state;
}

bool neighbournode_empty( TiNeighbourNode *svc)
{
    int i;
    bool ret;
    for ( i=0;i<svc->neighbourmaxcount;i++)
    {
        if ( svc->nodeinf[i].state ==1)
        {
            break;
        }
    }

    if ( i>=svc->neighbourmaxcount)
    {
        ret =1;
    } 
    else
    {
        ret =0;
    }

    return ret;
}

bool neighbournode_full( TiNeighbourNode *svc)
{
    int i;
    bool ret;
    for ( i=0;i<svc->neighbourmaxcount;i++)
    {
        if ( svc->nodeinf[i].state ==0)
        {
            break;
        }
    }

    if ( i>=svc->neighbourmaxcount)
    {
        ret =1;
    } 
    else
    {
        ret =0;
    }

    return ret;
}

void neighbournode_clear( TiNeighbourNode * svc)
{
    int i;
    for ( i=0;i<svc->neighbourmaxcount;i++)
    {
        svc->nodeinf[i].state=0;
    }
}
void neighbournode_delete( TiNeighbourNode * svc,uint8 id)
{
    svc->nodeinf[id].state = 0;
}

void dump_nodeinf( TiNeighbourNode *svc,uint8 id)
{
    USART_Send(svc->nodeinf[id].seqid);
    USART_Send( svc->nodeinf[id].rssi);
    USART_Send(svc->nodeinf[id].pan>>8);
    USART_Send(svc->nodeinf[id].pan&0xff);
    USART_Send(svc->nodeinf[id].addr>>8);
    USART_Send(svc->nodeinf[id].addr&0xff);
}

