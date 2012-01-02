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

#ifndef _BIT_TRANSCEIVER_H_4892_
#define _BIT_TRANSCEIVER_H_4892_
/*******************************************************************************
 * this file defines the transceiver interface that can be used by the MAC layer
 *
 * @state
 *  developing
 * 
 * @reference
 * 	- hal_bit_transceiver
 *
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 2010.05.10
 *	- revision
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_iobuf.h"

/* BTRX is the abbreviation of "bit transceiver". Bit transceiver can only send 
 * bit streams. Which means the micro-controller must use software to split the 
 * bit stream into continuous frames 
 */
#define BTRX_MODE_RX            0
#define BTRX_MODE_TX            1
#define BTRX_MODE_SLEEP         2
#define BTRX_MODE_POWERDOWN     3

struct _TiBitTxRxInterface;

typedef uint8  (* TiFunBtrxSend)( struct _TiBitTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef uint8  (* TiFunBtrxRecv)( struct _TiBitTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef void   (* TiFunBtrxEvolve)( struct _TiBitTxRxInterface * trx, TiEvent * e );
typedef void   (* TiFunBtrxSwitchToMode)( struct _TiBitTxRxInterface * trx, uint8 mode );
typedef uint8  (* TiFunBtrxIsChannelClear)( struct _TiBitTxRxInterface * trx );

typedef uint8  (* TiFunBtrxSetChannel)( struct _TiBitTxRxInterface * trx, uint8 chn );
typedef void   (* TiFunBtrxSetTxPower)( struct _TiBitTxRxInterface * trx, uint8 power );
typedef uint8  (* TiFunBtrxGetRssi)( struct _TiBitTxRxInterface * trx );

// typedef uint8  (* TiFunBtrxGetLqi)( struct _TiBitTxRxInterface * trx );
// typedef uint8  (* TiFunBtrxSetListener)( struct _TiBitTxRxInterface * trx, TiFunEventHandler listener );

/* _TiBitTxRxInterface
 * bit stream sending/receving transceiver interface
 */

struct _TiBitTxRxInterface{
    void * object;
    TiFunBtrxSend send;
    TiFunBtrxRecv recv;
    TiFunBtrxEvolve evolve;
    TiFunBtrxSwitchToMode switchtomode;
    TiFunBtrxIsChannelClear ischnclear;
    TiFunBtrxSetChannel setchannel;
    TiFunBtrxSetTxPower settxpower;
    TiFunBtrxGetRssi getrssi;
    //TiFunBtrxGetLqi getlqi;
    //TiFunBtrxSetListener;
};

#define TiBitTxRxInterface struct _TiBitTxRxInterface

#endif /* _BIT_TRANSCEIVER_H_4892_ */

