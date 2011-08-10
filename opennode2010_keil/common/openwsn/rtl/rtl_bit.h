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

#ifndef _RTL_BITS_H_4252_
#define _RTL_BITS_H_4252_

/*****************************************************************************
 * @name rtl_bits.h
 * @author zhangwei on 20070331
 * 
 * bit manipulation macros in runtime library(RTL)
 * 
 * @history
 * @modified by zhangwei on 200704
 * @modified by zhangwei on 200905
 *	- revision
 * @modified by zhangwei on 20100109
 * 	- revision
 * 
 ****************************************************************************/ 

/* reference
 * - BV operation, http://linux.die.net/man/3/_bv
 */

#define BM(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))

#ifndef _BV
#define _BV(index) (1 << (index)) 
#endif

//#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
//#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
//#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
//#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit)) 

#define bit_set(token,index) ((token) |= _BV(index))
#define bit_get(token,index) ((token) & _BV(index))
#define bit_clr(token,index) ((token) &= (~_BV(index))

#ifndef sbi
  #define sbi(port,index) ((port) |= _BV(index))
#endif

#ifndef cbi
  #define cbi(port,index) ((port) &= (~_BV(index))
#endif


// #define bit_range

/* using macro, please
bit_rshift
bit_lshift
bit_rrotate
bit_lrotate
bit_bic
bit_rmw
bit_xor
bit_rotate

*/

#endif

