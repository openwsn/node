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

#ifndef _SVC_SINKNODE_H_3128_
#define _SVC_SINKNODE_H_3128_

/******************************************************************************
 * @author zhangwei on 20070402
 * this is the full function node of OpenWSN. this version module contains all 
 * the necessary component for the wsn node, including timer, mac, net, sensing,
 * and remote management/upgrade. (remote management and upgrate has not developed 
 * yet)
 * 
 * @state
 * 	not test
 * 
 * @modified by zhangwei on 20070402
 * 
 *****************************************************************************/ 

#include "svc_foundation.h"

/******************************************************************************
 * commands are a special frame. they were sent by the computer to the sink node 
 * and can be recognized by the sinknode, then the sink node will perform the
 * specified operations. 
 * 
 * [PHY Header][MAC Header][SVC Header][APP Header][APP DATA][CHECKSUM]
 *                         |<------ MAC Frame Payload ------>|
 *
 * [SVC Header] := [SVC Header Length 1B] [SVC Command 1B]
 * SVC Header always existed. the minimal length is 0. but the header byte still
 * occupy 1 byte. the value of the length byte doesn't include the length byte 
 * itself.
 * 
 * [SVC Command Byte]
 * [b7  b6  b5  b4  b3  b2  b1  b0]
 *
 * [APP Header] := [APP Command 1B]
 * the APP Header only occupy 1 byte. 
 * the length of [APP Header][APP DATA] is calculated as:
 *		PHY frame length - MAC Header size - SVC Header size
 * 
 * [SVC Command Byte]
 * [b7  b6  b5  b4  b3  b2  b1  b0]
 *
 * if b7 == 1, then this is an application layer command. and it can be recognized
 * by the node software, which will lead to some special operations. 
 * if b7 == 0, then this is a pure data frame. 
 * 
 *****************************************************************************/ 

#define CMD_BASE                    0x80
#define CMD_NETWORK_STARTUP         (CMD_BASE+1)
#define CMD_NETWORK_SHUTDOWN        (CMD_BASE+2)
#define CMD_NETWORK_SLEEP           (CMD_BASE+3)
#define CMD_NETWORK_WAKEUP          (CMD_BASE+4)
#define CMD_PREPARE_ROUTING         (CMD_BASE+5)
#define CMD_REQUEST_DATA            (CMD_BASE+6)
#define CMD_TIME_SYNC               (CMD_BASE+7)
#define CMD_LOCALIZE                (CMD_BASE+8)

void sinknode( void );

#endif


