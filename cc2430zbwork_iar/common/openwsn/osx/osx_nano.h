#ifndef _OSX_NANO_H_9062_
#define _OSX_NANO_H_9062_
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

/*******************************************************************************
 * nano-os (nos)
 * this module is a nano/tiny/light embedded operating system core. 
 * nos (nanos) is an ultralight none-preemptive operating system kernel running
 * on atmega MCU only now. it's much more like a classical operating system.
 * generally, suggest using "osx kernel" instead of "nos" in the openwsn project. 
 * 
 * 
 * running on: 
 *	- Atmel Atmega MCU such as atmega128 MCU
 *
 * compiled by 
 *	- WinAVR 2008 (Portable WinAVR)
 *  - AVR Studio 4.x (with WinAVR 2009)
 *  - also compiled with the newest version of winavr and avrstudio in 2010 successfully.
 * 
 * attention
 *	this is super light os kernel which can run on small memory constraint MCU.
 *  it's not intended for complex task schedulings. and it may conflict with other
 *  embedded os kernels. generally, you should use only one os kernel in one 
 *  application.
 *
 * @state
 *	compile passed. released.
 *
 * @history
 * @author zhangwei(Dept of Control, TongJi University, China) on 2009.05.23
 *	- greate revison based on old source code
 * @author zhangwei on 20090523
 *	- greate revison based on old source code
 * @modified by openwsn on 2010.11.26
 *  - revision. 
 *
 ******************************************************************************/

#include "../hal/hal_configall.h"
#include "../hal/hal_foundation.h"

/* @todo
 * - build the nano fully on top of hal layer. so it will be more portable
 * - extend the support from 8bit MCU to 16b 
 * - @todo the _non_listener() and nos_test() should be upgarded in the future
 */

/*******************************************************************************
 * Q: how to use nano-os (nos)
 * A: nano os is a none preemptve os kernel. it's based on hardware abstraction 
 * layer (hal). so: 
 *	- porting hal layer first before porting the nano os kernel. 
 *  - in your application, you should call hal_nosinit(os_post). 
 *  - call nos_runtask() in an infinite while loop.
 *
 * example:
 *		int main(void)
 *		{
 *			target_init();
 *			hal_init();
 *			hal_nosinit(os_post)
 *			......
 *			while (1) {nos_runtask();}
 *		}
 * 
 * Q: how to create an task in interrupt service routines?
 * A: interrupt service routine usually belongs to hal layer. so it should NOT
 *	start a task by call os_post() function directly because os kernel is running
 *  on top of the hal layer. 
 *    an ordinary application architecture is as the following:
 *
 *		[                  Application                   ]
 *      [ Network Communication and Other Local Services ]
 *		[             Nano OS Kernel (nos)               ] 
 *      [        Hardware Abstraction Layer (hal)        ]
 *  
 *
 ******************************************************************************/
	
#define CONFIG_NOS_MAX_TASKS 8
#undef  CONFIG_NOS_TEST

/* assume: the CPU's status register is 8 bits only */

typedef void (* nos_task_t)(void);
typedef uint8 nos_atomic_t;

#ifdef __cplusplus
extern "C" {
#endif

/* os_atomic_start()
 * start an atomic processing
 * 
 * os_atomic_end()
 * end an atomic processing. this function must be used with os_atomic_start()
 * function and these two functions should be used inside one function only. 
 * there's no guarantee on the system behavior if the function pair appears in 
 * two functions. 
 */
nos_atomic_t nos_atomic_start(void);
void nos_atomic_end(nos_atomic_t state);
void nos_atomic_enable_interrupt(void);

/* os_init
 * this function must be called before you call os_post() function. 
 *
 * os_sleep
 * make the system into sleep mode. this function is called when there's no tasks
 * running. 
 *
 * os_post
 * create a task 
 */
void nos_init(void);
void nos_sleep(void);
bool nos_post(void (*tp)(void));
void nos_run_task(void);

#ifdef CONFIG_NOS_TEST
void nos_test();
#endif

#ifdef __cplusplus
}
#endif

#endif

