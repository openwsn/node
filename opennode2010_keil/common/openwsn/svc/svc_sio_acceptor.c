
/*
intx sac_send( TiNioAcceptor * nac, TiFrame * frame, uint8 option );

/** Move the frame received inside rxque into frame object */
/*intx sac_recv( TiNioAcceptor * nac, TiFrame * frame ,uint8 option);*/





#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_slipfilter.h"
#include "../rtl/rtl_frame.h"
#include "../hal/opennode2010/hal_uart.h"
#include "svc_sio_acceptor.h"

TiIoBuf  m_txbuf;
TiIoBuf  m_rxbuf;
TiIoBuf  m_tmpbuf;
TiIoBuf  m_rmpbuf;


/* define this macro to enable framing */


/**
 * Create an TiSioAcceptor object and returns the pointer to it.
 * 
 * @param name Refer to CreateFile() function in Win32 API. Attention it should be TCHAR.
 *		By default we use UNICODE, so here TCHAR should be actually wchar_t
 * @param baudrate Default 9600
 * @param databits Default 8
 * @param stopbits Default to 0 (which means has 1 stopbit)
 * @param parity Default to 0
 * 
 * @return An pointer value to the TxUartAdapter object created. This value is used
 * to call io_rs232_close, rs232_read and io_rs232_write.
 */

TiSioAcceptor * sac_open( TiSioAcceptor * sac, TiSlipFilter *slip,TiUartAdapter * uart )
{
	TiSioAcceptor * io = sac;

	sac_close(io);

	memset(io, 0x00, sizeof(TiSioAcceptor) );

    
	io->device = uart;
	
	if (io->device != NULL)
	{

		//io->rxbuf = iobuf_create( CONFIG_IO4RS232_RXBUF_CAPACITY );
		//io->txbuf = iobuf_create( CONFIG_IO4RS232_TXBUF_CAPACITY );

        io->txbuf = iobuf_construct( (void *)(&m_txbuf),IOBUF_HOPESIZE(CONFIG_IO4RS232_RXBUF_CAPACITY));
        io->rxbuf = iobuf_construct( (void *)(&m_rxbuf),IOBUF_HOPESIZE(CONFIG_IO4RS232_RXBUF_CAPACITY));

		#ifdef RS232_IOSERVICE_SLIP_ENABLE
		//io->tmpbuf = iobuf_create( CONFIG_IO4RS232_TMPBUF_CAPACITY );
        io->tmpbuf = iobuf_construct( (void *)(&m_tmpbuf),IOBUF_HOPESIZE(CONFIG_IO4RS232_TMPBUF_CAPACITY));
		io->rx_accepted = 0;
		io->slipfilter = slip;//slip_filter_construct((void *)(&sac->slipfilter),sizeof( sac->slipfilter) );
		#endif
	}
	else{
		if (io->rxbuf != NULL)
			iobuf_destroy(io->rxbuf);
		if (io->txbuf != NULL)
			iobuf_destroy(io->txbuf);

		#ifdef RS232_IOSERVICE_SLIP_ENABLE
		if (io->tmpbuf != NULL)
			iobuf_destroy(io->tmpbuf);
        
		if (io->slipfilter != NULL)
			slip_filter_destroy(io->slipfilter);
		#endif

		io = NULL;
	}

	return io;
}

/**
 * Close an TiSioAcceptor object and release resources allocated in io_rs232_open() function.
 */
void sac_close( TiSioAcceptor * sac ) 
{
	TiSioAcceptor * io = (TiSioAcceptor *)sac;

	if (io != NULL)
	{
//		if (io->device != NULL)
//		{
//			delete io->device;
//		}

		if (io->rxbuf != NULL)
			iobuf_destroy(io->rxbuf);
		if (io->txbuf != NULL)
			iobuf_destroy(io->txbuf);

		#ifdef RS232_IOSERVICE_SLIP_ENABLE
		if (io->tmpbuf != NULL)
			iobuf_destroy(io->tmpbuf);
		if (io->slipfilter != NULL)
			slip_filter_destroy(io->slipfilter);
		#endif

		memset(io, 0x00, sizeof(TiSioAcceptor));
	}
}

/**
 * Retrieve the first frame received through the rs232 serial port. 
 */
uint8 sac_read( TiSioAcceptor * sac, TiFrame * buf,uint8 size, uint8 option ) 
{
	TiSioAcceptor * io = (TiSioAcceptor *)sac;
	uint8 count=0;
    hal_assert( sac != NULL );
	#ifdef RS232_IOSERVICE_SLIP_ENABLE
	if (io->rx_accepted)
	{
		hal_assert(iobuf_length(io->rxbuf) > 0);
		if (iobuf_length(io->rxbuf) > 0)
		{
			count = iobuf_read(io->rxbuf, (char *)buf, size);
			iobuf_clear(io->rxbuf);
		}
		io->rx_accepted = 0;
	}
	#endif

	#ifndef RS232_IOSERVICE_SLIP_ENABLE
	if (iobuf_length(io->rxbuf) > 0)
	{
		count = iobuf_read(io->rxbuf,(char *) buf, size);
		iobuf_clear(io->rxbuf);
	}
	#endif

	sac_evolve(io);

	return count;
}

/**
 * Send a frame through the rs232 data channel. 
 *
 * @param buf Contains the frame to be sent.
 * @param len Length of the frame tobe send.
 * @param option Reserved. It should be 0x00 currently.
 * 
 * @return The data length successfully sent. It should equal to parameter "len". 
 *	But it may be not equal if some exception occurs. Attention the internal io->rxbuf
 *  should be large enough to accept all the buf data inputed.
 */
uint8 sac_write( TiSioAcceptor * sac, TiFrame * buf, uint8 len,uint8 option )  
{
   // TiIoBuf  m_nmpbuf;
    uint8 i;//todo for testing
    char *pc;//todo for testing
    char *ac;//todo for testing

	TiSioAcceptor * io = (TiSioAcceptor *)sac;
	uint8 count=0;
	#ifdef RS232_IOSERVICE_SLIP_ENABLE
	TiIoBuf  *tmpbuf;
	#endif

	/* @attention
	 * @warning
	 * The following assumes io->txbuf can accept all data inputed from parameter "buf". 
	 * If not, then the frame maybe losted
	 */
    hal_assert( sac != NULL );
	#ifdef RS232_IOSERVICE_SLIP_ENABLE
	if (iobuf_empty(io->txbuf))
	{
		tmpbuf = iobuf_create(len);
        //tmpbuf = iobuf_construct( ( void *)(&m_rmpbuf),IOBUF_HOPESIZE(len));
		iobuf_write(io->tmpbuf, frame_startptr(buf), len);//todo 这一句是有问题的
		count = slip_filter_tx_handler( io->slipfilter, io->tmpbuf, io->txbuf );
		iobuf_free(tmpbuf);
	}
	#endif

	#ifndef RS232_IOSERVICE_SLIP_ENABLE
	if (iobuf_empty(io->txbuf))
	{
		count = iobuf_write(io->txbuf,frame_startptr(buf), len);//todo 这一句是有问题的
	}
    
	#endif

	sac_evolve(io);

	return count;
}

void sac_evolve( TiSioAcceptor * sac) 
{
    int count;
	TiSioAcceptor * io = (TiSioAcceptor *)sac;
    hal_assert( sac != NULL );
	/* If io->rxbuf is empty, then try to retrieve data from the device adapter(io->device). */

	/* If framing is enabled, then do framing here */
	#ifdef RS232_IOSERVICE_SLIP_ENABLE
	if (!iobuf_full(io->tmpbuf))
	{
		// @attention
		// @todo
		// The following process depends on read(). Attention here read() should be 
		// an asynchronious call, however, i'm not sure how the current read() behaves.

		/* Read some data into io->tmpbuf first and do framing on this buffer. The frame
		 * found will be placed into io->rxbuf by the framing process.
		 */
		//count = io->device->read(iobuf_endptr(io->tmpbuf)+1, iobuf_available(io->tmpbuf));
        count = uart_read( io->device,iobuf_endptr(io->tmpbuf)+1, iobuf_available(io->tmpbuf),0);
		iobuf_adjustlength( io->rxbuf, count );
	}

	if ((io->rx_accepted == 0) && (!iobuf_empty(io->tmpbuf)))
	{
		/* If io->rxbuf is full, then we have no better idea but to drop the current frame.
		 * This is done by clear the rxbuf and restart the framing process.
		 * However, this should NOT happen if you can guarantee the rxbuf is large
		 * enough and can accept the largest possible frame.
		 */
		if (iobuf_full(io->rxbuf))
		{
			iobuf_clear(io->rxbuf);
		}

		/* slip_filter_rx_handler return a positive value means an frame found. The frame
		 * is placed in io->rxbuf 
		 */
		if (slip_filter_rx_handler(io->slipfilter, io->tmpbuf, io->rxbuf) > 0)
		{
			/* set the rx_accept flag to indicate an entire frame is successfully identified
			 * and be placed inside io->rxbuf. */
			io->rx_accepted = 1;
		}
	}
	#endif

	#ifndef RS232_IOSERVICE_SLIP_ENABLE
	if (iobuf_empty(io->rxbuf))
	{
		//count = io->device->read(iobuf_endptr(io->rxbuf)+1, iobuf_available(io->rxbuf));
        count = uart_read( io->device,iobuf_endptr(io->rxbuf)+1, iobuf_available(io->rxbuf),0);
		iobuf_adjustlength( io->rxbuf, count );
	}
	#endif

	/* If there's data in io->txbuf, then try to send it through io->device. */
	if (!iobuf_empty(io->txbuf))
	{
		//count = io->device->write(iobuf_ptr(io->rxbuf), iobuf_length(io->rxbuf));
        count = uart_write(io->device,iobuf_ptr(io->txbuf), iobuf_length(io->txbuf),0);
		iobuf_popfront(io->txbuf, count);
	}

	return;
}

