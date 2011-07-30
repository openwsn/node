/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 

/*******************************************************************************
 * @author zhangwei on 20060813
 * TiSioComm
 * Serial I/O Communication
 * based on TUartDriver
 * 
 * support both the stream interface and frame interface.
 * however, you'd better use one type interface only at the same time. 
 *
 * 构建在TUartDriver基础之上，Uart相当于物理层，SIO相当于Data Link Layer功能
 * 包括串口frame的区分功能
 * 内含一个供TUartDriver使用的通信缓冲区
 * 如果主程序来不及读数据，则可能导致该内部通信缓冲区种数据丢失。
 * 
 * TiUartAdapter => TiSioComm => TConsole
 *                            and other applications
 * 
 * @modified by zhangwei on 20061015
 * revision and released version 3, not test yet!
 * problem: shall i keep the length byte of the frame in the buffer?
 ******************************************************************************/

#include "svc_configall.h"
#include "..\hal\hal_foundation.h"
#include <../hal/hal_assert.h>
#include "..\hal\hal_uart.h"
#include "..\hal\hal_assert.h"
#include "svc_foundation.h"
#include "svc_siocomm.h"
 
typedef struct{
	char rxbuf;
}TiFixedByteFrameConvertor;

encoder
decoder

nbfc_construct
nbfc_rxhandle( bfc, inputbuf, outputbuf )
nbfc_rxhandle( bfc, in, out  )
nbfc_getinterface
nbfc_setinterface

typedef struct{
  TiUartAdapter * uart;
  uint8 rxlen;
  uint8 txlen;
  char rxbuf[SIO_RXBUFFER_CAPACITY];
  char txbuf[SIO_TXBUFFER_CAPACITY];  
  uint8 option;
  TiTimerAdapter * timer;
  TiFixedByteFrameConvertor * bfc;
}TiSioComm;

return 0 nothing
return >0 data ready
return -1 failed and crc error
return -2 failed

sio_read( sio, data, size, option )
sio_write( sio, data, len, option )

frame = {header, length, data, crc }

typedef TiSioFrame
{
	uint8 size;
	uint8 datalen;
	uint8 frame;
	uint8 * payload;
	uint8 * crc;
}




TiSioComm * sio_construct( char * buf, uint16 size, TiUartAdapter * uart, uint8 opt );
void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt, uint8 sof );
void sio_destroy( TiSioComm * sio );

uint8 sio_read( TiSioComm * sio, char * payload, uint8 size, uint8 opt );
{
	move data froom sio->rxbuf to output buf
}

uint8 sio_write( TiSioComm * sio, char * payload, uint8 len, uint8 opt );
{
	move data from input buffer to txbuf if txbuf is empty
}

sio_flush


void sio_evolve( TiSioComm * sio );
{
}

TiUart
TiSioComm
svc_uart


sio_setlistener 
















#define SIO_STATE_GENERAL 0
#define SIO_STATE_GETSOF 1
 
/* TiSioComm object
 * @param
 * 	sio		the object instance pointer
 * 	uart	on which uart this SioComm object is build
 * 	opt		unused now
 */
TiSioComm * sio_construct( char * buf, uint16 size, TiUartAdapter * uart, uint8 opt )
{
	TiSioComm * sio;
	
	if (sizeof(TiSioComm) > size)
		sio = NULL;
	else
		sio = (TiSioComm *)buf;
	
	if (sio != NULL)
	{
		memset( buf, 0x00, size );
		sio->uart = uart;
		sio->rxlen = 0;
		sio->txlen = 0;	
		sio->option = opt;
		sio->timer = NULL;
		sio->sof = SIO_DEFAULT_ESCAPE;
  		sio->fmstate = SIO_STATE_GENERAL;
  		sio->fmstate_sofcount = 0;
  		sio->fmstart = 0;
  		sio->fmlength = 0;
	}
	
	return sio;
}

void sio_destroy( TiSioComm * sio )
{
	if (sio != NULL)
	{
		sio->rxlen = 0;
		sio->txlen = 0;
		// @TODO disable sio interrupt here if interrupt read and write enabled
	}
}

void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt, uint8 sof )
{
	sio->option = opt;
	sio->timer = timer;
	sio->sof = sof;
}

#ifdef SIO_CONFIG_VERSION_10
/* Direct through version for testing only.
 * 
 * @return
 * 	the byte count actually put into the buffer;
 */
uint8 sio_read( TiSioComm * sio, char * payload, uint8 size, uint8 opt )
{
	char * buf;
	uint8 avail, count;
	
	do{
		avail = SIOCOMM_RXBUFFER_CAPACITY - sio->rxlen;
		buf = (char *)(sio->rxbuf) + sio->rxlen;
		count = uart_read( sio->uart, buf, avail, opt );
		if (count == 0)
			break;
			
		sio->rxlen += count;
	}while ((avail > 0) && (count > 0)); 
	
	count = min( sio->rxlen, size );
	if (count > 0)
	{
		memmove( payload, &(sio->rxbuf[0]), count );
		sio->rxlen -= count;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[count]), sio->rxlen ); 
	}
		
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_30
uint8 sio_read( TiSioComm * sio, char * payload, uint8 size, uint8 opt )
{
	uint8 count;
	
	sio_evolve( sio );
	count = min( sio->rxlen, size );
	if (count > 0)
	{
		memmove( payload, &(sio->rxbuf[0]), count );
		sio->rxlen -= count;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[count]), sio->rxlen ); 
		/*
		// the following code is to adjust the frame settings. this will enable
		// you to get almost the correct frame when call rawread() after read().
		// thus you can hybrid using of rawread() and read()
		//
		if (sio->fmstart == 0)
		{
			sio->fmlength = (sio->fmlength > count) ? (sio->fmlength - count) : 0;
		}
		else{
			if (sio->fmstart <= count)
			{
				sio->fmstart = 0;
				sio->fmlength = 0;
			}
			else
				sio->fmstart -= count;
		}
		*/
	}
	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/* 行为与sio_read类似，但sio_read中参数payload指向的是有效载荷，不含串口通信中可能加入的
 * 那些控制字节，如串口frame长度等信息和frame之间的区分字节等。
 * 本函数则是将一个完整的frame原封不动的放入到frame所指内存空间中。
 * this version is for test only
 */
uint8 sio_rawread( TiSioComm * sio, char * buf, uint8 size, uint8 opt )
{
	return uart_read( sio->uart, buf, size, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* @attention
 * when this function receive an corrupt frame, for example, only a part of a 
 * frame, it will wait for the left data. thus sio_rawread() will not return 
 * if it doesnot receive an full frame. 
 *  
 *    first frame          second frame
 *   0           fmstart                     txlen
 *   |--------------|-------------------------|------
 *                  |-------------fmlength---------|
 * 
 * @param
 * 	opt 	0x00, no use now
 */
uint8 sio_rawread( TiSioComm * sio, char * buf, uint8 size, uint8 opt )
{
	uint8 framelength = 0;
	
	sio_evolve( sio );
	if (sio->fmlength > 0) // frame found
	{
		if (sio->fmstart > 0)
			framelength = sio->fmstart;
		else
			framelength = (sio->fmlength < sio->txlen) ? 0 : sio->fmlength;
	}
		
	if (framelength > 0)
	{
		framelength = min( framelength, SIO_FRAMEBUFFER_LENGTH );
		buf[0] = 0x01;  // frame control byte
		buf[1] = 0x00;	// address field (no use, but you should keep them here)
		buf[2] = 0x00;  
		assert( framelength+3 <= size ); 
		memmove( &(buf[3]), &(sio->rxbuf[0]), framelength );
		sio->rxlen -= framelength;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[framelength]), sio->rxlen );

		if (sio->fmstart == 0)
			sio->fmlength = 0;
		else
			sio->fmstart -= framelength; // should equal to 0 now.
	}
	
	return framelength;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/*　write a frame to the TiSioComm object.
 * 
 *　@attention
 * @warning
 * 	since sio_write() is based on uart_write(), it's behavior is also resemble
 * uart_write(). You may often write multiple times to finish sending one buffer's
 * data.
 * 
 * @return
 * the byte count wrotten successfully. 
 */ 
uint16 sio_write( TiSioComm * sio, char * payload, uint16 len, uint16 opt )
{
	return sio_rawwrite( sio, payload, len, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* write a frame to the TiSioComm object.
 * @attention
 * @warning
 * 	since sio_write() is based on uart_write(), it's behavior is also resemble
 * uart_write(). You may often write multiple times to finish sending one buffer's
 * data.
 * 	before really sending to UART, the data may assembled to a frame.
 */
uint8 sio_write( TiSioComm * sio, char * buf, uint8 len, uint8 opt )
{
	uint8 count = 0;

	if (sio->txlen == 0)
	{
		count = uart_write( sio->uart, buf, len, opt );
	}
	else{
		count = min( SIO_TXBUFFER_CAPACITY - sio->txlen, len );
		memmove( (char *)(sio->txbuf) + sio->txlen, buf, count );
	}
	sio_evolve( sio );
	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/* write every byte in the buffer "sioframe" to UART driver. usually, the data 
 * in the buffer is an entire frame. this version is for test only!
 * 
 * @return
 * byte count wrotten successfully to UART.
 */
uint8 sio_rawwrite( TiSioComm * sio, char * buf, uint8 len, uint8 opt )
{
	return uart_write( sio->uart, buf, len, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* write an entire frame to the uart driver to send it to the HOST/PLC.
 * @param
 * opt = 0x00: the "buf" contains an entire frame with frame header in it. 
 * opt = 0x01: the "buf" contains payload of the frame only. "rawwrite" will 
 * 	construct the frame header (default settings) for the master program.
 * @return
 */
uint8 sio_rawwrite( TiSioComm * sio, char * buf, uint8 len, uint8 opt )
{
	uint8 count=0;
	
	if (sio->txlen == 0) 
	{
		if (opt & 0x01)
		{
			sio->txbuf[0] = 0x00; // frame control byte
			sio->txbuf[1] = 0x00; // address field
			sio->txbuf[2] = 0x00; 
			count = min( len, SIO_FRAMEBUFFER_LENGTH-3 );
			memmove( &(sio->txbuf[3]), buf, count );  
			sio->txlen = count+3;
		}
		else{
			count = min( len, SIO_FRAMEBUFFER_LENGTH );
			memmove( &(sio->txbuf[0]), buf, count );
			sio->txlen = count;
		}
	}
	sio_evolve( sio );
	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* @modified by zhangwei on 20061015
 * version 3 evolve function. this version will read and write data in evolve 
 * function instead of sio_read() and sio_write().
 * 
 * @attention
 * @TODO multiple frame identification and queuing
 * 
 * 								ATTENTION
 * this version of evolve() can only recognize 1 frame in the input stream!
 * the later frames in the "rxbuf" will be regarded as one whole frame! you'd 
 * better push these frames into a queue so that we can support multiple frames
 * in the future!
 */
void sio_evolve( TiSioComm * sio )
{
	char ch;
	uint8 framestart=0, framelength=0, n;
	boolean found = FALSE;

	if (sio->option & SIO_OPTION_FRAME_SUPPORT)
	{
		// execute query-based receiving process	
		// 
		do{
			if (sio->rxlen >= SIO_RXBUFFER_CAPACITY)
				break;
					
			if (uart_getchar(sio->uart, &ch) < 0)
				break;
					
			switch (sio->fmstate)
			{
			// initial state, no SOF byte received
			case SIO_STATE_GENERAL:
				if (ch == sio->sof)
				{
					sio->fmstate = SIO_STATE_GETSOF;
					sio->fmstate_sofcount = 1;
				}
				else
				{
					sio->rxbuf[sio->rxlen++] = ch;
				}
				break; 
					
			// received SOF. and the sofcount will keep how many SOF received
			// in this state.
			case SIO_STATE_GETSOF:
				if (ch == sio->sof)
				{
					sio->fmstate_sofcount ++;
					if (sio->fmstate_sofcount % 2 == 0) // sofcount is even 
						sio->rxbuf[sio->rxlen++] = ch;
				}
				else
				{
					if (sio->fmstate_sofcount % 2 == 1) // sofcount is odd
					{
						framelength = ch; // this is the length byte according to UART frame
						framestart = sio->rxlen - (sio->fmstate_sofcount >> 1);
						found = TRUE;
					}
					sio->fmstate = SIO_STATE_GENERAL;
				}
				break;
			}
		}while(TRUE);
		
		// @TODO now only support 1 frame identification! you should improve it
		// to support multiple frames. this needs an queue. 20061016
		//	
		if (found)
		{
			if (sio->fmlength == 0)
			{
				sio->fmstart = framestart;
				sio->fmlength = sio->rxbuf[framestart]; 
			}
		}

		// execute the write process
		// encapsulate the SioComm frame to PHY frame by adding a SOF start flag
		// byte and a length byte. 
		//
		if (sio->txlen > 0)
		{
			framelength = min( sio->txlen, SIO_FRAMEBUFFER_LENGTH ) + 1;
			while (uart_putchar( sio->uart, sio->sof ) != 0) NULL;
			// @TODO: whether the length byte has already placed into the buffer?
			// confirm it. pls 20061025
			//while (uart_putchar( sio->uart, framelength ) != 0) NULL;
			for (n=0; n<framelength-1; n++)
			{
				while (uart_putchar( sio->uart, sio->txbuf[n] ) !=0) NULL;
				if (sio->txbuf[n] == sio->sof)
					while (uart_putchar( sio->uart, sio->sof ) != 0) NULL;
			}
			
			framelength --;
			memmove( (char*)(sio->txbuf[0]), (char*)(sio->txbuf[0])+framelength, sio->txlen-framelength );
			sio->txlen -= framelength;
		}
	}
	
	// when you choose not to support frame
	// just try uart read and write byte by byte. 
	else{  
		while(TRUE)
		{
			if (sio->rxlen >= SIO_RXBUFFER_CAPACITY)
				break;
					
			if (uart_getchar(sio->uart, &ch) < 0)
				break;
			
			sio->rxbuf[sio->rxlen++] = ch;
		};
		
		n = uart_write( sio->uart, &(sio->txbuf[0]), sio->txlen, 0x00 );
		sio->txlen -= n;
	}
	
	return;		
}		

#undef SIO_STATE_GENERAL 
#undef SIO_STATE_GETSOF
#endif

