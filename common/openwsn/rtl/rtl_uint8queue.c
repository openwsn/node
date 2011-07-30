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
#include "tk_configall.h"
#include "tk_charqueue.h"

#define KCHARQUEUE_MAX_CAPACITY_LIMIT (0xFFFF-1)
#define kcharqueue_t TiCharQueue

typedef struct{
  uint16 count;
  uint16 capacity;
  uint16 head;
  uint16 tail;
  char * datatable;
}TiCharQueue; 


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
TiCharQueue * cque_construct( void * buf, uint16 size, uint16 capacity )
{
	TiCharQueue * que = (TiCharQueue *)buf;
	uint16 n;	
	
	if (sizeof(TCharKQueue) + capacity > size)
		que = NULL;
		
	if (que != NULL)
	{
		memset( buf, 0x00, size );
		que->count = 0;
		que->capacity = capacity;
		que->head = 0; 
		que->tail = 0;
		que->datatable = (char *)buf + sizeof(TiCharQueue);
	}
	
	return que;
}

void cque_destroy( TiCharQueue * que )
{
	if (que != NULL)
	{
		que->count = 0;
		que->head = 0;
		que->tail = 0;
	}
}

#define cque_isempty(que) (que->count == 0)

#define cque_isfull(que) (que->count >= que->capacity)

boolean cque_push( TiCharQueue * que, char ch )
{
	boolean ret;
	
	if (cque_isempty())
	{
		que->datatable[0] = ch;
		que->head = 0;
		que->tail = 1;
		que->count = 1;
		ret = true;
	}
	else if (cque_isfull())
	{
		ret = false;
	}
	else{
		que->datatable[que->tail] = ch;
		que->tail ++;
		que->tail %= que->capacity;
		que->count ++;
		ret = false;
	}
	
	return ret;
}

boolean cque_pop( TiCharQueue * que, char * ch )
{
	boolean ret;
	
	if (cque_isempty())
	{
		ret = false;
	}
	else{
		*ch = que->datatable[que->head];
		que->head ++;
		que->head %= que->capacity;
		que->count --;
		ret = true;
	}
	
	return ret;
}

boolean cque_feed( TiCharQueue * que, char ch )
{
	boolean ret;
	
	if (cque_isfull())
	{
		ret = false;
	}
	else{
		que->head = (que->head > 0) ? (que->head-1) : (que->capacity-1);
		que->datatable[que->head] = ch;
		que->count ++;
		ret = true;
	}
	
	return true;
}









