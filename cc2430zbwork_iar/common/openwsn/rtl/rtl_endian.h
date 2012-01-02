#ifndef _RTL_ENDIAN_H_3288_
#define _RTL_ENDIAN_H_3288_
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

/*******************************************************************************
 * Network Byte Order
 ******************************************************************************/


/* Network order problem is encountered when transmit word or qword variables. It's
 * a very fundmental problem in network programing or porting to other platforms. 
 * 
 * According to the network byte order, the highest byte should be sent first. So
 * it's essentially a "Big Endian" design. Attention some protocol or hardware design
 * may not comply with this regulations. 
 *
 * In order to avoid the Little Endian/Big Endian problem in communications, and 
 * also helping the developer to write platform-independent programs, we define the 
 * following macros to help coding. These macros can often be found in a lot of 
 * powerful C language developing environment. 
 *
 *      htons()--"Host to Network Short"
 *      htonl()--"Host to Network Long"
 *      ntohs()--"Network to Host Short"
 *      ntohl()--"Network to Host Long"
 */


/* reference
 * - Endian/Endianess on wikipedia, http://en.wikipedia.org/wiki/Endian;
 * - Introduction to Endianness, http://www.netrino.com/Embedded-Systems/How-To/Big-Endian-Little-Endian;
 */

#if defined(CONFIG_BIG_ENDIAN) && !defined(CONFIG_LITTLE_ENDIAN)

  #define htons(A) (A)
  #define htonl(A) (A)
  #define ntohs(A) (A)
  #define ntohl(A) (A)

#elif defined(CONFIG_LITTLE_ENDIAN) && !defined(CONFIG_BIG_ENDIAN)

  #define htons(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
                    (((uint16_t)(A) & 0x00ff) << 8))
  #define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
                    (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                    (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                    (((uint32_t)(A) & 0x000000ff) << 24))

  #define ntohs  htons
  #define ntohl  htonl

#else

  #error "Must define one of BIG_ENDIAN or LITTLE_ENDIAN"

#endif

#endif /* _RTL_ENDIAN_H_3288_ */
