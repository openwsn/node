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

#include "svc_foundation.h"
#include "svc_sensor.h"

static int8 sen_timer_callback( TiSensorService * sen, TiTimerAdapter * timer );

TiSensorService * sen_construct( char * buf, uint16 size )
{
	//assert( sizeof(TiSensorService) <= size );
	memset( buf, 0x00, size );
	return (TiSensorService *)buf; 
}

void sen_destroy( TiSensorService * sen )
{
	sen_stop( sen );
}

void sen_configure( TiSensorService * sen, TiTempSensorAdapter * temp, TiVibSensorAdapter * vib,
	TiStrainSensorAdapter * strain, TiTimerAdapter * timer )
{
	sen->temp = temp;
	sen->vib = vib;
	sen->strain = strain;
	sen->timer = timer;
}

/* read data from all kinds of sensors.
 * @param
 * 	opt		an option to decide which type sensor to use.
 * 			some MACROS are defined to facilitate developing such as SEN_TYPE_STRAIN.
 * @return
 * 	>=0		the length of measured data actually put into the buffer.
 * 	= 0		do nothing
 * 	< 0		failed 
 */
int8 sen_read( TiSensorService * sen, char * buf, uint8 size, uint8 opt )
{
	switch (opt)
	{
	case SEN_TYPE_STRAIN:
		//strain_read( sen->strain, buf, size, 0x00 ); 
		break;
		
	case SEN_TYPE_VIBRATION:
		vib_read( sen->vib, buf, size, 0x00 ); 
		break;
		
	case SEN_TYPE_TEMPRATURE:
		//temp_read( sen->temp, buf, size, 0x00 ); 
		break;
		
	case SEN_TYPE_LIGHT:
		//temp_read( sen->temp, buf, size, 0x00 ); 
		break;
	}
	return 0;
}

/* fill the packet's payload TOpenData buffer with the measured data */
int8 sen_fillpacket( TiSensorService * sen, uint8 type, TiOpenPacket * pkt, uint8 size )
{
	int8 ret;
	char * data = opt_data( (char*)pkt );
	data[0] = type;
	ret = sen_read( sen, data+1, sizeof(TiOpenData)-1, type );
	pkt->datalen = (uint8)(ret >= 0 ? ret : 0);
	return ret;
}

/* frame structure is as the following
 * [LENGTH 1][CONTROL 2][SEQU 1][PANID 2][NODEFROM 2][NODETO 2] [DATA TYPE 1] [DATA n]
 * this structure is defined in "hal\hal_openframe.h" and also "service\svc_openpacket.h"
 */
int8 sen_fillframe( TiSensorService * sen, uint8 type, TiOpenFrame * frame, uint8 size )
{
	int8 ret;
	char * data = opt_data( opf_packet((char *)frame) );
	data[0] = type;
	ret = sen_read( sen, data+1, sizeof(TiOpenData)-1, type );
	data --;
	*data = (uint8)(ret >= 0 ? ret : 0);
	frame->length = *data + OPF_HEADER_SIZE + 2;
	return ret;
}

/* if you need sampling, you should call start when your application start. 
 * then the sensor service will configure a timer to fulfill this periodic task
 * 
 * @param
 * 	interval	sampling interval between two measurements
 * 	delay		default 0. means the TiSensorService start sampling almost at once.
 * 				however, you may need to postphone the sampling for sometime, then 
 * 				you can set the duration of delay through this parameter.
 * 				usually, you can set the same value of "delay" and "interval". 				
 */
int8 sen_start( TiSensorService * sen, uint32 interval, uint32 delay )
{
	return 0;
}

int8 sen_stop( TiSensorService * sen )
{
	return 0;
}

int8 sen_timer_callback( TiSensorService * sen, TiTimerAdapter * timer )
{
	return 0;
}

