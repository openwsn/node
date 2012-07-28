#ifndef _HAL_ASSERT_H_5748_
#define _HAL_ASSERT_H_5748_
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

/* 
 * @state
 *  compiled successfully. released. stable.
 *
 * @modified by zhangwei on 2009.07.11
 *
 * @modified by zhangwei on 2010.08.17
 *  - revision. 
 */

#include "hal_configall.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * An implementation of the assert mechanism for runtime debugging.
 */
#ifdef CONFIG_DEBUG
  #define hal_assert(v) hal_assert_report((v), __FILE__, __LINE__)
#else
  #define hal_assert(v) 
#endif

void hal_assert_report( bool cond, char * file, uint16 line );

/**
 * An implementation of the assert mechanism for compiling stage debugging. If the 
 * assertion failed, then the compiler will report "divide by 0 error".
 *
 * @reference
 * - Compile-Time Assertions, Dr. Dobbs Website, 2004, http://drdobbs.com/184401873
 * - static_assert, Visual Studio 2010, http://msdn.microsoft.com/en-us/library/dd293588.aspx
 * - Chapter 20. Boost.StaticAssert, http://www.boost.org/doc/libs/1_46_1/doc/html/boost_staticassert.html
 * - static_assert, http://www.pixelbeat.org/programming/gcc/static_assert.html
 * - Proposal to Add Static Assertions to the Core Language (Revision 3),
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1720.html
 * - C++0x features in VC2010 - static_assert, 
 *   http://blogs.msdn.com/b/xiangfan/archive/2010/05/09/c-0x-features-in-vc2010-static-assert.aspx
 */
#define assert_static(e) \
	do { \
		enum { assert_static__ = 1/(e) }; \
    } while (0)


// @todo
// the following is for STM32 OpenNode2010 only
// should be eliminated in the near future

#ifdef CONFIG_DEBUG
  #define hal_assert(v) hal_assert_report((v), __FILE__, __LINE__)
  //#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  #define assert_param(expr) hal_assert_report((expr), (uint8_t *)__FILE__, __LINE__)
#else
  #define assert_param(expr) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif
