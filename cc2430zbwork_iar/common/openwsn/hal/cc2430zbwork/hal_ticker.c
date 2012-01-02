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
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_targetboard.h"
#include "../hal_debugio.h"
#include "../hal_assert.h"
#include "../hal_interrupt.h"
#include "../hal_ticker.h"

/*
 * @modified by zhangwei on 2010.12.03
 * - Change module name from hal_systimer to hal_systicker. This module is used to
 *   provide the tick pulse to drive the osx kernel to run.
 */

/* Q: Actually, we can use TiTimerAdapter directly intead of TiSysTimer. Why still
 * introducing a separate TiSysTimer?
 * R: Yes, it's true that TiTimerAdapter can fully replace TiSysTimer. and TiTimerAdapter
 * is more powerful than TiSysTimer. 
 *	However, there's the following benefit to introduce a separate TiSysTimer:
 *	- TiSysTimer is a self-contained module. which means it's easy to port the 
 *    TiSysTimer and the upper osx layer to other platforms. While, porting the 
 *    TiTimerAdapter is more complicated.
 *  - Simple means robust and high performance. This's also true for TiSysTimer.
 *  The disadvantage is:
 *	- you must attention not to affect the configurations of other timers in the TiSysTimer.
 */

/* attention:
 * There's only one systimer in the system. So we can choose a base timer by MACRO
 * instead of parameter. This will generate more efficient binary code and shrink 
 * the code size.
 */
#define CONFIG_SYSTIMER_INTERRUPT_NUMBER 26
#define CONFIG_SYSTIMER_USE_TIMER3
#define CONFIG_SYSTIMER_USE_HAL_ATTACH_HANDLER

/* attention
 *	- in the atmega 128 MCU, timer0 and timer1 are 8 bit. so they cannot support 
 * longer delays. that's why we use timer3 as the osx kernel system timer, because 
 * it's a 16 bit timer
 *  - the system timer should be used by the osx kernel only
 *	- by default, the system timer is initialized with interrupt disabled and period
 * 20 msec. but these can be changed to adapte to your own settings in osx_init().
 * this module provides the interface functions to do so.
 */ 

/* The only TiSysTimer instance. It's used by the OSX kernel only */
TiSysTimer * m_systimer = NULL;


static void _systimer_interrupt_handler( void * object, TiEvent * e );
static inline void _systm_enable( TiSysTimer * timer );
static inline void _systm_disable( TiSysTimer * timer );


/*******************************************************************************
 * TiSysTimer
 ******************************************************************************/

TiSysTimer * systm_construct( char * buf, uint8 size )
{
	TiSysTimer * timer;
	hal_assert( sizeof(TiSysTimer) <= size );
	memset( buf, 0x00, size );
	timer = (TiSysTimer *)buf;
	timer->state = 0;
	timer->interval = 200;
	timer->reginterval = 1;
	timer->listener = NULL;
	timer->lisowner = NULL;
	
	return timer;
}

void systm_destroy( TiSysTimer * timer )
{
	systm_close( timer );
}

/* param
 *	interval		based on milli-seconds
 */
TiSysTimer * systm_open( TiSysTimer * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner )
{
	uintx scale=0;
	uint16 value;

	/* only 1 system timer is allowed in the system. so if there're multiple calls
	 * to the function "systm_open()", then the later calls will failed.
	 */
	if (m_systimer != NULL)
	{
		return NULL;
	}

	timer->interval = interval;
	timer->listener = listener;
	timer->lisowner = lisowner;
    timer->time = 0;//todo
	#ifdef CONFIG_SYSTIMER_USE_TIMER3

	/* for atmega128 MCU
	 * refer to atmega128 datasheet, page 102
	 *
	 * TCCR Rgister: the last three bits is the clock select (prescale factor)
	 *	000		no clock source
	 *	001		no prescaling
	 *	010		/8
	 *	011		/32
	 *	100		/64
	 *	101		/128
	 *	110		/256
	 *	111		/1024
	 *
	 * the default setting for osx kernel is 7 which means prescale is 1024.
	 * this means the timer counter value will increase/decrease 1 when 1024
	 * clocks received.
	 */
	#define SYSTM_PRESCALE_FACTOR 7
	#define SYSTM_PRESCALE 1024

	/* HOPEINTERVAL is based on milliseconds
	 * MCU's clock frequency is defined in target.h as GAINZ_CLOCK_FREQUENCY
	 */
	#define SYSTM_HOPEINTERVAL 20
	//#define SYSTM_CLOCK_FREQUENCY (GAINZ_CLOCK_FREQUENCY)
    #define SYSTM_CLOCK_FREQUENCY (CONFIG_SYSTEM_CLOCK)
	
	/* assume the target clock frequency is fc = 7.3728 MHz (73728000 Hz), then prescale 1024
	 * means the timer counter will be increased by 1 after 73728000/1024 = 72000 clocks. 
	 * one increasing equals to 1/72000 sec, namely 1/7.2 msec
	 *
	 * if we want the interval to be x milliseconds, then the timer counter should 
	 * change x / (1/7.2) times, which should be the difference between TCNT and OCR
	 *
	 * however, the real settins is different from the above calculation
	 * the real parameters are:
	 *   clock frequency = 7.3728 MHz
	 *   hope interval 20ms
	 *	 prescale = 1024
	 *   TCNT0 = 0x71
	 *	 OCR0 = 0x8F
	 *   actual value = 19.861 msec
	 * ref: http://www.avrvi.com/avr_examples/timer.html
	 * 
	 * for GAINZ platoform
	 * the target MCU's clock frequency is 8MHz.
	 * 
	 * formula: 
	 *	fc = 8 MHz
	 *  prescale = 1024
	 *  hopeinterval = 20 ms
	 *  
	 * then 
	 *  each increase equals to   prescale/fc seconds (=prescale/fc*1000 msec)
	 *  we need the timer count hopeinterval / (prescale/fc) times
	 * 
	 * final results
	 *  20 * (10^-3) * 8 * 10^6 / 1024 = 20 * 8 / 1.024 = 156.25
	 */

	timer->reginterval = SYSTM_HOPEINTERVAL * (SYSTM_CLOCK_FREQUENCY/1000) / SYSTM_PRESCALE;

	/* set the value for compare of timer 3 */
	OCR3A = timer->reginterval;
	OCR3B = 0x0000;
	OCR3C = 0x0000;

	/* set the initial counter value of timer 3 */
	TCNT3 = 0x0000; 

	/* timer/counter3 control register */
	// todo
	TCCR3A = 0x00;
	TCCR3B |= 0x08;//todo 工作在CTC模式
	//scale |= 0x08;
	scale |= 0x05;//todo
	timer->TCCR = (TCCR3B | scale); 
	TCCR3B |= scale; 
	TCCR3C = 0x00;

    //todo  TCCR3B控制16位定时器的工作模式，时钟频率
	//todo  只有在CTC等特定的模式下top值才等于OCR3A值  datasheet.122 table16 
    timer->TCCR = TCCR3B;//todo
    timer->OCR3 = OCR3A;
	#ifdef CONFIG_SYSTIMER_USE_HAL_ATTACH_HANDLER
	hal_attachhandler( CONFIG_SYSTIMER_INTERRUPT_NUMBER, _systimer_interrupt_handler, timer );
	#endif

	/* timer/counter3 output compare A match interrupt enable (OCIE3 bit) */
	ETIMSK |= 0x10;

	#endif /* CONFIG_SYSTIMER_USE_TIMER3 */

	m_systimer = timer;
	return timer;
}

inline void systm_close( TiSysTimer * timer )
{
	_systm_disable( timer );
	m_systimer = NULL;
}

void systm_start( TiSysTimer * timer )
{
	#ifdef CONFIG_SYSTIMER_USE_TIMER3
	//TCCR3A = timer->TCCR;
	TCCR3B = timer->TCCR;
	#endif
}

void systm_stop( TiSysTimer * timer )
{
	#ifdef CONFIG_SYSTIMER_USE_TIMER3
	//TCCR3A = 0x00;
	TCCR3B = 0x00;//todo
	#endif
}

inline void _systm_enable( TiSysTimer * timer )
{
	#ifdef CONFIG_SYSTIMER_USE_TIMER3
	/* ETIMSK: interrupt control 
	 * attention this register doesn't exist when choose atmega103 compatible mode
	 *
	 * bit 1 --- OCIE3C
	 * Timer/Counter3, Output Compare C Match Interrupt Enable 
	 * default value is 0
	 */
	ETIMSK |= 0x10;
	#endif
}

inline void _systm_disable( TiSysTimer * timer )
{
	#ifdef CONFIG_SYSTIMER_USE_TIMER3
	ETIMSK &= (~0x10);
	#endif
}

/* From Atmega128 datasheet
 * The 8-bit comparator continuously compares TCNTx with the Output Compare Register
 * (OCR0x). Whenever TCNTx equals OCRx, the comparator signals a match. A match will
 * set the output compare flag (OCFx) at the next timer clock cycle. If enabled (OCIEx = 1),
 * the output compare flag generates an output compare interrupt. The OCFx flag is auto-
 * matically cleared when the interrupt is executed. Alternatively, the OCF0 flag can be
 * cleared by software by writing a logical one to its I/O bit location. 
 *
 * @attention
 *	- this function should be used only in non-interrupt driven mode. because it will
 * clear the interrupt flag bit.
 */
inline bool systm_expired( TiSysTimer * timer )
{
	bool ret;

	#ifdef CONFIG_SYSTIMER_USE_TIMER3

	/* extended timer/counter interrupt flag register 
	 * Bit 1 - OCF3C: Timer/Counter3, Output Compare C Match Flag
	 * atmega 128 datasheet, p. 139
	 */
	ret = (bool)(ETIFR & 0x02);

	/* clear the interrupt flag bit */
	ETIFR |= 0x02;

	#endif

	return ret;
}


/******************************************************************************
 * interrupt related 
 *****************************************************************************/

/* @attention
 *	- this function will be called by the hal_invokehandler() function. so you 
 * must call hal_attachhandler() and configure the interrupt number - handler map
 * before you can call it successfully inside hal_invokehandler().
 *	- this function is still inside the interrupt context. 
 *  - you can distinguish the which timer raised the interrupt by the parameter "object".
 */ 
/*
void _systimer_interrupt_handler( void * object, TiEvent * e )
{
	TiSysTimer * OCR3 = (TiSysTimer *)object;
     
    // todo ??? here seems a bug.  OCR3 or OCR3A
    OCR3A = m_systimer->OCR3; //todo
	systimer->listener( systimer->lisowner, NULL );
}
*/
//todo
void _systimer_interrupt_handler( void * object, TiEvent * e )
{
	TiSysTimer * systimer = (TiSysTimer *)object;
    
	// todo ??? here seems a bug.  OCR3 or OCR3A
	//dbc_putchar( 0xdd);//todo for testing
	//OCR3A = systimer->OCR3; //todo
	systimer->time = systimer->time + SYSTM_HOPEINTERVAL;
	systimer->listener( systimer->lisowner, NULL );
}

#ifdef CONFIG_SYSTIMER_USE_TIMER3
#ifndef CONFIm_systimer_USE_HAL_ATTACH_HANDLER
/* hardware timer 3 interrupt service routine. it must be no. 26 for atmega128 micro-controller.
 * other formats maybe:
 * INTERRUPT(__vector_26)
 * ISR(__vector_26)
 * TIMER3_COMPA_vect
 */

/*
void __attribute((interrupt)) __vector_26(void);
void __vector_26(void)
{
	hal_assert( m_systimer != NULL );
	hal_invokehandler( 26, NULL );
}
*/
#endif
#endif


