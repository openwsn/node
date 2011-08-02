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
#include <string.h>
#include "../hal/hal_debugio.h"
#include "rtl_foundation.h"
#include "rtl_textspliter.h"

TiTextSpliter * tspl_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiTextSpliter) <= size );
	memset( mem, 0x00, size );
	TiTextSpliter * split = (TiTextSpliter *)(mem);
	
	split->state = SPLITER_STATE_WAITFOR_START;
	split->tmphead = 0;
	split->tmprear = 0;

	return split;
}

TiTextSpliter * tspl_destroy( TiTextSpliter * split )
{
	return NULL;
}


/* tspl_read
 * This function should be called every time new data received. The new data is indicate 
 * by the parameter "input". If the rxhandle can find an entire frame/packet inside the
 * internal buffer, then it will move the frame/packet into "output" buffer. The return
 * value is the length of the frame/packet in the output buffer. If there's no packet/frame
 * found, then 0 will be returned.
 *
 * Example
 *   char buf[128];
 *   char packet[128]
 *   while (1)
 *   {
 *     len = uart_read( g_uart, buf, 128, 0x00 );
 *     if (len > 0)
 *     { 
 *       if (tspl_rxhandle(split, buf, len, packet, 128) > 0)
 *       {
 *          push the packet into a queue for later processing
 *       }
 *     }
 *   }
 *
 * In another thread, you can visit the queue to get the incoming packets.
 */
uint16 tspl_read( TiTextSpliter * split, char * input, uint8 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 pac_head = 0;
	uint16 pac_end = 0;

	//把input推入splitbuf队列中
	memmove(&split->buf[split->tmprear], input, len);
	split->tmphead = split->tmprear;
	split->tmprear += len ;


	//=> please change the variable name from option to state
	//=> 虽然简单,但这里也是小状态机的思想
	//=> 没细看这段程序,但基本的套路,用switch+当前状态判断的思想是正确的

	switch (split->state)
	{
		case SPLITER_STATE_WAITFOR_START://未收到PAC_START_FLAG
		
			for(i=0;i<split->tmprear;i++)//从头寻找PAC_START_FLAG；
			{
				if(split->buf[i]==PAC_START_FLAG)
				{
					split->state = SPLITER_STATE_WAITFOR_END;
					pac_head = i+1;
					break;
				}
			}

			if(pac_head==0)//没有收到过PAC_START_FLAG
			{
				split->tmphead = 0;
				split->tmprear = 0;//清掉buf
				return 0;
			}
			else// if( pac_head > 0 )//收到了PAC_START_FLAG
			{ 
				for(i=pac_head;i<split->tmprear;i++)//如果收到PAC_START_FLAG，则寻找此字节流中是否存在PAC_EDN_FLAG；
				{
					if(split->buf[i]==PAC_END_FLAG)
					{
						split->state = SPLITER_STATE_WAITFOR_START;
						pac_end = i;
						break;
					}
				}

				if(pac_end>0)//收到PAC_END_FLAG
				{
					memmove(output, &split->buf[pac_head], pac_end-pac_head);//取出packet
					memmove(&split->buf[0], &split->buf[pac_end+1], len-pac_end-1);//无用数据清除，有用数据前移
					split->tmprear -= (pac_end+1);
					return pac_end-pac_head;//下次仍进入此case
				}
				else //收到过PAC_START_FLAG但是没有PAC_END_FLAG
				{
					memmove(&split->buf[0], &split->buf[pac_head], len-pac_head);//无用数据清除，有用数据前移
					split->tmprear -= pac_head;
					return 0;//下次不进入此case
				}

			}
			break;

		case SPLITER_STATE_WAITFOR_END://如果收到过PAC_START_FLAG，却没收到PAC_END_FLAG

			for(i=split->tmphead;i<split->tmprear;i++)//直接寻找PAC_END_FLAG，
			{
				if(input[i]==PAC_END_FLAG)
				{
					split->state = SPLITER_STATE_WAITFOR_START;
					pac_end = i;
					break;
				}
			}

			if(pac_end==0)//如果没有找到
			{
				return 0;
			}
			else 
			{
				memmove(output, &split->buf[0], pac_end);//取出packet
				split->tmprear = 0;
				return pac_end;						
			}

			break;

			default:
				return 0;
				break;
	}
}



uint16 tspl_write( TiTextSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	rtl_assert( size > len+2 );
	output[0] = PAC_START_FLAG;
	memmove( &output[1], input, len );
	output[len] = PAC_END_FLAG;
	return len+2;

}
