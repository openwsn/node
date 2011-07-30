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

#ifndef _SVC_MACA_H_4829_
#define _SVC_MACA_H_4829_

/*******************************************************************************
 * svc_maca
 *
 * @state
 *  in developing
 *
 * @author zhangwei in 200912
 *  - first created
 * @modified by zhangwei on 2010.05.10
 *  - revision. 
 * @modified by zhangwei on 2010.06.14
 *  - revision. compile passed
 * 
 ******************************************************************************/

/* hal_transceiver.i contains the transceiver interface definition. generally, you 
 * should use functions only inside the interface to implement your medium access 
 * control layer(MACA).
 */
#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_frame_transceiver.h"


#define CONFIG_MACA_DEFAULT_PANID				0x0001
#define CONFIG_MACA_DEFAULT_LOCAL_ADDRESS		0x07
#define CONFIG_MACA_DEFAULT_REMOTE_ADDRESS      0x08
#define CONFIG_MACA_DEFAULT_CHANNEL            11
#define CONFIG_MACA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_MACA_MAX_RETRY                  3
#define CONFIG_MACA_ACK_RESPONSE_TIME          10

#define CONFIG_MACA_TRX_SUPPORT_ACK

#define CONFIG_MACA_WAIT_CTS_DURATION          5
#define CONFIG_MACA_RTS_SLEEPTIME              10

#define MACA_STATE_NULL                        0
#define MACA_STATE_IDLE                        1
#define MACA_STATE_WAIT_CTS                    2
#define MACA_STATE_RETRY_SEND                  5
#define MACA_STATE_SLEEP                       3
#define MACA_STATE_POWERDOWN                   4

#define MACA_SLEEP_REQUEST                     1
#define MACA_WAKEUP_REQUEST                    2
#define MACA_SHUTDOWN_REQUEST                  3
#define MACA_STARTUP_REQUEST                   4

#define MACA_EVENT_FRAME_ARRIVAL               234 // todo

#define MACA_RXBUF_MEMSIZE FRAME_HOPESIZE(128)
#define MACA_TXBUF_MEMSIZE FRAME_HOPESIZE(128)


#ifdef __cplusplus
extern "C"{
#endif

/* TiMACA:
 *	- state
 */
typedef struct{
    uint8 state;
    TiTimerAdapter * timer;
	TiFrameTxRxInterface * rxtx;
    void * rxtxowner;
    uint8 loadfactor;
    uint8 request;
    uint8 retry;
    uint16 backoff;
    TiFunEventHandler listener;
    void * lisowner;
    TiFrame * rxbuf;
    TiFrame * txbuf;
    char rxbufmem[MACA_RXBUF_MEMSIZE];
    char txbufmem[MACA_TXBUF_MEMSIZE];
    TiIEEE802Frame154Descriptor desc;
	uint8 option;
}TiMACA;

TiMACA * maca_construct( char * buf, uint16 size );
void maca_destroy( TiMACA * mac );
TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * rxtx, void * rxtxowner, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner );
void maca_close( TiMACA * mac );

uintx maca_send( TiMACA * mac, TiFrame * iobuf, uint8 option );
uintx maca_broadcast( TiMACA * mac, TiFrame * iobuf, uint8 option );
uintx maca_recv( TiMACA * mac, TiFrame * iobuf, uint8 option );
void  maca_evolve( void * macptr, TiEvent * e );

#define maca_setshortaddress(mac,addr) mac->rxtx->setshortaddress((mac->rxtxowner),addr)
#define maca_setremoteaddr(mac,addr) mac->rxtx->setremoteaddr((mac->rxtxowner),addr)
#define maca_setpanid(mac,pan) mac->rxtx->setpanid(mac->rxtx,pan)
#define maca_setchannel(mac,chn) mac->rxtx->setchannel((mac->rxtxowner),chn)
#define maca_ischannelclear(mac) mac->rxtx->ischannelclear((mac->rxtxowner))


#ifdef __cplusplus
}
#endif

#endif
