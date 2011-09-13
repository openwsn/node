#ifndef _SVC_SIOACCEPTOR_H_2143_
#define _SVC_SIOACCEPTOR_H_2143_
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

/**
 * Serial I/O Acceptor
 * The serial I/O architecture is as the following
 * 
 * - Top Layer: I/O Service (provides the high level abstraction of the serial I/O functions)
 * - Middle Layer: I/O Protocols
 * - Bottom Layer: I/O Acceptors (provides the basic frame based I/O operations and 
 *  		frame queue management if necessary)
 * - Platform: Serial I/O Devivce Adapters
 * 
 * @author zhangwei in 2009
 * @modified by JiangRidong(TongJi University, BeiHang University) on 2011.08.06
 * - Finished and compile passed.
 */

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_slipfilter.h"
#include "../hal/hal_uart.h"
#include "svc_foundation.h"

//#define CONFIG_DYNA_MEMORY 1
//#undef  CONFIG_DYNA_MEMORY 

#undef  SIO_ACCEPTOR_SLIP_ENABLE
#define SIO_ACCEPTOR_SLIP_ENABLE 1

#ifndef CONFIG_SIOACCEPTOR_TXBUF_CAPACITY 
#define CONFIG_SIOACCEPTOR_TXBUF_CAPACITY 384
#endif

#ifndef CONFIG_SIOACCEPTOR_RXBUF_CAPACITY 
#define CONFIG_SIOACCEPTOR_RXBUF_CAPACITY 192
#endif

#ifndef CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY 
#define CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY 192
#endif

#define SIO_ACCEPTOR_MEMSIZE(bufsize) sizeof(TiSioAcceptor)

/**
 * This module defined the interface of librs232 dynamic link library
 * which can be used in other program files such as C# or LabView.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TiSioAcceptor component
 * 
 * Q: What's the difference between TiSioAcceptor and TiUartAdapter?
 * R: TiUartAdapter implements a byte-oriented transceiver which is
 * an light-weight encapsulation of the UART/USART/Serial Port hardware.
 *
 * TiSioAcceptor is running on top of TiUartAdapter component. It further
 * implements a frame based transceiver interface. Everytime you call read()/write()
 * function of TiSioAcceptor, you will read/write an complete packet/frame.
 * The framing mechanism currently is based on the rules in SLIP protocol. 
 */
//#pragma pack(push) 
//#pragma pack(1) 
// the above two line equals #pragma pack(push,1)
typedef __packed struct{
	uint8 state;
	TiUartAdapter * device;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
    #ifdef SIO_ACCEPTOR_SLIP_ENABLE
	TiIoBuf * tmpbuf;
    uint8 rx_accepted;
	TiSlipFilter slipfilter;
    #endif
	char txbuf_block[CONFIG_SIOACCEPTOR_TXBUF_CAPACITY];
	char rxbuf_block[CONFIG_SIOACCEPTOR_RXBUF_CAPACITY];
    #ifdef SIO_ACCEPTOR_SLIP_ENABLE
	char tmpbuf_block[CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY];
    #endif
}TiSioAcceptor;
//#pragma pack(pop) 

#ifdef CONFIG_DYNA_MEMORY
TiSioAcceptor * sac_create( TiUartAdapter * uart )
#endif

#ifdef CONFIG_DYNA_MEMORY
void sac_free( TiSioAcceptr * sac );
#endif

#define sac_send(sac,buf,option) sac_framesend((sac),(buf),(option))
#define sac_recv(sac,buf,option) sac_framerecv((sac),(buf),(option))

TiSioAcceptor * sac_construct( char * buf, uint16 size );

TiSioAcceptor * sac_open( TiSioAcceptor * sac, uint16 memsize, TiUartAdapter * uart );
void sac_close( TiSioAcceptor * sac );
TiIoResult sac_framesend( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
TiIoResult sac_iobufsend( TiSioAcceptor * sac, TiIoBuf * buf, TiIoOption option );//TiIoResult sac_iobufsend( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
TiIoResult sac_rawsend( TiSioAcceptor * sac, TiFrame * buf, uintx len, TiIoOption option );//TiIoResult sac_rawsend( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
TiIoResult sac_framerecv( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
TiIoResult sac_iobufrecv( TiSioAcceptor * sac, TiIoBuf * buf, TiIoOption option );//TiIoResult sac_iobufrecv( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
TiIoResult sac_rawrecv( TiSioAcceptor * sac, char * buf, uintx size, TiIoOption option );//TiIoResult sac_rawrecv( TiSioAcceptor * sac, TiFrame * buf, TiIoOption option ); 
void sac_evolve( TiSioAcceptor * sac, TiEvent * event ); 

#ifdef __cplusplus
}
#endif

#endif

