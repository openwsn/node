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

#include "rtl_lightvector.h"

/* @status: finished. not tested yet */

/* construct a object in the memory */
TiVector * vec_construct( void * buf, uint16 size, uint16 itemlen )
{
	TiVector * vec;

	assert( sizeof(TiVector) < size );
	assert( itemlen > 0 );

	vec = (TiVector *)buf;
	vec->size = size;
	vec->itemlen = itemlen;
	vec->capacity = (size - sizeof(TiVector)) / itemlen;
	vec->count = 0;
	vec->index = 0;
}

void vec_destroy( TiVector * vec )
{
	return;
}

intx vec_capacity( TiVector * vec )
{
	return vec->capacity;
}

intx vec_count( TiVector * vec )
{
	return vec->count;
}

intx vec_capacity( TiVector * vec )
{
	return vec->capacity;
}

intx vec_empty( TiVector * vec )
{
	return vec->count = 0;
}

intx vec_full( TiVector * vec )
{
	return vec->count = vec->capacity;
}

void * vec_items( TiVector * vec )
{
	return (char*)vec + itemlen;
}

void * vec_getbuf( TiVector * vec, uint16 idx )
{
	assert( idx < vec->capacity );
	return (char*)vec->items + (idx * vec->itemlen);
}

bool vec_get( TiVector * vec, uint16 idx, void * buf )
{
	void * item = vec_getbuf( vec, idx );
	if (item != NULL)
	{
		memmove( buf, item, vec->itemlen );
		return true;
	}
	else
		return false;
}

bool vec_set( TiVector * vec, uint16 idx, void * item )
{
	char * buf;
	buf = vec_getbuf(vec, idx);
	if (buf)
	{
		memmove( buf, item, vec->itemlen );
		return true;
	}
	else
		return false;
}

bool vec_put( TiVector * vec, void * item )
{
	return vec_append(vec,item);
}

bool vec_append( TiVector * vec, void * item )
{
	char * buf;
	bool ret;

	if (vec->count < vec->capacity)
	{
		buf = vec_getbuf(vec, vec->count);
		memmove( buf, item, vec->itemlen );
		vec->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

bool vec_insert( TiVector * vec, uint16 index, void * item )
{
	char * buf;
	bool ret;

	assert( index <= vec->count );
	if (vec->count >= vec->capacity)
		return false;

	buf = vec_getbuf(vec, index);
	len = (vec->count - vec->index-1) * vec->itemlen;
	if (len > 0)
	{
		memmove( (char*)buf+vec->itemlen, buf, len );
	}

	assert( buf != NULL );
	memmove( buf, item, vec->itemlen );
	vec->count ++;

	return true;
}

bool vec_remove( TiVector * vec, uint16 index )
{
	if (index < vec->count)
	{
		buf = vec_getbuf(vec, index);
		len = (vec->count - index - 1) * vec->itemlen;
		if (len > 0)
		{
			memmove( (char*)buf, (char*)buf+vec->itemlen, len );
		}
		vec->count --;
		return true;
	}
	else
		return false;
}

void vec_clear( TiVector * vec )
{
	vec->count = 0;
}

bool vec_pushfront( TiVector * vec, void * item )
{
	return vec_append(vec, 0, item);
}

bool vec_pushback( TiVector * vec, void * item )
{
	return vec_insert(vec, 0, item);
}

bool vec_popfront( TiVector * vec )
{
	if (vec->count > 0)
	{
		vec->count --;
		return true;
	}
	else
		return false;
}

bool vec_poptail( TiVector * vec )
{
	vec_remove(vec, 0);
}

void vec_swap( TiVector * vec, uint16 i, uint16 j )
{
	char * item1 = vec_getbuf(vec, i);
	char * item2 = vec_getbuf(vec, j);

	assert( item1 != NULL );
	assert( item2 != NULL );

	for (k=0; k<vec->itemlen; k++)
	{
		p1 = item1 + k;
		p2 = item2 + k;
		*p1 = (*p1)^(*p2);
		*p2 = (*p2)^(*p1);
		*p1 = (*p1)^(*p2);
	}
}
