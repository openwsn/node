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
#ifndef _SVC_WLSMODEM_H_7243_
#define _SVC_WLSMODEM_H_7243_

#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_uart.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"

/* attention
 * the data inside serial I/O buffer(sio buffer) must be smaller enough so that 
 * it can be placed inside a wireless frame. 
 */
#define CONFIG_SIO_RXBUF_SIZE 75
#define CONFIG_SIO_TXBUF_SIZE 125
#define CONFIG_WIO_RXBUF_SIZE CONFIG_SIO_TXBUF_SIZE
#define CONFIG_WIO_TXBUF_SIZE CONFIG_SIO_TXBUF_SIZE

/* todo: In order to decrease the data loss probability, the sio_rxbuf and wio_rxbuf
 * should be replaced with an queue. However, the current solution is more appropriate
 * for memory-constraint systems. 
 *
 * attention: due to the memory management, the following variable order cannot be changed!!!
 */
typedef struct{
	TiUartAdapter * uart;
	TiAloha * mac;
	TiTimer timer;
	TiIoBuf * sio_rxbuf;
	TiIoBuf * sio_txbuf;
    TiOpenFrame * wio_rxbuf;
    TiOpenFrame * wio_txbuf;

    char sio_rxbuf_memory[IOBUF_HOPESIZE(CONFIG_SIO_RXBUF_SIZE)];
    char sio_txbuf_memory[IOBUF_HOPESIZE(CONFIG_SIO_TXBUF_SIZE)];
	char wio_rxbuf_memory[OPF_HOPESIZE(CONFIG_WIO_RXBUF_SIZE)];
	char wio_txbuf_memory[OPF_HOPESIZE(CONFIG_WIO_TXBUF_SIZE)];
	
}TiWlsModem;


TiWlsModem * wlsmodem_construct( void * mem, uint16 memsize );
void wlsmodem_destroy( TiWlsModem * modem );
TiWlsModem * wlsmodem_open( TiWlsModem * modem, TiUartAdapter * uart, TiAloha * mac );
void wlsmodem_close( TiWlsModem * modem );
void wlsmodem_evolve( void * svcptr, TiEvent * e );


#endif /* _SVC_WLSMODEM_H_7243_ */
