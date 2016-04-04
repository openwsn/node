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
#ifndef _HAL_DEBUGIO_H_6579_
#define _HAL_DEBUGIO_H_6579_

/* 
 *******************************************************************************
 * hal_debugio
 * Debug input/output utilities for HAL layer. Currently, this module support input 
 * from UART and output to group LED. 
 * 
 * design principle:
 *	- as simple as possible. so its easier to be ported to other platforms.
 *  - small code footprint.
 *
 * @state
 *  released
 * 
 * @author zhangwei in 2005.06
 *	- first created 
 * @modified by huanghuan in 2006.07
 *	- developed LPC2136 ARM7 version
 * @modified by zhangwei on 2010.07.25
 *  - revision. only the key part is kept in this module. most of the functions
 *    are moved into rtl_debugio module.
 * @modified by zhangwei on 2010.08.17
 *  - revision. more compatible with rtl_debugio module. the rtl_debugio doesn't
 *    depend on hal_debugio. you can initialize rtl_debugio with any standard byte
 *    device interface. hal_debugio with initialize rtl_debugio so you needn't care
 *    its initialization if you call dbo_open()
 * @modified by zhangwei on 2013.11.29
 *  - add hal_debug_open(...) and hal_debug_close() and hal_debug_init()
 *  - Tested Ok. Please refer to example hal_debugio
 *******************************************************************************
 */

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_debugio.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_assert.h"
//#include "hal_device.h"

/* The default configuration is to use UART0. You may need to modify it to adapt
 * to your own hardware platform */

/* choose which uart hardware module */
#define CONFIG_DBO_UART2
#undef  CONFIG_DBO_UART1

#define CONFIG_DBO_TXBUFFER_SIZE 64

#define TiDebugUart TiDebugIoAdapter

typedef struct{
  uint8 id;
  uint8 txlen;
  char txbuf[CONFIG_DBO_TXBUFFER_SIZE];
}TiDebugIoAdapter;

#ifdef __cplusplus
extern "C"{
#endif

/* dbo_open()
 * before you use the dbo functions, you should call dbo_open() first to initialize
 * necessary PIN directions and UART parameters.
 * 
 * dbo_led(state)
 * display state value using the default three LEDs
 * state = 000 wll turn off all the LEDs. other values include 001 to 111.
 */
                   
TiDebugIoAdapter * dbio_open( uint16 bandrate );
void dbio_close( TiDebugIoAdapter * dbio );
char dbio_getchar( TiDebugIoAdapter * dbio );
intx dbio_putchar( TiDebugIoAdapter * dbio, char ch );

TiDebugIoAdapter * hal_debug_open( uint8 usart_id, uint16 baudrate );
void hal_debug_close( TiDebugIoAdapter * dbio );
char hal_debug_getchar( TiDebugIoAdapter * dbio );
intx hal_debug_putchar( TiDebugIoAdapter * dbio, char ch );

void hal_debug_init(uint8 uartid, uint16 baudrate);

/**
 * Initialize the debug I/O functionality of the system. It will call dbio_open() 
 * and rtl_init() inside. 
 * 
 * @attention This function replace the former dbo_open() function.
 * 
 * @return None. 
 */
void dbio_init(void);

//TiByteDeviceInterface * dbio_interface( TiByteDeviceInterface * intf );


#ifdef CONFIG_DEBUG
void dbo_open( uint16 baudrate );
#define dbo_close() dbio_close()
//inline void dbo_open( uint8 uart_id, uint16 baudrate ) {_dbo_open((uart_id),(baudrate));}
//inline void dbo_close() {_dbo_close();}

__inline void dbo_getchar() {dbc_getchar();}
__inline void dbo_putchar( char c ) {dbc_putchar(c);}
__inline void dbo_putbyte( uint8 c ) {dbc_putchar(c);}
__inline void dbo_write( char * buf, uint8 len ) {dbc_write(buf,len);}
__inline void dbo_asyncwrite( char * buf, uint8 len ) {dbc_asyncwrite(buf,len);}
__inline char dbo_digit2hexchar( uint8 num ) {return dbc_digit2hexchar(num);}

__inline void dbo_mem(char * buf, uint8 len) {dbc_write(buf,len);}
__inline void dbo_string(char * str) {dbc_string(str);}
__inline void dbo_char(char c) {dbc_putchar(c);}
__inline void dbo_byte(uint8 c) {dbc_putchar(c);}
__inline void dbo_uint8(uint8 n) {dbc_putchar(n);}
__inline void dbo_uint16(uint16 n) {dbc_write((char*)&(n),sizeof(n));}
__inline void dbo_uint32(uint32 n) {dbc_write((char*)&(n),sizeof(n));}
__inline void dbo_n8toa( uint8 n ) {dbc_n8toa(n);}
__inline void dbo_n16toa( uint16 n ) {dbc_n16toa(n);}
__inline void dbo_write_n8toa( char * buf, uintx len ) {dbc_write_n8toa(buf,len);}
#endif 

#ifndef CONFIG_DEBUG
#define dbo_open(baudrate) {}
#define dbo_close() {}
#define dbo_getchar() {}
#define dbo_putchar(c) {}
#define dbo_write(buf,len) {}
#define dbo_asyncwrite(buf,len) {}
#define dbo_digit2hexchar(num) {}
#define dbo_mem(buf,len) {}
#define dbo_string(s) {}
#define dbo_char(c) {}
#define dbo_byte(c) {}
#define dbo_uint8(n) {}
#define dbo_uint16(n) {}
#define dbo_uint32(n) {}
#define dbo_n8toa(n) {}
#define dbo_n16toa(n) {}								

#endif 

#ifdef __cplusplus
}
#endif

extern TiDebugUart g_dbio;

#endif /* _HAL_DEBUGIO_H_6579_ */
