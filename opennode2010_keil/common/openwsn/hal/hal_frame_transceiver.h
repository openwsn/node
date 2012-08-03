#ifndef _FRAME_TRANSCEIVER_H_4892_
#define _FRAME_TRANSCEIVER_H_4892_
/*******************************************************************************
 * this file defines the transceiver interface that can be used by the MAC layer
 *
 * @state
 *  released. compile passed. need further developing
 *
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 20090718
 *	- revision
 * @modified by zhangwei on 20090819
 *	- add default settings of panid, local address and pan id in cc2420_open()
 * @modified by zhangwei on 20090927
 *	- revision. divide all the interface functions into serveral group according 
 *    to their functions.
 * @modified by zhangwei on 2010.05.10
 *  - revision. 
 * @modified by zhangwei on 2012.08.03
 *  - add TiFunIoFilter setrxfilter;
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_iobuf.h"

/* FTRX is the abbreviation of "frame transceiver" */

#define FTRX_MODE_RX            0
#define FTRX_MODE_TX            1
#define FTRX_MODE_SLEEP         2
#define FTRX_MODE_POWERDOWN     3

#define TiFrameRxTxInterface TiFrameTxRxInterface 

struct _TiFrameTxRxInterface;

//typedef uint8  (* TiFunFtrxSend)( void * trx, TiIoBuf * iobuf, uint8 option );
//typedef uint8  (* TiFunFtrxRecv)( void * trx, TiIoBuf * iobuf, uint8 option );
typedef intx  (* TiFunFtrxSend)( void * trx, char * buf, uintx len, uint8 option );
typedef intx  (* TiFunFtrxRecv)( void * trx, char * buf, uintx capacity, uint8 option );
typedef void   (* TiFunFtrxEvolve)( void * trx, TiEvent * e );
typedef void   (* TiFunFtrxSwitchToMode)( void * trx, uint8 mode );
typedef uint8  (* TiFunFtrxIsChannelClear)( void * trx );
typedef uint8  (* TiFunFtrxEnableAutoAck)( void * trx );
typedef uint8  (* TiFunFtrxDisableAutoAck)( void * trx );
typedef uint8  (* TiFunFtrxEnableAddrDecode)( void * trx );
typedef uint8  (* TiFunFtrxDisableAddrDecode)( void * trx );
typedef void   (* TiFunFtrxSetListener)( void * trx, TiFunEventHandler listener, void * lisowner );

typedef uint8  (* TiFunFtrxSetChannel)( void * trx, uint8 chn );
typedef uint8  (* TiFunFtrxSetPanId)( void * trx, uint8 panid );
typedef uint16 (* TiFunFtrxGetPanId)( void * trx );
typedef uint8  (* TiFunFtrxSetShortAddress)( void * trx, uint16 addr );
typedef uint16 (* TiFunFtrxGetShortAddress)( void * trx );
typedef void   (* TiFunFtrxSetTxPower)( void * trx, uint8 power );
typedef uint8  (* TiFunFtrxGetRssi)( void * trx );

// typedef uint8  (* TiFunFtrxGetLqi)( void * trx );
// typedef uint8  (* TiFunFtrxSetListener)( void * trx, TiFunEventHandler listener );


struct _TiFrameTxRxInterface{
    void * provider;
    TiFunFtrxSend send;
    TiFunFtrxRecv recv;
    TiFunFtrxEvolve evolve;
    TiFunFtrxSwitchToMode switchtomode;
    TiFunFtrxIsChannelClear ischnclear;
    TiFunFtrxEnableAutoAck enable_autoack;
    TiFunFtrxDisableAutoAck disable_autoack;
    TiFunFtrxEnableAddrDecode enable_addrdecode;
    TiFunFtrxDisableAddrDecode disable_addrdecode;
    TiFunFtrxSetChannel setchannel;
    TiFunFtrxSetPanId setpanid;
    TiFunFtrxGetPanId getpanid;
    TiFunFtrxSetShortAddress setshortaddress;
    TiFunFtrxGetShortAddress getshortaddress;
    TiFunFtrxSetTxPower settxpower;
    TiFunFtrxGetRssi getrssi;
    TiFunFtrxSetListener setlistener;
    TiFunIoFilter setrxfilter;
    //TiFunFtrxGetLqi getlqi;
    //TiFunFtrxSetListener;
};

#define TiFrameTxRxInterface struct _TiFrameTxRxInterface


/* @modified by openwsn on 2011.05
 * - Improve support to component based architecture
 */
/*
#define IMPL_FRAME_TRANSCEIVER_BY_COMPONENT(intf,impl,identifier)
*/  

#endif  /* _FRAME_TRANSCEIVER_H_4892_ */
