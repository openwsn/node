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

#ifndef rtl_base64H
#define rtl_base64H
/***************************************************************************
 * @state
 *	finished developing. should work. but not through tested.
 *
 * @reference
 *  http://www.cstc.net.cn/docs/docs.php?id=202
 *  浅谈Base64编码
 **************************************************************************/

#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"

/***************************************************************************
 * base64编码表
 *
 *		0 A 17 R 34 i 51 z
 *		1 B 18 S 35 j 52 0
 *		2 C 19 T 36 k 53 1
 *		3 D 20 U 37 l 54 2
 *		4 E 21 V 38 m 55 3
 *		5 F 22 W 39 n 56 4
 *		6 G 23 X 40 o 57 5
 *		7 H 24 Y 41 p 58 6
 *		8 I 25 Z 42 q 59 7
 *		9 J 26 a 43 r 60 8
 *		10 K 27 b 44 s 61 9
 *		11 L 28 c 45 t 62 +
 *		12 M 29 d 46 u 63 /
 *		13 N 30 e 47 v (pad) =
 *		14 O 31 f 48 w
 *		15 P 32 g 49 x
 *		16 Q 33 h 50 y
 *
 * base64编码步骤：
 *
 *		原文：
 *
 *		你好
 *		C4 E3 BA C3
 *		11000100 11100011 10111010 11000011
 *		00110001 00001110 00001110 00111010
 *		49       14							（十进制）
 *		x        O        O        6		字符
 *		01111000 01001111 01001111 00110110
 *		78									（十六进制）
 *		xOO6
 *
 *		解码：
 *		xOO6
 *		78 4f 4f 36
 *		01111000 01001111 01001111 00110110
 *		49       14
 *		00110001 00001110 00001110 00111010   31 0e 0e 3a
 *
 *		11000100 11100011 10111010
 *		C4       E3       BA
 **************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

// 编码后的长度一般比原文多占1/3的存储空间，请保证base64code有足够的空间
//inline int base64_encode(char * base64code, const char * src, int src_len = 0);
//inline int base64_decode(char * buf, const char * base64code, int src_len = 0);
inline int base64_encode(char * base64code, const char * src, int src_len);
inline int base64_decode(char * buf, const char * base64code, int src_len);


#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
#endif
