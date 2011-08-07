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
#ifndef _RTL_SPLITFILTER_H_6784_
#define _RTL_SPLITFILTER_H_6784_

#include "rtl_foundation.h"
#include "rtl_iobuf.h"

/*******************************************************************************
 * TiSpliterFilter
 * 对收到的数据流进行分frame处理
 * rxlen 记录当前frame的长度
 * 该程序可自动发现不规范的frame，例如frame不够长度（过短）或者超长（有多余字节）
 * 并丢弃之。输出的frame一定是一个合法的frame.
 *
 * this split filter can be used with socket stream and uart stream
 *
 * @state
 *	finished. still needs fully testing
 *
 * author zhangwei on 20090526
 *	- first version
 ******************************************************************************/

#define SPLITFILTER_SOF 0x01

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8 state;
	uintx rxlen;
}TiSplitFilter;

/* attention
 * txbuf is actaully not used in this object. it maybe eliminate in the future
 */

#ifdef CONFIG_DYNA_MEMORY
TiSplitFilter *     split_create( uintx rxbufsize, uintx txbufsize );
#endif

#ifdef CONFIG_DYNA_MEMORY
void 				split_free( TiSplitFilter * split );
#endif

TiSplitFilter *     split_open( void * mem, uintx size, uintx rxbufsize, uintx txbufsize );
void 				split_close( TiSplitFilter * split );
TiIoBuffer * 		split_rxbuf( TiSplitFilter * split );
TiIoBuffer * 		split_txbuf( TiSplitFilter * split );
uintx 				split_rxhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output );
uintx               split_txhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output );

#ifdef __cplusplus
}
#endif

#endif
