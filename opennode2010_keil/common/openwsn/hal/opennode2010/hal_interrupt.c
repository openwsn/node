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

/* 
 * @attention
 *	- you should call hal_init() before calling some functions in this module. 
 *  - you may encounter the following message when build the whole application:
 *          undefined reference to 'm_int2handler'
 *    this is due to you haven't add hal_foundation.c into your project. the above
 *    variable is declared in hal_foundation.c
 *
 * @reference
 * - AVR GCC Interrupt in WinAVR (found in your winavr)
 *   file:///D:/portable/WinAVR-20080610/doc/avr-libc/avr-libc-user-manual/group__avr__interrupts.html
 *   this is the most important document for interrupt programming.
 * 
 *   attention: please use new macro ISR() instead of the old SIGNAL() macro.
 *   in the past: 
 *
 * INTERRUPT
 * 用法：INTERRUPT（signame）
 * 说明：定义中断源signame对应的中断例程。在执行时，全局屏蔽位将清零，其他中断被使能。
 * ADC结束中断例程的例子如下所示：
 * 
 * SIGNAL
 * 用法：SIGNAL（signame）
 * 说明：定义中断源signame对应的中断例程。在执行时，全局屏蔽位保持置位，其他中断被禁止。
 * 
 *   now
 *   ISR(BADISR_vect)
 *	 ISR(XXX_vect, ISR_NOBLOCK) for nested interrupts
 *   ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect)) for shared ISR  
 *		ISR_ALIAS(vector, target_vector)
 *   EMPTY_INTERRUPT(ADC_vect);
 *   ISR(TIMER1_OVF_vect, ISR_NAKED)
 * 
 * - Better GCC Interrupt Macro, 2006  (obsolete, but it's still meaningful to understand)
 *   http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=37830
 * 
 * - AVR-GCC里定义的API, 2009, 
 *   http://hi.baidu.com/tao_%CC%CE/blog/item/7441e9eee32c0bf3b3fb9545.html
 */

#include "hal_configall.h" 
#include "hal_foundation.h" 
#include "hal_interrupt.h"
#include "hal_cpu.h"
#include "hal_led.h"
#include "hal_assert.h"


#ifdef __cplusplus
extern "C" {
#endif

void hal_disable_interrupt( uintx num )
{
	// todo
/*
    if (num < 8)
    {
        EIMSK |= (1 << num);
    }
*/
}

void hal_enable_interrupt( uintx num )
{
    /* for atmega128, there're 8 external interrupts. which can be controlled by 
     * the following: EICRA, EICRB, EIMSK, EIFR. 
     * for other interrupt source such as timer, the interrupt is controlled by
     * its own registers.
     * 
     * ref to atmega128 datasheet
     */

	// todo
	/*
    if (num < 8)
    {
        EIMSK &= (~(1 << num));
    }
	*/
}



/******************************************************************************
 * interrupt number - object's handler mapping table  (abbr. as iht)
 * this table records the relationships between interrupt number and related object's
 * event handler. it's usually used to find the related object by interrupt number
 * only in interrupt service routines (isr).
 *
 * 	{interrupt number, objects handler, object}
 *
 *****************************************************************************/

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_attachhandler( uint8 num, TiFunEventHandler handler, void * owner )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	m_int2handler[idx].num = num;
	m_int2handler[idx].handler = handler;
	m_int2handler[idx].owner = owner;

	return idx;
}
#endif

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_detachhandler( uint8 num )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	m_int2handler[idx].num = 0;
	m_int2handler[idx].handler = NULL;
	m_int2handler[idx].owner = NULL;

	return idx;
}
#endif

#ifdef CONFIG_INT2HANDLER_ENABLE
int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** powner )
{
	int idx;

	idx = num % CONFIG_INT2HANDLER_CAPACITY;
	*phandler = m_int2handler[idx].handler;
	*powner = m_int2handler[idx].owner;

	return (m_int2handler[idx].handler == NULL) ? -1 : idx;
}
#endif

/* transfer the call to an interrupt as the call to an object handler. this function 
 * is usually called inside an interrupt routine 
 */
inline void hal_invokehandler( uint8 num, TiEvent * e )
{
	_TiIntHandlerItem * item;
	item = &(m_int2handler[num % CONFIG_INT2HANDLER_CAPACITY]);
	if (item->handler != NULL)
	{
		item->handler( item->owner, e );
	}
}



/******************************************************************************
 * default interrupt handlers
 * this section provides an default implementation of interrupt service routines. 
 * these routines help to call the object handler when combined with interrupt 
 * handler table (iht) 
 *****************************************************************************/


/* Q: how to connect interrupt service handler in WinAVR
 * part of the interrupt symbols supported by avr-gcc and atmega128
 * ref: file:///D:/portable/WinAVR-20080610/doc/avr-libc/avr-libc-user-manual/group__avr__interrupts.html
 * 
 * Q: where can I find the interrupt macros and number defined in WinAVR?
 * R: for atmega 128, ref to: D:\portable\WinAVR-20080610\avr\include\avr\iom128.h
 *
 * ISR(ADC_vect)
 * ADC Conversion Complete 
 * 
 * ISR(ANALOG_COMP_vect)
 * Analog Comparator 
 * 
 * ISR(ANA_COMP_vect)
 * Analog Comparator 
 *
 * ISR(INT0_vect)
 * External Interrupt 0
 *
 * ISR(PCINT0_vect)
 * PIN change interrupt request 0, there's also PININT1_vect, PININT2_vect, PININT3_vect 
 *
 * ISR(SPI_STC_vect)
 * SPI Serial Transfer Complete
 *
 * ISR(TIMER0_COMPA_vect)
 * TimerCounter0 Compare Match A, formerly as SIG_OUTPUT_COMPARE0A 
 *
 * ISR(TIMER0_COMPB_vect)
 * Timer Counter 0 Compare Match B, formerly as SIG_OUTPUT_COMPARE0B, SIG_OUTPUT_COMPARE0_B 
 *
 * ISR(TIMER0_COMP_vect)
 * Timer/Counter0 Compare Match, formerly SIG_OUTPUT_COMPARE0 
 *
 * ISR(TIMER0_OVF_vect)
 * Timer/Counter0 Overflow. formerly SIG_OVERFLOW0
 *
 * ISR(TIMER1_CAPT_vect)
 * Timer/Counter Capture Event, formerly SIG_INPUT_CAPTURE1
 *
 * ISR(TIMER1_COMPA_vect)
 * Timer/Counter1 Compare Match A, formerly SIG_OUTPUT_COMPARE1A
 *
 * ISR(TIMER1_COMPB_vect)
 * Timer/Counter1 Compare MatchB, formerly SIG_OUTPUT_COMPARE1B
 *
 * TIMER1_COMPC_vect
 * Timer/Counter1 Compare Match C. formerly SIG_OUTPUT_COMPARE1C.
 * 
 * TIMER1_OVF_vect
 * Timer/Counter1 Overflow, formerly SIG_OVERFLOW1
 *
 * TIMER2_COMPA_vect
 * Timer/Counter2 Compare Match A, formerly SIG_OUTPUT_COMPARE2A
 *
 * TIMER2_COMPB_vect
 * Timer/Counter2 Compare Match, formerly SIG_OUTPUT_COMPARE2
 * 
 * TIMER2_OVF_vect
 * Timer/Counter2 Overflow, formerly SIG_OVERFLOW2
 * 
 * TIMER3_....
 *
 * TWI_vect
 * 2-wire Serial Interface. formerly SIG_2WIRE_SERIAL
 * 
 * USART...
 *
 * WDT_vect
 * Watchdog Timeout Interrupt. formerly SIG_WDT, SIG_WATCHDOG_TIMEOUT
 */

/* Interrupt vectors  defined in <iom128.h> in WinAVR
 *
 * #define INT0_vect			_VECTOR(1)
 * #define INT1_vect			_VECTOR(2)
 * #define INT2_vect			_VECTOR(3)
 * #define INT3_vect			_VECTOR(4)
 * #define INT4_vect			_VECTOR(5)
 * #define INT5_vect			_VECTOR(6)
 * #define INT6_vect			_VECTOR(7)
 * #define INT7_vect			_VECTOR(8)
 * #define SIG_INTERRUPT7		_VECTOR(8)
 * #define INT7_vect			_VECTOR(8)
 *
 * Timer/Counter2 Compare Match 
 * #define TIMER2_COMP_vect		_VECTOR(9)
 *
 * Timer/Counter2 Overflow 
 * #define TIMER2_OVF_vect		_VECTOR(10)
 *
 * Timer/Counter1 Capture Event 
 * #define TIMER1_CAPT_vect		_VECTOR(11)
 * 
 * Timer/Counter1 Compare Match A 
 * #define TIMER1_COMPA_vect	_VECTOR(12)
 * #define SIG_OUTPUT_COMPARE1A	_VECTOR(12)
 *
 * Timer/Counter Compare Match B 
 * #define TIMER1_COMPB_vect	_VECTOR(13)
 * #define SIG_OUTPUT_COMPARE1B	_VECTOR(13)
 * 
 * Timer/Counter1 Overflow 
 * #define TIMER1_OVF_vect		_VECTOR(14)
 * #define SIG_OVERFLOW1		_VECTOR(14)
 *
 * Timer/Counter0 Compare Match 
 * #define TIMER0_COMP_vect		_VECTOR(15)
 * #define SIG_OUTPUT_COMPARE0	_VECTOR(15)
 *
 * Timer/Counter0 Overflow 
 * #define TIMER0_OVF_vect		_VECTOR(16)
 * #define SIG_OVERFLOW0		_VECTOR(16)
 * ......
 *
 * Timer/Counter3 Compare Match A 
 * #define TIMER3_COMPA_vect	_VECTOR(26)
 * #define SIG_OUTPUT_COMPARE3A	_VECTOR(26)
 * ......
 */

/* 
ISR(INT0_vect)
{
	// hal_assert(false);
}
*/

/* used to response the cc2420 FIFOP interrupt (external interrupt request 7, _VECTOR(7)) */
/*
ISR(INT6_vect)
{
    hal_invokehandler( INTNUM_CC2420_FIFOP, NULL );
}
*/

/* _VECTOR(14)
 * used with the interrupt driven test application in <timer> directory. */
/*
ISR(TIMER0_OVF_vect)
{   
   	hal_invokehandler( INTNUM_TIMER0_OVF, NULL );
}
*/

/* _VECTOR(15) */
/*
ISR(TIMER0_COMP_vect)
{
   	hal_invokehandler( INTNUM_TIMER0_COMP, NULL );
}

ISR(TIMER1_OVF_vect)
{
   	hal_invokehandler( INTNUM_TIMER1_OVF, NULL );
}

ISR(TIMER1_COMPA_vect)
{
   	hal_invokehandler( INTNUM_TIMER1_COMPA, NULL );
}

ISR(TIMER2_OVF_vect)
{
   	hal_invokehandler( INTNUM_TIMER2_OVF, NULL );
}

ISR(TIMER2_COMP_vect)
{
   	hal_invokehandler( INTNUM_TIMER2_COMP, NULL );
}

ISR(TIMER3_OVF_vect)
{
   	hal_invokehandler( INTNUM_TIMER3_OVF, NULL );
}


ISR(TIMER3_COMPA_vect)
{
	hal_invokehandler( INTNUM_TIMER3_COMPA, NULL );
}
*/

/* used to response the TIMER3 comparison interrupt */
//void __attribute((interrupt))   __vector_26(void)
//ISR(__vector_26)
/*
ISR(TIMER3_COMPA_vect)
{
	hal_invokehandler( INTNUM_CC2420_SFD, NULL );
}
*/
//
//ISR(ADC_vect)
//{
//    hal_invokehandler( INTNUM_ADC_COMPLETE, NULL );
//}

///* USART0, Rx Complete, formerly SIG_UART0_RECV	*/
//ISR(USART0_RX_vect)
//{
//	hal_invokehandler( INTNUM_USART0_RX, NULL );
//}
//
///* USART0 Data Register Empty, SIG_UART0_DATA */
//ISR(USART0_UDRE_vect)
//{
//	hal_invokehandler( INTNUM_USART0_UDRE, NULL );
//}
//
//ISR(USART1_RX_vect)
//{
//	//hal_invokehandler( INTNUM_USART1_RX, NULL );
//}
//
//ISR(USART1_UDRE_vect)
//{
//	//hal_invokehandler( INTNUM_USART1_UDRE, NULL );
//}
//
///* Catch-all interrupt vector
// * If an unexpected interrupt occurs (interrupt is enabled and no handler is installed, 
// * which usually indicates a bug), then the default action is to reset the device 
// * by jumping to the reset vector. You can override this by supplying a function 
// * named BADISR_vect which should be defined with ISR() as such. (The name BADISR_vect 
// * is actually an alias for __vector_default. The latter must be used inside assembly 
// * code in case <avr/interrupt.h> is not included.)
// */
//
//ISR(BADISR_vect)
//{
//	/* attention: in real applications, you'd better reset the whole device here */
//	hal_assert(false);
//}
//
///* interrupt service routine sharing */
//ISR(INT1_vect, ISR_ALIASOF(INT0_vect));
//ISR(INT3_vect, ISR_ALIASOF(INT0_vect));
//ISR(INT4_vect, ISR_ALIASOF(INT0_vect));
//ISR(INT5_vect, ISR_ALIASOF(INT0_vect));
//
///* empty interrupt */
//EMPTY_INTERRUPT(INT2_vect);

#ifdef __cplusplus
}
#endif
