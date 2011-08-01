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

#ifndef _SVC_WLSCOMM_H_8432_
#define _SVC_WLSCOMM_H_8432_
/******************************************************************************* 
 * svc_wlscomm
 * wlscomm denotes "wireless communication". It bridges the low level wireless adapter
 * and the high level network service/protocol filter. 
 * 
 * It differs from the low level wireless transceiver adapter at:
 *	- Provide high level send()/recv() function and listening mechanism. But all
 *    these are running in non-interrupt mode. So you can freely use them.
 *    While, the TiCc2420Adapter's listener is actually running in interrupt mode.
 *  - contains a simple queue to reduce the frame loss probability. (However, the 
 *    queue isn't mandatory. Furthermore, for the frame based transceiver, the 
 *    frame queue is actually implemented as a I/O filter. So it's unnecessary 
 *    to provide queues inside "wlscomm"
 *
 * The the architecture of a wireless communication application is as the following:
 * 
 *     [application / filter chain iohandler]
 *                      |
 *  [filter_chain: includes protocol and service filters]
 *                      |
 *                  [wlscomm]
 *                      |
 *             [transceiver adapter]
 * 
 * For more about the filter architecture design, please refer to Apache MINA:
 *	- http://mina.apache.org/
 *  - http://mina.apache.org/conferences.data/Mina_in_real_life_ASEU-2009.pdf
 * 
 * The architecture of MINA is:
 *	Remote Peer ~~~ IoService -- IoFilterChain{IoFilters} -- IoHandler
 *  IoService := [IoAcceptor + IoProcessor] | [IoConnector]
 * 
 * The "wlscomm" module in "openwsn" is similar to the "ioservice" in Apache MINA. 
 * However, considering the special features of wireless communication and sensor
 * networks, we simplified MINA's architecture by eliminating the processor part.
 *
 ******************************************************************************/

/******************************************************************************* 
 * @state
 *	compile passed
 *
 * @history
 * @author zhangwei on 20051010
 *	- first created
 * @modified by huanghuan in 200511
 *  - "wlscomm" is designed for wireless communication. compile passed.  
 * @modified by zhangwei in 200610
 *	- move the hardware related source code into separate hardware abstraction layer(hal)
 *    modules such as "hal_cc2420". 
 * @modified by zhangwei in 200908
 *	- redefine the "wlscomm" module. Now this module is targeted to bridging the gap
 *    between wireless transceiver and network filter chain. 
 *
 ******************************************************************************/

#include "svc_configall.h"
#include "../hal/hal_cc2420.h"
#include "../rtl/rtl_openframe.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_foundation.h"

/* The TiWirelessCommunication is using TiCc2420Adapter by default. You can change 
 * the following macros to use other adapter object 
 */
#define _TiWirelessAdapter TiCc2420Adapter
#define _wlsbase_recv(obj,buf,size,opt) cc2420_recv(obj,buf,size,opt)
#define _wlsbase_send(obj,buf,len,opt) cc2420_send(obj,buf,len,opt)

typedef struct{
	uint8 state;
	_TiWirelessAdapter * adapter;    
	//TiFrameQueue * rxque;
	//TiFrameQueue * txque;
	//TiOpenFrame * txframe;
	TiIoFilterChain * filterchain;
    TiIoHandler iohandler;
	//char rxbufmem[OPF_SUGGEST_SIZE];
}TiNetIoService;

TiNetIoService * nio_construct( void * mem, uint16 size, _TiWirelessAdapter * adapter, 
	TiFilterChain * filterchain, TiIoHandler iohandler );
void nio_destroy( TiNetIoService * nio );
void nio_evolve( TiNetIoService * nio, TiEvent * e );

or name it as wio_

#endif
