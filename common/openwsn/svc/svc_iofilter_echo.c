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
#include "svc_foundation.h"

typedef struct{
	void * owner;
}TiIoEchoFilter;

TiIoEchoFilter * iof_echo_construct( void * mem, uint16 size )
{
	svc_assert( sizeof(TiIoEchoFilter) <= size );
	memset( mem, 0x00, size );
	return (TiIoEchoFilter *)mem;
}

void iof_echo_destroy( TiIoEchoFilter * iof )
{
	return;
}

TiIoEchoFilter * iof_echo_open( TiIoEchoFilter * iof, void * owner )
{
	iof->owner = owner;
	return;
}

void iof_echo_close( TiIoEchoFilter * iof )
{
	return;
}

uint16 iof_rxhandle( void * object, TiIoBuffer * input, TiIoBuffer * output )
{
	// TiIoEchoFilter * iof = (TiIoEchoFilter *)object; 
	// assume the TiIoBuffer is actually a TiOpenFrame structure
	opf_copyfrom( output, input );
	opf_swapaddress( output );
}

uint16 iof_txhandle( void * object, TiIoBuffer * input, TiIoBuffer * output )
{
	// TiIoEchoFilter * iof = (TiIoEchoFilter *)object; 
	opf_copyto( output, input );
}