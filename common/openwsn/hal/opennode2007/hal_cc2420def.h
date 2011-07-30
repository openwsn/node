/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
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
 ****************************************************************************/ 

#ifndef _HAL_CC2420CHIP_H_
#define _HAL_CC2420CHIP_H_

/*****************************************************************************
 * @modified by zhangwei on 20070627
 * revision format
 ****************************************************************************/ 

/*****************************************************************************
 * Cc2420 Register Definition
 ****************************************************************************/ 

#define CC2420_SNOP             0x00
#define CC2420_SXOSCON          0x01
#define CC2420_STXCAL           0x02
#define CC2420_SRXON            0x03
#define CC2420_STXON            0x04
#define CC2420_STXONCCA         0x05
#define CC2420_SRFOFF           0x06
#define CC2420_SXOSCOFF         0x07
#define CC2420_SFLUSHRX         0x08
#define CC2420_SFLUSHTX         0x09
#define CC2420_SACK             0x0A
#define CC2420_SACKPEND         0x0B
#define CC2420_SRXDEC           0x0C
#define CC2420_STXENC           0x0D
#define CC2420_SAES             0x0E

#define CC2420_MAIN             0x10
#define CC2420_MDMCTRL0         0x11
#define CC2420_MDMCTRL1         0x12
#define CC2420_RSSI             0x13
#define CC2420_SYNCWORD         0x14
#define CC2420_TXCTRL           0x15
#define CC2420_RXCTRL0          0x16
#define CC2420_RXCTRL1          0x17
#define CC2420_FSCTRL           0x18
#define CC2420_SECCTRL0         0x19
#define CC2420_SECCTRL1         0x1A
#define CC2420_BATTMON          0x1B
#define CC2420_IOCFG0           0x1C
#define CC2420_IOCFG1           0x1D
#define CC2420_MANFIDL          0x1E
#define CC2420_MANFIDH          0x1F
#define CC2420_FSMTC            0x20
#define CC2420_MANAND           0x21
#define CC2420_MANOR            0x22
#define CC2420_AGCCTRL          0x23
#define CC2420_AGCTST0          0x24
#define CC2420_AGCTST1          0x25
#define CC2420_AGCTST2          0x26
#define CC2420_FSTST0           0x27
#define CC2420_FSTST1           0x28
#define CC2420_FSTST2           0x29
#define CC2420_FSTST3           0x2A
#define CC2420_RXBPFTST         0x2B
#define CC2420_FSMSTATE         0x2C
#define CC2420_ADCTST           0x2D
#define CC2420_DACTST           0x2E
#define CC2420_TOPTST           0x2F
#define CC2420_RESERVED         0x30

#define CC2420_TXFIFO           0x3E
#define CC2420_RXFIFO           0x3F

/*****************************************************************************
 * Cc2420 RAM/FIFO: size and address
 ****************************************************************************/ 

/* RAM Sizes */

#define CC2420_RAM_SIZE			368
#define CC2420_FIFO_SIZE		128

/* RAM Address */

#define CC2420RAM_TXFIFO		0x000
#define CC2420RAM_RXFIFO		0x080
#define CC2420RAM_KEY0			0x100
#define CC2420RAM_RXNONCE		0x110
#define CC2420RAM_SABUF			0x120
#define CC2420RAM_KEY1			0x130
#define CC2420RAM_TXNONCE		0x140
#define CC2420RAM_CBCSTATE		0x150
#define CC2420RAM_IEEEADDR		0x160
#define CC2420RAM_PANID			0x168
#define CC2420RAM_SHORTADDR		0x16A

/*****************************************************************************
 * Cc2420 Status Byte Returned by Command 
 ****************************************************************************/ 

#define CC2420_XOSC16M_STABLE	6
#define CC2420_TX_UNDERFLOW		5
#define CC2420_ENC_BUSY			4
#define CC2420_TX_ACTIVE		3
#define CC2420_LOCK		        2
#define CC2420_RSSI_VALID		1

/*****************************************************************************
 * SECCTRL0
 ****************************************************************************/ 

#define CC2420_SECCTRL0_NO_SECURITY         0x0000
#define CC2420_SECCTRL0_CBC_MAC             0x0001
#define CC2420_SECCTRL0_CTR                 0x0002
#define CC2420_SECCTRL0_CCM                 0x0003

#define CC2420_SECCTRL0_SEC_M_IDX           2

#define CC2420_SECCTRL0_RXKEYSEL0           0x0000
#define CC2420_SECCTRL0_RXKEYSEL1           0x0020

#define CC2420_SECCTRL0_TXKEYSEL0           0x0000
#define CC2420_SECCTRL0_TXKEYSEL1           0x0040

#define CC2420_SECCTRL0_SEC_CBC_HEAD        0x0100
#define CC2420_SECCTRL0_RXFIFO_PROTECTION   0x0200

/*****************************************************************************
* end of file
****************************************************************************/ 
#endif
