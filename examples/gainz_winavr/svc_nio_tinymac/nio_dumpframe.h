#ifndef _NIO_DUMPFRAME_H_2442_
#define _NIO_DUMPFRAME_H_2442_

#include "../hal/hal_configall.h"
#include "../hal/hal_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_uart.h"

/**
 * @attention: before calling this function, you should already call rtl_init() to 
 * initialize the fundamental input/output interface 
 * 
 * For example:
 *     	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, 
 * 			hal_assert_report );
 *    	dbc_putchar( 0xF0 );
 * 		dbc_mem( msg, strlen(msg) );
 *		......
 *   	ieee802frame154_dump( frame );
 */
//void ieee802frame154_dump( TiFrame * frame, TiUartAdapter * uart );
void ieee802frame154_dump( TiFrame * frame );

#endif
