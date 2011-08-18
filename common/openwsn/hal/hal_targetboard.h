#ifndef _HAL_TARGETBOARD_H_7C83_
#define _HAL_TARGETBOARD_H_7C83_
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

#include "hal_configall.h"
#include "hal_foundation.h"


/* Hardware Platform Layer
 * including CPU/MCU specific source codes
 * 
 * Reference 
 * Hardware Abstraction Architecture, http://www.tinyos.net/tinyos-2.x/doc/html/tep2.html
 */

/* Q: What's the differene between module hal_targetboard and hal_targetinit?
 * R: hal_targetboard provides a set of utility functions to operate the targetboard.
 * While, the hal_targetinit module provides only one function "target_init" which
 * encapsulate the source from target startup to osx kernel startup.
 * 
 * Q: What's the difference between target_startup() in hal_startup module and target_init()
 * function in hal_targetinit module?
 * R: target_startup() is called automatically when the system is powered on. It's called
 * by the hardware and depends on the CPU architecture. For a lot of CPU, the developing 
 * environment has already provides their own startup source code, so the target_startup()
 * is unnecessary.
 *    Function target_init() is usually called as the first function in main() function. 
 * So it's always after target_startup(). This function is often called by osx kernel when
 * the kernel is startup to perform initialization process. If you don't use the osx 
 * kernel in your application, you can simply call target_init() in main() function.
 */

#ifdef CONFIG_TARGETBOARD_GAINZ
//#include "hal_configall.h"
//#include "./gainz/hpl_atmega128.h" 
#include "./gainz/hpl_gainz.h"
#endif

#ifdef CONFIG_TARGET_CC2520DK
#include "./cc2520/hpl_cc2520dk.h"
#endif

#ifdef CONFIG_TARGET_CC2430DK
#include "./cc2430/hpl_cc2430dk.h"
#endif

void target_init( void );
void target_reset( void );



/* Q: what's the difference between boot() or init()?
 * R: the startup process is as the following:
 *      boot => init => osx kernel run
 *
 * hal_boot module deals with the booting process only. then it will call init().
 * and init() will initialize the architecture related configuratioins and finally 
 * pass the CPU control to osx kernel. attention the init() doesn't do all the configuration
 * of the target board.
 */

#endif /* _HAL_TARGETBOARD_H_7C83_ */
