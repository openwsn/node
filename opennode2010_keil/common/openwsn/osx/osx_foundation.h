#ifndef _OSX_FOUNDATION_4328_
#define _OSX_FOUNDATION_4328_
/*
 *******************************************************************************
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
 *******************************************************************************
 */ 

/******************************************************************************
 * osx_foundation
 *
 * @author zhangwei on 200610
 * @author zhangwei on 200904
 *	- revision
 * @author zhangwei on 200906
 *	- add osx timer
 * @modified by zhangwei on 2010.08.05
 *  - remove the including of osx_timer
 *  - add macro osx_assert()
 * 
 * @modified by openwsn on 2010.09.02
 *  - merge osx_foundation and osx_ticker.h. and osx_ticker.h will be removed from
 * this project.
 ******************************************************************************/

/* fundamental declarations for the kernel
 *
 * Q: How to port the kernel to a new platform
 * R:
 * step1: implement necessary hardware adapter objects
 * step2: update this module "os_foundation.h" to use new adapter objects.
 * then you should be able to compile the kernel successfully, because the kernel
 * is standard ANSI C.
 *
 * the kernel is based on hardware adapter objects and osx_foundation, so what
 * you need to do is porting this module only to the new platform.
 *
 *   osx_kernel, osx_simplesche, osx_rtsche, osx_fsche...
 *        |-------------|-------------|
 *         osx_timer, osx_queue, osx_dba
 *                      |
 *               osx_foundation.h
 *                      |
 *               osx_configall.h
 *
 * 
 * Q: How to understand the osx_foundation module? (2011.03)
 * R: This module funtions as an hardware adaptation layer at the bottom of the 
 * osx kernel. This means the osx kernel can be ported to a new platform only if 
 * the osx_foundation and related modules are ported successfully. 
 */

#include "osx_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_lightqueue.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_rtc.h"

#define osx_assert(x) hal_assert(x)

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * TiOsxTask and TiFunEventHandler shares the same data structure so that the event 
 * handler can used as an osx task or vice versa. 
 */
#define TiOsxTask TiFunEventHandler

/*******************************************************************************
 * Initialize the core hardware to support the osx kernel to run. 
 ******************************************************************************/
//void osx_init(); 

/*******************************************************************************
 * Osx Critical Operation Support
 * The following two function are used during task switching or concurrent access
 * on shared resources. 
 * 
 * @attention: If your compiler doesn't support "inline", you can replace the inline
 * with macro as the following:
 * 
 * #define osx_atomic_begin() hal_atomic_begin()
 * #define osx_atomic_end() hal_atomic_end()
 ******************************************************************************/

inline void osx_atomic_begin() 
{
	hal_atomic_begin();
}

inline void osx_atomic_end() 
{
	////hal_atomic_end();
}

/*******************************************************************************
 * Osx Timer Adapter
 ******************************************************************************/

#define TiOsxTimer TiRtcAdapter



/*******************************************************************************
 * Power Management
 ******************************************************************************/

inline void _osx_cpu_sleep() {cpu_sleep();}
inline void _osx_cpu_nop() {cpu_nop();}
//inline void _osx_cpu_wakeup() {cpu_nop();}
inline void _osx_cpu_wakeup() {;}

inline void _osx_target_on_sleep( void * svcptr, TiEvent * e )
{
	// todo
	// do something before sleep
	// such as saving data in the RAM
	//cpu_sleep();
}

inline void _osx_target_on_wakeup( void * svcptr, TiEvent * e )
{
/*	cpu_nop();
	cpu_nop();
	cpu_nop();*/
	// todo
	// do something after wakeup, such as restore RAM content
}

inline void _osx_on_target_reboot( void * svcptr, TiEvent * e )
{
	// reboot the whole application
	// todo: save RAM content and necessary configurations
	//target_reboot();
}

inline void _osx_on_target_startup( void * svcptr, TiEvent * e )
{
	// todo do something for startup
}

inline void _osx_on_target_shutdown( void * svcptr, TiEvent * e )
{
	// todo do something before really shutdown
}

//inline void _osx_sleep()
// catnap, shortsleep, restsleep
// deepsleep
// softdown/shutdown/startup  shutdown is called by software,it's actually a soft shutdown, can be wakeup by RTC
// shutdown is totally shutdown. no software  method can start it up.
//_osx_wakeup
// which one should I call in RTC handler? osx_wakeup or _osx_base_startup?
/*
inline void _osx_sleep() {};  
inline void _osx_deepsleep() {};  
inline void _osx_set_energy_saving_mode() {};

_osx_cpu_cpu_sleep
generally, we should append NOP after sleep instruction to avoid work flow problems in ISR.

osx_target_sleep
osx_target_wakeup
osx_target_startup
osx_target_shutdown
*/

#ifdef __cplusplus
}
#endif

#endif
