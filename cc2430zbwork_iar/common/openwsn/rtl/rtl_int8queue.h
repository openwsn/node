#ifndef _RTL_INT8QUEUE_H_9032_
#define _RTL_INT8QUEUE_H_9032_
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

/* TiInt8Queue Data Structure
 * this is a C language based queue data structure. it's actaully an ring queue.
 * different to TiDynaQueue, the TiInt8Queue has fixed number of items/elements.
 * It will not increase memory when it's full. It's mainly used in kernel developing
 * or embedded developing without dynamic memory allocator support.
 *
 * another advantage of TiInt8Queue is that it can be moved to another place
 * inside the address space and can still work.
 *
 * portability
 *	- ansi c language
 *  - linux, windows, and embedded platforms with c compiler only
 *
 * @state
 *	finished but not tested yet
 *
 * @author zhangwei in 200503
 * @modified by zhangwei in 200803
 *	- eliminate the call of malloc() and free().
 * @modified by zhangwei in 200905
 *	- change the name as TiInt8Queue. the original name is TiRingQueue
 *    the new predix "int8que_" means "light weight queue"
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

#define INT8QUEUE_HOPESIZE(capacity) (sizeof(TiInt8Queue) + (capacity))


#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8 front;
	uint8 rear;
	uint8 capacity;
	uint8 count;
}TiInt8Queue;

TiInt8Queue *   int8que_construct( void * buf, uint16 size );
void		    int8que_destroy( TiInt8Queue * que );
uint8           int8que_count( TiInt8Queue * que );
uint8           int8que_capacity( TiInt8Queue * que );
bool            int8que_empty( TiInt8Queue * que );
bool            int8que_full( TiInt8Queue * que );
int8 *          int8que_getbuf( TiInt8Queue * que, uint8 idx );
int8 *          int8que_front( TiInt8Queue * que );
int8 *          int8que_rear( TiInt8Queue * que );
bool            int8que_pushback( TiInt8Queue * que, int8 value );
bool            int8que_pushfront( TiInt8Queue * que, int8 value );
bool            int8que_popfront( TiInt8Queue * que );
bool            int8que_poprear( TiInt8Queue * que );

#ifdef __cplusplus
}
#endif

#endif
