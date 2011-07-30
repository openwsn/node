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
 
#ifndef _HAL_VIC_H_4788_
#define _HAL_VIC_H_4788_

/* hal_vic
 * Vector Interrupt Controller(VIC) Management. VIC is usually a IO device inside 
 * CPU. It's different from the global interrupt flag inside the CPU's controller unit. 
 *
 * attention in a lot of MCU, there're no Vector Interrupt Controller support 
 * inside them. This module is currently used with ARM CPU only.
 *
 * @attention
 * there's also an interrupt abstraction and management module in OS layer. that 
 * module provides a common multi-interrupt interface for all the MCU/CPUs. while
 * the VIC module here is only an hardware's interface.
 *
 * @author zhangwei on 2008-12-19
 *	first version.
 */

#include "hal_foundation.h"
#include "..\src\target.h"

#ifdef CONFIG_OS_UCOSII
#include "..\arm\os_cpu.h"
#endif

/* for ARM CPU only:
 * opt
 * opt = 0x00 default
 *   [b7,..,b0]
 *   b0 = 0 general interrupt, 1 FIQ interrupt (ARM only)
 *   b1 = 0 voltage trigger, 1 pulse trigger
 */
void hal_vic_init( uint8 opt );
void hal_vic_attach( uint8 num, TInterruptHandler isr, uint8 prior, uint8 opt );
void hal_vic_detach uint8 num );
void hal_vic_trigger( uint8 num );
void hal_vic_enable( uint8 num );
void hal_vic_disable( uint8 num );
/* by inquire the state register, the developer can know whether there's an
 * interrupt request pending, or in servicing or masked.  */
uint8 hal_vic_state( uint8 num );  
void hal_vic_clearrequest( uint8 num );

#endif
