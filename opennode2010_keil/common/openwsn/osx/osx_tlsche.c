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
#include <string.h>
#include "osx_foundation.h"
#include "../hal/hal_led.h"
#include "osx_tlsche.h"
#include "osx_taskheap.h"
#include "osx_taskpool.h"

void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx );//jiade
void _osx_taskheap_dump( TiOsxTaskHeap * heap );//jiade


TiOsxTimeLineScheduler * osx_tlsche_open( TiOsxTimeLineScheduler * sche, TiOsxTimer2 * timer )
{
    osx_taskpool_construct( (char *)(&sche->taskpool), sizeof(TiOsxTaskPool) );
    osx_taskheap_open( &(sche->taskheap), &(sche->taskpool) );
    sche->timer = timer; 
	return sche;
}

void osx_tlsche_close( TiOsxTimeLineScheduler * sche )
{
    // rtc_stop( sche->timer );
    osx_taskheap_close( &(sche->taskheap) );
    osx_taskpool_destroy( &(sche->taskpool) );
}

int8 osx_tlsche_taskspawn( TiOsxTimeLineScheduler * sche, TiOsxTask taskfunction, 
    void * taskdata, int16 starttime, uint8 priority, uint8 option )
{
    TiOsxTaskHeapItem item;
    memset( &item, 0x00, sizeof(TiOsxTaskHeapItem) );
    item.taskfunction = taskfunction;
    item.taskdata = taskdata;
    item.timeline = starttime;
    item.priority = 0;
    item.deadline = 0xFFFF;

    return osx_taskheap_insert( &(sche->taskheap), &item );
}

void osx_tlsche_evolve( TiOsxTimeLineScheduler * sche, void * e )
{    
    TiOsxTaskHeapItem * desc;
	

    do{
        desc = osx_taskheap_root( &(sche->taskheap) );
        if (desc == NULL)
            break;

        if (desc->timeline <= 0)
        {
            desc->taskfunction( desc->taskdata, NULL );
            osx_taskheap_deleteroot( &(sche->taskheap) );
        }
        else
            break;
    }while (true);

    //osx_tlsche_stepforward( sche, CONFIG_OSX_TIMER_INTERVAL );
}

void osx_tlsche_execute( TiOsxTimeLineScheduler * sche )
{
	//rtc_setinterval( sche->timer, 0, 2, 0x01); //定时周期为一秒 
	//rtc_start( sche->timer );

    rtc_setprscaler( sche->timer,32767);
    rtc_start( sche->timer);

    while (1)
    {
        osx_tlsche_evolve( sche, NULL );
	    /*
        set_sleep_mode(SLEEP_MODE_IDLE);
	    sleep_enable();
	    sleep_cpu();
	    sleep_disable();
        */
    }

    // rtc_stop( sche->rtc );
}

void osx_tlsche_kill( TiOsxTimeLineScheduler * sche, uint8 id )
{
    osx_taskheap_delete( &(sche->taskheap), osx_taskpool_gettaskdesc(&(sche->taskpool), id) );
}

void osx_tlsche_restart( TiOsxTimeLineScheduler * sche, uint8 id, int16 starttime )
{
    //osx_taskheap_reinsert
}

void osx_tlsche_stepforward( TiOsxTimeLineScheduler * sche, uint16 slicecount )
{
    uint8 i;
    TiOsxTaskHeap * heap = &(sche->taskheap);

    for (i=0; i<osx_taskheap_count(heap); i++)
    {
        osx_assert( heap->items[i] != NULL );
        heap->items[i]->timeline -= slicecount;
    }
}

void osx_rtc_listener(TiOsxTimeLineScheduler * sche,TiEvent * e )  //0705
{
	hal_enter_critical();
	osx_tlsche_stepforward( sche, 1 );
	hal_leave_critical();
}


