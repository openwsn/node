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

#ifndef _HAL_STDC_REDIRECT_H_7689_
#define _HAL_STDC_REDIRECT_H_7689_
/*******************************************************************************
 * This module provides the most fundamental I/O functions used by the C language 
 * runtime library. The default implementation will help to redirect the printf()
 * output and sscanf() input stream to/from the UART stream. This is usually very
 * useful in debugging. 
 * 
 * @attention The openwsn foundation contains a self-contained debugging input/output 
 * module rtl_debugio(and hal_debugio). It's more simpler and easy to use. 
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_uart.h"

/**
 * \brief Redirect the standard input/output stream to the uart.
 * 
 * @attention The uart must be constructed and opened successfully before call this function.
 */
void stdc_redirect( TiUartAdapter * uart );

#endif
