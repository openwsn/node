#ifndef _RTL_DATAEXCHANGE_H_AA62_
#define _RTL_DATAEXCHANGE_H_AA62_
/***********************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2004-2010 zhangwei(TongJi University)
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
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_varsequ.h"

/***********************************************************************
 * rtl_dataexchange
 * data exchange packet manipulation functions
 * 
 * Data Exchange Packet := [Control 1B] [Packet Flow Identifier 1B] [Time-stamp 10B]
 *  [Variable Count 2B] {[Variable Value]}
 * 
 * Timestamp := [UTC Time 8B] [ Mil-Seconds 2B]
 * Variable Value := [IsNull 1b][Var Type 7b] [Length 2B] [Value nB]
 * If IsNull bit is 1, then this value is NULL.
 ******************************************************************************/

/* the low 4 bits indicate data exchange protocol's command */
#define DXC_COMMAND(ctrl) ((ctrl) & 0x0F)

#define DXC_COMMAND_VARDATA 		0
#define DXC_COMMAND_ARRAY			1
#define DXC_COMMAND_REQUEST 		2
#define DXC_COMMAND_RESPONSE 		3
#define DXC_COMMAND_SUBSCRIBE 		4
#define DXC_COMMAND_UNSUBSCRIBE 	5
#define DXC_COMMAND_ACTION 			6

#ifdef __cplusplus
extern "C" {
#endif

uint16 dxc_pack( uint8 ctrl, uint8 flowno, char * timepoint, TiXtpVarSequence * varsequ, char * buf, uint16 size );

/* @attention
 *  - before calling this function, you should already create the corresponding
 *    variables and append them into var sequence object. 
 */
uint16 dxc_unpack( uint8 * ctrl, uint8 * flowno, char * timepoint, TiXtpVarSequence * varsequ, char * buf, uint16 len );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
