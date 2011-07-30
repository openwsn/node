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

#ifndef _UNIQUEID_H_4289_
#define _UNIQUEID_H_4289_

/***************************************************************************** 
 * @author  makun on  2006-12-14
 * uniqueid 
 * based on Huanghuan's mature code. 
 * find the uniqueid of ds2401. 
 * 
 * @modified by zhangwei on 20061226
 * adjust some notation and re-format the file
 * modified the prototype of uid_read(). pls confirm the implementation.
 * i think serialnumber[6] is unecessary.
 *
 ****************************************************************************/
#include "hal_foundation.h"

typedef struct{
  uint8 id;
  uint8 familycode;
  uint8 serialnumber[6];
  uint8 crcbyte;
}TUniqueIdDriver;

TUniqueIdDriver * uid_construct( uint8 id, char * buf, uint8 size , uint8 opt);
void  uid_destroy( TUniqueIdDriver * uid );
uint8 uid_read( TUniqueIdDriver * uid, char * buf, uint8 size, uint8 opt );
uint8 uid_reset(void);

#endif