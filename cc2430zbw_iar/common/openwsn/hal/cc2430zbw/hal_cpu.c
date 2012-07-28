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

#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_cpu.h"

/** 
 * @todo
 * hal_delay_xxx
 * hal_disable_interrupts
 * hal_enable_interrupts
 */

//uint8 g_atomic_level = 0;//已在hal_foundation.c中定义
#if (CONFIG_CRITICAL_METHOD == 3) 
uint8 g_atomic_flag = 0;
#endif

/** 
 * @todo:
 * STM32 Lib provides following macros:
 * 
 #define SYSCLK_FREQ_HSE    HSE_VALUE
 #define SYSCLK_FREQ_24MHz  24000000
 #define SYSCLK_FREQ_HSE    HSE_VALUE
 #define SYSCLK_FREQ_24MHz  24000000 
 #define SYSCLK_FREQ_36MHz  36000000 
 #define SYSCLK_FREQ_48MHz  48000000 
 #define SYSCLK_FREQ_56MHz  56000000 
 #define SYSCLK_FREQ_72MHz  72000000 
 */

//#ifdef SYSCLK_FREQ_8MHz
//#define CONFIG_CPU_FREQUENCY_8MHZ
//#endif

//#ifdef SYSCLK_FREQ_8MHz
//#define CONFIG_CPU_FREQUENCY_8MHZ
//#endif

/**
 * A very short delay for only 250ns. This one is used to adjust hardware timing 
 * in some special cases. 
 * 
 * @attention At least Keil compiler can recognize #pragma O0
 * @attention 
 */
#ifdef CONFIG_COMPILER_IAR
#pragma optimize=none
#else
#pragma O0 
#endif
void cpu_delay250ns(void)
{
/*
#if defined(CONFIG_CPU_FREQUENCY_8MHZ)
    cpu_nop();
    cpu_nop();
#elif defined(CONFIG_CPU_FREQUENCY_24MHZ)
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
#elif defined(CONFIG_CPU_FREQUENCY_48MHZ)
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
#elif defined(CONFIG_CPU_FREQUENCY_72MHZ)
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
    cpu_nop();
#else
  #error "You must choose your CPU frequency and implement this function again."
#endif  
*/
}

/**
 * A very short delay for only 1us. This one is used to adjust hardware timing 
 * in some special cases. 
 * 
 * @attention Not tested yet.
 */


/*
 tested by ninghuaqiang in Sep/23/2011.
 when define counter = 2200, the  hal_delayms(1000) woule make the device delay 1000ms.
 For different CPU_FREQUENCY defined, it would have the same phenomenon.
 #todo
 in fact, it should not have the same delay for the same counter in different CPU_FREQUENCY,
 therefore, further test for this project is needed. 
 */
#ifdef CONFIG_COMPILER_IAR
#pragma optimize=z,none
#else
#pragma O0 
#endif
inline void cpu_delay1us()
{
    // for cc2430 MCU, seems running at 32MHz
    // so that CONFIG_CPU_FREQUENCY_32MHZ should be defined
    //
    if (1)
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
}

/* hardware dependent function.
 * on GAINZ platform, the main clock frequency is 8MHz. so 8 nop instruction
 * occupies approximately 1 u-second (usec)
 * 
 * @attention
 * - the input parameter value should not large than the maximum value of uint16 type
 */
/***********************************************************************************
* @fn          halMcuWaitUs
*
* @brief       Busy wait function. Waits the specified number of microseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions. The duration of one cycle depends on MCLK. In this HAL
*              , it is set to 8 MHz, thus 8 cycles per usec.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 usec - number of microseconds delay
*
* @return      none
*/
/* @history
 * @modified by zhangwei on 2012.04.16
 * - this function is formerly known as: 
 *     void halMcuWaitUs(uint16 usec)
 */
#ifdef CONFIG_COMPILER_IAR
#pragma optimize=none
#else
#pragma O0 
#endif
void cpu_delayus(uint16 usec)
{
    while(usec--)
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
}


/* attention: 
 * the input msec value should not exceed the maximum value of uint16 type 
 */
/***********************************************************************************
* @fn          halMcuWaitMs
*
* @brief       Busy wait function. Waits the specified number of milliseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 millisec - number of milliseconds delay
*
* @return      none
*/

#ifdef CONFIG_COMPILER_IAR
#pragma optimize=none
#else
#pragma O0 
#endif
void cpu_delayms(uint16 msec) 
{
    while(msec--)
        halMcuWaitUs(1000);
}



#ifdef CONFIG_TARGETBOARD_OPENNODE2010

#include "hal_types.h"
#include "hal_defs.h"
#include "hal_int.h"
#include "hal_board.h"

/***********************************************************************************
* @fn      halIntOn
*
* @brief   Enable global interrupts.
*
* @param   none
*
* @return  none
*/
void halIntOn(void)
{
    HAL_INT_ON();
}


/***********************************************************************************
* @fn      halIntOff
*
* @brief   Turns global interrupts off.
*
* @param   none
*
* @return  none
*/
void halIntOff(void)
{
    HAL_INT_OFF();
}


/***********************************************************************************
* @fn      halIntLock
*
* @brief   Turns global interrupts off and returns current interrupt state.
*          Should always be used together with halIntUnlock().
*
* @param   none
*
* @return  uint16 - current interrupt state
*/
uint16 halIntLock(void)
{
    istate_t key;
    HAL_INT_LOCK(key);
    return(key);
}


/***********************************************************************************
* @fn      halIntUnlock
*
* @brief   Set interrupt state back to the state it had before calling halIntLock().
*          Should always be used together with halIntLock().
*
* @param   key
*
* @return  none
*/
void halIntUnlock(uint16 key)
{
    HAL_INT_UNLOCK(key);
}

#endif /* CONFIG_TARGETBOARD_CC2430ZBW */

