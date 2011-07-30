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

#undef RINGQUEUE_TEST

#include "rtl_configall.h"
#include <string.h>

#ifdef RINGQUEUE_TEST
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef CONFIG_LINUX
#include <alloc.h>
#endif

#include "rtl_foundation.h"
#include "rtl_ringqueue.h"

TiRingQueue * rque_construct( void * buf, uintx size, uintx itemsize )
{
	TiRingQueue * rque;
	rque = (TiRingQueue *)buf;
	rque->front = 0;
	rque->rear = 0;
	rque->capacity = (size - sizeof(TiRingQueue))/itemsize;
	rque->count = 0;
	rque->itemsize = itemsize;
	return rque;
}

void rque_destroy( TiRingQueue * rque )
{
	return;
}

uintx rque_count( TiRingQueue * rque )
{
	return rque->count;
}

uintx rque_capacity( TiRingQueue * rque )
{
	return rque->capacity;
}

bool rque_empty( TiRingQueue * rque )
{
	return (rque->count == 0);
}

bool rque_full( TiRingQueue * rque )
{
	return ((rque->count != 0) && (rque->count == rque->capacity));
}

void * rque_getbuf( TiRingQueue * rque, uintx idx )
{
	rtl_assert( idx < rque->capacity );
	return (char*)rque + sizeof(TiRingQueue) + (idx * rque->itemsize);
}

void * rque_front( TiRingQueue * rque )
{
	void * item = (rque->count > 0) ? rque_getbuf(rque,rque->front) : NULL;
	return item;
}

void * rque_rear( TiRingQueue * rque )
{
	void * item = (rque->count > 0) ? rque_getbuf(rque,rque->rear) : NULL;
	return item;
}

bool rque_pushback( TiRingQueue * rque, void * item )
{
	bool ret;

	if (rque->count == 0)
	{
		rque->rear = 0;
		rque->front = 0;
		memmove( rque_getbuf(rque,0), item, rque->itemsize );
		rque->count++;
		ret = true;
	}
	else if (rque->count < rque->capacity)
	{
		rque->rear = (rque->rear == 0) ? (rque->capacity - 1) : (rque->rear-1);
		memmove( rque_getbuf(rque,rque->rear), item, rque->itemsize );
		rque->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

bool rque_pushfront( TiRingQueue * rque, void * item )
{
	bool ret;

	if (rque->count == 0)
	{
		rque->rear = 0;
		rque->front = 0;
		memmove( rque_getbuf(rque,0), item, rque->itemsize );
		rque->count++;
		ret = true;
	}
	else if (rque->count < rque->capacity)
	{
		rque->front ++;
		rque->front %= rque->capacity;
		memmove( rque_getbuf(rque,rque->front), item, rque->itemsize );
		rque->count ++;
		ret = true;
	}
	else
		ret = false;

	return ret;
}

bool rque_popfront( TiRingQueue * rque )
{
	bool ret;

	if (rque->count == 0)
	{
		ret = false;
	}
	else{
		if (rque->front == 0)
			rque->front = rque->capacity - 1;
		else
			rque->front --;
		rque->count --;
		ret = true;
	}

	return ret;
}

bool rque_poprear( TiRingQueue * rque )
{
	bool ret;

	if (rque->count == 0)
	{
		ret = false;
	}
	else{
		rque->rear ++;
		rque->rear %= rque->capacity;
		rque->count --;
		ret = true;
	}

	return ret;
}

#ifdef RINGQUEUE_TEST
void rque_test()
{

    SqQueue* q;
    char s[MaxSize],e;
    int i;
    printf("请输入队列中元素：");
    while(scanf("%s",&s))
    {
        InitQueue(q);
        CreateQueue(q,s,strlen(s));
        if(!QueueEmpty(q))
            printf("队列非空\n");
        else printf("队列是空的\n");
        for(i=1;i<=3;i++)
            EnQueue(q,'a'+i-1);
        DeQueue(q,e);printf("出队的元素：%c\n",e);
        printf("队列中的元素个数：%d\n",QueueLength(q));
        for(i=1;i<=3;i++)
            EnQueue(q,'d'+i-1);
        printf("队列中的元素个数：%d\n",QueueLength(q));
        printf("出队序列：");
        while(DeQueue(q,e))
            printf("%c",e);
        printf("\n");
        ClearQueue(q);
        printf("\n请输入队列中元素：");
    }
    system("pause");
    return 0;
}
#endif
