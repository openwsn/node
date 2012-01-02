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
