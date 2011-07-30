#ifndef _RTL_DEBUGIO_H_6787_
#define _RTL_DEBUGIO_H_6787_
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

/* rtl_debugio
 * debug input/output 
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

#define CONFIG_DBC_BUFFER_SIZE 64
#define CONFIG_DBC_TXBUFFER_SIZE 64

/* attention:
 * this module needs two function pointers to perform really device input/output. 
 * these two function pointers should be initialized by rtl_init() in rtl_foundation
 * module or debugio_open() function in this module.
 */

// _TiByteDeviceInterface

char _dbc_digit2hexchar( uint8 digit );

#define DBC_ASCIIOUTPUT_CHAR(c) {_dbc_putchar(&g_dbc,_dbc_digit2hexchar(((val)&0xF0) >> 4)); _dbc_putchar(&g_dbc,_dbc_digit2hexchar((val)&0x0F));}


#ifdef CONFIG_DEBUG
/* dbo_init()
 * before you use the dbo functions, you should call dbo_init() first to initialize
 */

#define dbc_init(io,putc,getc) dbc_open(io,putc,getc)
#define dbc_open(io,putc,getc) {_dbc_construct(&g_dbc, sizeof(TiDebugConsole)); _dbc_open(&g_dbc,io,putc,getc);}
#define dbc_close() _dbc_close()

#define dbc_getchar() _dbc_getchar(&g_dbc)
#define dbc_putchar(c) _dbc_putchar(&g_dbc,(c))

#define dbc_asciiputchar(val) _dbc_asciiputchar(&g_dbc,(val))
#define dbc_write(buf,len) _dbc_write(&g_dbc,(buf),(len))
#define dbc_write_n8toa(buf,len) _dbc_write_n8toa(&g_dbc,(buf),(len))
#define dbc_asyncwrite(buf,len) _dbc_asyncwrite(&g_dbc,(buf),(len))
#define dbc_evolve() _dbc_evolve(&g_dbc)
#define dbc_digit2hexchar(num) _dbc_digit2hexchar((num))
#define dbc_string(str) _dbc_string(&g_dbc,(str))

#define dbc_mem(buf,len) _dbc_write(&g_dbc,(char*)(buf),len);
#define dbc_char(c) _dbc_putchar(&g_dbc,c);
#define dbc_byte(c) _dbc_putchar(&g_dbc,c);
#define dbc_uint8(n) _dbc_putchar(&g_dbc,(uint8)n);
#define dbc_uint16(n) _dbc_write(&g_dbc,(char*)&(n),sizeof(n));
#define dbc_uint32(n) _dbc_write(&g_dbc,(char*)&(n),sizeof(n));

#define dbc_n8toa(n) _dbc_n8toa(&g_dbc,(n))
#define dbc_n16toa(n) _dbc_n16toa(&g_dbc,(n))
#endif

#ifndef CONFIG_DEBUG
#define dbc_init(p1,p2,p3) {}
#define dbc_getchar() {}
#define dbc_putchar(c) {}
#define dbc_asciiputchar(val) {}
#define dbc_write(buf,len ) {}
#define dbc_write_n8toa(buf,len) {}
#define dbc_asyncwrite(buf,len) {}
#define dbc_evolve() {}
#define dbc_digit2hexchar(num) {}
#define dbc_string(str) {}

#define dbc_mem(buf,len) {}
#define dbc_char(c) {}
#define dbc_byte(c) {}
#define dbc_uint8(uint8 n) {}
#define dbc_uint16(uint16 n) {}
#define dbc_uint32(uint32 n) {}
#define dbc_n8toa(n) {}
#define dbc_n16toa(n) {}
#endif

/*******************************************************************************
 * TiDebugConsole
 ******************************************************************************/

typedef struct{
  uint8 txlen;
  char txbuf[CONFIG_DBC_TXBUFFER_SIZE];
}TiDebugConsole;

extern TiDebugConsole g_dbc;

char _dbc_digit2hexchar( uint8 num );

#ifdef __cplusplus
extern "C"{
#endif

TiDebugConsole * _dbc_construct( char * buf, uint16 size );
void _dbc_destroy( TiDebugConsole * dbc );

// 
// TiByteDeviceInterface

TiDebugConsole * _dbc_open( TiDebugConsole * dbc, void * io_provider, TiFunDebugIoPutChar debugio_putchar, 
    TiFunDebugIoGetChar debugio_getchar );
void _dbc_close( TiDebugConsole * dbc );
void _dbc_putchar( TiDebugConsole * dbc, char c );
char _dbc_getchar( TiDebugConsole * dbc );
void _dbc_asciiputchar( TiDebugConsole * dbc, uint8 val );
void _dbc_write( TiDebugConsole * dbc, char * buf, uintx len );
void _dbc_write_n8toa( TiDebugConsole * dbc, char * buf, uintx len );
uintx _dbc_asyncwrite( TiDebugConsole * dbc, char * buf, uintx len );
void _dbc_evolve( TiDebugConsole * dbc );
char _dbc_digit2hexchar( uint8 num );
void _dbc_string( TiDebugConsole * dbc, char * string );
void _dbc_n8toa( TiDebugConsole * dbc, uint8 n );
void _dbc_n16toa( TiDebugConsole * dbc, uint16 n );

#ifdef __cplusplus
}
#endif


#endif /* _RTL_DEBUGIO_H_6787_ */
