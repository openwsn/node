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
#ifndef _WATCHDOG_H_5628_
#define _WATCHDOG_H_5628_

/******************************************************************************
 * @author zhangwei on 2006-08-13
 * TiWatchdogAdapter 
 * 
 * This module contains a TiWatchdogAdapter object. This is an software abstraction of 
 * the hardware watchdog. 
 * 
 * @attention: There's usually only one watchdog hardware in the MCU. however, 
 * the software's construct procedure does not check this.
 *****************************************************************************/
 
#include "hal_foundation.h"

typedef struct{
  uint32 interval;
  uint8 state;
}TiWatchdogAdapter;

/* construct a global TiWatchdogAdapter object. You can reference this object by global
 * variable "g_watchdog". Note that the watchdog only start by calling enable()
 * function.
 *
 * This implementation only allows one watchdog object exists in the system.
 */
TiWatchdogAdapter * watchdog_construct( char * buf, uint16 size, uint32 interval );
void watchdog_destroy( TiWatchdogAdapter * wdg );
void watchdog_configure( TiWatchdogAdapter * wdg, uint8 opt );
void watchdog_enable( TiWatchdogAdapter * wdg );
void watchdog_disable( TiWatchdogAdapter * wdg );
void watchdog_feed( TiWatchdogAdapter * wdg );

#endif