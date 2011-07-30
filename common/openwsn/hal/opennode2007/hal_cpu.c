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

#include "hal_foundation.h"
#include "hal_assert.h"
#include "hal_cpu.h"

/*******************************************************************************
 * @author zhangwei on 20060901
 * 
 * @history
 * @modified by zhangwei on 20060906
 *  - update compiling macros of hal_delay()
 *
 * @modified by zhangwei on 20070410
 * @modified by zhangwei on 20090513
 *	- revision: delay() functions
 ******************************************************************************/

//static uint8 IRQ_FLAG_STORE;

/*******************************************************************************
 * Runs an idle loop for [timeout] microseconds.
 * 		Note 1: The time spent processing interrupts will be added to the timeout!
 * 		Note 2: When porting to another MCU or compiler take care and count generated instructions!
 *
 * @warning
 * 	this is a obsolete function! better NOT to use this method for critical time
 * delay. pls use timer query technique to implment delay.
 *  
 * @attention
 * 	pay attention to the input parameter 
 * 	the time really delayed rely on the MCU and frequency.
 * 
 * 	the author recommended NOT to use function! the Timer and ActionScheduler 
 * 	object is more suitable for precise delays. you can start a timer and check
 * 	when it expired to wait for precise periods.
 * 
 * @param
 * 	uint16 timeout
 *  The timeout in microseconds ??? QQQ
 * 
 ******************************************************************************/

#include "hal_cpu.h"

#ifdef __GNUC__
#define _cpu_pause() NOP()

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
#define _cpu_pause() NOP()

#else
void _cpu_pause(void)
{
	static delaycounter=0;
	delaycounter ++;
}
#endif

void cpu_delay( uintx msec )
{
	uintx counter1 = CONFIG_CPU_CLOCK_MSEC_RATIO1;
	uintx counter2 = CONFIG_CPU_CLOCK_MSEC_RATIO2;

	do{
		do{
			do{
				_cpu_pause();
			}while (--counter2);
		}while (--counter1);
	} while (--msec);
}

critical_t cpu_enter_critical(void)
{
	critical_t cpu_state;
	/* cpu_state retrieve cpu stete */
	cpu_disable_interrupts();
}

void cpu_leave_critical( critical_t state )
{
	/* todo restore state*/
	cpu_enable_interrupts();
}


void cpu_sleep( void )
{
	hal_assert(false);
}

void cpu_reboot( void )
{
	hal_assert(false);
}

void cpu_sti( void )
{
	hal_assert(false);
}

void cpu_cli( void )
{
	hal_assert(false);
}


/************************ obsolete source code, will be removed soon *******************/

/* @param
 * 	delay		delay period by millisecond
 * 				1000 means delay for 1 second
 * @attention
 *  you should porting this function to adapt to new MCU and hardware platforms!
 * 
 * @state
 * 	developing, needs testing!
 */
/*
void hal_delay16( uint16 dly  )
{
	uint16 i;
   	for(; dly>0; dly--) 
		for(i=0; i<8000; i++)
			_hal_pause();	
}

void hal_delay32( uint32 dly  )
{
	uint32 i;
	for(; dly>0; dly--) 
    	for(i=0; i<8000; i++);
			_hal_pause();	
}
*/
/*
#ifdef __GNUC__
void hal_delay( uint16 timeout ) 
{
  	// usually the sequence uses exactly 8 clock cycle for each round 
  	do {
    	NOP();
    	NOP();
    	NOP();
    	NOP();
  	} while (--timeout);
}

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
void hal_delay( uint16 timeout ) 
  	do {
    	NOP();
  	} while (--timeout);
}

// for unknown compiles 
#else
void hal_delay( uint16 delay )
{
	uint32 count, n;
	
	count = delay * 10000;
	for (n=0; n<count; n++)
	{
		delay = 0;
		delay ++;
	}
}
#endif


// reboot the whole system
// TODO: hal_reboot
void hal_reboot()
{
	
}
*/
/*
// obsolete functions
void  halWait(uint32  dly)
{  
	uint32  i;

   for(; dly>0; dly--) 
      for(i=0; i<8000; i++);
}
*/