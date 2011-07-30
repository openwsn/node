#ifndef _RTL_ASSERT_H_7689_
#define _RTL_ASSERT_H_7689_
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


#include "rtl_configall.h"

/*****************************************************************************
 * @name rtl_assert.h
 * @author zhangwei on 20100103
 * 
 * assert support for the runtime library.
 * In the past, the rtl_assert() is support inside rtl_foundation. However, the 
 * assert implementation depends on hal layer modules. In order to break this 
 * dependency, i move the rtl_assert() from rtl_foundation and create this file.
 * 
 * In all the rtl library modules, this module is the only one depends on modules
 * outside of rtl.
 * 
 * 
 ****************************************************************************/ 

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DEBUG
    //#include "../hal/hal_assert.h"
    //#define rtl_assert(cond) hal_assert((cond))
    #define rtl_assert(cond)
#else
    #define rtl_assert(cond)
#endif

/******************************************************************/
#ifdef __cplusplus
}
#endif

#endif
