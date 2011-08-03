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
TiHashTable
TiRelation --- also {key,value} pair. however, it uses heap instead of hash table


hashtable_put
put
get
set
find(key,id)
findempty
indexof(key)
hashfun
hashkey2index
hashput
hashget


bool vex_apply( TiVectorX * vex, uintx * idx )
{
	uintx i;
	uint8 * pstate;
	bool found=false;

	for (i=vex->count; i<vex->capacity; i++)
	{
		pstate = vex_states(vex) + idx;
		if (*pstate == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		for (i=0; i<vex->count; i++)
		{
			pstate = vex_states(vex) + idx;
			if (*pstate == 0)
			{
				found = true;
				break;
			}
		}
	}

	if (found)
	{
		*idx = i;
		*pstate = 1;
		vex->count ++;
	}

	return found;
}

vex_findempty
= vex_find(EMPTY)

bool vex_indexof( vex, key )
{

}

void * vex_find( vex, key );
{
	uintx idx;
	if (vex_indexof(vex,key,idx))
		return vex_getbuf(vex,idx);
	else
		return NULL;
}

bool vex_findempty( TiVectorX * vex, uintx * idxfrom, uint8 state )
{
	uint8 *state, i;
	bool found=false;

	state = (char*)vex + sizeof(TiVectorX);
	for (i=*idxfrom; i<vex->capacity; i++)
	{
		if (*state == 0)
		{
			*idxfrom = i;
			found = true;
			break;
		}
		state++;
	}
	if (!found)
	{
		for (i=0; i<*idxfrom; i++)
		{
			if (*state == 0)
			{
				*idxfrom = i;
				found = true;
				break;
			}
			state++;
		}
	}

	return found;
}

/* hash function */
bool vex_hashkey2index( TiVectorX * vex, uintx key, uintx * idx )
{
	key %= vex->capacity;
}

bool _vex_hashfind_state( TiVectorX * vex, uintx key, uintx * idx, uint8 state )
{
	vex_hashkey2index(idx);
	if (_vex_find_state(vex, &idx, state))
	{
		return idx;
		return true;
	}
	else
		return false;
}

bool vex_hashindexof( TiVectorX * vex, uintx key, uintx * idx )
{
	return vex_hashfind(vex, key, idx, NON_EMPTY);
}

void * vex_hashfind(vex, key)
{
	uintx idx;
	if (vex_hashindexof(vex,key,idx))
		return vex_getbuf(vex,idx);
	else
		return NULL;
}

bool vex_hashfindempty( TiVectorX * vex, uintx key, uintx * idx )
{
	return vex_hashfind(vex, key, idx, EMPTY);
}

void * vex_hashget( TiVectorX * vex, uintx key )
{
	uintx idx;

	if (vex_hashfind(vex, key, &idx))
		return vex_get(vex, idx);
	else
		return NULL;
}

bool vex_hashput( TiVectorX * vex, uintx key, uintx * idx )
{
	uintx idx;

	if (vex_hashfindempty(vex, key, &idx))
		return vex_put(vex, idx);
	else
		return NULL;
}

bool vex_hashset( TiVectorX * vex, uintx key, uintx * idx )
{
	uintx idx;

	if (vex_hashfind(vex, key, &idx))
		return vex_set(vex, idx);
	else
		return NULL;
}


/*
bool vex_append( TiVectorX * vex, void * item )
{
	char * buf;
	bool ret;

	if (vex->count < vex->capacity)
	{
		buf = vex_getbuf(vex, vex->count);
		memmove( buf, item, vex->itemlen );
		vex->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

bool vex_insert( TiVectorX * vex, uintx index, void * item )
{
	char * buf;
	bool ret;

	assert( index <= vex->count );
	if (vex->count >= vex->capacity)
		return false;

	buf = vex_getbuf(vex, index);
	len = (vex->count - vex->index-1) * vex->itemlen;
	if (len > 0)
	{
		memmove( (char*)buf+vex->itemlen, buf, len );
	}

	assert( buf != NULL );
	memmove( buf, item, vex->itemlen );
	vex->count ++;

	return true;
}

bool vex_remove( TiVectorX * vex, uintx index )
{
	if (index < vex->count)
	{
		buf = vex_getbuf(vex, index);
		len = (vex->count - index - 1) * vex->itemlen;
		if (len > 0)
		{
			memmove( (char*)buf, (char*)buf+vex->itemlen, len );
		}
		vex->count --;
		return true;
	}
	else
		return false;
}

bool vex_push( TiVectorX * vex, void * item )
{
	return vex_insert(vex, 0, item);
}

bool vex_pop( TiVectorX * vex )
{
	if (vex->count > 0)
	{
		vex->count --;
		return true;
	}
	else
		return false;
}

void vex_swap( TiVectorX * vex, uintx i, uintx j )
{
}

void vex_resize( TiVectorX * vex, void * newbuf, uintx size )
{
	TiVectorX * newvec;

	newvec = vex_construct( newbuf, size );
	capacity = newvec->capacity;
	memmove( newbuf, vex, min(vex->size, size) );
	newvec->items = vex_getbuf( newvec, 0 );
	newvec->count = min( capacity, vex->count );
}
*/
