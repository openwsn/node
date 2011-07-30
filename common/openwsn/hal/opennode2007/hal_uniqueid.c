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
/***************************************************************************** 
 * @author  makun on  2006-12-14
 * uniqueid 
 * based on Huanghuan's mature code. 
 * find the uniqueid of ds2401. 
 * 
 * @modified by zhangwei on 20061226
 * adjust some notation and re-format the file
 * modified the prototype of uid_read(). pls confirm the implementation.
 * i think "serialnumber[6]" is unecessary.
 *
 ****************************************************************************/

#include "hal_foundation.h"
#include "hal_uniqueid.h"

#define  DS_PIN  20
#define  DS_PORT 1

#define  DQ (1 << DS_PIN)    //P1.20 are DS2401 data pin

static void ds_set(void);
static void ds_clr(void);
static uint8 ds_input(void);
static void delay(int time); // you should use the global delay function
static unsigned char read_byte(void);
static void write_byte(unsigned char val);


TUniqueIdDriver * uid_construct( uint8 id, char * buf, uint8 size , uint8 opt)
{
	TUniqueIdDriver * uid;
	
	if (sizeof(TUniqueIdDriver) <= size)
		uid = (TUniqueIdDriver *)buf;
	else
		uid = NULL;
		
	if (uid != NULL)
	{
		memset( buf, 0x00, size );
		uid->id = id;
		opt = opt;
	}
      return uid;
}

void uid_destroy( TUniqueIdDriver * uid )
{
}

uint8 uid_read( TUniqueIdDriver * uid, char * buf, uint8 size, uint8 opt )
{
   unsigned char i;
   
   while(uid_reset());
    
   write_byte(0x0F);
   uid->familycode=read_byte();
   for(i=0;i<6;i++)
     {
       uid->serialnumber[i]=read_byte();
     }
   uid->crcbyte=read_byte();
   memmove( buf, uid->serialnumber, min(size,6) );
   return min(size,6);
}

// obsolete function, should be deprecated
void delay(int time)
{
  int i;
  for(i=0;i<time;i++);
}

void ds_set()
{
  #if DS_PORT == 0 
  IO0DIR |= DQ;
  IO0SET = DQ;
  #endif
  
  #if DS_PORT == 1 
  IO1DIR |= DQ;
  IO1SET = DQ;
  #endif
}

void ds_clr()
{
  #if DS_PORT == 0 
  IO0DIR |= DQ;
  IO0CLR = DQ;
  #endif
  
  #if DS_PORT == 1 
  IO1DIR |= DQ;
  IO1CLR = DQ;
  #endif	
}

uint8 ds_input()
{
  #if DS_PORT == 0 
  IO0DIR &= ~DQ;
  return IO0PIN & DQ;
  #endif
  
  #if DS_PORT == 1 
  IO1DIR &= ~DQ;
  return IO1PIN & DQ;
  #endif	
}

uint8 uid_reset(void)
{
  
  uint8 isexist;
  
  ds_clr(); 
  delay(48);                  //保持DQ低480us
  ds_set();
  delay(8);                   //等待芯片应答信号
  ds_input();
  delay(40);                  //延时以完成整个时序
  isexist = ds_input();
  return (isexist);           //返回应答信号，有芯片应答isexist＝0;无芯片应答isexist=1
}

//读一字节子程序

unsigned char read_byte(void)
{
  uint8 i,t,value=0;
  for(i=0;i<8;i++) {
    ds_clr();  //将总线DQ拉低开始读时序
    t++;
    ds_set();    //释放总线
    t++;
    t++;
    if(ds_input())  value |= 0x01 << i;
    delay(6);
  }
    return value;
 }

void write_byte(unsigned char val)
{
  uint8 i;
  for(i=8;i>0;i++)
  {
    ds_clr();
    if(val & 0x80) ds_set();
    else           ds_clr();
    //IO0SET  = val & 0x01;  //每次写1位,通过val右移得到
    delay(6);              //延时60us
    ds_set();          //释放总线
    val <<= 1;             //计算的同时会产生一个2us的时间间隙
  }
}

