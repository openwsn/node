/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 
#include "hal_watchdog.h"

/* m_instance_count: the count of instances of TiWatchdogAdapter object.
 * This static variable controls only one TiWatchdogAdapter object exists in the system.
 * When you want to create the second or more TiWatchdogAdapter objects, the construct()
 * function will return NULL.
 */
static uint8 m_instance_count = 0;

TiWatchdogAdapter * watchdog_construct( char * buf, uint16 size, uint32 interval )
{
	TiWatchdogAdapter * wdg;
	
	if (m_instance_count == 0)
		wdg = (sizeof(TiWatchdogAdapter) <= size) ? (TiWatchdogAdapter*)buf : NULL;
	else
		wdg = NULL;
		
	if (wdg != NULL)
	{
		wdg->interval = interval;
		wdg->state = 1;
		m_instance_count ++;
	}
	
	return wdg;
}

void watchdog_destroy( TiWatchdogAdapter * wdg )
{
	watchdog_disable( wdg );
	if (m_instance_count > 0)
		m_instance_count ++;
}

void watchdog_configure( TiWatchdogAdapter * wdg, uint8 opt )
{
	return;
}

void watchdog_enable( TiWatchdogAdapter * wdg )
{
	// @TODO
}

void watchdog_disable( TiWatchdogAdapter * wdg )
{
	if (wdg->state == 1)
	{
		// @TODO
	}
}

void watchdog_feed( TiWatchdogAdapter * wdg )
{
	// @TODO
}
