#ifndef _HAL_CPU_H_1675_ 
#define _HAL_CPU_H_1675_ 
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

/* @modified by zhangwei on 2010.05.15
 *  - add macro support CONFIG_TARGET_GAINZ, CONFIG_TARGET_CC2520DK, CONFIG_TARGET_CC2430DK.
 *    by using the above macros, this interface file can be used with gainz, cc2520 developing
 *    kit or cc2430 developing kit.
 *  - you should #undef or #define the above macros inside devx_configall.h or hal_configall
 *    to activate/deactivate the above configuration macros.
 */

/*******************************************************************************
 * for platform GAINZ supplied by ICT controlled by macro CONFIG_TARGET_GAINZ
 * gainz is provided by ICT in China.
 ******************************************************************************/

#ifdef CONFIG_TARGET_GAINZ
#include "./gainz/hpl_cpu.h"
#endif 

/*******************************************************************************
 * for platform cc2520 developing kit controlled by macro CONFIG_TARGET_CC2520DK
 * cc2520dk is supplied by TI.com
 ******************************************************************************/

#ifdef CONFIG_TARGET_CC2520DK
#include "./cc2520dk/hpl_cpu.h"
#endif 

/*******************************************************************************
 * for platform cc2430 developing kit controlled by macro CONFIG_TARGET_CC2430DK
 * cc2430dk is supplied by TI.com
 ******************************************************************************/

#ifdef CONFIG_TARGET_CC2430DK
#include "./cc2430dk/hpl_cpu.h"
#endif 

#endif /* _HAL_CPU_H_1675_ */
