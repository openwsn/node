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

#include "svc_foundation.h"
#include "svc_openmac.h"




typedef struct{
  TiOpenLLC * mac;
  TiOpenNeighbor nodes[CONFIG_MAX_NEIGHBOR_COUNT];
}TiOpenLLC;

TiOpenLogicLink

TiOpenLLC * llc_construct( void * buf, uint16 size );
void llc_destroy( TiOpenLLC * llc ) 
llc_open( TiOpenLLC * llc, 
llc_close
llc_send( linkid, 
llc_read( linkid, 
llc_setlistener(


TiLogicLink llc_openlink( llc, linkid, listener )
connect
disconnect


mac

llc_




int8 llc_probe( TiOpenLLC * mac );
int8 llc_updatestatistics( TiOpenLLC * mac );
int8 llc_getnode( TiOpenLLC * mac, TiOpenLLCNode * node );
int8 llc_getneighbors( TiOpenLLC * mac, TiOpenAddress * addr[] );
uint8 llc_getlinkquality( TiOpenLLC * mac, TiOpenAddress * addr );
uint8 llc_getsignalstrength( TiOpenLLC * mac, TiOpenAddress * addr );




#endif
