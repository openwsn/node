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
#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_device.h"
#include "../hal_targetboard.h"
#include "../hal_led.h"
#include "../hal_assert.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "../hal_uart.h"
#include "../hal_device.h"
//#include <inttypes.h> 
//#include <avr/pgmspace.h>

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
 * @modified by zhangwei on 20061102
 * change "baudrate" from type uint16 to uint32
 * because the value of baudrate may be "115200". it exceeds the maximum value 
 * of uint16 65536.
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
 * @modified by zhangwei on 20090605
 *	- ported to atmega128
 *
 * @modified by zhangwei on 20090701
 *  - add macro CONFIG_UART_INTERRUPT_DRIVEN
 *  - support interrupt driven uart
 * 
 * @modified by zhangwei on 20090718
 *	- add UART pin direction initialization inside uart_open() function.
 *  - tested by Xiao Yuezhang(tongji university)
 * 
 * @modified by xiao yuezhang on 20090719
 *	- correct register initialization error in uart_open()
 * @modified by zhangwei on 20090718
 *	- bug fix in interrupt driven version of uart_putchar() and uart_write()
 *
 * @modified by zhangwei on 20100510
 *  - add block interface support
 * 
 * @modified by zhangwei on 2010.05.20
 *  - upgraded to winavr 20090319 and avrstudeo 4.17
 * 
 *****************************************************************************/

#define min(x,y) (((x)<(y))?(x):(y))
 
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx0_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_tx0_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_rx1_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_tx1_interrupt_handler( void * uartptr, TiEvent * e );
#endif

TiUartAdapter * uart_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiUartAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiUartAdapter *)buf;
}

void uart_destroy( TiUartAdapter * uart )
{
	uart_close( uart );
}

/****************************************************************************** 
 * initialze the UART hardware and object
 * @param
 * 	id		0	UART0
 * 			1 or other values	UART1
 * @return 
 * 	0		success
 *  -1		failed
 * 
 * @modified by zhangwei on 20061010
 * @TODO
 * zhangwei kept the old declaration of the function in order to keep other modules 
 * running. you should call uart_configure() after uart_construct()
 *****************************************************************************/
/****************************************************************************** 
 * @TODO 20061013
 * if the uart adapter is driven by interrupt, then you should enable the interrupt 
 * in configure function. however, whether the ISR really works or not still depends
 * on the global interrupt flag. 
 *
 * @assume: the global interrupt should be disabled before calling this function.
 *****************************************************************************/
TiUartAdapter * uart_open( TiUartAdapter * uart, uint8 id, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option )
{
    uint8 tmpn;

	/* assume: 
	 * - the global interrupt is disabled when calling this function 
	 * - you have already call HAL_SET_PIN_DIRECTIONS. the pin should initialized correctly. or else it doesn't work.
	 */

	uart->id = id;
	uart->baudrate = baudrate;
	uart->databits = databits;
	uart->stopbits = stopbits;
	uart->option = option;

    #ifdef CONFIG_UART_INTERRUPT_DRIVEN
    uart->txlen = 0;
	uart->txidx = 0;
    uart->rxlen = 0;
    uart->listener = NULL;
	uart->lisowner = NULL;
    #endif

	/* @warning
	 * @attention
	 * THIS FUNCTION IS AFFECTED BY YOUR HARDWARE CLOCK SETTINGS 
	 */
	
	/* for ICT GAINZ platform, the clock is 8 MHz */
	#ifndef CONFIG_GAINZ_CLOCK_FREQUENCY
	#error currently can only run on GAINZ platform. you may need modify it to adapt your own platform.
	#endif

	switch (uart->id)
	{
	/* uart0 configure */
	case 0:
		/* initialize PIN directions. PE0 should be input and PE1 should be output.
		 * @attention:
		 * however, the simple echo program can run successfully without these PIN
		 * directions configuration. I don't know why. 
		 */
		DDRE &= (~(1 << DDE0));		// set PE0/USART0 RXD as input
		DDRE |= (1 << DDE1 );		// set PE1/USART0 TXD as output

		/* set baudrate. assume the CPU clock is 8MHz (for GAINZ)
		 * tested settings: baudrate = 38400 bps 
		 * when UBRR0L is 12, baudrate is 38.4k baud (8.00MHz) error: 0.2%
		 */
		switch (baudrate)
		{
		case 57600: UBRR0L=8; break;
		case 38400: UBRR0L=12; break;
		default: UBRR0L=0; break; // todo correct it 
		}

		/* set frame format: 8 data, 2 stop bit */
		// UCSRC = (1 << USBS) | (3 < UCSZ0);

		#ifdef CONFIG_UART_INTERRUPT_DRIVEN
		UCSR0B = ((1 << RXCIE) | (1 << RXEN) | (1 << TXEN));
		hal_attachhandler( INTNUM_USART0_RX, _uart_rx0_interrupt_handler, uart );
		hal_attachhandler( INTNUM_USART0_UDRE, _uart_tx0_interrupt_handler, uart );
		#else
		/* enable receiver and transmitter */
		UCSR0B = (1 << RXEN0) | (1 << TXEN0);
		#endif
		break;

	/* uart1 configure */
	case 1: 		 
		//HAL_MAKE_PD2_INPUT();
		//HAL_MAKE_PD3_OUTPUT();

		/* {UBRRH, UBRRL} = system clock / (16 * baudrate) - 1 */
		tmpn = (CONFIG_SYSTEM_CLOCK / (16L * baudrate) - 1);
		UBRR1H = (unsigned char)(tmpn >> 8);
		UBRR1L = (unsigned char)tmpn;

		#ifdef CONFIG_UART_INTERRUPT_DRIVEN
		UCSR1B = ((1 << RXCIE) | (1 << RXEN1) | (1 << TXEN1));
		UCSR1C = 0x06;
		hal_attachhandler( INTNUM_USART1_RX, _uart_rx1_interrupt_handler, uart );
		hal_attachhandler( INTNUM_USART1_UDRE, _uart_tx1_interrupt_handler, uart );
		#else
		UCSR1B = (1 << RXEN1) | (1 << TXEN1);
		#endif
		break;

	default:
		uart = NULL;
		break;
	}

	return uart;
}

void uart_close( TiUartAdapter * uart )
{
	// todo:
	// you should disable interrutps here
	// UCSRnB

	#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	uart->rxlen = 0;
	uart->txlen = 0;
    
    switch (uart->id)
    {
    case 0:
		hal_detachhandler( INTNUM_USART1_RX );
		hal_detachhandler( INTNUM_USART1_UDRE );
        break;
    case 1:
		hal_detachhandler( INTNUM_USART1_RX );
		hal_detachhandler( INTNUM_USART1_UDRE );
    };
	#endif
}

/****************************************************************************** 
 * this function is hardware related
 * you should change the register in this function
 *
 * attention: this function will return immediately. it will not wait for the 
 * incoming data. if there's no arrival data pending in the USART's register,
 * this function will simply return -1.
 * 
 * @return
 * 	0		success, *ch is char just read from UART
 *  -1		failed
 *****************************************************************************/
intx uart_getchar( TiUartAdapter * uart, char * pc )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	intx ret = -1;
	hal_atomic_begin();
	if (uart->rxlen > 0)
	{
		*pc = uart->rxbuf[0];
		uart->rxlen --;
		memmove( (void *)&(uart->rxbuf[0]), (void *)&(uart->rxbuf[1]), uart->rxlen );
		ret = 0;
	}
	hal_atomic_end();
	return ret;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	int16 ret=0;

	switch (uart->id)
	{
	case 0:
		if (UCSR0A & (1<<RXC0))
			*pc = UDR0;
		else
			ret = -1;// @todo suggest 0
		break;

	case 1:
		if (UCSR1A & (1<<RXC1))
			*pc = UDR1;
		else
			ret = -1; // @todo suggest 0
		break;
	default:
		ret = -1;
	}

	return ret;
#endif
}

char uart_getchar_wait( TiUartAdapter * uart )   
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	char ch=0;
	while (uart->rxlen <= 0) {};
	uart_getchar( uart, &ch );
	return ch;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	char ch = 0;

	switch (uart->id)
	{
	case 0:
		while (!(UCSR0A & (1<<RXC0))) {};
		ch = UDR0;
		break;
	case 1:
		while (!(UCSR1A & (1<<RXC1))) {};
		ch = UDR1;
		break;
	}

	return ch;
#endif
}

/* uart_putchar()
 * this function sends one character only through the UART hardware. 
 * 
 * @return
 *	0 means success, and -1 means failed (ususally due to the buffer is full)
 *  when this functions returns -1, you need retry.
 */
 intx uart_putchar( TiUartAdapter * uart, char ch )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	intx ret;

	// put the character to be send in the internal buffer "txbuf"
	//
	hal_atomic_begin();
	ret = CONFIG_UART_TXBUFFER_SIZE - uart->txlen;
	if (ret > 0)
	{
		uart->txbuf[uart->txlen] = ch;
		uart->txlen ++;
	}

	// if the background ISR sending doesn't active, then enable and trigger this interrupt
	//
	if (uart->txidx == 0)
	{
		switch (uart->id)
		{
		case 0:
			while (!(UCSR0A & (1<<UDRE0))) {};      // check whether UDR0 is available for the 
													// new character to be sent
			UCSR0B |= (1 << UDRIE);                 // enable sending interrupt. the ISR will
													// continue sending until uart->txbuf is emtpy
			break;
		case 1:
			UCSR1B |= (1 << UDRIE);
			break;
		}
	}	
	hal_atomic_end();

	return (ret > 0) ? 0 : -1;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	/* wait for the transmit buffer empty */
	switch (uart->id)
	{
	case 0:
		while (!(UCSR0A & (1<<UDRE0))) {};
		UDR0 = ch;
		break;
	case 1:
		while (!(UCSR1A & (1<<UDRE1))) {};
		UDR1 = ch;
	}
	return 0;
#endif
}

/****************************************************************************** 
 * read data out from the TiUartAdapter's internal buffer. 
 * this is a non-block operation. it will return 0 if no data received.
 * 
 * @attention
 *	- this function will return immediately. it will return at most the characters.
 * the return value is the data length inside parameter "buf".
 * 
 * @modified by zhangwei on 20070411
 * you should call led_twinkle() before hal_atomic_end(), so i eliminate it.
 * 
 * @param
 * 	buf		the memory buffer to received the data
 * 	size	the capacity of the buffer
 *	option  (reserved for future)
 *
 * @return
 * 	the character count wroten to the buf
 * 	return 0 means there's no data in the TiUartAdapter buffer. i.e. no data received.
 *****************************************************************************/
uintx uart_read( TiUartAdapter * uart, char * buf, uintx size, uint8 opt )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	intx copied=0;

	hal_atomic_begin();
	copied = min( uart->rxlen, size );
	if (copied > 0)
	{
		memmove( (void *)buf, (void *)&(uart->rxbuf[0]), copied );
		uart->rxlen -= copied;
		if (uart->rxlen > 0)
			memmove( (void *)&(uart->rxbuf[0]), (void *)&(uart->rxbuf[copied]), uart->rxlen );
	}
	hal_atomic_end();

	return copied;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	intx ret;
	ret = uart_getchar( uart, buf );
	return (ret>=0) ? 1 : 0;
#endif

    return 0;
}

/****************************************************************************** 
 * write the data in the buffer to UART to sent out
 * this is a non-block operation. 
 * @param
 * 	buf		where's the data
 * 	len		length of the data in the buffer
 *  opt     bit0 == 0: synchronous call(default). == 1: asynchronous call. 
 *          
 *          in the none interrupt driven mode, this function will return until 
 *          all the data in the "buf" have been sent.
 * 
 *          in the interrupt driven mode: this function will return immediately
 *          when bit0 is 1, because it only needs to start the sending process 
 *          and the background interrupt service routine (ISR) can do the sending.
 *          if bit0 is 0 (default), then this function will return until it sends 
 *          all the pending data inside its internal buffer.
 *
 * @return
 * 	the count of characters actually sent
 *****************************************************************************/
uintx uart_write( TiUartAdapter * uart, char * buf, uintx len, uint8 opt )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	intx count = 0;

	hal_atomic_begin();
	count = min(CONFIG_UART_TXBUFFER_SIZE - uart->txlen, len);
	if (count > 0)
	{
		memmove( (void *)&(uart->txbuf[uart->txlen]), (void *)buf, count);
		uart->txlen += count;
	}

	// if the background ISR sending doesn't active, then enable and trigger this interrupt
	//
	if (uart->txidx == 0)
	{
		switch (uart->id)
		{
		case 0:
			while (!(UCSR0A & (1<<UDRE0))) {};      // check whether UDR0 is available for the 
													// new character to be sent
			UCSR0B |= (1 << UDRIE);                 // enable sending interrupt. the ISR will
													// continue sending until uart->txbuf is emtpy
			break;
		case 1:
			UCSR1B |= (1 << UDRIE);
			break;
		}
	}	
	hal_atomic_end();

	// default option is synchronous call. this function will return until all 
	// data being sent successfully.
	//
	if ((opt & 0x01) == 0)
	{
		// wait for sending complete
		while (uart->txlen > 0) {};

		// attention
		// if the parameter "len" is very large, then we "recursively" call uart_write()
		// to send all the data out.  
		//
		if (count < len)
		{
			uart_write( uart, (char *)buf+count, len-count, opt );
			while (uart->txlen > 0) {};
		}
		return len;
	}
	// if this is an asynchronous call, this function will return immediately 
	// and assume the "count" characters in the front of the "buf" has been 
	// sent successfully.
	else{
		return count;
	}
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	int16 count = 0;
	while (count < len)
	{
		if (uart_putchar(uart, buf[count]) < 0)
			break;
		count ++;
	}
	
	return count;
#endif
}


/****************************************************************************** 
 * interrupt handler
 *****************************************************************************/

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx0_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
	if (CONFIG_UART_RXBUFFER_SIZE -  uart->rxlen> 0)
	{
		uart->rxbuf[uart->rxlen] = UDR0;
		uart->rxlen ++;
	} 
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_tx0_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
	if (uart->txidx < uart->txlen)
	{
		UDR0 = uart->txbuf[uart->txidx];
		uart->txidx ++;
	}
	else{
		UCSR0B &= (~(1 << UDRIE));                 // disable sending interrupt. 
		uart->txlen = 0;
		uart->txidx = 0;
	}
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx1_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
	if (CONFIG_UART_RXBUFFER_SIZE -  uart->rxlen> 0)
	{
		uart->rxbuf[uart->rxlen] = UDR1;
		uart->rxlen ++;
	} 
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_tx1_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
	if (uart->txidx < uart->txlen)
	{
		UDR1 = uart->txbuf[uart->txidx];
		uart->txidx ++;
	}
	else{
		UCSR0B &= (~(1 << UDRIE));                 // disable sending interrupt. 
		uart->txlen = 0;
		uart->txidx = 0;
	}
} 
#endif



TiBlockDeviceInterface * uart_get_blockinterface( TiUartAdapter * uart, TiBlockDeviceInterface * intf )
{
	hal_assert( intf != NULL );
	memset( intf, 0x00, sizeof(TiBlockDeviceInterface) );
	intf->provider = uart;
	intf->read = (TiFunBlockDeviceWrite)uart_read;
	intf->write = (TiFunBlockDeviceWrite)uart_write;
	intf->evolve = NULL;
	intf->switchtomode = NULL;
	return intf;
}



