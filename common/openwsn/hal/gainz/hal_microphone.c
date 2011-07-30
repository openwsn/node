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

/******************************************************************************
 * microphone sensor 
 * @author zhangwei on 200907xx
 * @modified by Xiao-Yuazhang in 200909xx
 *	- developing
 *****************************************************************************/

#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_microphone.h"

/* Construct a empty TiMicrophone object in the memory */
TiMicrophone * mic_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiMicrophone) <= size );
	memset( buf, 0x00, size );
	return (TiMicrophone *)buf;
}

/* Destroy the TiMicrophone object */
void mic_destroy( TiMicrophone * mic )
{
	mic_close( mic );
}

/* Open the mic sensor object and start the measurements. The sensor is usually 
 * powered on or wakeup from sleep mode in the sensor's open() function.
 * 
 * @attention
 * @todo
 *	The PIN initialization can be executed in target_init() or in mic_open().
 * The following implementation assumes you have already initialized PIN configurations
 * before calling mic_open() or in adc_open().
 */
TiMicrophone * mic_open( TiMicrophone * mic, uint8 id, TiAdcAdapter * adc )
{
	mic->id = id;
	mic->adc = adc;
	adc_open( adc, adc->id, NULL, NULL, 0 );
    return mic;
}

/* Close the sensor. Usually power off the force the sensor module into sleep mode
 * in the close function 
 */
void mic_close( TiMicrophone * mic )
{
	adc_close( mic->adc );
}

/* Read the sensor value. Ususally the measurement is already started in sensor's 
 * open() function. For those who can started immediately, you can also power on 
 * the sensor in this function and power off it after the measurement.
 */
uint16 mic_value( TiMicrophone * mic, char * buf, uint16 size )
{
	uint8 count=0;
	adc_start( mic->adc );
	while (count == 0)
	{
		// dbo_putchar( '*' );
		count = adc_read( mic->adc, buf, size, 0x00 );
		// dbo_putchar( '&' );
	}
	return count;
}

