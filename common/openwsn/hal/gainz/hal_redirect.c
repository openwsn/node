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

#include "../hal_configall.h"
#include <stdio.h> 
#include <stdlib.h>
#include <avr/io.h> 
#include "../hal_foundation.h"
#include "../hal_uart.h"
#include "../hal_redirect.h"


TiUartAdapter * m_uart = NULL;

static int _usart_putchar(char c);
static int _usart_getchar(void);


/* redirect the standard input/output stream to the uart. the uart must be 
 * constructed and opened successfully before call this function.
 */
void stdc_redirect( TiUartAdapter * uart )
{
	m_uart = uart;

	/* @todo
     * fdevopen is declared in <stdio.h> but you may need to link with some library 
     * to enable it. i don't know why there's always a warning when calling fdevopen
     */
	fdevopen( _usart_putchar, _usart_getchar, 0 );	
}


int _usart_putchar(char c)
{
	if (m_uart != NULL)
		return uart_putchar( m_uart, c );
	else
		return -1;
}

int _usart_getchar(void)
{
	if (m_uart != NULL)
		return uart_getchar_wait( m_uart );
	else
		return 0;
}
