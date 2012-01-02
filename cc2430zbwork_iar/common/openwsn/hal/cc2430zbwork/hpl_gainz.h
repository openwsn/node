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

#ifndef _HPL_GAINZ_H_4874_
#define _HPL_GAINZ_H_4874_

/* hpl_gainz.h
 * this module contains the declarations specific to ICT's GAINZ platform/target board 
 */

#include "hpl_atmega128.h"

#define CONFIG_GAINZ_CLOCK_FREQUENCY 8000000UL
#define CONFIG_GAINZ_CLOCK_FREQUENCY_KHZ (CONFIG_GAINZ_CLOCK_FREQUENCY/1000)
#define CONFIG_GAINZ_WIRELESS_FREQUENCY 2400

#ifndef F_CPU
#define F_CPU CONFIG_GAINZ_CLOCK_FREQUENCY
#endif

#define CONFIG_SYSTEM_CLOCK CONFIG_GAINZ_CLOCK_FREQUENCY

#include <util/delay.h>


#endif

