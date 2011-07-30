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

#include "../rtl/rtl_configall.h"
#include <string.h>
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_debugio.h"
#include "rtl_textcodec.h"

inline static uint8 _text_digit2hexchar( uint8 num ); //二进制转字符
inline static uint8 _text_hexchar2digit( uint8 ch );//字符转二进制
inline static uint8 _text_combind_hex2digit( uint8 highbyte, uint8 lowbyte );//按高低字节合并两个二进制数

/* _text_digit2hexchar
 * Convert a 8bit number into ASCII characters. 
 * For example 0x0F to 'F'
 * 
 * attention the input digit must less than 0x0F
 */
uint8 _text_digit2hexchar( uint8 num )
{
	static char t_digit2hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return (t_digit2hextable[num & 0x0F]);
}


uint8 _text_hexchar2digit( uint8 ch )
{
	
	if( (ch>0x29)&&(ch<0x40) )
		return ch-0x30;
	else if( (ch>0x40)&&(ch<0x47) )
		return ch-0x37;
	else 
	{
		rtl_assert( false );
		return 0;
	}
}

uint8 _text_combind_hex2digit( uint8 highbyte, uint8 lowbyte )
{
	return (_text_hexchar2digit(highbyte)<<4) | (_text_hexchar2digit(lowbyte));
}

uint16 text_encode( char * input, uint16 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 idx = 0;

	rtl_assert( size >= (len * 2) );
	
	for(i=0; i<len; i++)
	{
		idx = i << 1;
		output[idx] = _text_digit2hexchar(((input[i])&0xF0) >> 4);	
		output[idx++] = _text_digit2hexchar((input[i])&0x0F);	
	}//先存高位，后存低位


	return 2*len;
}


uint16 text_decode( char * input, uint16 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 k = 0;

	rtl_assert( size >= (len>>1) );
	
	while (i < len)
	{
		output[k] = _text_combind_hex2digit( input[i], input[i+1]);
		i += 2;
		k++;
	}

	return k;
}
