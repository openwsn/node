/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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
 
#ifndef _HAL_INTERRUPT_H_4788_
#define _HAL_INTERRUPT_H_4788_

/******************************************************************************
 * hal_interrupt
 * Interrupt Hardware Abstraction. This module is used to control the global 
 * interrupt flag in the CPU only.
 *
 * you SHOULD always perfer to use hal_enter_atomic() instead of hal_enable_interrupts()
 * and hal_leave_atomic() instead of hal_disable_interrupts(). the reason is that 
 * hal_leave_atomic() can leave the global interrupt flag unchanged, while the 
 * disable()/enable() pair will leave the flag enabled!!! this is sometimes unwanted!
 *
 * Q: what's the difference between hal_enter_atomic()/hal_leave_atomic() and 
 * hal_enable_interrupts(), hal_disable_interrupts()?
 * R: 
 * hal_enable_interrupts()
 * hal_disable_interrupts()
 * direct control of the global interrupt flag inside CPU. this interrupt flag 
 * is usually a bit in CPU status register. If is was disabled, then CPU cann't
 * response to any external interrupts. These two functions are always effective 
 * no matter there's an RTOS or not.
 * 
 * hal_enter_atomic()
 * hal_leave_atomic()
 * atomic is an source code block that must be executed as a whole without any 
 * interruption. It's quite similar to the "critical section" supported by OS. 
 * however, the implementation of critical section is much more complex than a 
 * simple atomic block, because the critical section is usually related to the 
 * scheduling mechanism of OS. It may need to switch the context of the thread/process
 * while hal_enter_atomic()/hal_leave_atomic() doesn't do so. These two functions
 * are also supported no matter there's a RTOS or not.
 *
 * @attention
 * - when the system started, it should enable_interrupts() before using the following 
 * two atomic functions.
 * - this module should be OS independent. namely, these functions are existed 
 * no matter there's an RTOS or not.
 *
 * @author zhangwei on 2006-07-20
 *	first version.
 * @modified by zhangwei on 2007-07-21
 *  add two functions hal_enter_critical()/hal_leave_critical()
 * @modified by zhangwei on 2008-12-18
 *  remove two functions hal_enter_critical()/hal_leave_critical()
 *  they should be part of the OS layer. maybe os_enter_critical() in the future.
 *  add two functions hal_enter_atomic()/hal_leave_atomic()
 * @modified by zhangwei on 200905xx
 *	- revision
 *****************************************************************************/ 

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"

#ifdef CONFIG_MCU_ARM7
#include "arch_target.h"
#endif

#ifdef CONFIG_OS_UCOSII
#include "../ucos-ii/os_cpu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Common Interface Functions
 *****************************************************************************/ 



/******************************************************************************
 * ARM MCU Specfic Functions
 *****************************************************************************/ 

#ifdef CONFIG_MCU_ARM7

/* todo
 * attention
 * for ARM7 MCU, you should also disable FIQ though FIQ isn't used in the system 
 */
void hal_enable_interrupts( void );
void hal_disable_interrupts( void );


extern uint8 g_atomic_level;

/* when the whole system startup, the global interrupt flag should be disabled 
 * before using the following two functions. and the global variable g_atomic_level
 * should be reset to 0. So the global interrupt flag must be disabled when 
 * g_atomic_level is larger than 1. that's why you needn't to call disable interrupts 
 * when g_atomic_level is larger than 1.
 */
#define hal_enter_atomic() {g_atomic_level++; if (g_atomic_level==1) hal_disable_interrupts();}
#define hal_leave_atomic() {g_atomic_level--; if (g_atomic_level==0) hal_enable_interrupts();}

/* removed function since 20081218
 */
void hal_enter_critical( void );
void hal_leave_critical( void );

#endif /* CONFIG_MCU_ARM */

/******************************************************************************
 * Atmega 128 MCU Specfic Functions
 *****************************************************************************/ 

#ifdef CONFIG_MCU_ATMEGA

#define hal_disable_interrupts() cpu_disable_interrupts()
#define hal_enable_interrupts() cpu_enable_interrupts()

#define _hal_atomic_begin() _cpu_enter_critical()
#define _hal_atomic_end(state) _cpu_leave_critical(state)

#define hal_atomic_begin() _cpu_enter_critical()
#define hal_atomic_end() _cpu_leave_critical()


#endif /* CONFIG_MCU_ATMEGA */




#ifdef __cplusplus
}
#endif

#endif
