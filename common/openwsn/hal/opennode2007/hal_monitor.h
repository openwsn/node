/******************************************************************************
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
 *****************************************************************************/ 

#ifndef _HAL_MONITOR_H_4236_
#define _HAL_MONITOR_H_4236_

/******************************************************************************
 * @author zhangwei on 2006-09-12
 * External Event Monitor
 * 
 * this module is used to monitor the external hardware event. this is related 
 * to the power mode of the MCU. ideally, the MCU should run in sleep mode or 
 * even power down mode until an external hardware event(usually the interrupt)
 * wakes it. when it wakes up, it may need to do some necessary processing to 
 * recover its state. you can do it in this module.
 * 
 * usually, the monitor's handler is default interrupt handler of all interrupts.
 * then you can do some common processing in monitor's handler and then dispatch
 * the interrupt requests to each interrupt handler.
 * 
 * usually there's only 1 handler monitor handler in the system. however, this
 * module doesn't restrict you to have only 1.
 * 
 * the above features quite appropriate for NIOS II and PIC MCU, because the 
 * hardware only support 1 interrupt handler. while, for ATMEGA and ARM, the 
 * hardware support multiple interrupts. for these MCU, you can still share 
 * the same interrupt handler or let each hardware owns their own interrupt handler.
 * 
 * @attention
 * this module is high hardware related! you should modify it if you want to 
 * port to other platforms! 
 * 
 *****************************************************************************/

void mon_construct( void );
void mon_destroy( void );
void mon_install_handler( void );
void mon_common_handler( void );
void mon_dispatch( void );

/*
void interrupt_init( void );
void interrupt_enable( uint8 id );
void interrupt_disable( uint8 id );
void interrupt_enable_global( void );
void interrupt_disable_global( void );
*/
#endif
