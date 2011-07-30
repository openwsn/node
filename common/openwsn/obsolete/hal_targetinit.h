#ifndef _HAL_TARGETINIT_H_7483_
#define _HAL_TARGETINIT_H_7483_
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

/**
 * Q: What's the differene between module hal_targetboard and hal_targetinit?
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
 * 
 * The booting process is actually the following:
 *  1) target_startup() in hal_startup module
 *  2) main() function
 *  3) target_init() function in hal_targetinit module.
 * 
 * Q: Why we don't merge the hal_targetinit and hal_targetboard module as one module?
 * considering target_init() is actually a function belong to target module.
 * R: We separate these two because hal_targetinit is related to osx kernel startup.
 * If you want to port osx kernel to another hardware architecture, you can simply port
 * the hal_targetinit module without affected by the complicated targetboard module.
 */

void target_init( void );
void target_reset( void );

#endif /* _HAL_TARGETINIT_H_7483_ */



