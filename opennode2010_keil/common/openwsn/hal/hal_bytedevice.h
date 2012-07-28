#ifndef _BYTE_DEVICE_H_4892_
#define _BYTE_DEVICE_H_4892_
/*******************************************************************************
 * this file defines the byte based transceiver interface that can be used by the 
 * MAC layer.
 * 
 * currently, it's the same as the bit based transceiver interface
 *
 * @state
 *  developing
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

#define BDEV_MODE_POWERDOWN     0
#define BDEV_MODE_SLEEP         1
#define BDEV_MODE_ACTIVE        2

struct _TiByteDeviceInterface;

typedef uint8  (* TiFunEtrxSend)( struct _TiByteDeviceInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef uint8  (* TiFunEtrxRecv)( struct _TiByteDeviceInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef void   (* TiFunEtrxEvolve)( struct _TiByteDeviceInterface * trx, TiEvent * e );
typedef void   (* TiFunEtrxSwitchToMode)( struct _TiByteDeviceInterface * trx, uint8 mode );

/* _TiByteDeviceInterface
 * bit stream sending/receving transceiver interface
 */

struct _TiByteDeviceInterface{
    void * object;
    TiFunEtrxSend send;
    TiFunEtrxRecv recv;
    TiFunEtrxEvolve evolve;
    TiFunEtrxSwitchToMode switchtomode;
};

#define TiByteDeviceInterface struct _TiByteDeviceInterface

#endif /* _BYTE_DEVICE_H_4892_ */

