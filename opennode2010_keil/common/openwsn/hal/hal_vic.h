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
 * @author zhangwei on 2007-12-19
 *	- first version.
 * @modified by zhangwei on 20090715
 *	- revision
 */

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_targetboard.h"

// #include "..\src\target.h"

#ifdef CONFIG_OS_UCOSII
// #include "..\arm\os_cpu.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif
/* for ARM CPU only:
 * opt
 * opt = 0x00 default
 *   [b7,..,b0]
 *   b0 = 0 general interrupt, 1 FIQ interrupt (ARM only)
 *   b1 = 0 voltage trigger, 1 pulse trigger
 */
void hal_vic_init( uint8 opt );
void hal_vic_attach( uint8 num, TiFunInterruptHandler isr, uint8 prior, uint8 opt );
void hal_vic_detach( uint8 num );
void hal_vic_trigger( uint8 num );
void hal_vic_enable( uint8 num );
void hal_vic_disable( uint8 num );

/* by inquire the state register, the developer can know whether there's an
 * interrupt request pending, or in servicing or masked.  */
uint8 hal_vic_state( uint8 num );  
void hal_vic_clearrequest( uint8 num );

#ifdef __cplusplus
}
#endif
#endif
