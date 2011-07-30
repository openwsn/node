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

#include "svc_configall.h"  
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_openframe.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_led.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_adc.h"
#include "../hal/hal_luminance.h"
#include "svc_foundation.h"
#include "svc_nio_aloha.h"
#include "svc_timer.h"
#include "svc_one2many.h"



#define CONFIG_PANTO 0x01

TiOne2Many * one2many_construct( void * mem, uint16 memsize )
{
	memset( mem, 0x00, memsize );
	return (TiOne2Many *)mem;
}


void one2many_destroy( TiOne2Many * o2m )
{
	o2m = o2m;
	return;
}


TiOne2Many * one2many_open( TiOne2Many * o2m, TiAloha * mac )
{
	o2m->seqid = 0;
	o2m->mac = mac;
/*
	o2m->uart = uart;
	o2m->vti = vti;
	o2m->siobuf = iobuf_construct( (void *)(&o2m->siobuf_memory[0]), 
		sizeof(o2m->siobuf_memory) );
	o2m->opf = opf_open( (void *)(&o2m->opf_memory[0]), sizeof(o2m->opf_memory), 
		OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );


	if(o2m->type==GATEWAYTYPE)
	{
		o2m->vti = vtm_apply( vtm );
		vti_open( o2m->vti, _vti_listener, o2m );
		vti_setscale( o2m->vti, 1 );
		vti_setinterval( o2m->vti, 100, 0x00 );
		vti_start( o2m->vti );

		uint8 fcf = OPF_DEF_FRAMECONTROL_DATA; 
		uint8 total_length = 20;
		opf_cast( o2m->opf, total_length, fcf );
        opf_set_sequence( o2m->opf, 0xAA );
		opf_set_panto( o2m->opf, MAC_GATE_PANID );
		opf_set_shortaddrto( o2m->opf, MAC_GATE_REMOTE );
		opf_set_panfrom( o2m->opf, MAC_GATE_PANID);
		opf_set_shortaddrfrom( o2m->opf, MAC_GATE_LOCAL );

		for (i=0; i<opf->msdu_len; i++)
			(o2m->opf)->msdu[i] = i;
	}
	else if(o2m->type==SENSORTYPE)
	{
		o2m->vti = NULL
	}
	else
	{
		o2m->vti = NULL
	}

	*/

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
uint8 one2many_send( TiOne2Many * svc, TiIoBuf * buf, uint8 option )
{
	TiOpenFrame * opf;
	char opfmem[ OPF_SUGGEST_SIZE ];
	char * response = iobuf_ptr( buf );
	uint16 fcf;
	uint8 size, len;
	uint8 i;

	opf = opf_open( &opfmem[0], sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );

    fcf = OPF_DEF_FRAMECONTROL_DATA; 
	hal_assert( iobuf_length(buf) <= 0x7F-14 );  // 14 is the header size plus 2 byte checksum
	size = 14 + iobuf_length(buf);             // frame length, which is equal the length of PSDU
                                               // plus addtional 1.
	opf_cast( opf, size, fcf );

	opf_set_sequence( opf, svc->seqid );
	opf_set_panto( opf, CONFIG_PANTO );
	opf_set_shortaddrto( opf, OPF_MAKEWORD( response[1], response[2] ) );
	opf_set_panfrom( opf, CONFIG_PANTO );
	opf_set_shortaddrfrom( opf, OPF_MAKEWORD( response[3], response[4] ) );

	for (i=0; i<opf->msdu_len; i++)
		opf->msdu[i] = response[i];


	/*for(uint8 i=0; i<opf->datalen; i++)
	{
		dbo_putchar(opf->buf[i]);
	}*/

	len = aloha_send( svc->mac, opf, option );
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
	TiOpenFrame * opf;
	char opfmem[ OPF_SUGGEST_SIZE ];
	char * response = iobuf_ptr( buf );
	uint16 fcf;
	uint8 size, len;
	uint8 i;

	opf = opf_open( &opfmem[0], sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );

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

	for (i=0; i<opf->msdu_len; i++)
		opf->msdu[i] = response[i];


	/*for(uint8 i=0; i<opf->datalen; i++)
	{
		dbo_putchar(opf->buf[i]);
	}*/

	len = aloha_broadcast( svc->mac, opf, 0x00 );
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
	TiOpenFrame * opf;
	char opfmem[ OPF_SUGGEST_SIZE ];
	uint8 len;
	uint8 count = 0;
	char * response = iobuf_ptr( buf );

	opf = opf_open( &opfmem[0], sizeof(opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );
	len = aloha_recv( svc->mac, opf, option );
	if (len > 0)
	{	
		
		count = min( iobuf_size(buf), opf_msdu_len(opf) );
		memmove( response, opf->msdu, count );
		iobuf_setlength( buf, count );
		// count = min( iobuf_size(buf), opf_framelength(opf)+1 );
		// memmove( response, opf->buf[0], count );
		// iobuf_setlength( buf, count );
	}
	return count;
}


/*void _vti_listener(void * svcptr, TiEvent * e )
{
	TiOne2Many * o2m = (TiOne2Many *)(svcptr);

	o2m->time_expired = TIMEISUP;
}*/
