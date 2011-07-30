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

/* RingQueue Data Structure
 * This ring queue has fixed number of items/elements. It will not increase memory
 * when it's full. It's mainly used in kernel developing or embedded developing
 * without dynamic memory allocator support
 *
 * @author zhangwei in 200503
 * @modified by zhangwei in 200803
 *	- eliminate the call of malloc() and free().
 */

#include "rtl_configall.h"
#include <string.h>

#ifdef LIGHTQUEUE_TEST
  #include <stdlib.h>
  #include <stdio.h>
  #ifdef CONFIG_LINUX
	#include <alloc.h>
  #endif
#endif

#include "rtl_foundation.h"
#include "rtl_lightqueue.h"

inline TiLightQueue * lwque_construct( void * buf, uint16 size, uint16 itemsize )
{
	TiLightQueue * que;
	que = (TiLightQueue *)buf;
	que->front = 0;
	que->rear = 0;
	que->capacity = (size - sizeof(TiLightQueue))/itemsize;
	que->count=0;
	que->itemsize = itemsize;

	// capacity must be larger than 1 in order to create a queue.
	rtl_assert( que->capacity > 0 );
	return que;
}

inline void lwque_destroy( TiLightQueue * que )
{
	return;
}

inline uint8 lwque_count( TiLightQueue * que )
{
	return que->count;
}

inline uint8 lwque_capacity( TiLightQueue * que )
{
	return que->capacity;
}

inline bool lwque_empty( TiLightQueue * que )
{
	return (que->count == 0);
}

inline bool lwque_full( TiLightQueue * que )
{
	return ((que->count != 0) && (que->count == que->capacity));
}

/* lwque_getbuf()
 * This function returns the memory address of specified item with index input. It helps
 * to manipulate the item directly.
 */
inline void * lwque_getbuf( TiLightQueue * que, uint8 idx )
{
	rtl_assert( idx < que->capacity );
	return (char*)que + sizeof(TiLightQueue) + (idx * que->itemsize);
	
	
}

/* lwque_front()
 * Returns the memory address of the front item in the queue. If the queue is empty, 
 * then NULL will be returned.
 */
inline void * lwque_front( TiLightQueue * que )
{
	void * item = (que->count > 0) ? lwque_getbuf(que,que->front) : NULL;
	
    
	return item;
}

/* lwque_rear()
 * Returns the memory address of the rear item in the queue. If the queue is empty, 
 * then NULL will be returned.
 */
inline void * lwque_rear( TiLightQueue * que )
{
	void * item = (que->count > 0) ? lwque_getbuf(que,que->rear) : NULL;
	return item;
}

/* lwque_pushback()
 * Push the specified item into the queue. The item will be duplicated in the queue
 * so the source item can be discarded or reused.
 */
inline bool lwque_pushback( TiLightQueue * que, void * item )
{
	bool ret;
	if (que->count == 0)
	{   
		que->rear = 0;
		que->front = 0;
		memmove( lwque_getbuf(que,0), item, que->itemsize );
		que->count++;
		ret = true;
	}
	else if (que->count < que->capacity)
	{
		que->rear = (que->rear == 0) ? (que->capacity - 1) : (que->rear-1);
		memmove( lwque_getbuf(que,que->rear), item, que->itemsize );
		que->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

inline bool lwque_pushfront( TiLightQueue * que, void * item )
{
	bool ret;

	if (que->count == 0)
	{
		que->rear = 0;
		que->front = 0;
		memmove( lwque_getbuf(que,0), item, que->itemsize );
		que->count++;
		ret = true;
	}
	else if (que->count < que->capacity)
	{
		que->front ++;
		que->front %= que->capacity;
		memmove( lwque_getbuf(que,que->front), item, que->itemsize );
		que->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

/* lwque_popfront()
 * Pop the front item out from the queue. It's usually used with lwque_front().
 */
inline bool lwque_popfront( TiLightQueue * que )
{
	bool ret;

	if (que->count == 0)
	{
		ret = false;
	}
	else{
		if (que->front == 0)
			que->front = que->capacity - 1;
		else
			que->front --;
		que->count --;
		ret = true;
	}

	return ret;
}

inline bool lwque_poprear( TiLightQueue * que )
{
	bool ret;

	if (que->count == 0)
	{
		ret = false;
	}
	else{
		que->rear ++;
		que->rear %= que->capacity;
		que->count --;
		ret = true;
	}

	return ret;
}
