/**
 * frame.c
 * test TiFrame. running on Atmel ATmega128 simulator2
 * 
 * @state
 * 	tested by zhangwei, xufuzhen(Dept. of Control, TongJi Univ.)
 */

#define CONFIG_DEBUG

#include "../../common/openwsn/rtl/rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_target.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "debugdevice.h"

#define MEMSIZE FRAME_HOPESIZE(64)

static int _output_buffer_ascii_format( char * buf, int len, int max_per_line );
static int _output_buffer_binary_format( char * buf, int len, int max_per_line );
static void _frame_dump( TiFrame * frame );
static int test1();

int main()
{
    test1();
    return 0;
}

int test1()
{
    //TiUartAdapter m_uart, *uart;
    void * dbio;
    char memframe[MEMSIZE];
    TiFrame * f;

	target_init();
	//HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open(); 
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );
	//dbo_open( 0, 38400 );
    //dbo_string( "welcome from dbo_string\n" )

	//uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	//uart_open( uart, 0, 38400, 8, 1, 0x00 );

    dbio = (void *)dbio_open( 38400 );
    rtl_init( dbio, (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_string( "hello, this is frame demo application\n" );    

    f = frame_construct( &memframe[0], MEMSIZE );

    dbc_string( "frame_curlayer() = " );
    dbc_n16toa( frame_curlayer(f) );
    dbc_putchar( '\n' );

    memmove( frame_startptr(f), "abcdefg", 7 );
    frame_skipinner( f, 7, 2 );
    memmove( frame_startptr(f) , "0123456789", 10 );
    frame_skipinner( f, 10, 0 );
    memmove( frame_startptr(f), "hijklmn", 7 );
    frame_skipinner( f, 7, 1 );
    memmove( frame_startptr(f) , "0123456789", 10 );
    frame_skipinner( f, 10, 0 );
    memmove( frame_startptr(f), "opqrst", 6 );
    frame_skipinner( f, 6, 1 );
    memmove( frame_startptr(f) , "0123456789", 10 );
    frame_skipinner( f, 10, 0 );


    dbc_putchar( '\n' );
    dbc_string( "frame internal content = \n" );
    _output_buffer_ascii_format( frame_totalstartptr(f), frame_totallength(f), 10 );

    dbc_putchar( '\n' );
    dbc_string( "frame dump = \n" );
    _frame_dump( f );

    dbc_putchar( '\n' );
    dbc_string( "retrieve frame content out \n" );
    frame_movefirst( f );
    do{
        dbc_string( "frame package: current = " );
        dbc_n16toa( frame_curlayer(f) );
        _output_buffer_ascii_format( frame_startptr(f), frame_length(f), 10 );
    }while (frame_moveinner(f));    

    return 0;
}

int _output_buffer_ascii_format( char * buf, int len, int max_per_line )
{
    int i;

    dbc_putchar( '\n' );
    for (i=0; i<len; i++)
    {
        dbc_n8toa( buf[i] );
        dbc_putchar(' ');
        if (i % max_per_line == 0)
            dbc_putchar( '\n' );
    }

    return 0;
}

int _output_buffer_binary_format( char * buf, int len, int max_per_line )
{
    int i;

    dbc_putchar( '\n' );
    for (i=0; i<len; i++)
    {
        dbc_putchar( buf[i] );
        dbc_putchar( 0xAA );
    }

    return 0;
}

void _frame_dump( TiFrame * frame )
{
    uintx i;

    dbc_string( "\nframe_dump(frame): " );
    dbc_string( "\nframe_memsize = " );
    dbc_n16toa( frame->memsize );
    dbc_string( "\nframe->firstlayer = " );
    dbc_n16toa( frame->firstlayer );
    dbc_string( "\nframe->curlayer = " );
    dbc_n16toa( frame->curlayer );
    dbc_string( "\nframe->layercount = " );
    dbc_n16toa( frame->layercount );
    dbc_putchar( '\n' );

    for (i=0; i<CONFIG_FRAME_LAYER_CAPACITY; i++)
    {
        dbc_string( "layer " );
        dbc_n8toa( i );
        dbc_putchar( ':' );
        dbc_putchar( ' ' );
        dbc_n16toa( frame->layerstart[i] );
        dbc_putchar( ',' );
        dbc_n16toa( frame->layerlength[i] );
        dbc_putchar( ',' );
        dbc_n16toa( frame->layercapacity[i] );
        dbc_putchar( '\n' );
    }

    dbc_string( "content: " );
    dbc_mem( frame_totalstartptr(frame), frame_totallength(frame) );
    dbc_putchar( '\n' );
}

