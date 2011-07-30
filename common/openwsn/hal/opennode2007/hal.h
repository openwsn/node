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
#ifndef _HAL_H_2998_
#define _HAL_H_2998_

/******************************************************************************
 * @author zhangwei on 2006-08-30
 * hal is the summary header file of all HAL header files to faciliate using
 * in other modules. attention that all the "*.h" file in the "hal" directory  
 * should NOT include "hal.h".
 * 
 * @attention
 * HAL中的对象及其方法要保持最大程度的简明．每一个接口变量和函数，能不增加就不增加．这样做
 * 是为了最大限度简化HAL在不同硬件平台上的移植．
 * 
 * @modified by zhangwei on 20061013
 * made great revision today. 
 * this is just the style of future hal.h. you should only include other hal header
 * files here! so other modules in the application can include "hal.h" only. 
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_led.h"
#include "hal_assert.h"
#include "hal_cpu.h"
#include "hal_target.h"
#include "hal_boot.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_cc2420.h"
#include "hal_flash.h"
#include "hal_eeprom.h"
#include "hal_key.h"
#include "hal_watchdog.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_strain.h"
#include "hal_sensor_vibration.h"
#include "hal_uniqueid.h"
#include "hal_sensor_switch.h"
#include "hal_openframe.h"
#include "hal_global.h"

#endif
