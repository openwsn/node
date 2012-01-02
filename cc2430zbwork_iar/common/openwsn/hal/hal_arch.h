#ifndef _HAL_ARCH_H_8932_
#define _HAL_ARCH_H_8932_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

/*******************************************************************************
 * hal_arch.h
 * hardware hierachical layer is:
 * 
 * hal_arch / hal_cpu => mcu such hpl_atmega128 => hal_targetboard
 * 
 * target board often includes microprocessors and other independent IC's
 * an MCU includes CPU and other integrated peripherals.
 * an series CPU may share a same architecture. 
 * 
 * Q: what's the difference among hal_arch, hal_cpu, hpl_<mcu chip> and hal_targetboard?
 * R: 
 * - hal_arch: a series of CPU/MCU may shares the same architecture, such as 
 *   80x86 architecture, ARM architecture, PIC architecture, etc.
 * - hal_cpu: even though some CPU shares the same architecture, there maybe slightly
 *   some difference. they're in hal_cpu module. usually, we can merge hal_arch into
 *   hal_cpu. 
 * - hal_<mcu>: mcu = cpu + in-chip pheriphal devices. 
 * - hal_targetboard: mcu + indepedent external chip devices
 * 
 ******************************************************************************/

#endif