/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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
#include "hal_interrupt.h"

/*****************************************************************************
 * @modified by zhangwei on 20070627
 * revision the interrupt related source code. collected them into this module. 
 ****************************************************************************/

/* reference
 * μC/OS-II中缩短中断关闭时间的方法, 2007
 * http://www.guangdongdz.com/special_column/techarticle/200710/199441827823.html
 */

#if (!defined(CONFIG_OS_UCOSII))
  #define CONFIG_OS_DEFAULT
#endif  

uint8 g_atomic_level;

void _hal_enable_interrupts()
{
#ifdef CONFIG_OS_UCOSII
	IRQEnable();
#endif

#ifdef CONFIG_OS_DEFAULT
	_hal_irq_enable();
#endif
}

void _hal_disable_interrupts()
{
#ifdef CONFIG_OS_UCOSII
	IRQDisable();
#endif

#ifdef CONFIG_OS_DEFAULT
	_hal_irq_disable();
#endif
}

/*
hal_interrupt_attach
hal_interrupt_detach
hal_interrupt_initialize

TiInerruptController

irc_  
*/




/* removed from this project since 200812 */

/* @modified by zhangwei on 20070425
 * enble irq and fiq of arm
 * 		MRS	r1, cpsr		
 * 		BIC r1, r1, #0xC0
 * 		MSR	cpsr_c, r1
 * 
 * disable irq and fiq of arm
 * 		MRS	r1, cpsr
 * 		ORR	r1, r1, #0xc0
 * 		MSR	cpsr_c, r1
 */

  /* TODO hal_enter_critical() and hal_leave_critical() are essentially wrong!
   */

void hal_enter_critical( void )
{
#ifdef CONFIG_OS_UCOSII
   // @TODO 20061025
   // zhangwei comment the following to make it compile pass
  //IRQ_FLAG_STORE = IRQ_FLAG;
	IRQDisable();
#endif

#ifdef CONFIG_OS_DEFAULT
	hal_irq_disable();
	hal_fiq_disable();
#endif
}

void hal_leave_critical( void )
{
#ifdef CONFIG_OS_UCOSII
  //if(IRQ_FLAG_STORE) IRQDisable();	
  //else               IRQEnable();		
	IRQEnable();
#endif

#ifdef CONFIG_OS_DEFAULT
	hal_irq_enable();
	hal_fiq_enable();
#endif
}
