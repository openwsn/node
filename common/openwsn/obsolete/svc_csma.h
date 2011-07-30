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

#ifndef _OPENMAC_H_7248_
#define _OPENMAC_H_7248_

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * OpenMAC
 * an distributed medium access layer. this layer is kind of ALOHA and use 
 * backoff to avoid collision. it also uses the cc2420's CCA feature though you 
 * cannot see the code.
 * 
 * - 由于系统底层采用的是cc2420 802.15.4 support芯片，ACK/NAK是在TiCc2420Adapter
 *   中实现的
 * - 随机延迟发送
 * - 重发
 * - 在扩展函数中提供了neighbor发现，尽管这部分内容严格意义上应该属于拓扑管理，
 *   不属于MAC层
 * - 提供了信道质量估计
 *****************************************************************************/
  
#include "svc_configall.h"  
#include "../hal/hal_foundation.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_openframe.h"
#include "svc_foundation.h"
#include "svc_actsche.h"

/* @TODO: these two config macros will be moved to configure.h in the future
 * as global effective configuration settings. 
 */

#undef  CONFIG_OPENMAC_SIMPLE
#define CONFIG_OPENMAC_SIMPLE

#define CONFIG_OPENMAC_FULL
#undef  CONFIG_OPENMAC_FULL 

#undef  CONFIG_OPENMAC_EXTENSION_ENABLE
#define CONFIG_OPENMAC_EXTENSION_ENABLE

#define CONFIG_OPENMAC_SECURITY_ENABLE
#undef  CONFIG_OPENMAC_SECURITY_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

/* The following macros are used as the network PHY layers interface.
 * so you can easily port to other PHY implementations with the most less 
 * modifications on current MAC source code. 
 */
#define THdlDriver TCc2420Driver 
#define _hdl_read(cc,frame,size,opt) cc2420_read(cc,frame,len,opt)
#define _hdl_write(cc,frame,size,opt) cc2420_write(cc,frame,length,opt)
#define _hdl_rawread(cc,buf,size,opt) cc2420_rawread(cc,buf,size,opt)
#define _hdl_rawwrite(cc,buf,size,opt) cc2420_rawwrite(cc,buf,size,opt) 
#define _hdl_wakeup(phy) NULL
#define _hdl_sleep(phy) NULL 
	
/******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 * 
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 *  
 *****************************************************************************/

#define MAC_STATE_IDLE 0
#define MAC_STATE_PAUSE 1
#define MAC_STATE_RECVING 2
#define MAC_STATE_RX_SENDCTS 8
#define MAC_STATE_RX_WAITDATA 3
#define MAC_STATE_SENDING 4
#define MAC_STATE_TX_DELAY 5
#define MAC_STATE_TX_WAITCTS 6
#define MAC_STATE_TX_WAITACK 7

#define MAC_EVENT_NULL 0

/* #define OPENMAC_PAYLOAD_SIZE (OPENWSN_MAX_MAC_FRAME_LENGTH-7)
 */
#define OPENMAC_PAYLOAD_SIZE OPF_PAYLOAD_SIZE
#define OPENMAC_RETRY_LIMIT 3
#define OPENMAC_BUFFER_SIZE OPF_FRAME_SIZE

typedef struct{
  uint8 state;
  TiOpenAddress addr;
  uint8 linkquality;
  uint8 signalstrength;
  uint32 distance;  
}TiOpenMACNode;

#define MAC_CONFIG_PANID 			0x01 
#define MAC_CONFIG_LOCALADDRESS		0x02 
#define MAC_CONFIG_TUNNING_POWER 	0x03
#define MAC_CONFIG_CHANNEL			0x04
#define MAC_BASIC_INIT               0x05
#define MAC_XTAL_SWITCH              0x06
#define MAC_CONFIG_APPLY				0x07
#define MAC_SET_ACKREQUEST           0x08

/* retry	the count retried. 已经retry的次数
 * seqno	sequence number, 按照15.4规定
 */
typedef struct{
  uint8 	state;
  uint8 	event;
  TiCc2420Adapter * phy;
  uint8 	retry;  
  uint8 	seqno; 
  TiTimerAdapter *  timer; 
  uint8 	txlen;
  uint8 	rxlen;
  char * 	txframe;
  char * 	rxframe;
  char * 	ackbuf;
  uint8 	backoff;
  uint8 	backoff_rule;
  uint8 	sleepduration;
  TiOpenAddress localaddr;
  TiOpenAddress rmtaddr;
  char 		txbuf[OPF_FRAME_SIZE];
  char 		rxbuf[OPF_FRAME_SIZE];
  char 		rxheader[7];
  #ifdef CONFIG_OPENMAC_EXTENSION_ENABLE  
  //TiOpenMACNode ;?
  #endif
}TiOpenMAC;  

TiOpenMAC * mac_construct( char * buf, uint16 size );
void  mac_destroy( TiOpenMAC * mac );
void  mac_open( TiOpenMAC * mac, TiCc2420Adapter * hdl, TiActionScheduler * actsche, TiTimerAdapter * timer, TiOpenAddress * addr ); 
void  mac_close( TiOpenMAC * mac );
void  mac_init( TiOpenMAC * mac, TiCc2420Adapter * phy, TiTimerAdapter * timer );
void  mac_configure( TiOpenMAC * mac, uint8 ctrlcode, uint16 value );

uint8 mac_read( TiOpenMAC * mac, TiOpenFrame * frame, uint8 size, uint8 opt );
uint8 mac_rawread( TiOpenMAC * mac, char * framebuffer, uint8 size, uint8 opt );
uint8 mac_write( TiOpenMAC * mac, TiOpenFrame * frame, uint8 len, uint8 opt );
uint8 mac_rawwrite( TiOpenMAC * mac, char * framebuffer, uint8 len, uint8 opt );
uint8 mac_state( TiOpenMAC * mac );
int8  mac_evolve( TiOpenMAC * mac );
uint8 mac_sleep( TiOpenMAC * mac );
uint8 mac_wakeup( TiOpenMAC * mac );

uint8 mac_setrmtaddress( TiOpenMAC * mac, TiOpenAddress * addr );
uint8 mac_setlocaladdress( TiOpenMAC * mac, TiOpenAddress * addr );
uint8 mac_getrmtaddress( TiOpenMAC * mac, TiOpenAddress * addr );
uint8 mac_getlocaladdress( TiOpenMAC * mac, TiOpenAddress * addr );
uint8 mac_installnotify( TiOpenMAC * mac, TiFunEventHandler * callback, void * owner );

#ifdef CONFIG_OPENMAC_EXTENSION_ENABLE
int8 mac_probe( TiOpenMAC * mac );
int8 mac_updatestatistics( TiOpenMAC * mac );
int8 mac_getnode( TiOpenMAC * mac, TiOpenMACNode * node );
int8 mac_getneighbors( TiOpenMAC * mac, TiOpenAddress * addr[] );
uint8 mac_getlinkquality( TiOpenMAC * mac, TiOpenAddress * addr );
uint8 mac_getsignalstrength( TiOpenMAC * mac, TiOpenAddress * addr );
#endif

#ifdef __cplusplus
}
#endif

#endif
