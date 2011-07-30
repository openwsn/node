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

#ifndef _HAL_INIT_H_6798_
#define _HAL_INIT_H_6798_

/*******************************************************************************
 * hal_osxbase.h
 * this file is used to support the running of osx kernel. it collects the necessary
 * components and utility functions to support osx. if you want to port the osx
 * kernel to a new platform, you only need to re-develop hal_osxbase.h, hal_osxbase.c
 * and very few assemble source codes.
 * 
 * @state
 *	developing
 * 
 * @history
 * @author zhangwei on 2010.05.08
 *  - first created
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"

#include "hal_cpu.h"
#include "hal_systimer.h"

/* Q: what's the difference between boot() or init()?
 * R: the startup process is as the following:
 *      boot => init => osx kernel run
 *
 * hal_boot module deals with the booting process only. then it will call init().
 * and init() will initialize the architecture related configuratioins and finally 
 * pass the CPU control to osx kernel. attention the init() doesn't do all the configuration
 * of the target board.
 */


#endif
