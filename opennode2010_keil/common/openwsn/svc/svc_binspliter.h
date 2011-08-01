/* TiBinSpliter
 * This object is used to split byte-based stream into frame-based stream. This 
 * can be used with any USART adapter or byte-based wireless adapter such as the 
 * Chipcon/TI's cc1000 transceiver. 
 * 
 * The default frame format:
 *	[SOF 1B] [Length 1B] [Frame Control 2B] [Data nB] {Checksum 2B}
 * 
 * SOF means the start of frame. 
 * Length is the byte count from frame control to checksum. 
 * Frame Control decides whether to use checksum. If use CRC checksum, then there's 
 *	another 2 bytes at the end of the data.
 */

typedef struct{
	adapter;
	TiFunAdapterGetChar
	TiFunAdapterPutChar;
	char tempbuf;
	char rxbuf;
	char txbuf;
}TiBinSpliter;

bspl_construct
bspl_destroy
bspl_open( crc, )  XXX( adapter's interface: getchar, putchar, bspl_adapterlistener, iohandler ) );
bspl_close
bspl_rxhandle( char * stream_data, len1, char * frame, size )
bspl_txhandle( char * frame, len, char * stream_data, size );





bspl_feed
bspl_readframe
bspl_writeframe
bspl_readdata
bspl_writedata

bspl_feed
bspl_iohandler( char * rx, char * tx );
	


bspl_rxhandle( char * stream, char * frame );
bspl_txhandle( char * frame, char * stream );
bspl_iohandler( char * rxframe, char * txframe );
