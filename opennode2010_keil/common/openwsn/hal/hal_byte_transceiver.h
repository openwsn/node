#ifndef _HAL_BYTE_TRANSCEIVER_H_4892_
#define _HAL_BYTE_TRANSCEIVER_H_4892_
/*******************************************************************************
 * this file defines the byte based transceiver interface that can be used by the 
 * MAC layer.
 * 
 * currently, it's the same as the bit based transceiver interface
 *
 * @state
 *  developing
 * 
 * @reference
 * 	- hal_frame_transceiver
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

/* The byte transceiver can only send byte streams. Which means the micro-controller 
 * must use software to split the byte stream into continuous frames 
 */
#define ETRX_MODE_RX            0
#define ETRX_MODE_TX            1
#define ETRX_MODE_SLEEP         2
#define ETRX_MODE_POWERDOWN     3

struct _TiByteTxRxInterface;

typedef uint8  (* TiFunEtrxSend)( struct _TiByteTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef uint8  (* TiFunEtrxRecv)( struct _TiByteTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef void   (* TiFunEtrxEvolve)( struct _TiByteTxRxInterface * trx, TiEvent * e );
typedef void   (* TiFunEtrxSwitchToMode)( struct _TiByteTxRxInterface * trx, uint8 mode );
typedef uint8  (* TiFunEtrxIsChannelClear)( struct _TiByteTxRxInterface * trx );

typedef uint8  (* TiFunEtrxSetChannel)( struct _TiByteTxRxInterface * trx, uint8 chn );
typedef void   (* TiFunEtrxSetTxPower)( struct _TiByteTxRxInterface * trx, uint8 power );
typedef uint8  (* TiFunEtrxGetRssi)( struct _TiByteTxRxInterface * trx );

// typedef uint8  (* TiFunEtrxGetLqi)( struct _TiByteTxRxInterface * trx );
// typedef uint8  (* TiFunEtrxSetListener)( struct _TiByteTxRxInterface * trx, TiFunEventHandler listener );

/* _TiByteTxRxInterface
 * bit stream sending/receving transceiver interface
 */

struct _TiByteTxRxInterface{
    void * object;
    TiFunEtrxSend send;
    TiFunEtrxRecv recv;
    TiFunEtrxEvolve evolve;
    TiFunEtrxSwitchToMode switchtomode;
    TiFunEtrxIsChannelClear ischnclear;
    TiFunEtrxSetChannel setchannel;
    TiFunEtrxSetTxPower settxpower;
    TiFunEtrxGetRssi getrssi;
    //TiFunEtrxGetLqi getlqi;
    //TiFunEtrxSetListener;
};

#define TiByteTxRxInterface struct _TiByteTxRxInterface

#endif /* _HAL_BYTE_TRANSCEIVER_H_4892_ */

