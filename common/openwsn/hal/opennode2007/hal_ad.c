
/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 
#include "hal_foundation.h"
#include <stdlib.h>
#include "hal_ad.h"
#include "hal_uart.h"
#include "hal_global.h"


/* id的最低一位代表通道，再高一位为ad的选者，如14 则为ad1的四通道 */
TiAdConversion * ad_construct( uint8 id, char * buf, uint8 size )
{  
	
	TiAdConversion *ad;
	
	char* out_string = "ad consturct succesful!\n";
   	
   	if (sizeof(TiAdConversion) > size)
		ad = NULL;
	else
		
		ad = (TiAdConversion *)buf;
		
	if (ad != NULL)
	{
		memset( (char*)ad, 0x00, sizeof(TiAdConversion) );
	 	
		ad->id = id;
		switch(id)
		{
		 case 0:  PINSEL1 = 0x00400000;break;
		 case 1:  PINSEL1 = 0x01000000;break;
		 case 2:  PINSEL1 = 0x04000000;break;
		 case 3:  PINSEL1 = 0x10000000;break;
		 case 4:  PINSEL1 = 0x00040000;break;
		 case 5:  PINSEL1 = 0x00100000;break;//?datasheet not consisitent
		 case 6:  PINSEL0 = 0x00000300;break;
		 case 7:  PINSEL0 = 0X0000C000;break;
		 default: break;
		}     
	     /*
　　            switch(id)
                {
                case 0:  PINSEL1 = 0x00400000;break;
                case 1:  PINSEL1 = 0x01000000;break;         
　　            case 2:  PINSEL1 = 0x04000000;break;
　　            case 3:  PINSEL1 = 0x10000000;break;
　　            case 4:  PINSEL1 = 0x00040000;break;
                case 5:  PINSEL1 = 0x00100000;break;//?datasheet not consisitent
　　            case 6:  PINSEL0 = 0x00000300;break;
　　            case 7:  PINSEL0 = 0X0000C000;break;
　　            //case 10:  PINSEL0=0x00003000;break;
　　            //case 11:  PINSEL0=0x00030000;break;
　　            //case 12:  PINSEL0=0x00300000;break;
　　            //case 13:  PINSEL0=0x03000000;break;
                //case 14:  PINSEL0=0x0c000000;break;
　　            //case 15:  PINSEL0=0xc0000000;break;
　　            //case 16:  PINSEL1=0x00000800;break;
　　            //case 17:  PINSEL1=0x00001000;break;
                  default:  break;
　           }*/
		 
             uart_write(g_uart, out_string,24, 0);
	}

	return ad ; 
}

void ad_configutre(TiAdConversion * ad)
{
	//ad->id = 0;
	AD0CR = (1 << 3)						|	// SEL=8,选择通道3
			((Fpclk / 1000000 - 1) << 8)	|	// CLKDIV=Fpclk/1000000-1,转换时钟为1MHz
			(0 << 16)						|	// BURST=0,软件控制转换操作
			(0 << 17)						|	// CLKS=0, 使用11clock转换
			(1 << 21)						|  	// PDN=1,正常工作模式
			(0 << 22)						|  	// TEST1:0=00,正常工作模式
			(1 << 24)						|	// START=1,直接启动ADC转换
			(0 << 27);						 	// 直接启动ADC转换时，此位无效
}

void ad_destroy( TiAdConversion * ad )
{
	if (ad)
	{
		//ad->callback = NULL;
	}
}

// not used now.
void ad_start( TiAdConversion * ad, TiFunEventHandler callback, void * owner )
{
	//ad->callback = callback;
	//ad->callback_owner = owner;
}

uint16 ad_read( TiAdConversion * ad, char * buf, uint8 size, uint8 opt )
{      
       
       uint32  temp;
       uint16  value;
       
       temp = AD0DR;		// 读取ADC结果，并清除DONE标志位
       
       AD0CR |= 1 << 24;					// 进行第一次转换
       while ((AD0DR & 0x80000000) == 0);	// 等待转换结束
       AD0CR |= 1 << 24;					// 再次启动转换
       while ((AD0DR & 0x80000000) == 0);	// 等待转换结束
		
       
      
       temp=AD0DR;
       temp = (temp >> 6) & 0x3ff;
       
       value = (uint16) temp;
       
       return value;       		        		        
}

