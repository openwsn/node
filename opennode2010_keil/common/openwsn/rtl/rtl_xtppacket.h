#ifndef _RTL_XTP_PACKET_H_AA78_
#define _RTL_XTP_PACKET_H_AA78_
/***********************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2004-2010 zhangwei(TongJi University)
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

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_iobuf.h"
#include "rtl_xtpframe.h"
#include "rtl_xtpaddress.h"

/* TiXtpPacketDescriptor
 * help to manipulate xtp packet format
 *
 * Packet format:
 * [Packet Control 2B] [Address Descriptor 0B|2B] [Destination  Address 0B|4B|8B|16B] 
 * [Source Address 0B|4B|8B|16B] [Destination Endpoint 0B|2B] [Source Endpoint 0B|2B] 
 * [Sequence Number 2B] [Acknowledgement Number 2B]  [Payload nB]
 */
typedef struct{
  uint16 length;
  char * ctrl;
  char * addrdesc;
  char * addrto;
  char * addrfrom;
  char * endpointto;
  char * endpointfrom;
  char * seqno;
  char * ackno;
  char * payload;
  TiXtpFrame * frame;
}TiXtpPacketDescriptor;

#ifdef __cplusplus
extern "C" {
#endif

TiXtpPacketDescriptor * xtp_pktdesc_create( uint16 len );
void xtp_pktdesc_free( TiXtpPacketDescriptor * pkt );
TiXtpPacketDescriptor * xtp_pktdesc_construct( char * mem, uint16 memsize );
void xtp_pktdesc_destroy( TiXtpPacketDescriptor * pkt );

/* The parse() function will parse the packet inside the buffer */
bool xtp_pktdesc_parse( TiXtpPacketDescriptor * pkt, TiXtpFrame * frame );

/* the format() function will calculate the memory pointers so as to format memory structures */
bool xtp_pktdesc_format( TiXtpPacketDescriptor * pkt, TiXtpFrame * frame , uint16 ctrl, uint16 addrdesc );

/* after format() call, you can place data into this packet, and in the end, call
 * assemble() to finish it.
 * @return
 *  the memory pointer to the first byte of the packet 
 */
bool xtp_pktdesc_assemble( TiXtpPacketDescriptor * pkt, uint16 * length );

uint16 xtp_pktdesc_length( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_headersize( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_ctrl( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_addrdesc( TiXtpPacketDescriptor * pkt );
char * xtp_pktdesc_addrto( TiXtpPacketDescriptor * pkt, uint8 * addrlen );
char * xtp_pktdesc_addrfrom( TiXtpPacketDescriptor * pkt, uint8 * addrlen );
uint16 xtp_pktdesc_endpointto( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_endpointfrom( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_seqno( TiXtpPacketDescriptor * pkt );
uint16 xtp_pktdesc_ackno( TiXtpPacketDescriptor * pkt );
char * xtp_pktdesc_payload( TiXtpPacketDescriptor * pkt, uint16 * len );
TiXtpFrame * xtp_pktdesc_frame( TiXtpPacketDescriptor * pkt );

void xtp_pktdesc_set_length( TiXtpPacketDescriptor * pkt, uint16 len );
void xtp_pktdesc_set_ctrl( TiXtpPacketDescriptor * pkt, uint16 ctrl );
void xtp_pktdesc_set_addrdesc( TiXtpPacketDescriptor * pkt, uint16 addrdesc );
void xtp_pktdesc_set_addrfrom( TiXtpPacketDescriptor * pkt, char * addrfrom, uint16 addrlen );
void xtp_pktdesc_set_addrto( TiXtpPacketDescriptor * pkt, char * addrto, uint16 addrlen );
void xtp_pktdesc_set_endpointto( TiXtpPacketDescriptor * pkt, uint16 endpointto );
void xtp_pktdesc_set_endpointfrom( TiXtpPacketDescriptor * pkt, uint16 endpointfrom );
void xtp_pktdesc_set_seqno( TiXtpPacketDescriptor * pkt, uint16 seqno );
void xtp_pktdesc_set_ackno( TiXtpPacketDescriptor * pkt, uint16 ackno );
void xtp_pktdesc_set_payload( TiXtpPacketDescriptor * pkt, char * payload, uint16 datalen );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _RTL_XTP_PACKET_H_AA78_ */
