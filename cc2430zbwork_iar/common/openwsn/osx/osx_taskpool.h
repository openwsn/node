#ifndef _OSX_TASKPOOL_H_5678_
#define _OSX_TASKPOOL_H_5678_
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

#define CONFIG_OSX_TASKPOOL_CAPACITY 8

// #define OSX_TASKPOOL_HOPESIZE(capacity) (sizeof(TiOsxTaskPool)+sizeof(_TiOsxTaskPoolItem)*(capacity))

/**
 * This structure is used in osx internal to record the task related information.
 * 
 * - taskid: task id. this id keep unchanged when a task descriptor is allocated. 
 * - taskfunction: function pointer
 * - taskptr: an pointer to the structure variable of the current task
 * - timeline: when the task should be started to run
 * - deadline: deadline for the task (reserved for future)
 * - listprev: (reserved for future) 
 * - listnext: (reserved for future) 
 * - itemprev: the index of the previous item in the allocated list or freed list
 * - itemnext: the index of the next item in the allocated list or freed list
 */
struct _TiOsxTaskPoolItem;
struct _TiOsxTaskPoolItem{
    uint8       state;
    int8        taskid;
    TiOsxTask   taskfunction;
    void *      taskdata;
    int16       timeline;
    int8        priority;
    int16       deadline;
    int8        heapindex;
    // int8     listprev;
    // int8     listnext;
    // int8     itemprev;
    int8        itemnext;
};
typedef struct _TiOsxTaskPoolItem  TiOsxTaskPoolItem;

/**
 * Implements a task descriptor pool. This pool can support allocating and free a 
 * task descriptor structure in O(1) time.
 * 
 * - count: how many items has been allocated
 * - allocated: this is the head item of the allocated item list
 * - freed: this is the head item of the freed item list.
 */
typedef struct{
    //int8 capacity;
    int8 count;
    //int8 allocated;
    int8 emptylist;
    TiOsxTaskPoolItem items[CONFIG_OSX_TASKPOOL_CAPACITY];
}TiOsxTaskPool;

TiOsxTaskPool * osx_taskpool_construct( char * mem, uint16 memsize );
void osx_taskpool_destroy( TiOsxTaskPool * tpl );

int8 osx_taskpool_apply( TiOsxTaskPool * tpl );
void osx_taskpool_release( TiOsxTaskPool * tpl, int8 id );

uint8 osx_taskpool_capacity( TiOsxTaskPool * tpl );
uint8 osx_taskpool_count( TiOsxTaskPool * tpl );
void osx_taskpool_clear( TiOsxTaskPool * tpl );

TiOsxTaskPoolItem * osx_taskpool_gettaskdesc( TiOsxTaskPool * tpl, int8 id );
int8 osx_taskpool_gettaskid( TiOsxTaskPool * tpl, TiOsxTaskPoolItem * item );

bool osx_taskpool_full( TiOsxTaskPool * tpl );
bool osx_taskpool_empty( TiOsxTaskPool * tpl );

#endif
