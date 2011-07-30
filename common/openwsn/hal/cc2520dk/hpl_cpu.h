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
#include <in430.h>
#include <msp430x44x.h>
#include "../hal_foundation.h"

/*******************************************************************************
 * @author zhangwei on 2010.05.15
 *  - first created. this file is used with cc2520dk supplied by TI.com
 * @modified by nikan on 2010.05.xx
 ******************************************************************************/

#ifndef CONFIG_TARGET_CC2520DK
#error "this module is used with TI's cc2520dk only. should be compiled with IAR tools"
#endif

/* reference
 * - lierda company, msp430 learning board instruction manual, 
 *   http://wenku.baidu.com/view/9f3d17d276a20029bd642db9.html
 * - 正伟MSP430的软硬件C延时程序设计
 *   http://www.gkong.com/co/zwtech/learn_detail.asp?learn_id=13841
 * - MSP430的软硬件C延时程序设计
 *   http://article.ednchina.com/Embeded/200703121016001.htm
 * - lierda company, uC/OS-II porting to msp430 v0.9, 
 *   http://wenku.baidu.com/view/47d051cfa1c7aa00b52acbaa.html
 */

/* @attention
 * @todo
 *  you should modifie the following macro to reflect cc2520dk's hardware settings 
 *  how fast cc2520dk runs?
 */
#define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
#define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
#define CONFIG_CPU_CLOCK_MSEC_RATIO (CONFIG_CPU_CLOCK_MSEC_RATIO1*CONFIG_CPU_CLOCK_MSEC_RATIO2)

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Interrupt Management
 *****************************************************************************/
	
#define hal_disable_interrupts() cpu_disable_interrupts()
#define hal_enable_interrupts() cpu_enable_interrupts()

/* method 1: control global interrupt with single instruction
 * @attention the interrupt will automatically enabled after you returned from 
 * @todo: we don't need the above feature
 */

#define cpu_disable_interrupts() _DINT()
#define cpu_enable_interrupts() _EINT()



/******************************************************************************
 * Atomic Functions
 * _cpu_atomic_begin / _cpu_atomic_end
 * hal_atomic_begin / hal_atomic_end
 * 
 * strongly suggest using these function pair inside one function only. 
 *****************************************************************************/

/* @todo:
 * should be ported to msp430
 */
inline cpu_atomic_t _cpu_atomic_begin(void)
{
    /*
	cpu_atomic_t result = * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20);
	__asm volatile ("cli");
	return result;
    */
    return 0;
}

// todo
inline void _cpu_atomic_end(cpu_atomic_t state)
{
    /*
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20) = state;
    */
    return;
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
        // better save it into the stack and then pop from the stack. that will be
        // more wonderful.
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

/******************************************************************************
 * short delay functions:
 *
 * @attention:
 * before using this function, you should configure the following two macros to
 * adapter to you own hardware platform:
 *
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
 * 
 * the product of these two macros defines how many clock cycle equals to 1 milli-second.
 * 
 * @modified by zhangwei on 2010.05.15
 *  - remove the deprecated function cpu_pause() and cpu_delay250ns(). we should
 *    only use cpu_delayus(usec) and cpu_delayms(msec)
 *****************************************************************************/

/* attention
 *  - the following delay functions are hardware dependent. they depend on how fast
 * your CPU runs. please adjust the configuration macro to adapte the following 
 * delay functioins to your own platform.
 *  - for the cc2520dk supplied by TI.com, the clock frequency is XXXX MHz. each
 * _NOP() will occupy about XXXX usec. 
 */

#define cpu_nop() _NOP()



#define hal_delayus(usec) cpu_delayus(usec)
#define hal_delay(msec)   cpu_delayms(msec)
#define hal_delayms(msec) cpu_delayms(msec)

/* delay for usec.
 * this function is hardware dependent and should used for very short delays only. 
 * if you need accurate long delays, please use timer to help you do so.
 * 
 * @attention
 * the input parameter value should not large than the maximum value of int16 type
 */
void cpu_delayus(uint16 usec);


/* delay for micro seconds.
 * this function is hardware dependent and should used for short delays only. 
 * if you need very long accurate delays, please use timer to help you do so.
 * 
 * attention: 
 * the input msec value should not exceed the maximum value of uint16 type 
 */
inline void cpu_delay(int16 msec) 
{
    while (msec > 0)
    {
        cpu_delayus( 1000 );
        msec --;
    }
}

/* cpu sleep and wakeup
 * the cpu is usually wakeup by external hardware interrupts such as 
 * - UART interrupts
 * - cc2420 FIFOP interrupts
 * - RTC timer expire interrupts
 */
inline void cpu_sleep(void)
{
	// * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x35 + 0x20) |= 1 << 5;
	// __asm volatile ("sleep");
}

void cpu_reset(void);
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
