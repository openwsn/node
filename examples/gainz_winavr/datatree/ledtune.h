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
#ifndef _LEDTUNE_H_4698_
#define _LEDTUNE_H_4698_

/* TiLedTune
 * This service emulate the light tuning process. An hardware timer will drive this
 * service to run. The LED will flash according to a specified frequency. The frequency 
 * will change according to the inputed "luminance" parameter.
 */

#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_timer.h"

typedef struct{
	uint16 luminance;
	TiTimer * timer;
}TiLedTune;

TiLedTune * ledtune_construct( void * mem, uint16 memsize, TiTimer * timer );
void ledtune_destroy( TiLedTune * ledtune );
TiLedTune * ledtune_open( TiLedTune * ledtune );
void ledtune_close( TiLedTune * ledtune );
void ledtune_write( TiLedTune * ledtune, uint16 lum );

#endif
