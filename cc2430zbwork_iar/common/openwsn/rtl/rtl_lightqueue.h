#ifndef _RTL_LIGHTQUEUE_H_9032_
#define _RTL_LIGHTQUEUE_H_9032_
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

/* TiLightQueue Data Structure
 * this is a C language based queue data structure. it's actaully an ring queue.
 * different to TiDynaQueue, the TiLightQueue has fixed number of items/elements.
 * It will not increase memory when it's full. It's mainly used in kernel developing
 * or embedded developing without dynamic memory allocator support.
 *
 * another advantage of TiLightQueue is that it can be moved to another place
 * inside the address space and can still work.
 *
 * portability
 *	- ansi c language
 *  - linux, windows, and embedded platforms with c compiler only
 *
 * @state
 *	most are tested. released.
 *
 * @author zhangwei in 200503
 * @modified by zhangwei in 200803
 *	- eliminate the call of malloc() and free().
 * @modified by zhangwei in 200905
 *	- change the name as TiLightQueue. the original name is TiRingQueue
 *    the new predix "lwque_" means "light weight queue"
 * @modified by zhangwei in 2011.08.03
 *  - change the variable "itemsize" to "datasize"
 * 	- add lwque_datasize(), lwque_setbuf(), lwque_applyfront(), lwque_applyback(),
 *		lwque_readdata(), lwque_writedata()
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

#define LIGHTQUEUE_HOPESIZE(datasize,capacity) (sizeof(TiLightQueue) + (datasize)*(capacity))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8 front;
	uint8 rear;
	uint8 capacity;
	uint8 count;
	uint16 datasize;
}TiLightQueue;

TiLightQueue *  lwque_construct( void * buf, uint16 size, uint16 datasize );
void		    lwque_destroy( TiLightQueue * que );
uint8           lwque_count( TiLightQueue * que );
uint8           lwque_capacity( TiLightQueue * que );
bool            lwque_empty( TiLightQueue * que );
bool            lwque_full( TiLightQueue * que );
uint16 			lwque_datasize( TiLightQueue * que );
void *          lwque_getbuf( TiLightQueue * que, uint8 idx );

#define lwque_setbuf(que,idx,data,len) lwque_writedata(que,idx,data,len)

uint16 			lwque_readdata( TiLightQueue * que, uint8 idx, void * buf, uint16 size );
uint16 			lwque_writedata( TiLightQueue * que, uint8 idx, void * data, uint16 len );
void *          lwque_front( TiLightQueue * que );
void *          lwque_rear( TiLightQueue * que );
bool            lwque_pushback( TiLightQueue * que, void * item );
bool            lwque_pushfront( TiLightQueue * que, void * item );
bool            lwque_popfront( TiLightQueue * que );
bool            lwque_poprear( TiLightQueue * que );
bool 			lwque_applyback( TiLightQueue * que, uint8 * pidx );
bool 			lwque_applyfront( TiLightQueue * que, uint8 * pidx );

/* bool            lwque_extend( TiLightQueue * que, uint16 newsize ); */


#ifdef __cplusplus
}
#endif

#endif
