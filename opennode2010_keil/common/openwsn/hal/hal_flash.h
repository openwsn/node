/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

#ifndef _HAL_FLASH_H_5783_
#define _HAL_FLASH_H_5783_

/*******************************************************************************
 * This module provides the interface to read/write flash hardware.
 * 
 * @attention This module is firstly introduced on OpenWSN 2.0/3.5 hardware, which
 * uses LPC2136/LPC2146 from Philips(now NXP).
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"

#define flash_read(faddr,maddr,size) flash_rawread(faddr,maddr,size)
#define flash_write(faddr,maddr,size) flash_rawwrite(faddr,maddr,size)

void flash_rawread( uint32 flashaddr, uint32 memaddr, uint32 size );
void flash_rawwrite( uint32 flashaddr, uint32 memaddr, uint32 size );

#endif