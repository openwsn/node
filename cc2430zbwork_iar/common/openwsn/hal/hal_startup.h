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
#ifndef _HAL_STARTUP_5874_
#define _HAL_STARTUP_5874_

/*******************************************************************************
 * hal_startup.
 * This module contains the C source code used when booting up a system. Since
 * these functions may also provided by the developing environments, this file
 * maybe empty for some platforms
 *
 * This module is used to initialize necessary components to be used by the osx kernel.
 * You should call the init functions here before calling osx startup. If you don't 
 * use the osx kernel, you needn't use this module.
 *
 * @modified by zhangwei in 2009.08
 * - replace the previous hal_boot module.
 * 
 * @modified by zhangwei on 2010.12.03
 *
 ******************************************************************************/


/**
 * @attention
 * - Generally, you needn't call this module in your project after main()
 * - This modules is called when the application started.
 * - a lot of developing environment has already provide startup source code.
 *   so this module is often unnecessary.
 */

#endif

