/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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
 *****************************************************************************/ 

#ifndef _UART_H_2130_
#define _UART_H_2130_

/****************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TiUartAdapter object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 * @modified by zhangwei on 2006-07-20
 * modify the prototype of uart_configure()
 * update some comments of the functions.
 *
 * @modified by zhangwei on 2006-08-18
 * add support to RS232 and RS485 by using macros
 * the configure macro CONFIG_UART_RS232 and CONFIG_UART_RS485 should be defined
 * in "configall.h" in the application.
 * 
 * @modified by zhangwei on 20070410
 * revision. format the source file 
 * eliminate led_twinkle() in interrupt disable state. because it may cause data
 * loss in fast communication.
 * 
 * @modified by makun on 20070412
 * correct the error of data loss
 * modified interrupt service routine
 * 
 *****************************************************************************/
 
#include "hal_foundation.h"

#define UART_TXBUFFER_SIZE 0x7F
#define UART_RXBUFFER_SIZE UART_TXBUFFER_SIZE


#ifdef __cplusplus
extern "C" {
#endif

/* @TODO: txbuf can be eliminated in the future 20061010
 */
typedef struct{
  uint8 id;
  uint8 txlen;
  volatile uint8 rxlen;
  char txbuf[UART_TXBUFFER_SIZE];
  char rxbuf[UART_RXBUFFER_SIZE];
  TiFunEventHandler handler;
  uint8 databits;
  uint8 stopbits;
  uint8 parity;
  uint32 baudrate;
}TiUartAdapter;

TiUartAdapter * uart_construct( uint8 id, char * buf, uint16 size );
void uart_destroy( TiUartAdapter * uart );
int16 uart_configure (TiUartAdapter * uart,uint32 baudrate, uint8 databits, uint8 stopbits, 
	uint8 parity, uint8 optflag );
void uart_reset( TiUartAdapter * uart );
void uart_setnotifier( TiUartAdapter * uart, TiFunEventHandler handler );

/******************************************************************************
 * read data from UART driver. you can consider the UART's input as an continuous 
 * stream. This function will read the fist of this stream and place the data 
 * into the "buf". the maximum data count is constraint by the buffer's "size". 
 * If the data exceed the buffer's size, then you should call uart_read() for 
 * the second time to retrieve them into the buffer. 
 * 
 * @attention
 * 	for the current implementation, uart_read() will always read upto size
 * characters before it returned! this is due to uart_read adopts "Query and 
 * receive" techniques. you'd better change it to interrupt driven in the future.
 * you should not assume this function always return size characters.
 * 
 * @return
 * 	the data length actually returned in the buffer.
 *****************************************************************************/ 
#ifdef CONFIG_UART_READ_ENABLE
uint16 uart_read( TiUartAdapter * uart, char * buf, uint16 size, uint16 opt );
#endif

/******************************************************************************
 * write the data in the buffer to UART. 
 * 
 * @attention
 * 	you should NOT assume this function can write "len" characters to UART.
 * due to the UART's ability, this function may write less than "len" characters
 * to the UART.
 *****************************************************************************/ 
#ifdef CONFIG_UART_WRITE_ENABLE
uint16 uart_write( TiUartAdapter * uart, char * buf, uint16 len, uint16 opt ); 
#endif

/******************************************************************************
 * get one character from the UART input stream. 
 * @return 
 * 	0		success, and "*pc" is the character received.
 * 	-1		failed
 *****************************************************************************/ 
int16 uart_getchar( TiUartAdapter * uart, char * pc );

/******************************************************************************
 * put one character to the UART output stream
 * @return
 * 	0		success
 * 	-1		failed.
 *****************************************************************************/ 
int16 uart_putchar( TiUartAdapter * uart, char ch );


#ifdef __cplusplus
}
#endif

#endif

