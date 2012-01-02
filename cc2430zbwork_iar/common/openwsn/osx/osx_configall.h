#ifndef _OSX_CONFIGALL_7898_
#define _OSX_CONFIGALL_7898_
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

#include "../configall.h"


/* controls whether to use malloc() to allocate memory to hold the osx kernel */
#define CONFIG_OSX_DYNAMIC_MEMORY 
#undef CONFIG_OSX_DYNAMIC_MEMORY 

/* configure the maximum event count in the system event queue */
#define CONFIG_OSX_QUEUE_CAPACITY 8

/* configure how many objects can be placed in the dispatcher list 
 * 
 * @attention
 *	If you want to support more than 8 dispatchers, you MUST enlarge this macro 
 */
#define CONFIG_OSX_DISPATCHER_CAPACITY 8

/* enable debug agent */
#define CONFIG_DBA_ENABLE

#endif
