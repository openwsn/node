/* shtxx.c
 * test and demonstrate how to read data from the SHT tempature and humidity sensor 
 * 
 * @author zhangwei on 20090717
 *	- first created
 * @modified by Xiao Yuezhang on 20090719
 *	- tested
 */


#include "../common/hal/hal_shtxx.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"

int main(void)
{
	TiShtSensor sht;
	TiUartAdapter uart;
	float tp, humi;
	int i;

	// HAL_SET_PIN_DIRECTIONIS

	uart_construct( (void*)(&uart), sizeof(TiUartAdapter) );
	sht_construct( (void*)(&sht), sizeof(TiShtSensor) );

	uart_open( &uart, 0, 38400, 8, 1, NULL, NULL );
	sht_open( &sht, 0 );
	led_open();

	for (i=0; i<10; i++)
	{
		led_off( LED_RED );
		if (sht_value( &sht, &tp, &humi ) == 0)
		{
			uart_write( &uart, &tp, sizeof(tp) );
			uart_putchar( '\n' );
			uart_write( &uart, &tp, sizeof(tp) );
		}
		hal_delay( 500 );
		led_on( LED_RED );
		hal_delay( 500 );
	}

	led_close();
	sht_close( &sht );
	uart_close( &uart );

	uart_destroy( &uart );
	sht_destroy( &sht );
}
