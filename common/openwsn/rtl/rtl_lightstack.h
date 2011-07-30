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
/*
 * rtl_stack.h
 * this version of stack data structure is implemented by using TiVector
 *
 * @author zhangwei in 200803
 * @modified by zhangwei in 200803
 */

#ifndef _RTL_STACK_H_5998_
#define _RTL_STACK_H_5998_

#include "rtl_vector.h"

#define TiStack TiVector

#define stk_construct(buf,size,itemsize) vec_construct((buf),(size),(itemlen))
#define stk_destroy(stk) vec_destroy(stk)
#define stk_push(stk) vec_pushfront(stk)
#define stk_pop(stk) vec_popfront(stk)
#define stk_top(stk) ((stk->count>0) ? vec_getbuf(stk,stk->count) : NULL)
#define stk_empty(s) vec_empty(s)
#define stk_full(s) vec_full(s)

#endif /* _RTL_STACK_H_5998_ */
