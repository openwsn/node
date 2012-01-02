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
#ifndef _RTL_VECTOR_H_1868_
#define _RTL_VECTOR_H_1868_

/* TiVector
 * @author zhangwei in 200411
 * @modified by zhangwei in 200803
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

/*
 * TiVector is part of the data structure library (C::Objects).
 * vector data type provides array like operations with enhanced features
 * such as append(), insert() and remove().
 *
 * The data structure library includes:
 * - allocators:
 *		TiVectorX(vex): add state(EMPTY or not) support to TiVector. TiVector
 *			and TiVectorX are usually used for fast access because they can
 *			locate an item by index directly. fast access but slow in apply and release.
 *		TiArrayList(arraylist): an linked list implemented based on the array. fast
 *			in apply and release a item. fast access unless you know the index.
 * - hash related structures based on TiVectorX
 *		TiHashTable:
 *			nhash: using uint16 as key. such as object id. id is usually uint16 type.
 *			mhash: using multibyte as key such as {buffer, len} or a string.
 *		TiHashMap
 *		TiHashSet
 * - linked array based
 *		TiTree, TiBinaryTree, TiAvlTree
 *		TiHeap, TiPriortyQueue
 *		TiList
 *		TiGraph/TiNetwork
 * - data structures doesn't use allocator
 *		TiVector: = array + some methods
 *		TiStack: based on TiVector
 *		TiQueue: based on TiVector
 */

#define VECTOR_HOPESIZE(itemsize, capacity) (sizeof(TiVector) + itemsize*capacity)

typedef struct{
	uint16 size;
	uint16 itemlen;
	uint16 capacity;
	uint16 count;
}TiVector;

TiVector *	vec_construct( void * buf, uint16 size, uint16 itemlen );
void        vec_destroy( TiVector * vec );

intx        vec_capacity( TiVector * vec );
intx        vec_count( TiVector * vec );
intx        vec_empty( TiVector * vec );
intx        vec_full( TiVector * vec );

void *      vec_items( TiVector * vec );
void *      vec_getbuf( TiVector * vec, uint16 idx );
bool        vec_get( TiVector * vec, uint16 idx, void * buf );
bool        vec_set( TiVector * vec, uint16 idx, void * item );
bool        vec_put( TiVector * vec, void * item );
bool        vec_append( TiVector * vec, void * item );
bool        vec_insert( TiVector * vec, uint16 index, void * item );
bool        vec_remove( TiVector * vec, uint16 index );
void        vec_clear( TiVector * vec );
bool        vec_pushfront( TiVector * vec, void * item );
bool        vec_pushback( TiVector * vec, void * item );
bool        vec_popfront( TiVector * vec );
bool        vec_poptail( TiVector * vec );
void        vec_swap( TiVector * vec, uint16 i, uint16 j );

#endif
