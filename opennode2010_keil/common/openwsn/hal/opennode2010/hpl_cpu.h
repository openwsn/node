#ifndef _HPL_CPU_H_1675_ 
#define _HPL_CPU_H_1675_ 
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

#include "../hal_configall.h"
#include "../hal_foundation.h"

/* @modified by zhangwei on 2010.05.15
 *  - in order to support multiple hardware platforms, i split the hal_cpu.h into 
 *    serveral indepent files. each hardware platform now has its own "hpl_cpu.h" 
 *    and we include correct "hpl_cpu.h" in "hal_cpu.h" according to macro configurations.
 */

#ifndef CONFIG_TARGET_GAINZ
#error "this file should be compile with GAINZ target platform only. you should enable CONFIG_TARGET_GAINZ in configall.h to activate this platform."
#endif

#define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
#define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
#define CONFIG_CPU_CLOCK_MSEC_RATIO (CONFIG_CPU_CLOCK_MSEC_RATIO1*CONFIG_CPU_CLOCK_MSEC_RATIO2)

#ifdef __cplusplus
extern "C" {
#endif

/* attention
 * this is the atmega mcu version 
 *
 * @modified by zhangwei on 20090714
 *  - correct the error in hal_delay and cpu_delay. attention the input parameter 
 * should be int16 instead of uint16 or intx(which maybe int8 for 8bit MCUs). 
 */

/*******************************************************************************
 * Interrupt Management
 ******************************************************************************/
	
#define hal_disable_interrupts() cpu_disable_interrupts()
#define hal_enable_interrupts() cpu_enable_interrupts()

#define cpu_disable_interrupts() __asm volatile ("cli")
#define cpu_enable_interrupts() __asm volatile ("sei")

/*******************************************************************************
 * Atomic Functions
 * _cpu_atomic_begin / _cpu_atomic_end
 * hal_atomic_begin / hal_atomic_end
 * 
 * suggest using these function pair inside one function only. 
 ******************************************************************************/

/* @attention
 * @warning
 * the following is adapte to atmega128 8 bit mcu only
 * you should re-typedef cpu_atomic_t if your mcu is a 8bit one.
 */
inline cpu_atomic_t _cpu_atomic_begin(void)
{
	cpu_atomic_t result = * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20);
	__asm volatile ("cli");
	return result;
}

// todo
inline void _cpu_atomic_end(cpu_atomic_t state)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20) = state;
}

/* @attention
 * @warning
 * the following implementation of hal_atomic_begin() and hal_atomic_end() maybe 
 * unsafe theoretically. however, it maybe the most easy way to implement atomic 
 * mechanism for most simple microcontrollers.
 * 
 * maximum allowed re-enter depth is: max(uint8) which is 255.
 */

/* before you call this this function, assume your global interrupt is enabled */
inline void hal_atomic_begin( void )
{
	if (g_atomic_level == 0)
	{
		// g_atomic_flags = _cpu_atomic_begin();
		cpu_disable_interrupts();
	}
	g_atomic_level ++;
}

inline void hal_atomic_end( void )
{
	g_atomic_level --;
	if (g_atomic_level == 0)
	{
		cpu_enable_interrupts();
	}
}

#ifdef CONFIG_TARGET_GAINZ
// #define hal_atomic_begin() cpu_push(_cpu_atomic_begin())
// #define hal_atomic_end() _cpu_atomic_end(cpu_pop())
#endif

/*******************************************************************************
 * Delay Functions:
 *
 * _cpu_pause()
 * not recommended to use directly.
 * make the program pause for a short time. we don't recomment you use this function
 * directly because the pause duration is determined by CPU and hardware. it's different
 * on different hardware platform. for current implmentation on ARM7 processor,
 * it's about 2 cycle @todo really ???
 *
 * cpu_delay( msec )
 * recommend to use comparing to _hal_pause(). the "delay" parameter is based on 
 * millseconds. for example, if you want to delay 10 millisecond, you can simply call:
 *     hal_delay( 10 );
 *
 * @attention:
 * before using this function, you should configure the following two macros to
 * adapter to you own hardware platform:
 *
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
 * 
 * the product of these two macros defines how many clock cycle equals to 1 milli-second.
 ******************************************************************************/

/* attention
 * cpu_delay250ns() and cpu_delay() are hardware dependent. the current implementation
 * only adapts to the ICT GAINZ platform which uses ATMEGAL128 MCU and 8MHz oscilator.
 * 
 * if you use other hardware platform, you should port these two functions.
 */

#define hal_delay250ns() cpu_delay250ns()
#define hal_delayus(usec) cpu_delayus(usec)
#define hal_delay(msec) cpu_delay(msec)

/* this function maybe obsolete in the future. currently, it only used in wireless 
 * protocol implementation for very short delays
 */
inline void cpu_delay250ns() 
{
	asm volatile  ("nop" ::);
	asm volatile  ("nop" ::);
}

/* hardware dependent function.
 * on GAINZ platform, the main clock frequency is 8MHz. so 8 nop instruction
 * occupies approximately 1 u-second (usec)
 * 
 * @attention
 * the input parameter value should not large than the maximum value of int16 type
 */
inline void cpu_delayus( uint16 usec )
{
	while (usec > 1) 
	{
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		asm volatile  ("nop" ::);
		usec --;
	}
}

/* attention: 
 * the input msec value should not exceed the maximum value of uint16 type 
 */
inline void cpu_delay(uint16 msec) 
{
    while (msec > 1)
    {
        cpu_delayus( 1000 );
        msec --;
    }
}

/*
// todo
// cpu_pause() will be removed soon
#ifdef __GNUC__
#define _cpu_pause() NOP()

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
#define _cpu_pause() NOP()

#else
void _cpu_pause( void );
#endif
*/

/* cpu sleep and wakeup
 * the cpu is usually wakeup by external hardware interrupts such as 
 * - UART interrupts
 * - cc2420 FIFOP interrupts
 * - RTC timer expire interrupts
 */
inline void cpu_sleep(void)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x35 + 0x20) |= 1 << 5;
	__asm volatile ("sleep");
}

inline void cpu_nop() 
{
	asm volatile  ("nop" ::);
}

void cpu_reboot( void );

//uintx cpu_state(void);
//void cpu_setstate( uintx state );

// todo
// 200905

//#define cpu_push(x) (x)
//#define cpu_pop(x) (x)

#ifdef __cplusplus
}
#endif


#endif /* _HPL_CPU_H_1675_ */
