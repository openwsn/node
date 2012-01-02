#ifndef _RTL_TEXTSPLITER_H_2857_
#define _RTL_TEXTSPLITER_H_2857_
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


/* attention
 * This module reconized the following macros:
 *
 * CONFIG_DYNA_MEMORY
 * If this macro defines, then iobuf_create() and iobuf_free() will be enabled.
 */

#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_iobuf.h"

/* attention
 * - avoid the very long packet lead to deadlock in this object. The reason is:
 *   tspl_read() can only return a packet when the input buffer size is big enough 
 *   to hold an entire packet. However, if the input buffer isn't big enough, then
 *   the packet will continue stay inside the spliter and it will be never read out.
 */

//模块名称：rtl_textspliter(ANSI C, no depends)

#undef  TSPL_VERSION10
#undef  TSPL_VERSION20
#define TSPL_VERSION30

#define CONFIG_TSPL_PACKET_SIZE 255
#define TSPL_HOPESIZE(size) (sizeof(TiTextSpliter))
#define TSPL_RXBUF_SIZE (CONFIG_TSPL_PACKET_SIZE*2 + 4)

#define PAC_START_FLAG 0x55
#define PAC_END_FLAG 0x02

#define SPLITER_STATE_WAITFOR_START 0x01
#define SPLITER_STATE_WAITFOR_END 0x02

#define SPLITER_STATE_1 0x01
#define SPLITER_STATE_2 0x02
#define SPLITER_STATE_3 0x03
#define SPLITER_STATE_4 0x04

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TSPL_VERSION20
typedef struct{
	unsigned char buf[TSPL_RXBUF_SIZE];
	uint8 state;
	uint16 tmphead;
	uint16 tmprear;
	uint16 count;
}TiTextSpliter;
#endif

#ifdef TSPL_VERSION30
typedef struct{
	char txmem[ IOBUF_HOPESIZE(0xFF) ];
	char rxmem[ IOBUF_HOPESIZE(0xFF) ];

	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
	uint8 state;
	uint16 exp_len;
}TiTextSpliter;
#endif

#ifdef CONFIG_DYNA_MEMORY
TiTextSpliter * tspl_create( uintx pktsize );
void tspl_free( TiTextSpliter * split );
#endif

TiTextSpliter * tspl_construct( void * mem, uint16 size );
void tspl_destroy( TiTextSpliter * split );

void tspl_clear( TiTextSpliter * split );

// RX stream spliting.
// server（socket server or uart server）只要收到数据，
// 就可以把调用split_rxhandle处理之。
// Parameter-
// input: 	server（socket server or uart server）收到的数据
// len: 	传入数据的长度
// output:	完整的packet, 已经剔出Packet的START和END字符
// size: 	输出缓冲区大小，output buffer必须大到可以放下最长的packet
// return:	0 when not found a packet; >0 data length inside output buffer
#ifdef TSPL_VERSION10
uint16 tspl_rxhandle( TiTextSpliter * split, char * input, uint8 len, char * output, uint16 size );
#endif

#ifdef TSPL_VERSION20
uint16 tspl_rxhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output );
#endif

#ifdef TSPL_VERSION30
uint16 tspl_rxhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output, uint8 * success );
#endif
// TX stream assemble
// Parameter-
// input: 传入的packet，应是一个完整的packet
// output: 加装了START/END标记字符之后的packet
#ifdef TSPL_VERSION10
uint16 tspl_txhandle( TiTextSpliter * split, char * input, uint16 len, char * output, uint16 size );
#endif

#ifdef TSPL_VERSION20
uint16 tspl_txhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output );
#endif

#ifdef TSPL_VERSION30
uint16 tspl_txhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output );
#endif

#ifdef __cplusplus
}
#endif

#endif
