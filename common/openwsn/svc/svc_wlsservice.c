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
#include "../rtl/rtl_openframe.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_foundation.h"
#include "svc_acceptor.h"




TiNetworkAcceptor * nac_construct( void * mem, uint16 size )
{
	svc_assert( sizeof(TiNetworkAcceptor) <= size );
	memset( mem, 0x00, size );
	return (TiNetworkAcceptor *)mem;
}

void nac_destroy( TiNetworkAcceptor * nac )
{
	nac_close(nac);
}

TiNetworkAcceptor * nac_open( TiNetworkAcceptor * nac, _TiNetworkAdapter * adt, TiIoHandler * handler )
{
	nac->state = 0;
	nac->adt = adt;
}

void nac_close( TiNetworkAcceptor * nac )
{
}

void nac_evolve( TiNetworkAcceptor * nac, TiEvent * e )
{
	uintx len;

	/* If the acceptor receives an event to indicate the data is ready, then the acceptor
	 * should try to retrieve the data out from the adapter */
	if (e->id == EVENT_DATA_ARRIVAL)
	{
		/* assert there must have available spaces to hold the received data, or else
		 * not necessary to call _adt_read() */
		if nac->rxframe is not empty
		{
			len = _adt_read(nac->adapter, nac->rxframe, 0x00);
			if (len > 0)
			{
				// dbo_putchar('9');
				fmque_push(nac->rxframe);
				svc_assert( always push success here );
			}
		}
	}

	if (!fmque_empty(nac->txque))
	{
		if (_adt_send(nac->adapter, fmque_front(nac->txque)) > 0)
		{
			fmque_popfront(nac->txque);
		}
	}

	if (fmque_count(nac->rxque) > 0)
	{
		frame = fmque_front(nac->rxque);
		len = iof_chain_rxhandle( chain, input, output );
		if (len > 0)
		{
			nac->iohandler( nac->iohandler_owner, output, output );
			fmque_pushback( nac->txque, output );
			assert: if (pushsuccess)
		}
	}

	nac->txiohandler( );
	fmque_push( txque );

}

uint8 nac_send( TiNetworkAcceptor * nac, TiIoBuffer * iobuf )
{
	len = iof_chain_txhandle( nac->chain, iobuf, output );
	if (len > 0)
		fmque_pushback( nac->txque, iobuf );
}

uint8 nac_adt_send( TiNetworkAcceptor * nac, TiIoBuffer * iobuf )
{
	return fmque_pushback( nac->txque, iobuf );
}

#endif
