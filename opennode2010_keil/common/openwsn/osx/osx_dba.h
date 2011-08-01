#ifndef _OSX_DEBUGAGENT_H_4778_
#define _OSX_DEBUGAGENT_H_4778_
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

/*******************************************************************************
 * dba: debug agent
 *
 * @state
 *	- in developing. not tested
 *
 * @author zhangwei on 200507
 * @modified by zhangwei in 200903
 *	- revision
 ******************************************************************************/

#include "osx_configall.h"
#include "../hal/hal_uart.h"
#include "osx_foundation.h"

/*******************************************************************************
 * dba: debug agent
 * this is an runnable object which can help debugging. it will send the system
 * output to the host computer through one of the UART/RS232 interface.
 *
 * most of the I/O functions can be called safely even in interrupt service routines
 * except the dba_evolve(). this feature can help debugging the interrupt services.
 *
 * debug agent (dba) provides an input/output interface for the device. it can
 * be configured with output only by disable  CONFIG_DBA_INPUT
 * or bi-directional.
 *
 * if CONFIG_DBA_INPUT is defined, then the dba_interpret() is effective
 * and it can execute some simple commands such as (similar to the ancient dos
 * debug program):
 *
 *  m <addr>  print memory content at the specified address
 *  r <addr>  print register content
 *  rst reset
 *  rbt reboot
 *
 * dba will use one uart for input/output. it needs the TiUartAdapter's support.
 *
 * @todo
 * - this implementation require you have a correct implementation of hal_enter_critical()
 *  and hal_leave_critical(). however, they two in current release are not correct yet!
 *  so you should be very careful when using TiDebugAgent object in your program,
 *  especially when you dealing with interrupt service routines.
 *
 ******************************************************************************/

/* Q: what's debug agent (dba)?
 * R: dba is an debugging agent running with the osx kernel. It can output the debug
 * data through UART or accept incoming commands from UART. Further, "dba" has internal
 * buffering mechnism which made it can be freely called even in fast interrupt routines,
 * though the PC side may not get the output immediately due to the osx kernel really 
 * send them through UART.
 */

/* controls whether the debug agent can accept incoming commands from uart */

#define CONFIG_DBA_INPUT   1
#define CONFIG_DBA_OUTPUT  1

#define CONFIG_DBA_TXBUF_SIZE 127
#define CONFIG_DBA_RXBUF_SIZE 20

/* when data arrival from uart, then the following event will be sent to debug agent
 * through the dba_evolve(e) function. */

#ifdef CONFIG_DBA_OUTPUT
#define dba_putchar(dba,c)      uart_putchar((dba)->uart,(c))
#define dba_getchar(dba,pc)     uart_getchar((dba)->uart,(pc))
#define dba_read(dba,buf,size)  uart_read((dba)->uart,buf,size,0)

#define dba_char(dba,ch)        dba_write((dba),&(ch),1)
#define dba_byte(dba,ch)        dba_write((dba),&(ch),1)
#define dba_int8(dba,n)         dba_write((dba),&(n),1)
#define dba_uint8(dba,n)        dba_write((dba),&(n),1)
#define dba_writeint16(dba,n)   dba_write((dba),&(n),2)
#define dba_writeuint16(dba,n)  dba_write((dba),&(n),2)
#define dba_writeint32(dba,n)   dba_write((dba),&(n),4)
#define dba_writeuint32(dba,n)  dba_write((dba),&(n),4)
#define dba_writestring(dba,pc) dba_write((dba),(pc),strlen(pc))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * TiDebugAgent
 * debug agent is an service running inside osx, which can give flexible input/output
 * for the programs.
 ******************************************************************************/

typedef struct{
  TiUartAdapter * uart;
  #ifdef CONFIG_DBA_OUTPUT
  uint8 txlen;
  char txbuf[CONFIG_DBA_TXBUF_SIZE];
  #endif
  #ifdef CONFIG_DBA_INPUT
  uint8 rxlen;
  char rxbuf[CONFIG_DBA_RXBUF_SIZE];
  #endif
}TiDebugAgent;

TiDebugAgent *  dba_construct( char * buf, uint16 size );
void            dba_destroy( TiDebugAgent * dba );
TiDebugAgent *  dba_open( TiDebugAgent * dba, TiUartAdapter * uart );
void            dba_close( TiDebugAgent * dba );

/* the evolve function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 *
 * dba_evolve() should be called only in non-interrupt mode.
 */
void            dba_evolve( TiDebugAgent * dba, TiEvent * e );

#ifdef CONFIG_DBA_OUTPUT
uint16          dba_write( TiDebugAgent * dba, char * buf, uint16 size );
#endif

#ifdef CONFIG_DBA_INPUT
void            dba_interpret( TiDebugAgent * dba, char * input, uint16 len, char * output, uint16 * size );
#endif

char _dba_digit2hexchar( uint8 num );

#ifdef __cplusplus
}
#endif

#endif
