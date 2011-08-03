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

#include "hal_configall.h"
#include <string.h>
#include "hal_foundation.h"
#include "../../rtl/rtl_debugio.h"
#include "hal_debugio.h"
#include "hal_assert.h"
#include "hal_mcu.h"

#ifdef CONFIG_DEBUG

static bool g_dbio_init = false;
TiDebugUart g_dbio;

/*******************************************************************************
 * raw debug input/output device
 ******************************************************************************/

TiDebugIoAdapter * dbio_open( uint16 bandrate )
{
    USART_InitTypeDef USART_InitStructure;
	if (g_dbio_init)
		return  &g_dbio;

	memset( &g_dbio, 0x00, sizeof(TiDebugUart) );

	#ifdef CONFIG_DBO_UART2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    // Configure USART2 Rx (PA.3) as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bandrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init( USART2,&USART_InitStructure);
    USART_Cmd( USART2,ENABLE);
//	/* initialize PIN directions. PE0 should be input and PE1 should be output.
//	 * actaully, you needn't do so for Atmega128.  
//	 */
//	DDRE &= (~(1 << DDE0));		// set PE0/USART0 RXD as input
//	DDRE |= (1 << DDE1 );		// set PE1/USART0 TXD as output
//
//	/* set baudrate. 
//	 * when MCU system clock is 8MHz (for ICT's GAINZ platform)
//	 *	UBRR0L=8  => 57600 bps
//	 *  UBRR0L=12 => 38400 bps, error 0.2 %  
//	 */
//	UBRR0L = 12;
//
//	/* enable receiver and transmitter */
//	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	#endif
	
	#ifdef CONFIG_DBO_UART1
//	// HAL_MAKE_PD2_INPUT();
//	// HAL_MAKE_PD3_OUTPUT();
//
//	/* {UBRRH, UBRRL} = system clock / (16 * baudrate) - 1 */
//	tmpn = (CONFIG_SYSTEM_CLOCK / (16L * baudrate) - 1);
//	UBRR1H = (unsigned char)(tmpn >> 8);
//	UBRR1L = (unsigned char)tmpn;
//
//	/* enable receiver and transmitter */
//	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	#endif

	g_dbio_init = true;
    return &g_dbio;

    // todo: plan to switch dbo to dbc_
    //rtl_init( NULL, (TiFunDebugIoPutChar)debug_putchar, (TiFunDebugIoGetChar)debug_getchar, debug_assert_report );
}


void dbio_close( TiDebugIoAdapter * dbio )
{
	g_dbio.txlen = 0;
	g_dbio_init = false;
}

/* getchar from UART. if there's no input from UART, then this function will wait
 * until there's a input. 
 */
char dbio_getchar( TiDebugIoAdapter * dbio )
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }
    return (USART_ReceiveData(USART2) & 0x7F); 
//	#ifdef CONFIG_DBO_UART0
//	while (!(UCSR0A & (1<<RXC0))) {};
//	return UDR0;
//	#endif
//
//	#ifdef CONFIG_DBO_UART1
//	while (!(UCSR1A & (1<<RXC1))) {};
//	return UDR1;
//	#endif
	//return 0;
}

/* _dbio_putchar()
 * this function sends one character only through the UART hardware. 
 * 
 * @return
 *	0 means success, and -1 means failed (ususally due to the buffer is full)
 *  when this functions returns -1, you need retry.
 */
intx dbio_putchar( TiDebugIoAdapter * dbio, char ch )
{
    USART_SendData( USART2,ch);
    while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
    }
//	/* wait for the transmit buffer empty */
//	#ifdef CONFIG_DBO_UART0
//	while (!(UCSR0A & (1<<UDRE0))) {};
//	UDR0 = ch;
//	#endif
//
//	#ifdef CONFIG_DBO_UART1
//	while (!(UCSR1A & (1<<UDRE1))) {};
//	UDR1 = ch;
//	#endif

	return 1;
}

/*
TiByteDeviceInterface * dbio_interface( TiByteDeviceInterface * intf )
{
    intf->&g_dbio;
    intf->putchar = dbio_putchar;
    intf->getchar = dbio_getchar;
    return intf;
}
*/

void dbo_open( uint16 baudrate )
{
    dbio_open( baudrate );
    rtl_init( (void*)&g_dbio, (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
}

/*******************************************************************************
 * higher debug functions
 ******************************************************************************/

/* getchar from UART. if there's no input from UART, then this function will wait
 * until there's a input. 
 */
/*
char _dbo_getchar()
{
	#ifdef CONFIG_DBO_UART0
	while (!(UCSR0A & (1<<RXC0))) {};
	return UDR0;
	#endif

	#ifdef CONFIG_DBO_UART1
	while (!(UCSR1A & (1<<RXC1))) {};
	return UDR1;
	#endif
}
*/

/* _dbo_putchar()
 * this function sends one character only through the UART hardware. 
 * 
 * @return
 *	0 means success, and -1 means failed (ususally due to the buffer is full)
 *  when this functions returns -1, you need retry.
 */
/*
void _dbo_putchar( char ch )
{
	// wait for the transmit buffer empty 
	#ifdef CONFIG_DBO_UART0
	while (!(UCSR0A & (1<<UDRE0))) {};
	UDR0 = ch;
	#endif

	#ifdef CONFIG_DBO_UART1
	while (!(UCSR1A & (1<<UDRE1))) {};
	UDR1 = ch;
	#endif

	return;
}
*/
/*

char _dbo_digit2hexchar( uint8 num )
{
	static char g_digit2hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return (g_digit2hextable[num & 0x0F]);
}

void _dbo_putbyte( uint8 val )
{
	_dbo_putchar(dbc_digit2hexchar(((val)&0xF0) >> 4));	
	_dbo_putchar(dbc_digit2hexchar((val)&0x0F));	
}

void _dbo_write( char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		dbo_putchar(buf[i]);
	}
}

void _dbo_write_n8toa( char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		dbo_n8toa( buf[i] );
		dbo_putchar( ' ');
	}
}

void dbo_string( char * string )
{
	uintx i;
	for (i=0; i<strlen(string); i++)
		//dbo_n8toa( string[i] );
		dbo_putchar(string[i]);
}
*/
/* simply wrotten to internal memory. this is pretty fast so that this function can
 * help debugging ISR */

/*

uintx _dbo_asyncwrite( char * buf, uintx len )
{
	uintx count = min( CONFIG_DBO_TXBUFFER_SIZE - g_dbio.txlen, len );
	memmove( &(g_dbio.txbuf[g_dbio.txlen]), buf, count );
	g_dbio.txlen += count;
	return count;
}

void _dbo_evolve()
{
	_dbo_write( &(g_dbio.txbuf[0]), g_dbio.txlen );
}
*/

#endif /* CONFIG_DEBUG */
