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

#include "rtl_queue.h"

#define KQUEUE_MAX_CAPACITY_LIMIT (0xFFFF-1)

#define RTL_QUEUE_INVALID_ID 0xFFFF
#define TRtlQueueItem TiQueueItem
#define TRtlQueue TiQueue

typedef struct{
  void * data;
  uint16 datalen;
  uint8 prev;
  uint8 next;
}TiQueueItem;

typedef struct{
  uint8 head;
  uint8 tail;
  uint8 capacity;
  uint8 count;
  uint16 datasize;
  TiQueueItem * itemtable;
}TiQueue; 

que_attachbuffer

TiQueue * que_construct( char * buf, uint16 size, uint16 datasize )
{
	TiQueue * que;
	char * expandbuf;
	uint8 adjust;

	que = (TiQueue *)buf;
	if (que != NULL)
	{
		memset( buf, 0x00, size );
		que->head = RTL_QUEUE_INVALID_ID;
		que->tail = RTL_QUEUE_INVALID_ID;
		que->capacity = 0;
		que->count = 0;
		que->datasize = datasize;
		expandbuf = (char*)(buf) + sizeof(TiQueue);
		adjust = (uint8) & 0x03;
		if (adjust > 0)
			expandbuf += (4 - adjust);
			
		que_attachbuffer( que, expandbuf, size-(buf-expandbuf), datasize );
		if (que->capacity == 0)
			que = NULL;
	}
	
	return que;
}

void que_destroy( TiQueue * que )
{
	que->head = 0;
	que->tail = 0;
	que->count = 0;
}

uint8 void que_attachbuffer( TiQueue * que, char * buf, uint16 size, uint16 datasize )
{
	TiQueueItem * item;

	if (datasize == 0)
	{
		que->capacity = size / sizeof(TiQueueItem);
		que->datasize = 0;
	}
	else{
		que->capacity = size / (size(TiQueueItem) + datasize);
		que->datasize = datasize;
	}

	que->item = (TiQueueItem *)buf;
	memset( buf, 0x00, size );
	item = que->item;
	item[0]->prev = RTL_QUEUE_INVALID_ID;
	item[0]->next = 1;	
	for (n=1; n<que->capacity; n++)
	{
		item[n]->prev = n-1;
		item[n]->next = n+1;
		item[n]->data = NULL;
		item[n]->datalen = 0;
	}
	item[que->capacity]->next = RTL_QUEUE_INVALID_ID;
	
	if (datasize > 0)
	{
		buf = buf + sizeof(TiQueueItem) * que->capacity;
		for (n=0; n < que->capacity; n++)
		{
			item[n]->data = buf;
			buf += datasize;
		}
	}
	
	return que->capacity; 
}

#define que_front(q) que_first(q)
#define que_back(q) que_last(q)
#define que_head(q) que_first(q)
#define que_tail(q) que_tail(q)

uint8 que_first( TiQueue * que )
{
	return (count>0) ? que->head : RTL_QUEUE_INVALID_ID;
}

uint8 que_next( TiQueue * que )
{
	return (count>0) ? que->itemtable[que->head].next : RTL_QUEUE_INVALID_ID;
}

uint8 que_last( TiQueue * que )
{
	return (count>0) ? que->tail : RTL_QUEUE_INVALID_ID;
}

uint8 que_prev( TiQueue * que )
{
	return (count>0) ? que->itemtable[que->tail].prev : RTL_QUEUE_INVALID_ID;
}

boolean que_pophead( TiQueue * que )
{
	boolean ret;
	
	if (que->count > 0)
	{
		que->head ++;
		que->head %= que->capacity;
		que->count --;
		ret = TRUE; 
	}
	else
		ret = FALSE;
		
	return ret;
}

uint8 que_pushtail( TiQueue * que )
{
	uint8 ret;
	
	if (que->count < que->capacity)
	{
		if (count == 0)
		{
			que->head = 0;
			que->tail = 0;
		}
		else{
			que->tail ++;
			que->tail %= que->capacity;
		}
		que->count ++;
		ret = que->tail;
	}
	else
		ret = RTL_QUEUE_INVALID_ID;
	
	return ret;	
}

boolean que_poptail( TiQueue * que )
{
	boolean ret;
	
	if (que->count > 0)
	{
		if (que->tail > 0)
			que->tail --;
		else
			que->tail = que->capacity;
		
		que->count --;
		ret = TRUE;
	}
	else
		ret = FALSE;
	
	return ret;
}

uint8 que_pushhead( TiQueue * que )
{
	uint8 ret;
	
	if (que->count < que->capacity)
	{
		if (que->head > 0)
			que->head --;
		else
			que->head = que->capacity;
		que->count ++;
		ret = que->head;
	}
	else
		ret = RTL_QUEUE_INVALID_ID;
		
	return ret;
}

uint8 que_insert( TiQueue * que, uint8 index )
{
	// @TODO que_insert
	return RTL_QUEUE_INVALID_ID;
}

uint16 que_get( TiQueue * que, uint8 id, char * buf, uint16 capacity )
{
	TiQueueItem * item;
	uint16 ret;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	if (item != NULL)
	{
		ret = min(item->datalen, capacity );
		memmove( buf, item->data, ret );
	}
	else
		ret = 0;
		
	return ret;
}

void * que_getdatabuf( TiQueue * que, uint8 id, uint16 * len )
{
	TiQueueItem * item;
	void * ret;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	if (item != NULL)
	{
		ret = item->data;
		if (len != NULL))
			*len = item->datalen;
	}
	else
		ret = NULL;
	
	return ret;
	
}

void que_set( TiQueue * que, uint8 id, char * buf, uint16 datalen )
{
	TiQueueItem * item;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	item->datalen = datalen;
	item->data = buf;
}


/* different to function "set", "put" will copy the input data into queue's internal 
 * buffer. while, "set" function will only change the value of the queue item member
 */
void que_put( TiQueue * que, uint8 id, char * buf, uint16 datalen )
{
	TiQueueItem * item;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	item->datalen = min( que->datasize, datalen );
	memmove( item->data, buf, item->datalen );
}





que_

alloc

que_alloc
que_release
que_enqueue
que_push
que_pophead

que_getitem
que_setitem
que_items()
que_gethead
que_gettail
alloctail
allochead

que_apply
que_release


/* construct a queue object in a memory area
 * the memory is allocated as the following:
 *
 *	[KQueue structure][item 0][item 1]...[item count-1][data 1][data 2]...
 * 
 * all the items in the above memory area are grouped into two list:
 * the queue list and the available list. you can get one new item from 
 * the available list through que_applyitem() and return it back through  
 * que_releaseitem().
 * 
 * @param
 * 	buf			memory are
 * 	size		the memory size
 * 	capacity	maximum number of items the queue can hold. it should be 
 * 				less than KQUEUE_MAX_CAPACITY_LIMIT
 * @return 
 * 	an pointer to the KQueue object if success
 * 	NULL when failed
 */
TKQueue * que_construct( void * buf, uint16 size, uint16 capacity, uint16 datalen )
{
	TKQueue * que = (TKQueue *)buf;
	uint16 n;	
	
	if (capacity * sizeof(TKQueueItem) + sizeof(TKQueue) + size(TQueueItem)*capacity) > size)
		que = NULL;
		
	if (que != NULL) && (capacity >= KQUEUE_MAX_CAPACITY_LIMIT)
		que = NULL;
		
	if (que != NULL)
	{
		memset( buf, 0x00, size );
		que->capacity = capacity;
		que->count = 0;
		que->list = KQUEUE_MAX_CAPACITY_LIMIT;
		que->avaliables = 0;
		que->items = (char *)buf + sizeof(TKQueue);
		que->data = (char *)buf + sizeof(TKQueue) + size(TQueueItem)*capacity;
		
		que->items[0]->prev = 0;
		que->items[0]->next = 1;
		for (n=1: n<capacity-2; n++)
		{
			que->items[n]->prev = n-1;
			que->items[n]->next = n+1;			
		}
		que->items[capacity-1]->prev = capacity - 2;
		que->items[capacity-1]->next = KQUEUE_MAX_CAPACITY_LIMIT;
	}
	
	return que;
}

void que_destroy( TKQueue * que )
{
	uint16 n;	

	if (que != NULL)
	{
		que->count = 0;
		que->list = KQUEUE_MAX_CAPACITY_LIMIT;
		que->avaliables = 0;
		que->items = (char *)buf + sizeof(TKQueue);
		
		que->items[0]->prev = 0;
		que->items[0]->next = 1;
		for (n=1: n<capacity-2; n++)
		{
			que->items[n]->prev = n-1;
			que->items[n]->next = n+1;			
		}
		que->items[capacity-1]->prev = capacity - 2;
		que->items[capacity-1]->next = KQUEUE_MAX_CAPACITY_LIMIT;
	}
}

int16 que_applyitem( TKQueue * que, uint16 * itemidx );
{
	int16 ret;
	
	if (que->availables != KQUEUE_MAX_CAPACITY_LIMIT)
	{
		*itemidx = que->availables;
		que->availables = que->items[*itemidx]->next;
		ret = 0;
	}
	else{
		ret = -1;
	}
	
	return ret;	 
}

void que_releaseitem( TKQueue * que, uint16 itemidx )
{
	que->items[itemidx]->next = que->availables;
	que->availables = itemidx;
}

/* set the item content from another item
 * no matter the item in the queue list or available list
 */
void que_set( TKQueue * que, int idx, TQueueItem * item )
{
	assert( idx < que->capacity );
	memmove( &(que->items[idx]), item );
}

TQueueItem * que_get( TKQueue * que, int idx )
{
	assert( idx < que->capacity );
	return &(que->items[idx]);
}

#define que_getdata(que,idx) que->items[idx]->data

que_setdata( TKQueue * que, int idx, void * data )
que_first()
que_next()
que_prev()
que_last()
que_current()
que_back/tail
que_front/head

que_insert( que, int idx, data/item )

que_push( que, data )
que_pop( que )



















