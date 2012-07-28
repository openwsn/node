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

#define CONFIG_OSX_FIFOSCHE_CAPACITY

#define TiOsxFifoSchedulerItem TiOsxTaskPoolItem

typedef struct{
    uint8 count;
    TiOsxFifoSchedulerItem * items[CONFIG_OSX_FIFOSCHE_CAPACITY];
    TiOsxTaskPool * taskpool;
}TiOsxFifoScheduler;

TiOsxFifoScheduler * osx_fifosche_construct( char * mem, uint16 memsize )
{

}

void osx_fifosche_destroy( TiOsxFifoScheduler * sche )
{

}

TiOsxFifoScheduler * osx_fifosche_open( TiOsxFifoScheduler * heap, TiOsxTaskPool * tpl );
{
}

void osx_fifosche_close( TiOsxFifoScheduler * heap );
{
}

int8 osx_fifosche_taskspawn( TiOsxFifoScheduler * sche, void * taskfunction, void * taskdata, int16 starttime, uint8 option )
{

}


/**
 * Check the task queue and execute them. After exection, the task will be removed
 * from the internal task queue and return to the osx task pool.
 */
void osx_fifosche_evolve( TiOsxFifoScheduler * sche, void * e )
{

}

void osx_fifosche_execute( TiOsxFifoScheduler * sche )
{

}

void osx_fifosche_kill( TiOsxFifoScheduler * sche, uint8 id )
{

}

void osx_fifosche_restart( TiOsxFifoScheduler * sche, uint8 id, int16 starttime )
{
}




uint8 osx_fifosche_capacity( TiOsxFifoScheduler * heap );
uint8 osx_fifosche_count( TiOsxFifoScheduler * heap );
bool osx_fifosche_empty( TiOsxFifoScheduler * heap );
bool osx_fifosche_full( TiOsxFifoScheduler * heap );
void osx_fifosche_clear( TiOsxFifoScheduler * heap );
TiOsxFifoSchedulerItem ** osx_fifosche_items( TiOsxFifoScheduler * heap );

