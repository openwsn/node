/******************************************************************************
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
 *****************************************************************************/

#ifndef _HAL_GLOBAL_H_6382_
#define _HAL_GLOBAL_H_6382_

//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables used in HAL layer
//
// 	This module is very important in the whole application. It does not only
// declares and initializes all the global objects, but also create the
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//
//----------------------------------------------------------------------------

#include "hal_foundation.h"
#include "hal_spi.h"
#include "hal_timer.h"
#include "hal_watchdog.h"
#include "hal_uart.h"
#include "hal_cc2420.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_vibration.h"

/* these 4 definitions should be moved from HAL to service or application layer
 * @attention
 * you should guarantee you have already created these objects before you using them!
 * such a error maybe: you try to output some information to "g_uart" before you initialize it.
 */

#ifdef CONFIG_TARGET_OPENNODE_10
#define g_spi g_spi0
#endif

#ifdef CONFIG_TARGET_OPENNODE_20
#define g_spi g_spi0
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
#define g_spi g_spi0
#endif

#ifdef CONFIG_TARGET_WLSMODEM_11
#define g_spi g_spi0
#endif

#define g_uart g_uart0
//#define g_debuguart g_uart1
#define g_debuguart g_uart0
#define g_timer (g_timer1)

extern TiUartAdapter *		g_uart0;
extern TiUartAdapter *		g_uart1;
extern TiWatchdogAdapter * 	g_watchdog;
extern TiSpiAdapter *		g_spi0;
extern TiSpiAdapter *		g_spi1;
extern TiCc2420Adapter *	g_cc2420;
extern TiTimerAdapter * 	g_timer0;
extern TiTimerAdapter * 	g_timer1;
extern TiTimerAdapter * 	g_timer2;
extern TiMcp6s26Adapter *   g_mcp6s26;
extern TiVibSensorAdapter * g_vibration;
extern TiAdConversion *     g_ad;


// construct all the necessary global object instances. The relationship between
// all these objects are also created.
//
// @return
//	0			success
//	negative	failed
//
int8 hal_global_construct( void );

// destroy all the global object instances and their relationships.
//
// @return
//	0			success
//	negative	failed
//
int8 hal_global_destroy( void );

#endif
