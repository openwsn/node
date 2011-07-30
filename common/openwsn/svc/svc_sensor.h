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

#ifndef _SENSOR_H_6278_
#define _SENSOR_H_6278_


 * @description sensing service
 * this is the higher level encapsulation of all kinds of low level sensors.
 * different to hal_sensor_xxx, this service include timer-driven sampling. 
 * while, the hal_sensor_xxx only encapsulate the basic read/write operations 
 * for a sensor.
 * 
 ****************************************************************************/ 		

#include "hal_foundation.h" 
#include "hal_sensor_temp.h"
#include "hal_sensor_vibration.h"
#include "hal_sensor_strain.h"
#include "hal_timer.h"
#include "svc_openpacket.h"

#define SEN_TYPE_STRAIN ODA_TYPE_STRAINSENSOR
#define SEN_TYPE_VIBRATION ODA_TYPE_VIBSENSOR
#define SEN_TYPE_TEMPRATURE ODA_TYPE_TEMPSENSOR
#define SEN_TYPE_LIGHT ODA_TYPE_LIGHTSENSOR

typedef struct{
  uint8 opt;
  TiTempSensorAdapter * temp;
  TiVibSensorAdapter * vib;
  TiStrainSensorAdapter * strain;
  TiTimerAdapter * timer;
}TiSensorService;

TiSensorService * sen_construct( char * buf, uint16 size );
void sen_destroy( TiSensorService * sen );
void sen_configure( TiSensorService * sen, TiTempSensorAdapter * temp, TiVibSensorAdapter * vib,
	TiStrainSensorAdapter * strain, TiTimerAdapter * timer );
int8 sen_read( TiSensorService * sen, char * buf, uint8 size, uint8 opt );
int8 sen_fillpacket( TiSensorService * sen, uint8 type, TiOpenPacket * pkt, uint8 size );  
int8 sen_fillframe( TiSensorService * sen, uint8 type, TiOpenFrame * frame, uint8 size );  
int8 sen_start( TiSensorService * sen, uint32 interval, uint32 delay );
int8 sen_stop( TiSensorService * sen );

#endif
