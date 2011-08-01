

typedef struct{
}TiXtp;

xtp_create
xtp_free
xtp_construct
xtp_free


/* the TiIoBuf contains an entire TiXtpFrame. You need manually extract TiXtpPacket from it, 
 * and then TiXtpVarSequence from the packet 
 */
xtp_rawsend( TiXtp * xtp, TiIoBuf * pkt );
xtp_rawrecv( TiXtp * xtp, TiIoBuf * pkt );

xtp_connect( TiXtp * xtp, void * rmtaddr );
xtp_listen( TiXtp * xtp );
