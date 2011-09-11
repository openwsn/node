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

#define UARTECHO_INTERRUPT
#undef  UARTECHO_INTERRUPT

#include "../../common/openwsn/hal/hal_configall.h"
#include <avr/wdt.h>
#include <string.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_target.h"

#define BUF_SIZE 128

static void _targetinit(void);
static void uartecho_simpleoutput(void);
static void uartecho_simple( void );
static void uartecho_readwrite( void );
static void uart_putbyte( TiUartAdapter * uart, uint8 val );

int main( void )
{
	//uartecho_simple();
	uartecho_readwrite();
    //uartecho_simpleoutput();

	return 0;
}

void _targetinit(void)
{
    // todo for xiaoyuezhang
    // 请参考该宏，将有关PIN direction设置直接写到uart_open函数中，最终要取消
    // 该宏。我们之所以不能正常收发，极大的可能是由于没有正确设置pin direction
	// HAL_SET_PIN_DIRECTIONS();

	target_init();

	wdt_disable();
	led_open();
	led_on( LED_ALL );
	hal_delay(500);
	led_off( LED_ALL );
	// hal_delay(500);
	// led_on( LED_RED );
}

/* putchar/getchar based uartecho */
void uartecho_simple( void )
{
	TiUartAdapter uart;
    char ch;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );

	#ifdef UARTECHO_INTERRUPT
    hal_enable_interrupts();
    #endif

    while (1) 
    {
        led_off( LED_RED );
        if (uart_getchar(&uart, &ch) >= 0)
        {
            led_on( LED_RED );
            uart_putchar(&uart, ch+1 );
        }
    }
    
    uart_close( &uart );
    uart_destroy( &uart );
}

/* read/write based uartecho */
void uartecho_readwrite( void )  
{
	TiUartAdapter uart;
    char mem[BUF_SIZE], * buf;
    uint8 count, len;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );


	#ifdef UARTECHO_INTERRUPT
    //hal_enable_interrupts();
    #endif

    uart_putchar( &uart, '*');

    buf = &(mem[0]);
    len = 0;
	while (1)
	{
        led_off( LED_ALL );
		count = uart_read( &uart, buf+len, BUF_SIZE-len, 0x00 );
		len += count;
        
        if (count > 0) 
        {
            // led_on( LED_YELLOW );
			// uart_putchar( &uart, '*' );
			// uart_putbyte( &uart, count );
			// uart_putchar( &uart, ' ' );
        }

		if (len > 0)
		{
			count = uart_write( &uart, buf, len, 0x00 );
			if (count > 0)
			{
				memmove( buf, buf+count, len-count );
				len -= count;
				led_toggle( LED_GREEN );
			}
		}

		hal_delay( 50 ); 
	}
    
    uart_close( &uart );
    uart_destroy( &uart );
}

void uart_putbyte( TiUartAdapter * uart, uint8 val )
{
    uart_putchar( uart, (val >> 4) + '0' );
    uart_putchar( uart, (val & 0x0F) + '0' );    
}

void uartecho_simpleoutput(void)
{
	TiUartAdapter uart;
    char i;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );

	#ifdef UARTECHO_INTERRUPT
    hal_enable_interrupts();
    #endif

    while (1) 
    {
        led_on( LED_RED );
        for (i=0; i<10; i++)
        {
            uart_putchar( &uart, '*');
            led_toggle( LED_RED );
            hal_delay( 100 );
        }
        uart_putchar( &uart, '\n' );
		hal_delay( 200 );
		led_off( LED_RED );
		hal_delay( 200 );
    }
    
    uart_close( &uart );
    uart_destroy( &uart );
}
