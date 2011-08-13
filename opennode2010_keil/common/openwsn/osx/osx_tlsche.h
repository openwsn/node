#ifndef _OSX_TLSCHE_H_4343_
#define _OSX_TLSCHE_H_4343_
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
#include "../hal/opennode2010/hal_rtc.h"
#include "../hal/opennode2010/hal_timer.h"
#include "osx_taskpool.h"
#include "osx_taskheap.h"

#define CONFIG_OSX_TIMER_INTERVAL 500


typedef struct{
    TiOsxTaskHeap taskheap;
    TiOsxTaskPool taskpool;
    TiOsxTimer2 * timer;
}TiOsxTimeLineScheduler;

TiOsxTimeLineScheduler * osx_tlsche_open( TiOsxTimeLineScheduler * sche, TiOsxTimer2 * timer );
void osx_tlsche_close( TiOsxTimeLineScheduler * sche );
int8 osx_tlsche_taskspawn( TiOsxTimeLineScheduler * sche, TiOsxTask taskfunction, 
    void * taskdata, int16 starttime, uint8 priority, uint8 option );
void osx_tlsche_evolve( TiOsxTimeLineScheduler * sche, void * e );
void osx_tlsche_execute( TiOsxTimeLineScheduler * sche );
void osx_tlsche_kill( TiOsxTimeLineScheduler * sche, uint8 id );
void osx_tlsche_restart( TiOsxTimeLineScheduler * sche, uint8 id, int16 starttime );
#define osx_tlsche_stepbackward(sche,slicecount) osx_tlsche_stepforward((sche),(slicecount))
void osx_tlsche_stepforward( TiOsxTimeLineScheduler * sche, uint16 slicecount );

/*
Future Scheduler:

evt是对event而言，sche是对任务而言
osx_evtchecker + eventhandler
osx_evtdispatcher + default event handler

task list至少要有两种：
- 给event dispatcher用的：类似于周期性任务队列，除非用户显示的调用detach，否则一直执行，与service机制配套
- 给timeline sche用的，调度完一次，任务即结束，如果再次启动，必须再次tlsche_taskspawn/kill/postphone/restart/activate
*/

#endif /* _OSX_TLSCHE_H_4343_ */
