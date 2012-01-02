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
#ifndef _RTL_TEXTCODEC_H_4789_
#define _RTL_TEXTCODEC_H_4789_
/*******************************************************************************
 * rtl_textcodec
 * 
 * @history
 * @author yan-shixing on 20091220
 *	- first created
 ******************************************************************************/

// 模块名称：rtl_textcodec(ANSI C, no depends)
// encode/decode的单位是packet，不必对不完整的packet做此操作

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_iobuf.h"
/* CONFIG_TEXTCODEC_TEST
 * testing the codec. the input will be directly copied to the output.
 */

#undef  CONFIG_TEXTCODEC_TEST
//#define CONFIG_TEXTCODEC_TEST

#ifdef __cplusplus
extern "C" {
#endif

uint16 text_encode( TiIoBuf * input, TiIoBuf * output );//数据编码
// 成功时返回output缓冲区中数据长度，失败返回0

uint16 text_decode( TiIoBuf * input, TiIoBuf * output );//数据解码
// 成功时返回output缓冲区中数据长度，失败返回0


#ifdef __cplusplus
}
#endif

#endif
