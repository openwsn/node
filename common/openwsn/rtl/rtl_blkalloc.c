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

/*******************************************************************************
 * @author zhangwei on 2006-07-07
 * @note
 *  BlockAllocator
 *  It's convenient to allocate fix size memory blocks using this object.
 *
 * @history
 * @modified by zhangwei on 2006-07-07
 *
 ******************************************************************************/

#include "rtl_configall.h"
#include "rtl_blkalloc.h"

#define GET_BLOCK_ADDR(alloc,dataindex) (alloc->blockdata + dataindex * alloc->blocksize)

TBlockAllocator * blkalloc_construct( char * buf, uint16 bufsize, uint16 blockcapacity, uint16 blocksize )
{
	TBlockAllocator * alloc = NULL;
	uint16 blockcount, n;
	uint32 tmp, size;
	char * data;
    short ret = 0;

	assert( buf != NULL );
	if (buf == NULL)
	{
		ret = -1;
	}
	else{
		n = sizeof(TBlockAllocator) + sizeof(TBlockAllocatorItem) * blockcapacity
			+ blocksize * blockcapacity;
		if (n > bufsize)
			ret = -2;
	}
	
	if (ret == 0)
	{
		alloc = (TBlockAllocator *)buf;
		alloc->emptylist = 0;
		alloc->activelist = 0;
		alloc->blocktable = (char *)buf + sizeof(TBlockAllocator);
		alloc->blockdata = (char *)buf + sizeof(TBlockAllcator) + sizeof(TBlockAllocatorItem) * blockcapacity;
		alloc->chunksize = bufsize;
		alloc->blocksize = blocksize;
		alloc->blockcapacity = capacity;
		alloc->blockcount = 0;
		
		blocktable = alloc->blocktable;
		for (n=0; n<blockcapacity-1; n++)
		{
			blocktable[n]->next = n+1;
			blocktable[n]->index = n;
		}
		blocktable[blockcapacity-1]->next = BLKALLOC_END_FLAG;
		blocktable[blockcapacity-1]->index = blockcapacity - 1;
	}
	
	return (ret == 0) ? alloc : NULL;
}

void alloc_destroy( TBlockAllocator * alloc )
{
	if (alloc != NULL)
	{
		alloc->emptylist = 0;
		alloc->activelist = 0;
		alloc->blockcount = 0;
		
		blocktable = alloc->blocktable;
		for (n=0; n<blockcapacity-1; n++)
		{
			blocktable[n]->next = n+1;
			blocktable[n]->index = n;
		}
		blocktable[blockcapacity-1]->next = BLKALLOC_END_FLAG;
		blocktable[blockcapacity-1]->index = alloc->blockcapacity - 1;
	}
}

uint16 blkalloc_memory_estimate( uint16 blockcapacity, uint16 blocksize )
{
	return (sizeof(TBlockAllocator) + sizeof(TBlockAllocatorItem) * blockcapacity
		+ blocksize * blockcapacity);
}

void * blkalloc_apply( TBlockAllocator * alloc )
{
	uint16 cur;
	void * block;

    assert( alloc != NULL );
	if (alloc->blockcount <= alloc->blockcapacity)
	{
		cur = alloc->emptylist;
		alloc->emptylist = alloc->blocktable[cur].next;
		alloc->blocktable[cur].next = alloc->activelist;
		alloc->activelist = cur;
		block = blkalloc_getblock( alloc, alloc->blocktable[cur].index );
	}
	else
		block = NULL;

	return block;
}

void blkalloc_release( TBlockAllocator * alloc, void * block )
{
	uint16 prev, cur, next;
	bool found = false;

    assert( alloc != NULL );
	prev = alloc_END_FLAG;
	cur = alloc->activelist;
	next = alloc_END_FLAG;
	while (cur != alloc_END_FLAG)
	{
		if (blkalloc_getblock(alloc, alloc->blocktable[cur].index) == block)
		{
			found = true;
			next = alloc->blocktable[cur].next;
			break;
		}
		prev = cur;
		cur = alloc->blocktable[cur].next;
	}

	/* you should always found the block, or else the input value "block" must
	 * be invalid
	 */
	if (found)
	{
		if (prev != BLKALLOC_END_FLAG)
		{
			alloc->blocktable[prev].next = next;
		}
        else{
			alloc->activelist = next;
		}

		alloc->blocktable[cur].next = alloc->emptylist;
		alloc->emptylist = cur;
	}
}

bool blkalloc_isempty( TBlockAllocator * alloc )
{
    assert( alloc != NULL );
	return (alloc->blockcount == 0);
}

bool blkalloc_isfull( TBlockAllocator * alloc )
{
    assert( alloc != NULL );
	return (alloc->blockcount >= alloc->blockcapacity);
}

uint16 blkalloc_blocksize( TBlockAllocator * alloc )
{
    assert( alloc != NULL );
	return alloc->blocksize;
}

uint16 blkalloc_blockcount( TBlockAllocator * alloc )
{
    assert( alloc != NULL );
	return alloc->blockcount;
}

uint16 blkalloc_blockcapacity( TBlockAllocator * alloc )
{
    assert( alloc != NULL );
	return alloc->blockcapacity;
}

