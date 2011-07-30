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
#include "../common/hal/hal_configall.h"  
#include "../common/svc/svc_configall.h"  
#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"
#include "svc_wlsmodem.h"

#define CONFIG_ALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS		0x01
#define CONFIG_ALOHA_DEFAULT_REMOTE_ADDRESS		0x02
#define CONFIG_ALOHA_DEFAULT_CHANNEL            11


TiWlsModem * wlsmodem_construct( void * mem, uint16 memsize )
{
	memset( mem, 0x00, memsize );
	return (TiWlsModem *)mem;
}

void wlsmodem_destroy( TiWlsModem * modem )
{
	modem = modem;
	return;
}

TiWlsModem * wlsmodem_open( TiWlsModem * modem, TiUartAdapter * uart, TiAloha * mac )
{
	modem->uart = uart;
	modem->mac = mac;
	modem->sio_rxbuf = iobuf_construct( (void *)(&modem->sio_rxbuf_memory[0]), sizeof(modem->sio_rxbuf_memory) );
	modem->sio_txbuf = iobuf_construct( (void *)(&modem->sio_txbuf_memory[0]), sizeof(modem->sio_txbuf_memory) );
	modem->wio_rxbuf = opf_open( (void *)(&modem->wio_rxbuf_memory[0]), sizeof(modem->wio_rxbuf_memory), 
		OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	modem->wio_rxbuf = opf_open( (void *)(&modem->wio_rxbuf_memory[0]), sizeof(modem->wio_rxbuf_memory), 
		OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	return modem;
}

void wlsmodem_close( TiWlsModem * modem )
{
	modem = modem;
	return;
}

/* wlsmodem_evolve()
 * This function implements the state machine of wireless modem:
 *
 * - data path: Uart => sio_rxbuf => wio_txbuf => Transceiver
 *
 * if the sio_rxbuf isn't full, then
 *   read data into sio_rxbuf from uart
 * endif
 * if the wio_txbuf isn't empty, then 
 *   try send the frame inside wio_rxbuf;
 *   if sending success then 
 *     clear wio_txbuf;
 * endif
 * if the wio_txbuf is empty and sio_rxbuf isn't empty, then
 *   encapsulate the data inside sio_rxbuf as a frame and put it into wio_txbuf
 *   clear sio_rxbuf
 * endif
 *
 * - data path: Transceiver => wio_rxbuf => sio_rxbuf => Uart
 * 
 * if the wio_rxbuf is empty
 *   try receive frame into wio_rxbuf from transceiver
 * endif
 * if the sio_rxbuf is empty
 *   move the payload inside frame received only into sio_txbuf
 *   clear wio_rxbuf
 * endif
 * if the sio_txbuf isn't empty
 *   try uart send sio_txbuf
 *   if send successful, then clear the data successfully sent from sio_rxbuf
 * endif
 */

void wlsmodem_evolve( void * svcptr, TiEvent * e )
{
	TiWlsModem * modem = (TiWlsModem *)svcptr;
	uint8 count, size;
	uint16 fcf;
	
	/* - data path: Uart => sio_rxbuf => wio_txbuf => Transceiver
	 *
	 * if the sio_rxbuf isn't full, then
	 *   read data into sio_rxbuf from uart
	 * endif
	 */

    if( !iobuf_full(modem->sio_rxbuf) )
    {
        count = uart_read( modem->uart, iobuf_ptr( modem->sio_rxbuf )+
			iobuf_length( modem->sio_rxbuf ), iobuf_size( modem->sio_rxbuf ) 
				- iobuf_length( modem->sio_rxbuf ), 0x00 );
        if(count>0)
		{
			iobuf_setlength( modem->sio_rxbuf, iobuf_length( modem->sio_rxbuf ) + count );
		}
		else
		{
			led_toggle(LED_GREEN);
		}
    }

	/* if the wio_txbuf isn't empty, then 
	 *   try send the frame inside wio_rxbuf;
	 *   if sending success then 
	 *     clear wio_rxbuf;
	 * endif
	 */

    if( !opf_empty( modem->wio_txbuf ) )
	{
		count = aloha_send( modem->mac, modem->wio_txbuf, 0x00);
		if( count == opf_datalen( modem->wio_txbuf) )
		{
			opf_clear( modem->wio_txbuf );
		}
		else 
		{
			dbo_string( " Sending fail !!! " );
		}
	}

	/* if the wio_txbuf is empty and sio_rxbuf isn't empty, then
	 *   encapsulate the data inside sio_rxbuf as a frame and put it into wio_txbuf
	 *   clear sio_rxbuf
	 * endif
	 */
	
	if( opf_empty( modem->wio_txbuf ) && !( iobuf_empty(modem->sio_rxbuf) ) )
	{	
		fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
		hal_assert( iobuf_length(modem->sio_rxbuf) <= 0x7F-14 ); // 14 is the header size plus 2 byte checksum
		size = 14 + iobuf_length(modem->sio_rxbuf);	// frame length, which is equal the length of PSDU
                                               		// plus addtional 1.	                                           
		opf_cast( modem->wio_txbuf, size, fcf );

		opf_set_sequence( modem->wio_txbuf, 0x88 );
		opf_set_panto( modem->wio_txbuf, CONFIG_ALOHA_DEFAULT_PANID );
		opf_set_shortaddrto( modem->wio_txbuf, CONFIG_ALOHA_DEFAULT_REMOTE_ADDRESS );
		opf_set_panfrom( modem->wio_txbuf, CONFIG_ALOHA_DEFAULT_PANID );
		opf_set_shortaddrfrom( modem->wio_txbuf, CONFIG_ALOHA_DEFAULT_LOCAL_ADDRESS );

		memmove( opf_msdu(modem->wio_txbuf), iobuf_data(modem->sio_rxbuf), iobuf_length(modem->sio_rxbuf));

		iobuf_clear( modem->sio_rxbuf );

	}
	/* - data path: Transceiver => wio_rxbuf => sio_txbuf => Uart
	 * 
	 * if the wio_rxbuf is empty
	 *   try receive frame into wio_rxbuf from transceiver
	 * endif
	 */
	
	if( opf_empty( modem->wio_rxbuf ) )
	{
		count = aloha_recv( modem->mac, modem->wio_rxbuf, 0x00 );
		if( count <= 0 ) led_toggle( LED_RED );
	}
	
	/* if the sio_txbuf is empty
	 *   move the payload inside frame received only into sio_txbuf
	 *   clear wio_rxbuf
	 * endif
	 */
	
	if( iobuf_empty( modem->sio_txbuf ) )
	{
		memmove( iobuf_ptr(modem->sio_txbuf), opf_msdu(modem->wio_rxbuf), opf_msdu_len(modem->wio_rxbuf) );
		iobuf_setlength( modem->sio_txbuf, opf_msdu_len(modem->wio_rxbuf) );
		opf_clear( modem->wio_rxbuf );
	}

	/* if the sio_txbuf isn't empty
	 *   try uart send sio_txbuf
	 *   if send successful, then clear the data successfully sent from sio_rxbuf
	 * endif
	 */

	if( !iobuf_empty( modem->sio_txbuf ) )
	{
		count = uart_write( modem->uart, iobuf_data(modem->sio_txbuf), iobuf_length(modem->sio_txbuf), 0x01);
	}
}
