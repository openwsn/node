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
//#include <assert.h>
#include "hal_sensor_vibration.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_switch.h"
#include "hal_uart.h"
#include "hal_global.h"

TiVibSensorAdapter * vib_construct( uint8 id, char * buf, uint8 size, TiMcp6s26Adapter * mcp, TiAdConversion * ad  )
{
	TiVibSensorAdapter * vib;
	char* out_string = "vibration consturct succesful!\n";

	if ( sizeof(TiVibSensorAdapter) > size )
	    vib = NULL;
	else
	    vib = (TiVibSensorAdapter *)buf;

	if (vib != NULL)
	{
	    memset( (char*)vib, 0x00, sizeof(TiVibSensorAdapter) );
	    vib->id = id;
	    vib->mcp = mcp;
	    vib->ad = ad;
            uart_write(g_uart, out_string,31, 0);
        }

	return vib;

}

void vib_destroy( TiVibSensorAdapter * vib )
{
	NULL;
}

uint8 vib_read( TiVibSensorAdapter * vib, char * buf, uint8 size, uint8 opt )
{
        int8 i;
        uint16 value;

	avccs_on();

	for(i=0; i<4; i++)
   	{
    	    mcp6s26_setchannel( vib->mcp, i);
	    value = ad_read( vib->ad,NULL, 2, opt );
        }
    *buf = (uint8)value;
    *(buf + 1) = (uint8) (value >> 8);
    //avccs_off();
    return 8;
}

void vib_config(uint8 ifsleep)
{
  switch(ifsleep)
  {
      case SLEEP  :
      	   #if     VIBRATION_SLEEP_PORT == 0
           IO0CLR = BM(VIBRATION_SLEEP_PIN);  break;
           #endif

           #if     VIBRATION_SLEEP_PORT == 1
           IO1CLR = BM(VIBRATION_SLEEP_PIN);  break;
           #endif

      case WAKE   :
      	   #if     VIBRATION_SLEEP_PORT == 0
           IO0SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif

           #if     VIBRATION_SLEEP_PORT == 1
           IO1SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif

      default:
           #if     VIBRATION_SLEEP_PORT == 0
           IO0SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif

           #if     VIBRATION_SLEEP_PORT == 1
           IO1SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
  }
}



