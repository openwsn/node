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

#include "svc_configall.h"
#include <string.h>
#include "../hal/hal_cpu.h"
#include "../hal/hal_assert.h"
//#include "svc_foundation.h"
#include "svc_debugio.h"

/*****************************************************************************
 * @todo
 * - this implementation require you have a correct implementation of hal_enter_critical()
 *  and hal_leave_critical(). however, they two in current release are not correct yet!
 *  so you should be very careful when using TiDebugIo object in your program, 
 *  especially when you dealing with interrupt service routines.
 *  
 ****************************************************************************/ 

TiDebugIo * debug_construct( char * buf, uint16 size )
{
	TiDebugIo * db;
	
	db = (sizeof(TiDebugIo) <= size) ? (TiDebugIo *)buf : NULL;
	if (db != NULL)
	{
		memset( buf, 0x00, sizeof(TiDebugIo) );
		db->datalen = 0;
	}
	
	return db;
}

void debug_destroy( TiDebugIo * db )
{
	return;
}

TiDebugIo * debug_open( TiDebugIo * db, TiUartAdapter * uart )
{
	if (db != NULL)
	{
		db->datalen = 0;
		db->uart = uart;
	}
	return db;
}

void debug_close( TiDebugIo * db )
{
	return;
}

/* this function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 */
void debug_evolve( TiDebugIo * db )
{
	uint16 count;
	char * buf;
	if (db->datalen > 0)
	{
		//hal_enter_critical();
		count = uart_write( db->uart, &(db->buf[0]), db->datalen, 0 );
		if (db->datalen >= count)
			db->datalen -= count;
		else
			db->datalen = 0;
		
		if (count > 0)
		{
			buf = (char *)(&(db->buf[0]));
			memmove( buf, buf + count, db->datalen );
		}
		//hal_leave_critical();
	}
}

uint16 debug_write( TiDebugIo * db, char * buf, uint16 size )
{   
	uint16 copied;
	uint16 i;
	
    //hal_enter_critical();
	
	i = CONFIG_DEBUGIO_BUFFER_CAPACITY - db->datalen;
	hal_assert( i <= CONFIG_DEBUGIO_BUFFER_CAPACITY ); 

	copied = min(i, size);
	if (copied > 0)
	{
		//memmove( &(db->buf[0] )+ db->datalen, buf, copied );
		for (i=0; i<copied; i++)
		{
		   db->buf[i+db->datalen] = buf[i];
		}
		db->datalen += copied;
	}
	//hal_leave_critical();
	
	return copied;
}

