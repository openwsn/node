#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_xtpframe.h"
#include "rtl_xtppacket.h"

/*******************************************************************************
 * rtl_xtppacket
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.09
 *  - finish the implementation, not test.
 ******************************************************************************/

/* TiXtpPacketDescriptor  */

TiXtpPacketDescriptor * xtp_pktdesc_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiXtpPacketDescriptor) <= size );
	memset( buf, 0x00, size );
	return (TiXtpPacketDescriptor *)buf;

}
void xtp_pktdesc_destroy( TiXtpPacketDescriptor * pkt )
{

}

TiXtpPacketDescriptor * xtp_pktdesc_create( uint16 size )
{
	TiXtpPacketDescriptor * pkt = (TiXtpPacketDescriptor *)malloc( sizeof(TiXtpPacketDescriptor) );
	xtp_pktdesc_construct( (char *)pkt, sizeof(pkt) );
	return pkt;
}
void xtp_pktdesc_free( TiXtpPacketDescriptor * pkt )
{
	free(pkt);
}

/*
 * xtp_pktdesc_parse( TiXtpPacketDescriptor * pkt, char * buf, uint16 len )
 * The parse() function will parse the packet inside the buffer
 * the input char * buf should be the payload of xtpframe, which can be obtained
 * by function xtp_frame_dataptr(xtpframe)
 * And the uint16 len is the length of payload of xtpframe, which can be obtained by
 * function xtp_frame_length(xtpframe)
 * */
/*
 * The parse() function will parse the packet inside the TiXtpFrame
 * */
bool xtp_pktdesc_parse( TiXtpPacketDescriptor * pkt, TiXtpFrame * frame )
{
	char * buf = xtp_frame_dataptr( frame );
	pkt->length =  xtp_frame_length(frame);
	pkt->ctrl = &buf[0];
	pkt->addrdesc = &buf[2];//Q: when the address discription would be 0B
	pkt->addrto = &buf[4];
	pkt->addrfrom = &buf[4+8];//todo: check if the addrto and addrfrom is changeable.
	pkt->endpointto = &buf[4+8+8];
	pkt->endpointfrom = &buf[4+8+8+2];
	pkt->seqno = &buf[4+8+8+2+2];
	pkt->ackno = &buf[4+8+8+2+2+2];
	pkt->payload = &buf[4+8+8+2+2+2+2];
	return true;
}
/* the format() function will calculate the memory pointers so as to format memory structures
 * the input char * buf should be the payload of a xtpframe.
 * */
bool xtp_pktdesc_format( TiXtpPacketDescriptor * pkt, TiXtpFrame * frame, uint16 ctrl, uint16 addrdesc )
{
	char * buf = xtp_frame_dataptr( frame );
	pkt->length =  xtp_frame_length( frame );
	pkt->ctrl = &buf[0];
	pkt->addrdesc = &buf[2];//Q: when the address discription would be 0B
	pkt->addrto = &buf[4];

	pkt->addrfrom = &buf[4+8];//todo: check if the addrto and addrfrom is changeable.
	pkt->endpointto = &buf[4+8+8];
	pkt->endpointfrom = &buf[4+8+8+2];
	pkt->seqno = &buf[4+8+8+2+2];
	pkt->ackno = &buf[4+8+8+2+2+2];
	pkt->payload = &buf[4+8+8+2+2+2+2];
	return true;
}
/* after format() call, you can place data into this packet, and in the end, call
 * assemble() to finish it.
 * @return
 *  the memory pointer to the first byte of the packet 
 */
bool xtp_pktdesc_assemble( TiXtpPacketDescriptor * pkt, uint16 * length )
{
	return true;
}

uint16 xtp_pktdesc_length( TiXtpPacketDescriptor * pkt )
{
	return pkt->length;
}

/* returns the header size of current packet
 *
 * the frame format for two neighbor nodes communication are:
 * [Start Flag of Packet 1B] [Length 2B] [Data nB] {Checksum 2B}
 *
 * the packet format for two arbitrary nodes communication are:
 * [Packet Control 2B] [Address Descriptor 0B|2B] [Destination  Address 0B|4B|8B|16B]
 * [Source Address 0B|4B|8B|16B] [Destination Endpoint 0B|2B] [Source Endpoint 0B|2B]
 * [Sequence Number 2B] [Acknowledgement Number 2B]  [Payload nB]
 */
uint16 xtp_pktdesc_headersize( TiXtpPacketDescriptor * pkt )
{
	// todo: currently, we adopt the fixed header format. it's total 30B since the
	// length bytes. Xing: it should be 28B, because the 2 B length is not included.
	return 2 + 2 + 8 + 8 + 2 + 2 + 2 + 2;
}

uint16 xtp_pktdesc_ctrl( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->ctrl[0]&0x00FF)<<8 ) | (pkt->ctrl[1]&0x00FF);//网络字节序，高位在前
}

uint16 xtp_pktdesc_addrdesc( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->addrdesc[0]&0x00FF)<<8 ) | (pkt->addrdesc[1]&0x00FF);//网络字节序，高位在前
}

char * xtp_pktdesc_addrto( TiXtpPacketDescriptor * pkt, uint8 * addrlen )
{
	*addrlen = xtp_addr_nodelength((uint8)(pkt->addrdesc[0]));
	return pkt->addrto;
}

char * xtp_pktdesc_addrfrom( TiXtpPacketDescriptor * pkt, uint8 * addrlen )
{
	*addrlen = xtp_addr_nodelength((uint8)(pkt->addrdesc[1]));
	return pkt->addrfrom;//网络字节序，高位在前
}

uint16 xtp_pktdesc_endpointto( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->endpointto[0]&0x00FF)<<8 ) | (pkt->endpointto[1]&0x00FF);//网络字节序，高位在前
}

uint16 xtp_pktdesc_endpointfrom( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->endpointfrom[0]&0x00FF)<<8 ) | (pkt->endpointfrom[1]&0x00FF);//网络字节序，高位在前
}

uint16 xtp_pktdesc_seqno( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->seqno[0]&0x00FF)<<8 ) | (pkt->seqno[1]&0x00FF);//网络字节序，高位在前
}

uint16 xtp_pktdesc_ackno( TiXtpPacketDescriptor * pkt )
{
	return ( (pkt->ackno[0]&0x00FF)<<8 ) | (pkt->ackno[1]&0x00FF);//网络字节序，高位在前
}


char * xtp_pktdesc_payload( TiXtpPacketDescriptor * pkt, uint16 * len )
{
	*len = xtp_pktdesc_length(pkt) - 28;//Todo: check if the length of the packet's head is variable.
	return pkt->payload;
}

TiXtpFrame * xtp_pktdesc_frame( TiXtpPacketDescriptor * pkt )
{
	return pkt->frame;
}

//Set methods
void xtp_pktdesc_set_length( TiXtpPacketDescriptor * pkt, uint16 len )
{
	pkt->length = len;
}

void xtp_pktdesc_set_ctrl( TiXtpPacketDescriptor * pkt, uint16 ctrl )
{
	pkt->ctrl[0] = (ctrl&0xFF00)>>8;//网络字节序，高位在前
	pkt->ctrl[1] = (ctrl&0x00FF);
}

void xtp_pktdesc_set_addrdesc( TiXtpPacketDescriptor * pkt, uint16 addrdesc )
{
	pkt->addrdesc[0] = (addrdesc&0xFF00)>>8;//网络字节序，高位在前
	pkt->addrdesc[1] = (addrdesc&0x00FF);
}

void xtp_pktdesc_set_addrto( TiXtpPacketDescriptor * pkt, char * addrto, uint16 addrlen )
{
	memmove(pkt->addrto, addrto, addrlen);
}

void xtp_pktdesc_set_addrfrom( TiXtpPacketDescriptor * pkt, char * addrfrom, uint16 addrlen )
{
	memmove(pkt->addrfrom, addrfrom, addrlen);
}

void xtp_pktdesc_set_endpointto( TiXtpPacketDescriptor * pkt, uint16 endpointto )
{
	pkt->endpointto[0] = (endpointto&0xFF00)>>8;//网络字节序，高位在前
	pkt->endpointto[1] = (endpointto&0x00FF);
}

void xtp_pktdesc_set_endpointfrom( TiXtpPacketDescriptor * pkt, uint16 endpointfrom )
{
	pkt->endpointfrom[0] = (endpointfrom&0xFF00)>>8;//网络字节序，高位在前
	pkt->endpointfrom[1] = (endpointfrom&0x00FF);
}

void xtp_pktdesc_set_seqno( TiXtpPacketDescriptor * pkt, uint16 seqno )
{
	pkt->seqno[0] = (seqno&0xFF00)>>8;//网络字节序，高位在前
	pkt->seqno[1] = (seqno&0x00FF);
}
void xtp_pktdesc_set_ackno( TiXtpPacketDescriptor * pkt, uint16 ackno )
{
	pkt->ackno[0] = (ackno&0xFF00)>>8;//网络字节序，高位在前
	pkt->ackno[1] = (ackno&0x00FF);
}
void xtp_pktdesc_set_payload( TiXtpPacketDescriptor * pkt, char * payload, uint16 datalen )
{
	memmove(pkt->payload, payload, datalen);
}

