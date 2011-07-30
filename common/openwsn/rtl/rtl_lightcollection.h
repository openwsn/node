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
#ifndef _RTL_vexTORX_H_1869_
#define _RTL_vexTORX_H_1869_

/*
attention
this module is upgraded as TiLightCollection now  lwt_collection
*/

/* rtl_vectorx
 * an extension of the standard vector (TiVector).
 *
 * @state
 *	finished. not fully tested
 *
 * @author zhangwei in 200904 
 *	- first created. 
 */ 

#define VECTORX_HOPESIZE(itemsize,capacity) (sizeof(TiVectorX) + itemsize + itemsize*capacity)
#define VECTORX_STATE_AVAILABLE 0
#define VECTORX_STATE_ACTIVE 1

#define vex_isavail(vex,idx) vex_isavailable(vex,idx)
#define vex_isused(vex,idx) (!vex_isavailable(vex,idx))

/* reference
 * The Collections Framework,
 * http://java.sun.com/j2se/1.4.2/docs/guide/collections/index.html
 */

#ifdef __cplusplus
extern "C" {
#endif

/* TiVectorX
 * memory map:
 *  ---------------------
 *  Header
 *  ---------------------
 *  Item State Table      (each state occupies only 1 byte)
 *  ---------------------
 *  Item Data Table
 *  ---------------------
 *
 * TiVectorX(vex) 与 TiVector(vec)不同，增加了对每个item的state的支持，可根据state
 * 判断该item是否已经放了数据。这一特性可方便更高级的hashmap, tree, list, heap等
 * 数据结构的实现
 */
typedef struct{
	uint16 size;
	uint16 itemlen;
	uint16 capacity;
	uint16 count;
	uint16 offset;
}TiVectorX;

TiVectorX *	vex_construct( void * buf, uint16 size, uint16 itemlen );
void        vex_destroy( TiVectorX * vex );

uint16       vex_capacity( TiVectorX * vex );
uint16       vex_count( TiVectorX * vex );
bool        vex_empty( TiVectorX * vex );
bool        vex_full( TiVectorX * vex );

bool        vex_apply( TiVectorX * vex, uint16 * idx );
void        vex_release( TiVectorX * vex, uint16 idx );
bool        vex_isavailable( TiVectorX * vex, uint16 idx );	

//uint8 *     vex_states( TiVectorX * vex );
uint8       vex_states( TiVectorX * vex, uint16 idx );
void *      vex_items( TiVectorX * vex );
void *      vex_getbuf( TiVectorX * vex, uint16 idx );
bool        vex_get( TiVectorX * vex, uint16 idx, void * buf );
bool        vex_set( TiVectorX * vex, uint16 idx, void * item );
bool        vex_put( TiVectorX * vex, void * item );
bool        vex_remove( TiVectorX * vex, uint16 idx );
void        vex_clear( TiVectorX * vex );
void        vex_swap( TiVectorX * vex, uint16 i, uint16 j );

#ifdef __cplusplus
}
#endif

#endif

