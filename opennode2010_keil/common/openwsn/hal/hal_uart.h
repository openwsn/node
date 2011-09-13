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
#ifndef _HAL_UART_H_2130_
#define _HAL_UART_H_2130_

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
 * @modified by yan-shixing, xiao-yuezhang, and zhangwei(tongji university) on 20090719
 *	- ported to GAINZ platform (based on atmega128)
 *	- tested both query driven and interrupt driven. uart_putchar()/uart_getchar()
 *		uart_read()/uart_write() are ok now.
 *	- didn't test uart_getchar_wait().
 * 
 * @modified by zhangwei on 20100510
 *  - add block interface support
 * 
 * @modified by Jiang Ridong in 2011.08
 *  - Port to stm32f103 and tested ok
 * 
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_device.h"

//#define CONFIG_UART_INTERRUPT_DRIVEN
#undef  CONFIG_UART_INTERRUPT_DRIVEN

#define CONFIG_UART_TXBUFFER_SIZE 0x7F
#define CONFIG_UART_RXBUFFER_SIZE CONFIG_UART_TXBUFFER_SIZE


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1) 
typedef struct{
  uint8 id;
  uint16 baudrate;
  uint8 databits;
  uint8 stopbits;
  uint8 option;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
  volatile uint8 txlen;
  volatile uint8 txidx;
  volatile uint8 rxlen;
  volatile char txbuf[CONFIG_UART_TXBUFFER_SIZE];
  volatile char rxbuf[CONFIG_UART_RXBUFFER_SIZE];
  TiFunEventHandler listener;
  void * lisowner;
#endif
}TiUartAdapter;

TiUartAdapter * uart_construct( char * buf, uint16 size );
void uart_destroy( TiUartAdapter * uart );
TiUartAdapter * uart_open( TiUartAdapter * uart, uint8 id, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option );
void uart_close( TiUartAdapter * uart );

/******************************************************************************
 * get one character from the UART input stream. 
 * @return 
 * 	0		success, and "*pc" is the character received.
 * 	-1		failed
 *****************************************************************************/ 
intx uart_getchar( TiUartAdapter * uart, char * pc );
char uart_getchar_wait( TiUartAdapter * uart ); 

//uint8 uart_getchar( TiUartAdapter * uart, char * pc );
//char uart_getchar_wait( TiUartAdapter * uart ); 

/******************************************************************************
 * put one character to the UART output stream
 * @return
 * 	0		success
 * 	-1		failed.
 *****************************************************************************/ 
intx uart_putchar( TiUartAdapter * uart, char ch );

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
intx uart_read( TiUartAdapter * uart, char * buf, intx size, uint8 opt );

/******************************************************************************
 * write the data in the buffer to UART. 
 * 
 * @param uart TiUartAdapter object.
 * @param buf Data to be sent.
 * @param len The data length in the buffer.
 * @param opt 0x00 means blocked operation and 0x01 means non-blocked operation. 
 *      In the non-blocked mode, the function will return immediately, so not
 *      all the data can be sent.
 * 
 * @attention
 * 	you should NOT assume this function can write "len" characters to UART.
 * due to the UART's ability, this function may write less than "len" characters
 * to the UART.
 *****************************************************************************/ 
intx uart_write( TiUartAdapter * uart, char * buf, intx len, uint8 opt ); 

TiBlockDeviceInterface * uart_getblockinterface( TiUartAdapter * uart, TiBlockDeviceInterface * intf );
//TiByteDeviceInterface * uart_getbyteinterface( TiUartAdapter * uart, TiByteDeviceInterface * intf );

// todo 
// to be deleted

    //TiBlockDeviceInterface * uart_get_blockinterface( TiUartAdapter * uart, TiBlockDeviceInterface * intf );
    //以下三个函数临时保留
//    void   halUartInit(uint16 baudrate, uint8 options);//uart_open( TiUartAdapter * uart,2, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option );
//    uint8 USART_Send( uint8 ch);//intx uart_putchar( TiUartAdapter * uart, char ch );
//    uint8 USART_Get( uint8 ch); //uint8 uart_getchar( TiUartAdapter * uart, char * pc );


#ifdef __cplusplus
}
#endif

#endif
