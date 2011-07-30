/* stdstream 
 * this example tests the standard input/output stream in embedded device
 * 
 * before you running this example, you should connect the target device and 
 * your developing computer using a RS232 cable. you should be able to receive
 * the characters sent from the device through printf() call.
 *
 * require
 *  hal_foundation.c, hal_uart.c, hal_stdc, hal_led.c
 *
 * attention 
 *  you may still need to add "libprintf.lib" to your project to enable printf.
 * 
 * @state
 *  - compile passed. 
 *
 * @author zhangwei on 20090714
 *  - first created.
 */

#include "../common/hal/hal_configall.h"
#include <stdio.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_redirect.h"

int main(void)
{
    TiUartAdapter uart;

    /* the current settings is for GAINZ hardware. 
     * you may need to change them to adapter your own hardware */

    uint16 baudrate = 38400;
    uint8 databits = 8, stopbits = 1, parity = 0;

    hal_init( NULL, NULL );
    uart_construct( (void*)&uart, sizeof(uart), 0 );
    uart_configure( &uart, baudrate, databits, stopbits, parity );
    stdc_redirect( &uart );

    printf("Hello sent from hardware!\n");
    //putchar('$');

    uart_destroy( &uart );
}

