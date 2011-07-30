#include "svc_configall.h"
#include "svc_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_device.h"
#include "../hal/hal_debugio.h"
#include "svc_siocomm.h"

/* @modified by zhangwei on 2010.05.03
 *  - comment the 3 #define below.
 *  - convert the  sio_construct, sio_construct to the version used in node.
 *  - modified the initial of spliter in sio_construct()
 */

//#define _sio_rxbuf(sio) ((TiIoBuf*)((char*)sio + sizeof(TiSioComm)))
//#define _sio_txbuf(sio) ((TiIoBuf*)((char*)sio + sizeof(TiSioComm) + IOBUF_HOPESIZE(CONFIG_SIOCOMM_PKTSIZE)))
//#define _sio_rxque(sio) ((TiIoBuf*)((char*)sio + sizeof(TiSioComm) + IOBUF_HOPESIZE(CONFIG_SIOCOMM_PKTSIZE)*2))

/* Construct an TiSioComm service in the specified memory block */
TiSioComm * sio_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiSioComm) <= size );
	memset( buf, 0x00, size );
	return (TiSioComm *)buf;
}

/* Destroy an TiSioComm service */
void sio_destroy( TiSioComm * sio )
{
	sio_close(sio);
}

#ifdef CONFIG_DYNA_MEMORY
TiSioComm * sio_create( uint16 size )
{
	TiSioComm * sio = (TiSioComm *)malloc( sizeof(TiSioComm) );//SIOCOMM_HOPESIZE(size) );
	if (sio != NULL)
	{
		sio_construct( (void*)sio, sizeof(TiSioComm) );
	}
	return sio;
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void sio_free( TiSioComm * sio )
{
	if (sio != NULL)
	{
		sio_destroy( sio );
		free( sio );
	}
}
#endif

/* Open an TiSioComm service */
/*
TiSioComm * sio_open( TiSioComm * sio, TiUartAdapter * uart, uint8 opt )
{
	sio->uart = uart;
	sio->option = opt;
	sio->spliter = tspl_create(SIO_RXBUFFER_CAPACITY*2);
	sio->txbuf = iobuf_construct( &sio->txmem[0], sizeof(sio->txmem) );
	sio->rxbuf = iobuf_construct( &sio->rxmem[0], sizeof(sio->rxmem) );
	sio->rxque = iobuf_construct( &sio->quemem[0], sizeof(sio->quemem) );
	sio->tmp_iobuf = iobuf_construct( &sio->tmp_buf[0], sizeof(sio->tmp_buf) );
	return sio;
}
*/

TiSioComm * sio_open( TiSioComm * sio, TiBlockDeviceInterface * device, uint8 opt )
{
	sio->option = opt;
	sio->spliter = tspl_construct( &sio->splitermem[0], sizeof(sio->splitermem) );
	sio->txbuf = iobuf_construct( &sio->txmem[0], sizeof(sio->txmem) );
	sio->rxbuf = iobuf_construct( &sio->rxmem[0], sizeof(sio->rxmem) );
	sio->rxque = iobuf_construct( &sio->quemem[0], sizeof(sio->quemem) );
	sio->tmp_iobuf = iobuf_construct( &sio->tmp_buf[0], sizeof(sio->tmp_buf) );

	memmove( &(sio->device), device, sizeof(TiBlockDeviceInterface) );
	sio->device.owner = sio;
	return sio;
}

void sio_close( TiSioComm * sio )
{
	iobuf_clear(sio->tmp_iobuf);
	iobuf_clear(sio->txbuf);
	iobuf_clear(sio->rxbuf);
	iobuf_clear(sio->rxque);
}

/* Configure an TiSioComm service 
 * 
 * @todo
 * in the future, we'd better use TiSvcTimer instead of TiTimerAdapter now.
 */
 
void sio_configure( TiSioComm * sio, uint8 opt )
{
	// sio->timer = timer;
	// sio->option = option;
}

uintx sio_read( TiSioComm * sio, TiIoBuf * iobuf, uintx option )
{
	uintx available, count;
	uint8 success = 0;
	iobuf_clear(sio->tmp_iobuf);

	available = iobuf_available(sio->rxbuf);
	// count = uart_read( sio->uart, iobuf_endptr(sio->rxbuf), available, 0x00 );
	count = sio->device.read( sio->device.provider, iobuf_endptr(sio->rxbuf), available, 0x00 );

	if (count > 0)
	{
		iobuf_setlength( sio->rxbuf, iobuf_length(sio->rxbuf)+count );
	}

	// If uart_read() got some data and the sio->rxque is empty, then scan
	// the frame flag bytes in the cache and perform frame spliting. If it founds an
	// entire frame, then move this frame into sio->rxque
	//
	if (!iobuf_empty(sio->rxbuf) && (iobuf_empty(sio->rxque)))
	{
		// scan the data stream in "sio->rxbuf" for packet according to start and stop characters.
		// if found entire packet, then move it into "sio->rxque".

		count = tspl_rxhandle( sio->spliter, sio->rxbuf, sio->rxque, &success );
		if (count > 0)
		{
			iobuf_popfront( sio->rxbuf, count );
		}
		if( success == 0 )
			return 0;

		count = text_decode( sio->rxque, sio->tmp_iobuf );
		if(count > 0)
			iobuf_clear(sio->rxque);
		else
			return 0;

		iobuf_pushbyte( sio->rxque, SIO_START );
		iobuf_pushbyte( sio->rxque, (count>>8)&0xFF );
		iobuf_pushbyte( sio->rxque, count&0xFF );
		iobuf_append( sio->rxque, sio->tmp_iobuf );
	}

	// if the sio->rxque has an entire frame
	//
	if (!iobuf_empty(sio->rxque))
	{
		iobuf_copyto( sio->rxque, iobuf );
		iobuf_clear( sio->rxque );
		return iobuf_length( iobuf );
	}
	else
		return 0;
}


/* Write a packet into TiSioComm. The packet will be sent out through UART interface */
uintx sio_write( TiSioComm * sio, TiIoBuf * iobuf, uintx option )
{
	uint16 count=0, result=0, available=0;

	hal_assert( !iobuf_empty(iobuf) );

	iobuf_clear( sio->tmp_iobuf );
	available = iobuf_available(sio->txbuf);
	if (available >= iobuf_length(iobuf))
	{
		iobuf_popfront(iobuf, 3);
		result = text_encode( iobuf, sio->tmp_iobuf );
		result = tspl_txhandle( sio->spliter, sio->tmp_iobuf, sio->txbuf );
	}

	if (!iobuf_empty(sio->txbuf))
	{
		// count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf), 0x00 );
		count = sio->device.write( sio->device.provider, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf), 0x00 );
		if (count > 0)
		{
			iobuf_popfront(sio->txbuf, count);
		}
	}

	return result;

}

/* Evolve the TiSioComm object */
void sio_evolve( TiSioComm * sio, TiEvent * e )
{
	// sio->device.evolve( sio->device.provider, e );

	/*
	uint8 count;

	if (!iobuf_empty(sio->txbuf)
	{
		count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf) );
		if (count > 0)
		{
			iobuf_popfront(count);
		}
	}
	*/
}

