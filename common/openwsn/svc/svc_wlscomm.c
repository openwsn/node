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

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_openframe.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_wlscomm.h"

/* prefix meanings:
 * "nac_" => "network acceptor"
 * "ios_" => "input/output service"
 * "nio_" => "network input/output"
 * "fc" => filter chain
 */

TiNetIoService * nio_construct( void * mem, uint16 size, _TiWirelessAdapter * adapter, 
	TiFilterChain * filterchain, TiIoHandler iohandler )
{
	TiNetIoService * svc;

	hal_assert( (adapter != NULL) && (filterchain != NULL) && (iohandler != NULL) );

	memset( mem, 0x00, size );
	svc = (TiNetIoService *)mem;
	svc->state = 0;
	svc->adapter = adapter;
	svc->filterchain = filterchain;
	svc->iohandler = iohandler;

	return iohandler;
}

void nio_destroy( TiNetIoService * nio )
{
	nio = nio;
	return;
}

void nio_evolve( TiNetIoService * nio, TiEvent * e )
{
	if (iobuf_empty( nio->rxbuf ))
	{
		len = _wlsbase_recv( nio->adapter, iobuf->buf, iobuf->size, 0x00 );
		iobuf->length = len;
	}
	if (!iobuf_empty(nio->rxbuf) && iobuf_empty(nio->txbuf))
	{
		fltc_rxhandle( nio->rxbuf, nio->txbuf );
	}
	if (!iobuf_empty(nio->txbuf))
	{
		len = _wlsbase_send( nio->adapter, iobuf->buf, iobuf->len, 0x00 );
		if (len > 0)
			iobuf_clear( nio->txbuf );
	}

	fltc_evolve( nio->filterchain, NULL );
}

