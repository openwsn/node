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
#ifndef _VIBSENSOR_H_7689_
#define _VIBSENSOR_H_7689_

/*****************************************************************************
 * @note
 * this module is based on hal_mcp6s26 and hal_ad
 * it provide a high level support to vibration sensor
 ****************************************************************************/
#include "hal_foundation.h"
#include "hal_ad.h"
#include "hal_mcp6s.h"

#define VIBRATION_SLEEP_PIN  0
#define VIBRATION_SLEEP_PORT 0

#define SLEEP  0
#define WAKE   1

typedef struct{
  uint8 id;
  TiMcp6s26Adapter * mcp;
  TiAdConversion *ad;
}TiVibSensorAdapter;

TiVibSensorAdapter * vib_construct( uint8 id, char * buf, uint8 size, TiMcp6s26Adapter * mcp, TiAdConversion * ad  );
void vib_destroy( TiVibSensorAdapter * vib );
uint8 vib_read( TiVibSensorAdapter * vib, char * buf, uint8 size, uint8 opt );

void vib_config(uint8 ifsleep);   //sleep or wake


#endif
