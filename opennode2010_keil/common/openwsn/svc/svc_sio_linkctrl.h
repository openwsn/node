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

/*
 * Data Link Layer based on UART
 * This data link layer is similar to HDLC and LCP sub-layer in PPP protocol.
 * 
 * Frame format:
 * 	+----------------+------------+------------+------------------------+---------+-------------+--------------+
 * 	| Header Flag 1B | Address 1B | Control 1B | Protocol Identifier 1B | Data nB | FCS 2 or 4B | Tail Flag 1B |
 * 	+----------------+------------+------------+------------------------+---------+-------------+--------------+
 *
 *  Header Flag: must be 01111110b
 *  Address: must be 11111111 (refer to PPP protocol)
 *  Control: must be 00000011 (refer to PPP protocol)
 *  Protocol Identifier 2B default to 0x00 now (we don't use it now)
 *  Data <1500 bytes
 *  FCS 2 or 4 bytes (usually 2B)
 *  Tail Flag: must be 01111110b
 *
 * @reference
 * - PPP数据帧的格式, http://apps.hi.baidu.com/share/detail/15553132
 * - High Level Data Link Control Protocol(HDLC), http://baike.baidu.com/view/525479.htm
 * - Serial Line Protocol (SLIP), [RFC 1055], http://www.scutde.net/t3courses/0326-eemlbkkhoh/chapter3/Class7/3_6_2.htm
 */

/**
 * SDLC, Serial line based data Link control protocol.
 */
typedef struct{

}TiSioDataLinkControl;  

sdlc_construct
sdlc_destroy
sdlc_open
sdlc_close

or using TiIoBuf

sdlc_send( TiSioDataLinkControl * sdlc, char * buf, uint16 len, uint8 option );
sdlc_recv( TiSioDataLinkControl * sdlc, char * buf, uint16 size, uint8 option );


