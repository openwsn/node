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

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_int2str.h"

//#define _isnumber(ch) ((ch)>='0' && (ch)<='9')

//static int _strlen( char * str );

int str2int(char * str)
{
   	int result = 0;
   	for (int i=0; _isnumber(str[i]); i++)
   	{
    	result = result*10;
       	result += str[i] - '0';
   	}
   	return result;
}

/* @warning
 *	the buffer should large enough to hold the string or else unstable error occured!
 */
void int2str(int n, char str[])
{
 	int high=n/10;
   	if (high!=0)
       	int2str(high,str);
   	else
       	str[0]='\0';
   int low=n%10;
   int len=strlen(str);
   str[len] = '0' + low;
   str[len+1] = '\0';
}

int _strlen( char * str )
{
	for(i=1; str[i]!='\0'; i++)
   	{
		;
	}
	return i;
}

