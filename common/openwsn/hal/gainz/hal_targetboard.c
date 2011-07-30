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

#ifndef _HAL_TARGETBOARD_C_7483_
#define _HAL_TARGETBOADR_C_7483_

/* Hardware Platform Layer
 * including CPU/MCU specific source codes
 * 
 * Reference  
 * Hardware Abstraction Architecture, http://www.tinyos.net/tinyos-2.x/doc/html/tep2.html
 */ 


#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_targetboard.h"

#ifdef CONFIG_TARGET_GAINZ
#include "./hpl_atmega128.h" 
#include "./hpl_gainz.h"
#endif

#ifdef CONFIG_TARGET_CC2520DK
#include "./hpl_msp430f2618.h" 
#include "./hpl_cc2520dk.h"
#endif


void target_init( void )
{
	hal_init( NULL, NULL );
	HAL_SET_PIN_DIRECTIONS();
}

void target_reset( void )
{
	// todo
	target_init();
}

#endif /* _HAL_TARGETBOARD_C_7483_ */
