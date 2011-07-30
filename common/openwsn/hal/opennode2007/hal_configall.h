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

#ifndef _HAL_CONFIGALL_H
#define _HAL_CONFIGALL_H

// This is the config file of HAL. It should be as simple as possible.
// Only the configuration macros should be defined here.

#include "../configall.h"

#define CONFIG_UART0_ENABLE
#define CONFIG_UART1_ENABLE
#define CONFIG_CC2420_ENABLE
#define CONFIG_TIMER0_ENABLE
#define CONFIG_TIMER1_ENABLE
#define CONFIG_TIMER2_ENABLE
#define CONFIG_TIMER3_ENABLE
#define CONFIG_WATCHDOG_ENABLE
#define CONFIG_RTC_ENABLE
#define CONFIG_SPI0_ENABLE
#define CONFIG_SPI1_ENABLE
#define CONFIG_I2C0_ENABLE
#define CONFIG_I2C1_ENABLE
#define CONFIG_UNIQUEID_ENABLE


#define CONFIG_UART_READ_ENABLE 1
#define CONFIG_UART_WRITE_ENABLE 1

#endif
