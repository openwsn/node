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
#ifndef _SVC_SCHEDULER_321G_
#define _SVC_SCHEDULER_321G_

/*****************************************************************************
 * Scheduler service
 * 
 * scheduler is an very important service in the system. when you want to do 
 * something immediatly, you can call their function directly. but when you want 
 * to do something in the future or do it periodically such as a sampling task, 
 * you can tell scheduler to do so. furthermore, the scheduler also decides the 
 * best chance to sleep and wakeup in order to save energy.
 *
 * the scheduler can be droven by the periodical timer interrupt, or by a infinit
 * loop call to the evolve() function.
 * 
 * different to the timer object, the scheduler adopts milli-seconds as its basic
 * time unit which is CPU and hardware-independent. 
 *
 * In most cases, one scheduler service is enough in the system. and it will be
 * the most efficient. but this module is still designed to support multi-schedulers.
 * and the evolve() function in one scheduler can be the task of another scheduler.
 *
 * @author openwsn on 20061010
 * @modified by zhangwei on 20081218
 *	revise the interface.
 *
 ****************************************************************************/ 

#include "svc_foundation.h"

typedef struct
{
}TiScheduler;

void sche_construct();
void sche_destroy();
void sche_taskspawn( int taskid, void * task, void * owner, int period, int opt );
void sche_taskdelay( int taskid, int delay );
void sche_taskdelete( int taskid );
void sche_run();
void sche_evolve();
void sche_timer();

#endif
