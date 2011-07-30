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

#ifndef _SIOCOMM_H_1478_
#define _SIOCOMM_H_1478_

/*******************************************************************************
 * @author zhangwei on 20060813
 * TiSioComm
 * Serial I/O Communication
 * based on TUartDriver
 * 
 * support both the stream interface and frame interface.
 * however, you'd better use one type interface only at the same time. 
 *
 * TUartAdapter => TiSioComm => TConsole
 *                            and other applications
 ******************************************************************************/

#include "svc_foundation.h"
#include "..\hal\hal_uart.h"
#include "..\hal\hal_timer.h"

/*******************************************************************************
 * UART PDU format
 * SIO PDU is the data frame transmitted on the physical UART. 
 * 		[1B SOF][1B Framelength][1B Frame Control][nB PSDU/Payload]{[2 FCS]}
 * SOF means start of frame. The length byte is the data length + control 1 + length 1
 * 
 * SIO PDU is the part after deleting SOF and length byte in UART PDU
 * 		[1B Frame Control][nB PSDU/Payload]
 * 
 * actually, they two share almost the same settings except the first byte SOF.
 * when you adopt frame support, you should check every byte from the framelength 
 * to the end. if the byte equals to SOF, then should insert another SOF to indicate
 * the case.
 ******************************************************************************/

/* configure the SIOCOMM implementation
 * 
 * SIO_CONFIG_VERSION_10
 * this macro will enable the SIOCOMM module adopt direct "pass through" model.
 * this model is very simple and is ideal for testing.
 * 
 * SIO_CONFIG_VERSION_20 
 * has been elminated from the source code
 * 
 * SIO_CONFIG_VERSION_30(default)
 * support smart frame splitting of input stream from UART.
 * support frame & non-frame hybrid programming without data loss.
 * however, hybrid programming may lead to time delay some times! @TODO 20061015
 * this occurs when you receive half a frame in the "rxbuf", however, this case
 * seldom happens. 
 */
#define SIO_CONFIG_VERSION_10
#undef  SIO_CONFIG_VERSION_10

#undef  SIO_CONFIG_VERSION_30
#define SIO_CONFIG_VERSION_30

/* configure TiSioComm's internal buffer size
 * MAX_FRAAME_LENGTH should be less than 0xFF. the maximum value of it is 0xFE
 * the default settings in our application is 0x7E (127). not 0xFF(128)
 */
#define SIO_FRAMEBUFFER_LENGTH OPENWSN_MAX_UART_FRAME_LENGTH-3
#define SIO_RXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH
#define SIO_TXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH

#define SIO_DEFAULT_ESCAPE CONFIG_START_OF_FRAME 

/* this object is built on top of TUartDriver object. It functions like the 
 * data link layer. 
 * 
 * 	rxlen		character count in the rxbuf
 * 	rxbuf		receiving buffer
 * 	sof			start of frame
 * 	lengthindex	the index of length byte in the frame. usually it is 1.
 * 				which means the second byte in the frame is the length.
 * 				the first byte is the start flag of the fame(namely SOF).
 * 	framestart	where's the new frame start
 * 	framestop	where's the new frame stop
 * 
 * @attention
 * due to the memory management, the txqueue and txbuf, rxqueue and txbuf 
 * should be neighbors.
 */
 
#define SIO_OPTION_FRAME_SUPPORT 0x01
#define SIO_OPTION_INTERRUPT_READ 0x02
#define SIO_OPTION_INTERRUPT_WRITE 0x04
#define SIO_OPTION_OTHER5 0x10
#define SIO_OPTION_OTHER6 0x20
#define SIO_OPTION_REMOTE_CONTROL 0x40
 
typedef struct{
  TiUartAdapter * uart;
  uint8 rxlen;
  uint8 txlen;
  char rxbuf[SIO_RXBUFFER_CAPACITY];
  char txbuf[SIO_TXBUFFER_CAPACITY];  
  uint8 option;
  TiTimerAdapter * timer;
  // the following are to support version 3
  uint8 sof; 
  uint8 fmstate;
  uint8 fmstate_sofcount;
  uint8 fmstart;
  uint8 fmlength;
}TiSioComm;

TiSioComm * sio_construct( char * buf, uint16 size, TiUartAdapter * uart, uint8 opt );
void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt, uint8 sof );
void sio_destroy( TiSioComm * sio );
uint8 sio_read( TiSioComm * sio, char * payload, uint8 size, uint8 opt );
uint8 sio_rawread( TiSioComm * sio, char * buf, uint8 size, uint8 opt );
uint8 sio_write( TiSioComm * sio, char * payload, uint8 len, uint8 opt );
uint8 sio_rawwrite( TiSioComm * sio, char * buf, uint8 len, uint8 opt );
void sio_evolve( TiSioComm * sio );

svc_siocomm_raw

sio_construct( 

#endif
