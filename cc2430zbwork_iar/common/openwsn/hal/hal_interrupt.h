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
 
#ifndef _HAL_INTERRUPT_H_4788_
#define _HAL_INTERRUPT_H_4788_

/*******************************************************************************
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
 * @status 
 * 	- released. tested ok on ATmega128 micro-controller.
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
 ******************************************************************************/ 

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_vic.h"

#ifdef CONFIG_TARGETBOARD_OPENNODE2010
//#include "arch_target.h"
#endif

#ifdef CONFIG_TARGETBOARD_GAINZ
#include <avr/interrupt.h>
#endif

#ifdef CONFIG_OS_UCOSII
#include "../ucos-ii/os_cpu.h"
#endif

/* Q: how to connect interrupt service handler in WinAVR
 * part of the interrupt symbols supported by avr-gcc and atmega128
 * ref: file:///D:/portable/WinAVR-20080610/doc/avr-libc/avr-libc-user-manual/group__avr__interrupts.html
 * 
 * Q: where can I find the interrupt macros and number defined in WinAVR?
 * R: for atmega 128, ref to: D:\portable\WinAVR-20080610\avr\include\avr\iom128.h
 */

/* the interrupt number defined here should better smaller than the capacity of 
 * interrupt handler table declared in hal_foundation.h */

#define INTNUM_INT0				1
#define INTNUM_INT1				2
#define INTNUM_INT2				3
#define INTNUM_INT3				4
#define INTNUM_INT4				5
#define INTNUM_INT5				6
#define INTNUM_INT6				7
#define INTNUM_INT7				8
#define INTNUM_TIMER2_COMP		9
#define INTNUM_TIMER2_OVF		10
#define INTNUM_TIMER1_CAPT		11
#define INTNUM_TIMER1_COMPA		12
#define INTNUM_TIMER1_COMPB		13
#define INTNUM_TIMER1_OVF		14
#define INTNUM_TIMER0_COMP		15
#define INTNUM_TIMER0_OVF		16
#define INTNUM_TIMER3_COMPA		26
#define INTNUM_TIMER3_OVF		29

#define INTNUM_USART0_RX		18
#define INTNUM_USART0_UDRE		19
#define INTNUM_USART1_RX		30
#define INTNUM_USART1_UDRE		31
#define INTNUM_ADC_COMPLETE     21

/* or else change to INTNUM_INT6 */
#define INTNUM_CC2420_FIFOP     INTNUM_INT6
#define INTNUM_CC2420_SFD       INTNUM_TIMER3_COMPA

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Common Interface Functions
 ******************************************************************************/ 

void hal_disable_interrupt( uintx num );
void hal_enable_interrupt( uintx num );

/*******************************************************************************
 * interrupt number - object's handler mapping table  (abbr. as iht)
 * this table maps integer id to object event handler. so it can actually used
 * with non-interrupt service routines.
 ******************************************************************************/

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_attachhandler( uint8 num, TiFunEventHandler handler, void * owner );
int hal_detachhandler( uint8 num );
int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** powner );
void hal_invokehandler( uint8 num, TiEvent * e );
#endif

#ifdef __cplusplus
}
#endif

#endif /* _HAL_INTERRUPT_H_4788_ */





 
