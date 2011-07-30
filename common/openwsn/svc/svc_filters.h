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

/* Interface: TiFilter
 * xxx_leftexec(TiFilter * filter, char * inbuf, uint8 input_len, char * outbuf, uint8 outbuf_size )
 * xxx_rightexec(TiFilter * filter, char * inbuf, uint8 input_len, char * outbuf, uint8 outbuf_size )
 */

typedef uint8 (* TiFilterLeftExecute)(void * filter, char * inbuf, uint8 input_len, char * outbuf, uint8 outbuf_size );
typedef uint8 (* TiFilterRightExecute)(void * filter, char * inbuf, uint8 input_len, char * outbuf, uint8 outbuf_size );

typedef struct{
}TiIoFilters;

iofilters_leftexecute(void * filter, char * inbuf, uint8 input_len, char * outbuf, uint8 outbuf_size )
{
	flt;
	leftexec( flt, inbuf, len, outbuf, outbuf );
	for each filter do
		flt = ;
		leftexec = ;
		leftexec( flt, outbuf, len, outbuf, len );
	end
}






lread
lwrite
rread
rwrite


attach
detach

iofilter
iohandler


TiFilters
{
}

flt_lread
flt_lwrite
flt_rread
flt_rwrite

flt_insertfilter

TiFilter 
同样是4个R/W函数
rprocess


filters_process
filters_
char buf[]

l

定义一种新的数据类型叫

sysint_t  自动根据系统架构调整大小，例如8bit, 16bit或32bit
sysuint_t 

SYSINT
SYSINTU
SYSUINT
INTX
INTY
INTX
UINTX

INT
UINT


// push mode
flt_lwrite( TiIoFilterchar * buf, uint16 len )
{
	for each filter in the list
		flt = 
		flt_lwrite// flt_setbuf
		flt_rread // flt_getbuf 
	end
	// for the last one is iohandler, there's no rread
	handler_lwrite( buf, )
}

flt_setbuf
flt_getbuf

// pull mode
flt_lread()
{
	flt_rsetbuf
	for each buffer
	process
	return buf
}

对每个filter，似乎都需要提供上述4个function
但是对filters
似乎只要两个即可
lexec/levolve( object, inbuf, len )  没有output参数，是因为最后一级的iohandler会包括output，不需在filters中考虑
rexec/revolve

也可以提供output buffer参数，这样就无须为filter提供iohandler，因为主程序会处理之
while (1)
	uart_read
	flt_lwrite/lexec( COM_FILTER, 
	process buffer data in the main thread => is actually the iohandler
	flt_rwrite/rexec( RF_FILTER, data )
	wls_write
end while

或者
通过notifier连接uart和filter





