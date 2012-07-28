#ifndef _HAL_CPU_H_1675_ 
#define _HAL_CPU_H_1675_ 
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

#include "hal_configall.h"
#include "hal_foundation.h"

/* If you use IAR Workbench to compile the project, you will have some intrinsic 
 * functions which can be used to operate CPU.
 * Currently, the cc2430zbw platform(cc2430 zigbee workbench) is compiled with IAR.
 */
#ifdef CONFIG_TARGETBOARD_CC2430ZBW
#include <intrinsics.h>
#endif

/* @modified by zhangwei on 2010.05.15
 *  - add macro support CONFIG_TARGET_GAINZ, CONFIG_TARGET_CC2520DK, CONFIG_TARGET_CC2430DK.
 *    by using the above macros, this interface file can be used with gainz, cc2520 developing
 *    kit or cc2430 developing kit.
 *  - you should #undef or #define the above macros inside devx_configall.h or hal_configall
 *    to activate/deactivate the above configuration macros.
 * @modified by zhangwei on 2011.08.14
 *  - Replace CONFIG_TARGET_GAINZ with CONFIG_TARGETBOARD_GAINZ
 *  - Add support to OpenNode 2010.
 * @modified by zhangwei on 2011.08.14
 *  - Revision on hal_disable_interrupts() and hal_enable_interrupts() 
 *  - Revision on hal_atomic_begin() and hal_atomic_end()
 */
 
 
/*
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
 */ 

//#define CONFIG_CPU_FREQUENCY_8MHZ   //已在configll中定义
//#define CONFIG_CPU_FREQUENCY_48MHZ
//#define CONFIG_CPU_FREQUENCY_72MHZ

#define CONFIG_CRITICAL_METHOD 2

/**
 * @attention Before you use hal_atomic_begin, you MUST disable the interrupts by 
 * calling hal_disable_interrupts() first!!!
 */
#define hal_enable_interrupts() cpu_enable_interrupts()
#define hal_disable_interrupts() cpu_disable_interrupts() 
#define hal_atomic_begin() cpu_atomic_begin() 
#define hal_atomic_end() cpu_atomic_end()
#define hal_enter_critical() cpu_atomic_begin() 
#define hal_leave_critical() cpu_atomic_end()

/**
 * cpu_atomic_t
 * The following type is used to implement atomic mechanism in hal_cpu module.
 * You should change the definition when porting to a new architecture. 
 */
#define TiCpuState cpu_atomic_t 
#define hal_atomic_t cpu_atomic_t

#ifdef CONFIG_TARGETBOARD_GAINZ
  typedef uint8 cpu_atomic_t;
#endif

#ifdef CONFIG_TARGETBOARD_OPENNODE2010
  typedef uint32 cpu_atomic_t;
#endif

/* IAR WorkBench support type _istate_t which is the same as cpu_atomic_t */  
#ifdef CONFIG_TARGETBOARD_CC2430ZBW
  #define cpu_atomic_t __istate_t 
#endif

#if (!defined(CONFIG_TARGETBOARD_GAINZ) && !defined(CONFIG_TARGETBOARD_OPENNODE2010) && !defined(CONFIG_TARGETBOARD_CC2430ZBW))
  #error "You should define cpu_atomic_t type according to your CPU core's state register width."
#endif

/**
 * global variable: g_atomic_level
 * to keep the atmic nested level. defined in this module. 
 */
//extern uint8 g_atomic_level;//hal_foundation中已定义
#if (CONFIG_CRITICAL_METHOD == 3)
extern uint8 g_atomic_flag;
#endif

/**
 * @attention: 
 * - You must define one and only one of the following macros to tell this module
 * the CPU frequency. The delay functions will use this macro to configure itself.
 * - Currently, the OpenNode 2010 version can recognize 8MHZ, 48MHZ and 72MHZ macro.
 */

/* @attention
 * @modified by zhangwei on 2011.08.14
 * - OpenWSN eliminate the function hal_delay(). You should use hal_delayms() instead.
 */
 
#define hal_delay250ns() cpu_delay250ns()
#define hal_delayus(usec) cpu_delayus(usec)
#define hal_delayms(msec) cpu_delayms(msec)

void cpu_delay250ns(void);
void cpu_delayus(uint16 usec);
void cpu_delayms(uint16 msec);

/*
#if (CONFIG_CRITICAL_METHOD == 1)
__inline TiCpuState cpu_atomic_begin(void);
__inline void cpu_atomic_end(TiCpuState state);
#else
__inline void cpu_atomic_begin(void);
__inline void cpu_atomic_end(void);
#endif
*/
 

/*******************************************************************************
 * For target board OpenNode 2010:
 *
 * OpenNode 2010 is based on STM32F10x (Cortex-M3 core) and TI cc2520 transceiver.
 * Please refer to STM32 firmware library for more CPU functions. Most of them 
 * are implemented in "cm3/core/core_cm3.h"
 ******************************************************************************/
 
#ifdef CONFIG_TARGETBOARD_OPENNODE2010

/* @modified by Jiang Ridong in 2011.07
 * @attention You should place "stm32f10x.h" before "core_cm3" or else the compiler
 * will complain some symbols cannot found and may report strange errors.
 */
#include "./opennode2010/cm3/device/stm32f10x/stm32f10x.h"
#include "./opennode2010/cm3/core/core_cm3.h"

#define CPU_REGISTER_WIDTH 32
#define CPU_DATAPATH_WIDTH 32

/** 
 * cpu_atomic_t is used to store the CPU global interrupt status. The value is usually
 * used to recover previous interrupt state in cpu_atomic_begin() and cpu_atomic_end().
 */
#define cpu_atomic_t uint32

#if defined (__CC_ARM)
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */

#elif defined (__ICCARM__)
  #define __ASM           __asm                                       /*!< asm keyword for IAR Compiler          */
  #define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

#elif defined (__GNUC__)
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */

#elif defined (__TASKING__)
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
  
#else
  #error "Unsupport compile and toolsuite. More proting work needed."  
#endif

//#if defined(__CC_ARM)
  #ifdef CONFIG_COMPILER_ARMCC
  #define cpu_nop() __nop()
  #define cpu_sleep() __WFI()
  #define cpu_enable_interrupts() __enable_irq()
  #define cpu_disable_interrupts() __disable_irq()
  //#define cpu_atomic_begin() __get_PRIMASK()
  //#define cpu_atomic_end(state) __set_PRIMASK(state)
#elif defined(__GNUC__)
  #define cpu_nop() __NOP()
  #define cpu_sleep() __WFI()
  #define cpu_enable_interrupts() __enable_irq()
  #define cpu_disable_interrupts() __disable_irq()
  //#define cpu_atomic_begin() __get_PRIMASK()
  //#define cpu_atomic_end(state) __set_PRIMASK(state)
#endif

#if (CONFIG_CRITICAL_METHOD == 1)
__inline TiCpuState cpu_atomic_begin()
{
    TiCpuState state = __get_PRIMASK();
    __disable_irq();    
    return state;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 1)
__inline void cpu_atomic_end(TiCpuState state)
{
    __set_PRIMASK(state);
}
#endif

#if (CONFIG_CRITICAL_METHOD == 2)
/** 
 * Begin the critical section. 
 * @attention Before you calling this function, you must guarantee the global interrupt control 
 * is enabled!!! Or else the hal_atomic_end() will be failed to recover correct status.
 */
__inline void cpu_atomic_begin( void )
{
	if (g_atomic_level == 0)
	{
		cpu_disable_interrupts();
	}
	g_atomic_level ++;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 2)
__inline void cpu_atomic_end( void )
{
	g_atomic_level --;
	if (g_atomic_level == 0)
	{
		cpu_enable_interrupts();
	}
}
#endif

/** 
 * Begin the critical section. 
 */
#if (CONFIG_CRITICAL_METHOD == 3)
__inline void cpu_atomic_begin( void )
{
	if (g_atomic_level == 0)
	{
		g_atomic_flag = __get_PRIMASK();
		cpu_disable_interrupts();
	}
	g_atomic_level ++;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 3) 
__inline void cpu_atomic_end( void )
{
	g_atomic_level --;
	if (g_atomic_level == 0)
	{
        __set_PRIMASK(g_atomic_flag);
	}
}
#endif


/**
 * @brief  Initiate a system reset request.
 *
 * Initiate a system reset request to reset the MCU. Function NVIC_SystemReset is
 * implemented in "core_cm3.h" in STM32F10x firmware library.
 */
#define cpu_reset() NVIC_SystemReset()


/* The following are for Cortex-M3 core only */

#if defined(__CC_ARM)

#define cpu_wfi() __wfi
#define cpu_wfe() __wfe

#elif defined(__GNUC__)

#define cpu_wfi() __WFI()
#define cpu_wfe() __WFE()

#endif 

/**
 * @brief  Reverse byte order in unsigned short value
 *
 * @param   value  value to reverse
 * @return         reversed value
 *
 * Reverse byte order in unsigned short value
 */
#define cpu_rev16(value) __REV16((uint16_t)value)
// @todo __REV16 in core_cm3 returns uint32_t. why?

/**
 * @brief  Reverse byte order in signed short value with sign extension to integer
 *
 * @param   value  value to reverse
 * @return         reversed value
 *
 * Reverse byte order in signed short value with sign extension to integer
 */
#define cpu_revsh(value)  __REVSH((int16_t)value)
// @todo __REVSH in core_cm3 returns uint32_t. why?

#endif /* CONFIG_TARGETBOARD_OPENNODE2010 */


/*******************************************************************************
 * for platform GAINZ supplied by ICT controlled by macro CONFIG_TARGET_GAINZ
 * gainz is provided by ICT in China.
 ******************************************************************************/

#ifdef CONFIG_TARGETBOARD_GAINZ

#define CPU_REGISTER_WIDTH 8
#define CPU_DATAPATH_WIDTH 8

#include "./gainz/hpl_cpu.h"
#endif 

/*******************************************************************************
 * for platform cc2520 developing kit controlled by macro CONFIG_TARGET_CC2520DK
 * cc2520dk is supplied by TI.com
 ******************************************************************************/

#ifdef CONFIG_TARGETBOARD_CC2520DK
#define CPU_REGISTER_WIDTH 16
#define CPU_DATAPATH_WIDTH 16
#include "./cc2520dk/hpl_cpu.h"
#endif 

/*******************************************************************************
 * for platform cc2430 developing kit controlled by macro CONFIG_TARGET_CC2430DK
 * cc2430dk is supplied by TI.com
 ******************************************************************************/

#ifdef CONFIG_TARGETBOARD_CC2430DK
#include "./cc2430dk/hpl_cpu.h"
#endif 


// #define halIntOn() hal_enable_interrupts()    
// #define halIntOff() hal_disable_interrupts()    
// #define halIntLock() hal_atomic_begin()
// #define halIntUnlock(x)  hal_atomic_end(x)


/*******************************************************************************
 * for platform cc2430 zigbee workbench (cc2430zbw)
 * @modified by zhangwei, and zhang zhengqing(TongJi University) in 2012
 * - add support to cc2430 transceiver from TI.com
 ******************************************************************************/

#ifdef CONFIG_TARGETBOARD_CC2430ZBW

#define cpu_reset() {}
#define cpu_nop() _no_operation()
#define cpu_sleep() {} // todo  __WFI()
#define cpu_enable_interrupts() __enable_interrupt()
#define cpu_disable_interrupts() __disable_interrupt()
#define cpu_get_interrupt_state() __get_interrupt_state()
#define cpu_set_interrupt_state(state) __set_interrupt_state(state)

#if (CONFIG_CRITICAL_METHOD == 1)
#pragma inline=forced
TiCpuState cpu_atomic_begin()
{
    TiCpuState state = cpu_get_interrupt_state();
    cpu_disable_interrupts();    
    return state;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 1)
#pragma inline=forced
void cpu_atomic_end(TiCpuState state)
{
    cpu_get_interrupt_state(state);
}
#endif

#if (CONFIG_CRITICAL_METHOD == 2)
/** 
 * Begin the critical section. 
 * @attention Before you calling this function, you must guarantee the global interrupt control 
 * is enabled!!! Or else the hal_atomic_end() will be failed to recover correct status.
 */
#pragma inline=forced
void cpu_atomic_begin( void )
{
	if (g_atomic_level == 0)
	{
		cpu_disable_interrupts();
	}
	g_atomic_level ++;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 2)
#pragma inline=forced
void cpu_atomic_end( void )
{
	g_atomic_level --;
	if (g_atomic_level == 0)
	{
		cpu_enable_interrupts();
	}
}
#endif

/** 
 * Begin the critical section. 
 */
#if (CONFIG_CRITICAL_METHOD == 3)
#pragma inline=forced
void cpu_atomic_begin( void )
{
	if (g_atomic_level == 0)
	{
		g_atomic_flag = cpu_get_interrupt_state();
		cpu_disable_interrupts();
	}
	g_atomic_level ++;
}
#endif

#if (CONFIG_CRITICAL_METHOD == 3) 
#pragma inline=forced
void cpu_atomic_end( void )
{
	g_atomic_level --;
	if (g_atomic_level == 0)
	{
        cpu_set_interrupt_state(g_atomic_flag);
	}
}
#endif

// @todo the following should be removed from openwsn project



#if (defined __ICC430__) || defined(__MSP430__)

// Use the macros below to reduce function call overhead for common
// global interrupt control functions


#if (defined __ICC430__)
#define HAL_INT_ON(x)      st( __enable_interrupt(); )
#define HAL_INT_OFF(x)     st( __disable_interrupt(); )
#define HAL_INT_LOCK(x)    st( (x) = __get_interrupt_state(); \
                               __disable_interrupt(); )
#define HAL_INT_UNLOCK(x)  st( __set_interrupt_state(x); )
#endif

#if (defined __MSP430__)
#define HAL_INT_ON(x)      st( _enable_interrupts(); )
#define HAL_INT_OFF(x)     st( _disable_interrupts(); )
#define HAL_INT_LOCK(x)    st( (x) = _get_SR_register(); \
                               _disable_interrupts(); )
#define HAL_INT_UNLOCK(x)  st( _enable_interrupts(); /*_bis_SR_register(x);*/ )
#endif

#elif defined __ICC8051__

#define HAL_INT_ON(x)      st( EA = 1; )
#define HAL_INT_OFF(x)     st( EA = 0; )
#define HAL_INT_LOCK(x)    st( (x) = EA; EA = 0; )
#define HAL_INT_UNLOCK(x)  st( EA = (x); )

typedef unsigned short istate_t;

#elif defined WIN32

#define HAL_INT_ON()
#define HAL_INT_OFF()
#define HAL_INT_LOCK(x)    st ((x)= 1; )
#define HAL_INT_UNLOCK(x)

#elif defined __KEIL__

#define HAL_INT_ON(x)      st( EA = 1; )
#define HAL_INT_OFF(x)     st( EA = 0; )
#define HAL_INT_LOCK(x)    st( (x) = EA; EA = 0; )
#define HAL_INT_UNLOCK(x)  st( EA = (x); )



#else
#error "Unsupported compiler"
#endif


/******************************************************************************
 * GLOBAL FUNCTIONS
 */

void   halIntOn(void);
void   halIntOff(void);
uint16 halIntLock(void);
void   halIntUnlock(uint16 key);




#endif /* CONFIG_TARGETBOARD_CC2430ZBW */


#endif /* _HAL_CPU_H_1675_ */
