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
#ifndef _RTL_RANDOM_H_3425_
#define _RTL_RANDOM_H_3425_

/******************************************************************************
 * rtl_random
 * This module implements a random generator.
 *
 * @author zhangwei(TongJi University) on 20070130
 *	- first created.
 * 
 * @history
 * @modified by zhangwei on 20091102
 *	- revision
 * @modified by Sun Qiang(TongJi University) on 20091112
 *  - revision
 * @modified by Sun Qiang(TongJi University) on 20091123
 *  - remove higer statistic functions to new module rtl_stochastic
 *
 *****************************************************************************/ 
#include "rtl_foundation.h"
#include <stdlib.h>

/* this is the default initial seed. You can change it by calling rand_init(newseed)  */
#define RANDOM_INITIAL_SEED 0xB6A3

/* attention:
 * assume the target device supports 16-bit operations.
 */

#define rand_init(n) rand_open(n)
#define rand_next() rand_read()

/***************************************************************************** 
 * Basic functions of generating random nums
 *****************************************************************************/
void rand_open( uint16 n );
uint16 rand_read();
void rand_close();

/* Generate 8-bit,16-bit,32-bit random nums
 */
uint8 rand_uint8( uint8 n );
uint16 rand_uint16( uint16 n );
uint32 rand_uint32( uint32 n );

/* Generate a random float/double random num in [0,1) 
 */
float rand_float();
double rand_double();

#endif

