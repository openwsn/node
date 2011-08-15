#ifndef _SVC_FOUNDATION_H_4278_
#define _SVC_FOUNDATION_H_4278_
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

/*****************************************************************************
 * @author zhangwei on 2006-08-20
 * this is the foundation file of service layer.
 * every service modules in this directory should include this one as their first file.
 *
 *****************************************************************************/

#include "svc_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../hal/hal_assert.h"

/*
#ifdef CONFIG_WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif
*/

#define svc_assert(cond) hal_assert(cond)

#endif

