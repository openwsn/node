#ifndef _OSX_TASKHEAP_H_4698_
#define _OSX_TASKHEAP_H_4698_
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


#include "osx_configall.h"
#include "osx_foundation.h"
#include "osx_taskpool.h"

/**
 * The future task list uses heap data structure to balance the performance of task 
 * searching, inserting and removing. 
 * 
 * TiOsxTaskHeap is also an independent component which can be used in applications. 
 * You can select it and enable the future-scheduling algorithm or unselect it to 
 * choose simple round robin scheduling.
 */

#define CONFIG_OSX_TASKHEAP_CAPACITY CONFIG_OSX_TASKPOOL_CAPACITY

#define TiOsxTaskHeapItem TiOsxTaskPoolItem

typedef struct{
    uint8 count;
    TiOsxTaskHeapItem * items[CONFIG_OSX_TASKHEAP_CAPACITY];
    TiOsxTaskPool * taskpool;
}TiOsxTaskHeap;


//TiOsxTaskHeap * osx_taskheap_construct( char * mem, uint16 memsize );
//void osx_taskheap_destroy( TiOsxTaskHeap * heap );
TiOsxTaskHeap * osx_taskheap_open( TiOsxTaskHeap * heap, TiOsxTaskPool * tpl );
void osx_taskheap_close( TiOsxTaskHeap * heap );


uint8 osx_taskheap_capacity( TiOsxTaskHeap * heap );
uint8 osx_taskheap_count( TiOsxTaskHeap * heap );
bool osx_taskheap_empty( TiOsxTaskHeap * heap );
bool osx_taskheap_full( TiOsxTaskHeap * heap );
void osx_taskheap_clear( TiOsxTaskHeap * heap );
TiOsxTaskHeapItem ** osx_taskheap_items( TiOsxTaskHeap * heap );

TiOsxTaskHeapItem * osx_taskheap_apply( TiOsxTaskHeap * heap );
void osx_taskheap_release( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );

TiOsxTaskHeapItem * osx_taskheap_root( TiOsxTaskHeap * heap );
void osx_taskheap_deleteroot( TiOsxTaskHeap * heap );
int8 osx_taskheap_delete( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
bool osx_taskheap_deleteat( TiOsxTaskHeap * heap, uint8 idx );
int8 osx_taskheap_insert( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
int8 osx_taskheap_search( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
void osx_taskheap_sort( TiOsxTaskHeap * heap );
void create_task( void *funptr, TiOsxTaskHeap * heap, int16 timeline);

#endif /* _OSX_TASKHEAP_H_4698_ */
