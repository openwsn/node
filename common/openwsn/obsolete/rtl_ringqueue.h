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
#ifndef _RTL_RINGQUEUE_H_9032_
#define _RTL_RINGQUEUE_H_9032_

/* RingQueue Data Structure
 * This ring queue has fixed number of items/elements. It will not increase memory
 * when it's full. It's mainly used in kernel developing or embedded developing
 * without dynamic memory allocator support
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
 */

#include "rtl_configall.h"

#define RINGQUEUE_HOPESIZE(itemsize, capacity) (sizeof(TiRingQueue) + (itemsize)*(capacity))

typedef struct{
	uintx front;
	uintx rear;
	uintx capacity;
	uintx count;
	uintx itemsize;
}TiRingQueue;

TiRingQueue *   rque_construct( void * buf, uintx size, uintx itemsize );
void		    rque_destroy( TiRingQueue * rque );
uintx           rque_count( TiRingQueue * rque );
uintx           rque_capacity( TiRingQueue * rque );
bool            rque_empty( TiRingQueue * rque );
bool            rque_full( TiRingQueue * rque );
void *          rque_getbuf( TiRingQueue * rque, uintx idx );
void *          rque_front( TiRingQueue * rque );
void *          rque_rear( TiRingQueue * rque );
bool            rque_pushback( TiRingQueue * rque, void * item );
bool            rque_pushfront( TiRingQueue * rque, void * item );
bool            rque_popfront( TiRingQueue * rque );
bool            rque_poprear( TiRingQueue * rque );

#endif
