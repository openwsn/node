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

#ifndef _RTL_CRC_H_4278_
#define _RTL_CRC_H_4278_

/*******************************************************************************
 * @author zhangwei on 2005-07-19
 * @note: Interface file CRC check module.
 *	thanks Ruijie to give the original source code (first version). 
 * 
 * @history
 * 2005-07-19 first created by zhangwei
 * @modified by zhangwei on 20061030
 * revised and released as part of RTL
 *
 * @modified by openwsn on 2011.03.26 
 * - Merged the fast version and slow into one module. Also add the configuration
 *   macro CONFIG_CRC_FAST_VERSION and CONFIG_CRC_SLOW_VERSION to help choosing
 *   different implementation.
 *
 ******************************************************************************/

/**
 * CONFIG_CRC_FAST_VERSION 
 * Configure the CRC module to use the fast version.
 *
 * CONFIG_CRC_SLOW_VERSION
 * Configure the CRC module to use the slow version.
 */
 
#include "rtl_configall.h"
#include "rtl_foundation.h"

/* By default, the CRC module is configured to use the fast version. But this version
 * will occupy a lot of memory. */
 
#ifndef CONFIG_CRC_FAST_VERSION
  #ifndef CONFIG_CRC_SLOW_VERSION
    #define CONFIG_CRC_FAST_VERSION
  #endif
#endif


/*******************************************************************************
 * calculate the CRC checksum
 * @param buf, 帧缓冲区的首指针，指向第一个待校验的字符
 * @param number, 待校验帧的字符个数
 * @return fcsOut, CRC校验码
 *   另外，本函数在输入buf缓冲区的末尾添加了CRC校验码  
 ******************************************************************************/
extern unsigned short crc_produce(unsigned char *buf, unsigned short number);

/*******************************************************************************
 * do CRC verification
 * @param buf,  帧缓冲区的首指针，指向第一个已校验的字符
 * @param number, 已校验帧的字符个数，可以包括CRC校验码
 * @return fcsOut, CRC校验码
 *                    另外，当检验帧包含CRC校验码时，CRC校验正确时输出fcsOut为0  
 ******************************************************************************/
extern unsigned short crc_check(unsigned char *buf, unsigned short number);

#endif
