/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 *****************************************************************************/ 

#ifndef _OPENMAC_H_7248_
#define _OPENMAC_H_7248_

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * OpenMAC
 * an distributed medium access layer. this layer is kind of ALOHA and use 
 * backoff to avoid collision. it also uses the cc2420's CCA feature though you 
 * cannot see the code.
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

#undef CONFIG_OPENMAC_SIMPLE
#define CONFIG_OPENMAC_SIMPLE

#define CONFIG_OPENMAC_FULL
#undef CONFIG_OPENMAC_FULL 


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
  THdlDriver * phy;
  uint8 	retry;  
  uint8 	seqno; 
  TTimer *  timer; 
  uint8 	txlen;
  uint8 	rxlen;
  char * 	txframe;
  char * 	rxframe;
  char * 	ackbuf;
  uint8 	backoff;
  uint8 	backoff_rule;
  uint8 	sleepduration;
  TOpenAddress localaddr;
  TOpenAddress rmtaddr;
  char 		txbuf[OPF_FRAME_SIZE];
  char 		rxbuf[OPF_FRAME_SIZE];
  char 		rxheader[7];
}TOpenMAC;  

TOpenMAC * mac_construct( char * buf, uint16 size );
void  mac_destroy( TOpenMAC * mac );
void  mac_open( TOpenMAC * mac, TCc2420 * hdl, TActionScheduler * actsche, TTimer * timer, TOpenAddress * addr ); 
void  mac_close( TOpenMAC * mac );
void  mac_init( TOpenMAC * mac, THdlDriver * phy, TTimer * timer );
void  mac_configure( TOpenMAC * mac, uint8 ctrlcode, uint16 value );

uint8 mac_read( TOpenMAC * mac, TOpenFrame * frame, uint8 size, uint8 opt );
uint8 mac_rawread( TOpenMAC * mac, char * framebuffer, uint8 size, uint8 opt );
uint8 mac_write( TOpenMAC * mac, TOpenFrame * frame, uint8 len, uint8 opt );
uint8 mac_rawwrite( TOpenMAC * mac, char * framebuffer, uint8 len, uint8 opt );
uint8 mac_state( TOpenMAC * mac );
int8  mac_evolve( TOpenMAC * mac );
uint8 mac_sleep( TOpenMAC * mac );
uint8 mac_wakeup( TOpenMAC * mac );

uint8 mac_setrmtaddress( TOpenMAC * mac, TOpenAddress * addr );
uint8 mac_setlocaladdress( TOpenMAC * mac, TOpenAddress * addr );
uint8 mac_getrmtaddress( TOpenMAC * mac, TOpenAddress * addr );
uint8 mac_getlocaladdress( TOpenMAC * mac, TOpenAddress * addr );
uint8 mac_installnotify( TOpenMAC * mac, TEventHandler * callback, void * owner );

typedef struct{
  uint8 state;
  TOpenAddress addr;
  uint8 linkquality;
  uint8 signalstrength;
  uint32 distance;  
}TOpenMACNode;

int8 mac_probe( TOpenMAC * mac );
int8 mac_updatestatistics( TOpenMAC * mac );
int8 mac_getnode( TOpenMAC * mac, TOpenMACNode * node );
int8 mac_getneighbors( TOpenMAC * mac, TOpenAddress * addr[] );
uint8 mac_getlinkquality( TOpenMAC * mac, TOpenAddress * addr );
uint8 mac_getsignalstrength( TOpenMAC * mac, TOpenAddress * addr );

#endif
