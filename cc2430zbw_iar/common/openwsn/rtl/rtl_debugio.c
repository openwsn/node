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
#include <stdlib.h> 
#include "rtl_foundation.h"
#include "rtl_debugio.h"

TiDebugConsole g_dbc;

char _dbc_digit2hexchar( uint8 digit ) 
{
	static char m_digit2hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return (m_digit2hextable[digit & 0x0F]);
}

/*******************************************************************************
 * TiDebugConsole
 ******************************************************************************/

TiDebugConsole * _dbc_construct( char * buf, uint16 size )
{
    TiDebugConsole * dbc = (TiDebugConsole *)buf;
    memset( buf, 0x00, size );
    return dbc;
}

void _dbc_destroy( TiDebugConsole * dbc )
{
    return;
}

TiDebugConsole * _dbc_open( TiDebugConsole * dbc, void * io_provider, TiFunDebugIoPutChar debugio_putchar, 
    TiFunDebugIoGetChar debugio_getchar )
{
    if (io_provider != NULL) 
        g_dbc_io_provider = io_provider;
    if (debugio_putchar != NULL)
        g_dbc_putchar = debugio_putchar;
    if (debugio_getchar)
        g_dbc_getchar = debugio_getchar;

    dbc->txlen = 0;
    return dbc;
}

void _dbc_close( TiDebugConsole * dbc )
{
    dbc->txlen = 0;
    return;
}

void _dbc_putchar( TiDebugConsole * dbc, char c )
{
    if (g_dbc_putchar)
        g_dbc_putchar( g_dbc_io_provider, c ); 
}

char _dbc_getchar( TiDebugConsole * dbc )
{
    if (g_dbc_getchar)
        return g_dbc_getchar( g_dbc_io_provider ); 
    else
        return 0x00;
}

void _dbc_asciiputchar( TiDebugConsole * dbc, uint8 val )
{
    DBC_ASCIIOUTPUT_CHAR(c);
}

void _dbc_write( TiDebugConsole * dbc, char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		_dbc_putchar( dbc, buf[i] );
	}
}

void _dbc_write_n8toa( TiDebugConsole * dbc, char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		_dbc_n8toa( dbc, buf[i] );
		_dbc_putchar( dbc, ' ');
	}
}

void _dbc_string( TiDebugConsole * dbc, char * string )
{
	uintx i;
	for (i=0; i<strlen(string); i++)
		// _dbc_n8toa( dbc, string[i] );
        _dbc_putchar( dbc, string[i] );
}

/* simply wrotten to internal memory. this is pretty fast so that this function can
 * help debugging ISR */

uintx _dbc_asyncwrite( TiDebugConsole * dbc, char * buf, uintx len )
{
	uintx count = min( CONFIG_DBC_TXBUFFER_SIZE - dbc->txlen, len );
	memmove( &(dbc->txbuf[dbc->txlen]), buf, count );
	dbc->txlen += count;
	return count;
}

void _dbc_evolve( TiDebugConsole * dbc )
{
	_dbc_write( dbc, &(dbc->txbuf[0]), dbc->txlen );
}

void _dbc_n8toa( TiDebugConsole * dbc, uint8 n )
{
	_dbc_putchar(dbc, _dbc_digit2hexchar(((n)&0xF0) >> 4));	
	_dbc_putchar(dbc, _dbc_digit2hexchar((n)&0x0F));
}			

void _dbc_n16toa( TiDebugConsole * dbc, uint16 n )
{
	_dbc_putchar( dbc, _dbc_digit2hexchar(((n)>>12) & 0x000F) );   
	_dbc_putchar( dbc, _dbc_digit2hexchar(((n)>>8) & 0x000F) );	
	_dbc_putchar( dbc, _dbc_digit2hexchar((n)>>4 & 0x000F) );	
	_dbc_putchar( dbc, _dbc_digit2hexchar(n & 0x000F) );
}



