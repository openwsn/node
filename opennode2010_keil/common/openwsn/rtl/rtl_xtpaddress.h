#ifndef _RTL_XTPADDRESS_H_4287_
#define _RTL_XTPADDRESS_H_4287_
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


typedef struct{
  uint8 desc;
  unsigned char addrbuf[16];
}TiXtpAddress;

#ifdef __cplusplus
extern "C" {
#endif

TiXtpAddress * xtp_addr_create( uintx size );
void xtp_addr_free( TiXtpAddress * addr );
TiXtpAddress * xtp_addr_construct( void * mem, uintx memsize );
void xtp_addr_destroy(TiXtpAddress * addr);

uint8 xtp_addr_type( TiXtpAddress * addr );
void xtp_addr_set_type( TiXtpAddress * addr, uint8 type);
char * xtp_addr_value( TiXtpAddress * addr, uint8 * len );
void xtp_addr_setvalue( TiXtpAddress * addr, char * value, uint8 len );
uint8 xtp_addr_nodelength( uint8 addrdesc );

void xtp_addr_set_ipv4( TiXtpAddress * addr, uint32 net, uint32 ipv4 );
//void xtp_addr_set_ipv6( TiXtpAddress * addr, uint32 ipv6 )
//void xtp_addr_set_ieee802_64b( TiXtpAddress * addr, char * buf );
//void xtp_addr_set_cellphone( TiXtpAddress * addr, char * cellphone, uint8 len );

char * xtp_addr_get_nodeptr( TiXtpAddress * addr, uint8 * len );
char * xtp_addr_get_netptr( TiXtpAddress * addr, uint8 * len );

#ifdef __cplusplus
}
#endif

#endif /* _RTL_XTPADDRESS_H_4287_ */
