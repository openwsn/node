#ifndef _HAL_EVENT_H_7876_
#define _HAL_EVENT_H_7876_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

/*******************************************************************************
 * @author zhangwei on 2012.08.02
 * - First version
 ******************************************************************************/


#include "hal_configall.h"
#include <stdint.h>
#include "../rtl/rtl_foundation.h"

/* System wide event identifier */

#define INIT_EVENT(id,objfrom,objto,handler) hal_initevent((id),(objfrom),(objto),(handler))

#define EVENT_RESET                     1
#define EVENT_RESTART                   2
#define EVENT_WAKEUP                    3
#define EVENT_SLEEP                     4
#define EVENT_TIMER_EXPIRED             5
#define EVENT_UART_DATA_ARRIVAL         6
#define EVENT_DATA_ARRIVAL              7
#define EVENT_DATA_COMPLETE             8
#define EVENT_REQUEST                   9
#define EVENT_REPLY                     10
#define EVENT_ACTION_DONE               11

extern TiEvent  g_hal_event;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an temperal event variable.
 */
TiEvent * hal_initevent( TiEventId id, void * objectfrom, void * objectto, TiFunEventHandler handler );

#ifdef __cplusplus
}
#endif


#endif /* #define _HAL_EVENT_H_7876_ */
