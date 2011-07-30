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
/******************************************************************************
 * rtl_stochastic
 * This module realizes some statistic functions.
 *
 * @author sunqiang(TongJi University) on 20091123
 *	- first created.
 *****************************************************************************/ 
#ifndef _RTL_RANDOM_H_3425_
#define _RTL_RANDOM_H_3425_

#include "rtl_foundation.h"
#include <stdlib.h>

/***************************************************************************** 
 * Higher-level functions of statistic
 *****************************************************************************/

double rand_uniform(double x1, double x2);
double rand_normal(double avg, double sigma);
int rand_poisson(double lamda);
double rand_triangle(double a, double b);

#endif