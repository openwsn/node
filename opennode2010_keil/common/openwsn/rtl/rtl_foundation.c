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

#include "rtl_configall.h"
#include <stdlib.h>
#include "rtl_foundation.h"

TiFunAssertReport   g_assert_report = NULL;
void *              g_dbc_io_provider = NULL;
TiFunDebugIoPutChar g_dbc_putchar = NULL;
TiFunDebugIoGetChar g_dbc_getchar = NULL;


void rtl_init( void * io_provider, TiFunDebugIoPutChar debugio_putchar, TiFunDebugIoGetChar debugio_getchar, 
    TiFunAssertReport assert_report )
{
	rtl_assert( sizeof(TiHandleId) == sizeof(void*) );
	
    g_assert_report = assert_report;
    g_dbc_io_provider = io_provider;
    g_dbc_putchar = debugio_putchar;
    g_dbc_getchar = debugio_getchar;
}

void rtl_assert_report( bool cond, char * file, int line )
{
    if (g_assert_report != NULL)
        g_assert_report( cond, file, line );
}
