#include "openwsn/hal/hal_configall.h"
#include "openwsn/hal/hal_foundation.h"
#include "apl_foundation.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_uart.h"

TiUartAdapter               m_uart;


void main( void)
{
    TiUartAdapter * uart;
    uint8 ch;
    char buf[40];
    uintx count;

    count = 0;

    led_open();

    uart = uart_construct( (void *)(&m_uart),sizeof(m_uart));

    uart = uart_open( uart,2,9600,8,1,0);


    while ( 1)
    {
        count = uart_read(uart,buf,40,0);

        if ( count)
        {
            uart_write( uart,buf,count,0);
            led_toggle(LED_RED);
        }
        /*
        if ( uart_getchar(uart,&ch))
        {
            uart_putchar( uart,ch);
            led_toggle(LED_RED);

        }*/

//        if ( uart_getchar(uart,&ch))
//        {
//            uart_putchar(uart,ch);
//        }

    }
}
