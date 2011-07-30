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

#ifndef _HAL_LIGHT_H_8723_
#define _HAL_LIGHT_H_8723_

/******************************************************************************
 * light sensor 
 * @author zhangwei on 200907xx
 * @modified by xuxizhou in 200908xx
 *	- tested ok
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_adc.h"

#define TiLumSensor TiLightSensor
#define lum_construct(v1,v2) light_construct(v1,v2) 
#define lum_destroy(v1) light_destroy(v1) 
#define lum_open(v1,v2,v3) light_open(v1,v2,v3) 
#define lum_close(v1) light_close(v1) 
#define lum_value(v1) light_value(v1)

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
	uint8 id;
	TiAdcAdapter * adc;
}TiLightSensor;

TiLightSensor * light_construct( char * buf, uint16 size );
void light_destroy( TiLightSensor * light );
TiLightSensor * light_open( TiLightSensor * light, uint8 id, TiAdcAdapter * adc );
void light_close( TiLightSensor * light );
uint16 light_value( TiLightSensor * light );
#ifdef __cplusplus
}
#endif

#endif