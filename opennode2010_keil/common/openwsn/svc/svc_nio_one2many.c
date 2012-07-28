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
#include "svc_nio_one2many.h"

TiOne2Many * one2many_construct( void * mem, uint16 memsize )
{
	memset( mem, 0x00, memsize );
	return (TiOne2Many *)mem;
}

void one2many_destroy( TiOne2Many * o2m )
{
	/* The following line is used to eliminate the compiling warning only */
	o2m = o2m;
	return;
}

TiOne2Many * one2many_open( TiOne2Many * o2m, TiAloha * mac )
{
    int i;
    TiIEEE802Frame154Descriptor * desc;
    char *buf;
	desc = &(o2m->desc);
	
	o2m->seqid = 0;
	o2m->mac = mac;

	// o2m->uart = uart;
    o2m->rxbuf = frame_open( (char*)(&o2m->rxbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    o2m->txbuf = frame_open( (char*)(&o2m->txbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    o2m->tmpbuf = frame_open( (char*)(&o2m->tmpbuf_memory), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

	//if(o2m->type==GATEWAYTYPE)
	//{
		/*//o2m->vti = vtm_apply( vtm );
		vti_open( o2m->vti, _vti_listener, o2m );
		vti_setscale( o2m->vti, 1 );
		vti_setinterval( o2m->vti, 100, 0x00 );
		vti_start( o2m->vti );*/
        desc = ieee802frame154_format( desc, frame_startptr(o2m->txbuf), 20, /*frame_capacity(o2m->txbuf), */
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
	//}
	//else if(o2m->type==SENSORTYPE)
	//{
		//o2m->timer = NULL
	//}
	//else
	//{
		//o2m->timer = NULL
	//}

	return o2m;
}


void one2many_close( TiOne2Many * o2m )
{
	o2m = o2m;
	return;
}

void one2many_evolve( void * svcptr, TiEvent * e )
{
	/*
	TiOne2Many o2m = (TiOne2Many *)(svcptr);

	if(o2m->type==GATEWAYTYPE)
	{
		if(o2m->gsend_expired == TIMEISUP)
		{				
			if(aloha_send(o2m->mac,o2m->opf,0x00) > 0)
			{
				o2m->time_expired == TIMEISNOTUP;
				vti_setinterval( o2m->vti, 100, 0x00 );
				vti_setscale( o2m->vti, 1 );			
			}
			else
			{
			
			}


		}
	}
	else if(o2m->type==SENSORTYPE)
	{
	
	}
	else
	{}
	*/
}

/* option = 0x00  No ACK support in the MAC 
 * option = 0x01 ACK is used in MAC (default value)
 */
uint8 one2many_send( TiOne2Many * svc,uint16 addr, TiIoBuf * buf, uint8 option )
{
	//TiOpenFrame * opf;
	TiFrame * frame;
	//char opfmem[ OPF_SUGGEST_SIZE ];
	char frame_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	char * response;
	char * o2m;
	char * pkt;
    uint8 legth;
	uint16 fcf;
	uint8 size, len;
	uint8 i;

    response = iobuf_ptr( buf );
    legth = iobuf_length( buf);//todo for testing
	//opf = opf_open( &opfmem[0], sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );
	frame = frame_open( (char*)(&frame_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    o2m = frame_startptr( frame);
    //fcf = OPF_DEF_FRAMECONTROL_DATA; 

	//hal_assert( iobuf_length(buf) <= 0x7F-14 );  // 14 is the header size plus 2 byte checksum
	//size = 14 + iobuf_length(buf);             // frame length, which is equal the length of PSDU
                                               // plus addtional 1.
	/*
	opf_cast( opf, size, fcf );
    opf_set_sequence( opf, svc->seqid );
	opf_set_panto( opf, CONFIG_PANTO );
	opf_set_shortaddrto( opf, OPF_MAKEWORD( response[1], response[2] ) );
	opf_set_panfrom( opf, CONFIG_PANTO );
	opf_set_shortaddrfrom( opf, OPF_MAKEWORD( response[3], response[4] ) );
	*/
	O2M_SET_SEQUENCEID( o2m,svc->seqid);
	O2M_SET_SHORTADDRTO( o2m,O2M_MAKEWORD( response[1],response[2]));
	O2M_SET_SHORTADDRFROM( o2m,O2M_MAKEWORD( response[3],response[4]));
	O2M_SET_PANTO(o2m,CONFIG_PANTO);
	O2M_SET_PANFROME(o2m,CONFIG_PANTO);
    pkt = O2M_PAYLOAD_PTR( o2m);
	for (i=0; i<legth; i++)
		pkt[i] = response[i];
    frame_setlength( frame,( legth+9));//在PAYLOAD之前还有8个数据位		  //JOE 原有为8   bug fix  0426
    //frame_setlength( frame,( legth+8));//在PAYLOAD之前还有8个数据位


	/*for(uint8 i=0; i<opf->datalen; i++)
	{
		dbo_putchar(opf->buf[i]);
	}*/

	len = aloha_send( svc->mac,addr, frame, option );
	if (len > 0)
		svc->seqid ++;

	return len;
}

/* one2many_broadcast()
 * broadcast the data to all neighbor nodes.
 * the broadcast frame doesn't need ACK 
 */
uint8 one2many_broadcast( TiOne2Many * svc, TiIoBuf * buf, uint8 option )
{
	TiFrame * frame;
	char frame_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	char * response;
	uint16 fcf;
	uint8 size, len;
    uint8 legth;
	uint8 i;
	char * o2m;
	char * pkt;

    response = iobuf_ptr( buf );
    legth = iobuf_length( buf);//todo for testing
	frame = frame_open( (char*)(&frame_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );


	frame_reset( frame,3,20,0);//todo for testing
    o2m = frame_startptr( frame);
	/*

    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	hal_assert( iobuf_length(buf) <= 0x7F-14 );  // 14 is the header size plus 2 byte checksum
	size = 14 + iobuf_length(buf);             // frame length, which is equal the length of PSDU
                                               // plus addtional 1.	                                           
	opf_cast( opf, size, fcf );

	opf_set_sequence( opf, svc->seqid );
	opf_set_panto( opf, CONFIG_PANTO );
	opf_set_shortaddrto( opf, OPF_MAKEWORD( 0xFF, 0xFF ) );
	opf_set_panfrom( opf, CONFIG_PANTO );
	opf_set_shortaddrfrom( opf, OPF_MAKEWORD( response[3], response[4] ) );
	*/

	O2M_SET_SEQUENCEID( o2m,0x02);
	O2M_SET_SHORTADDRTO( o2m,O2M_MAKEWORD( 0xff,0xff));
	O2M_SET_SHORTADDRFROM( o2m,O2M_MAKEWORD( response[3],response[4]));
	O2M_SET_PANTO(o2m,CONFIG_PANTO);
	O2M_SET_PANFROME(o2m,CONFIG_PANTO);

	pkt = O2M_PAYLOAD_PTR( o2m);

	for (i=0; i<legth; i++)
		pkt[i] = response[i];

     frame_setlength( frame,( legth+9));//在PAYLOAD之前还有8个数据位	    //JOE 原有为8   bug fix  0426
     //frame_setlength( frame,( legth+8));//在PAYLOAD之前还有8个数据位
	/*for(uint8 i=0; i<opf->datalen; i++)
	{
		dbo_putchar(opf->buf[i]);
	}*/
	len = aloha_broadcast( svc->mac, frame, 0x00 );


	if (len > 0)
		svc->seqid ++;

	return len;
}


/* one2many_recv()
 * Try to receive RESPONSE replied from neighbor nodes. The RESPONSE will be put 
 * into "buf". Attention the "size" must be larger enough to hold the RESPONSE.
 * buf, and "len" is the length of the REQUEST.
 * 
 * REQUEST format:
 *  [type 1B][2B receiver address][2B sender address][lumnance 2B]
 *  type = 0x02 indicates this is a RESPONSE
 */
uint8 one2many_recv( TiOne2Many * svc, TiIoBuf * buf, uint8 option )
{
	TiFrame * rxbuf;
	char rx_mem[ FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ) ];
	uint8 len;
	uint8 count = 0;
	char * response = iobuf_ptr( buf );
	char * o2m;
	char * pkt;

    char *pc;//todo for testing
    uint8 i;//todo for testing

	rxbuf = frame_open( (char*)(&rx_mem), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 0 );
    
	len = aloha_recv( svc->mac, rxbuf, option );

	o2m = frame_startptr( rxbuf);

    if ( len>0)//todo for testing
    {
        pc = frame_startptr( rxbuf);//todo for testing
        for ( i=0;i<len;i++)//todo for testing
        {
            //USART_Send( pc[i]);//todo for testing
        }
    }
	
	if (len > 0)
	{	
		pkt = O2M_PAYLOAD_PTR(o2m);
		
		count = min( iobuf_size(buf), (len-9) );//len -9：pkt是从o2m的第9个字节开始的。//JOE_0426
		//count = min( iobuf_size(buf), (len-9) );//len -9：pkt是从o2m的第9个字节开始的。
		memmove( response, pkt, count );
		iobuf_setlength( buf, count );
	}
	
	return count;
}

