#ifndef _RTL_SLIP_SPLITER_H_6577_
#define _RTL_SLIP_SPLITER_H_6577_
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

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_iobuf.h"

typedef struct{
  // int received;
  // uint8 tx_state; // not used now
  uint8 rx_state;
}TiSlipFilter;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DYNA_MEMORY
TiSlipFilter * slip_filter_create();
void slip_filter_free( TiSlipFilter * slip );
#endif

TiSlipFilter * slip_filter_open( TiSlipFilter * slip, uintx size );
void slip_filter_close( TiSlipFilter * slip );
int slip_filter_tx_handler( TiSlipFilter * slip, TiIoBuf * input, TiIoBuf * output );
int slip_filter_rx_handler( TiSlipFilter * filter, TiIoBuf * input, TiIoBuf * output );

#ifdef __cplusplus
}
#endif

#endif /* _RTL_SLIP_SPLITER_H_6577_ */
