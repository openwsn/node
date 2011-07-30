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

#include "rtl_iobuf.h"

TiIoBuffer * buf_construct( void * mem, uintx size )
{
	TiIoBuffer * buf = (TiIoBuffer *)mem;
	assert( size > sizeof(TiIoBuffer) );

	buf->size -= sizeof(TiIoBuffer);
	buf->skiplen = 0;
	buf->len = 0;
	buf->index = 0;
	return buf;
}

void buf_destroy( TiIoBuffer * buf )
{
	NULL;
}

uintx buf_read( TiIoBuffer * buf, char * mem, uintx size )
{
	uintx len;

	assert( buf->skiplen + buf->len >= buf->index );
	len = min( buf->skiplen + buf->len - buf->index, size );
	if (len > 0)
	{
		memmove( mem, (char*)buf_data(iobuf) + buf->index, len );
		buf->index += len;
	}
	return len;
}

uintx buf_write( TiIoBuffer * buf, char * mem, uintx len )
{
	len = min( buf->size - buf->index, len );
	if (len > 0)
	{
		memmove( (char*)buf_data(buf) + buf->index, mem, len );
		buf->index += len;
		if (buf->index > buf->skiplen + buf->len)
			buf->len = buf->index - buf->skiplen;
	}
	return len;
}

uintx buf_append( TiIoBuffer * buf, char * mem, uintx len )
{
	uintx avail;

	len = min( buf->size - (buf->skiplen - buf->len), len );
	if (len > 0)
	{
		memmove( buf_tail(iobuf), mem, len );
		buf->len += len;
	}
	return len;
}

void buf_replace( TiIoBuffer * buf, uintx start, char * newdata, uintx len )
{
	char * data = buf_header(buf);
	assert( start+len <= buf->size );
	memmove( data+start, newdata, len );
}

uintx buf_setskiplen( TiIoBuffer * buf, uintx value )
{
	intx delta = value - buf->skiplen;
	buf->skiplen = value;
	buf->len -= delta;
}

uintx buf_clear( TiIoBuffer * buf )
{
	buf->skiplen = 0;
	buf->len = 0;
	buf->index = 0;
}


