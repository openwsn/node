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
 * 
 * running on: 
 *	- Atmel Atmega MCU such as atmega128 MCU
 *
 * compiled by 
 *	- WinAVR 2008 (Portable WinAVR)
 *  - AVR Studio 4.x (with WinAVR 2009)
 *
 * @author zhangwei on 2009.05.23
 *	- greate revison based on old source code
 * @modified by openwsn on 2010.11.26
 *  - revision. 
 *
 ******************************************************************************/


#include "../hal/hal_foundation.h"
#include "osx_nano.h"  

#define NOS_TASK_BITMASK (CONFIG_NOS_MAX_TASKS-1)

typedef struct _nos_sched_entry_t {
	void (*tp)(void);
}nos_sched_entry_t;

volatile nos_sched_entry_t g_taskqueue[CONFIG_NOS_MAX_TASKS];
uint8_t g_nos_sched_full;
volatile uint8_t g_nos_sched_free;

static bool _nos_run_next_task(void);
static void _nos_wait(void);

#ifdef CONFIG_NOS_TEST
void _nos_listener( void * object, TiEvent * e);
#endif

inline void nos_atomic_end(nos_atomic_t state)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20) = state;
}

inline nos_atomic_t nos_atomic_start(void )
{
	nos_atomic_t result = * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20);
	__asm volatile ("cli");
	return result;
}

inline void nos_atomic_enable_interrupt(void)
{
	__asm volatile ("sei");
}

inline void _nos_wait(void)
{
   __asm volatile ("nop");
   __asm volatile ("nop");}

 inline void nos_sleep(void)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x35 + 0x20) |= 1 << 5;
	__asm volatile ("sleep");
}
   
inline void nos_init(void )
{
	g_nos_sched_free = 0;
	g_nos_sched_full = 0;
}

bool nos_post(void (*tp)(void))
{
	nos_atomic_t flag;
	uint8_t tmp;
	flag = nos_atomic_start();
	tmp = g_nos_sched_free;

    if (g_taskqueue[tmp].tp == (void *)NULL) 
	{
		g_nos_sched_free = (tmp + 1) & NOS_TASK_BITMASK;
		g_taskqueue[tmp].tp = tp;
		nos_atomic_end(flag);
		return TRUE;
    }
	else {
		nos_atomic_end(flag);
		return FALSE;
    }
}

inline bool _nos_run_next_task(void)
{
	nos_atomic_t flag;
	uint8_t old_full;
	void (*func)(void );

	flag = nos_atomic_start();
	old_full = g_nos_sched_full;
	func = g_taskqueue[old_full].tp;
	if (func == (void *)0) 
    {
		nos_atomic_end(flag);
		return 0;
    }

	g_taskqueue[old_full].tp = (void *)0;
	g_nos_sched_full = (old_full + 1) & NOS_TASK_BITMASK;
	nos_atomic_end(flag);
	func();
	return 1;
}

inline void nos_run_task(void)
{
	while (_nos_run_next_task()) {};
	nos_sleep();
	_nos_wait();
}

/* @attention
 * - the developer should provide the implementation of this function
 * developer should start the threads (by call os_post function)
 * according to the event information 
 * 
 * - this function should registered in hal layer by call hal_setlistener()
 * or else the hal layer doesn't know which where's the listener handler.
 * 
 * - the function name _nos_listener() isn't mandatory. you can name it with any
 * other names unless TiFunEventHandler type.
 */
#ifdef CONFIG_NOS_TEST
void _nos_listener( void * object, TiEvent * e)
{
	// todo: 根据event中的内容post一个thread起来
	// 通常，该listener应有开发者创建，本身并非nano os组成部分
	// os_post( os_task_t * task );
}
#endif

// @todo the _non_listener() and nos_test() should be upgarded in the future
#ifdef CONFIG_NOS_TEST
void nos_test()
{
	hal_init( _nos_listener, NULL );
	os_run_task();
}
#endif
