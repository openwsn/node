/*******************************************************************************
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
 ******************************************************************************/ 
#ifndef _CC2420CONST_H_4891_
#define _CC2420CONST_H_4891_


/******************************************************************************
 * cc2420 base
 * register definition
 * 
 * @state
 *	released
 * 
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 20070627
 * 	- revision format
 *****************************************************************************/

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

#define CC2420_RAM_TXFIFO		0x000
#define CC2420_RAM_RXFIFO		0x080
#define CC2420_RAM_KEY0			0x100
#define CC2420_RAM_RXNONCE		0x110
#define CC2420_RAM_SABUF		0x120
#define CC2420_RAM_KEY1			0x130
#define CC2420_RAM_TXNONCE		0x140
#define CC2420_RAM_CBCSTATE		0x150
#define CC2420_RAM_IEEEADDR		0x160
#define CC2420_RAM_PANID		0x168
#define CC2420_RAM_SHORTADDR	0x16A

/*****************************************************************************
 * Cc2420 Status Byte Returned by Command (SPI operation returns)
 * reference: cc2420 data sheet, p. 27
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


// MDMCTRL0 Register Bit Positions
#define CC2420_MDMCTRL0_FRAME        13  // 0 : reject reserved frame types, 1 = accept
#define CC2420_MDMCTRL0_PANCRD       12  // 0 : not a PAN coordinator
#define CC2420_MDMCTRL0_ADRDECODE    11  // 1 : enable address decode
#define CC2420_MDMCTRL0_CCAHIST      8   // 3 bits (8,9,10) : CCA hysteris in db
#define CC2420_MDMCTRL0_CCAMODE      6   // 2 bits (6,7)    : CCA trigger modes
#define CC2420_MDMCTRL0_AUTOCRC      5   // 1 : generate/chk CRC
#define CC2420_MDMCTRL0_AUTOACK      4   // 1 : Ack valid packets
#define CC2420_MDMCTRL0_PREAMBL      0   // 4 bits (0..3): Preamble length

// MDMCTRL1 Register Bit Positions
#define CC2420_MDMCTRL1_CORRTHRESH   6   // 5 bits (6..10) : correlator threshold
#define CC2420_MDMCTRL1_DEMOD_MODE   5   // 0: lock freq after preamble match, 1: continous udpate
#define CC2420_MDMCTRL1_MODU_MODE    4   // 0: IEEE 802.15.4
#define CC2420_MDMCTRL1_TX_MODE      2   // 2 bits (2,3) : 0: use buffered TXFIFO
#define CC2420_MDMCTRL1_RX_MODE      0   // 2 bits (0,1) : 0: use buffered RXFIFO

// RSSI Register Bit Positions
#define CC2420_RSSI_CCA_THRESH       8   // 8 bits (8..15) : 2's compl CCA threshold

// TXCTRL Register Bit Positions
#define CC2420_TXCTRL_BUFCUR         14  // 2 bits (14,15) : Tx mixer buffer bias current
#define CC2420_TXCTRL_TURNARND       13  // wait time after STXON before xmit
#define CC2420_TXCTRL_VAR            11  // 2 bits (11,12) : Varactor array settings
#define CC2420_TXCTRL_XMITCUR        9   // 2 bits (9,10)  : Xmit mixer currents
#define CC2420_TXCTRL_PACUR          6   // 3 bits (6..8)  : PA current
#define CC2420_TXCTRL_PADIFF         5   // 1: Diff PA, 0: Single ended PA
#define CC2420_TXCTRL_PAPWR          0   // 5 bits (0..4): Output PA level

// RXCTRL0 Register Bit Positions
#define CC2420_RXCTRL0_BUFCUR        12  // 2 bits (12,13) : Rx mixer buffer bias current
#define CC2420_RXCTRL0_HILNAG        10  // 2 bits (10,11) : High gain, LNA current
#define CC2420_RXCTRL0_MLNAG          8  // 2 bits (8,9)   : Med gain, LNA current
#define CC2420_RXCTRL0_LOLNAG         6  // 2 bits (6,7)   : Lo gain, LNA current
#define CC2420_RXCTRL0_HICUR          4  // 2 bits (4,5)   : Main high LNA current
#define CC2420_RXCTRL0_MCUR           2  // 2 bits (2,3)   : Main med  LNA current
#define CC2420_RXCTRL0_LOCUR          0  // 2 bits (0,1)   : Main low LNA current

// RXCTRL1 Register Bit Positions
#define CC2420_RXCTRL1_LOCUR         13  // Ref bias current to Rx bandpass filter
#define CC2420_RXCTRL1_MIDCUR        12  // Ref bias current to Rx bandpass filter
#define CC2420_RXCTRL1_LOLOGAIN      11  // LAN low gain mode
#define CC2420_RXCTRL1_MEDLOGAIN     10  // LAN low gain mode
#define CC2420_RXCTRL1_HIHGM          9  // Rx mixers, hi gain mode
#define CC2420_RXCTRL1_MEDHGM         8  // Rx mixers, hi gain mode
#define CC2420_RXCTRL1_LNACAP         6  // 2 bits (6,7) Selects LAN varactor array setting
#define CC2420_RXCTRL1_RMIXT          4  // 2 bits (4,5) Receiver mixer output current
#define CC2420_RXCTRL1_RMIXV          2  // 2 bits (2,3) VCM level, mixer feedback
#define CC2420_RXCTRL1_RMIXCUR        0  // 2 bits (0,1) Receiver mixer current

// FSCTRL Register Bit Positions
#define CC2420_FSCTRL_LOCK            14 // 2 bits (14,15) # of clocks for synch
#define CC2420_FSCTRL_CALDONE         13 // Read only, =1 if cal done since freq synth turned on
#define CC2420_FSCTRL_CALRUNING       12 // Read only, =1 if cal in progress
#define CC2420_FSCTRL_LOCKLEN         11 // Synch window pulse width
#define CC2420_FSCTRL_LOCKSTAT        10 // Read only, = 1 if freq synthesizer is loced
#define CC2420_FSCTRL_FREQ             0 // 10 bits, set operating frequency 

// SECCTRL0 Register Bit Positions
#define CC2420_SECCTRL0_PROTECT        9 // Protect enable Rx fifo
#define CC2420_SECCTRL0_CBCHEAD        8 // Define 1st byte of CBC-MAC
#define CC2420_SECCTRL0_SAKEYSEL       7 // Stand alone key select
#define CC2420_SECCTRL0_TXKEYSEL       6 // Tx key select
#define CC2420_SECCTRL0_RXKEYSEL       5 // Rx key select
#define CC2420_SECCTRL0_SECM           2 // 2 bits (2..4) # of bytes in CBC-MAX auth field
#define CC2420_SECCTRL0_SECMODE        0 // Security mode

// SECCTRL1 Register Bit Positions
#define CC2420_SECCTRL1_TXL            8 // 7 bits (8..14) Tx in-line security
#define CC2420_SECCTRL1_RXL            0 // 7 bits (0..7)  Rx in-line security

// BATTMON  Register Bit Positions
#define CC2420_BATTMON_OK              6 // Read only, batter voltage OK
#define CC2420_BATTMON_EN              5 // Enable battery monitor
#define CC2420_BATTMON_VOLT            0 // 5 bits (0..4) Battery toggle voltage

// IOCFG0 Register Bit Positions
#define CC2420_IOCFG0_FIFOPOL         10 // Fifo signal polarity
#define CC2420_IOCFG0_FIFOPPOL         9 // FifoP signal polarity
#define CC2420_IOCFG0_SFD              8 // SFD signal polarity
#define CC2420_IOCFG0_CCAPOL           7 // CCA signal polarity
#define CC2420_IOCFG0_FIFOTHR          0 // 7 bits, (0..6) # of Rx bytes in fifo to trg fifop

// IOCFG1 Register Bit Positions
#define CC2420_IOCFG1_HSSD            10 // 2 bits (10,11) HSSD module config
#define CC2420_IOCFG1_SFDMUX           5 // 5 bits (5..9)  SFD multiplexer pin settings
#define CC2420_IOCFG1_CCAMUX           0 // 5 bits (0..4)  CCA multiplexe pin settings


/* the following two vlaues cannot be changed! they're the maximum size of cc2420
 * transceiver's internal TX buffer and RX buffer size */

#define CC2420_TXFIFO_SIZE 128
#define CC2420_RXFIFO_SIZE 128


#endif  /* _CC2420CONST_H_4891_ */
