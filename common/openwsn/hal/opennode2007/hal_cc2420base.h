/******************************************************************************
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
 *****************************************************************************/

#ifndef _HAL_CC2420RF_H_3957_
#define _HAL_CC2420RF_H_3957_

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * TiCc2420Adapter
 * this is the fundamental module under TCc2420.*. most of the hardware related
 * operations are implemented in this module. so if you want to port TCc2420
 * to other platforms, you can port this module only!
 *
 * @history
 * @modified by zhangwei on 20060624
 *	this header file replaced the old "basic_rf.h" provided by HuangHuan.
 * @modified by zhangwei on 20070601
 *  change the name from hal_cc2420rf.* to hal_cc2420base.*
 *
 *****************************************************************************/

#include "hal_foundation.h"
#include "hal_spi.h"
#include "hal_cc2420.h"

/******************************************************************************
 * general constants to be used in TCc2420
 * hardware related
 *****************************************************************************/

// Constants concerned with the Basic RF packet format
//
// Packet overhead ((frame control field, sequence number, PAN ID, destination and source) + (footer))
// Note that the length byte itself is not included included in the packet length

#define BASIC_RF_PACKET_OVERHEAD_SIZE   ((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_MAX_PAYLOAD_SIZE		(127 - BASIC_RF_PACKET_OVERHEAD_SIZE)
#define BASIC_RF_ACK_PACKET_SIZE		5

// The time it takes for the acknowledgment packet to be received after the data packet has been
// transmitted
#define BASIC_RF_ACK_DURATION			(0.5 * 32 * 2 * ((4 + 1) + (1) + (2 + 1) + (2)))
#define BASIC_RF_SYMBOL_DURATION	    (32 * 0.5)

// The length byte
#define BASIC_RF_LENGTH_MASK            0x7F

// Frame control field
#define BASIC_RF_FCF_NOACK              0x8841
#define BASIC_RF_FCF_ACK                0x8861
#define BASIC_RF_FCF_ACK_BM             0x0020
#define BASIC_RF_FCF_BM                 (~BASIC_RF_FCF_ACK_BM)
#define BASIC_RF_ACK_FCF		        0x0002

// Footer
#define BASIC_RF_CRC_OK_BM              0x80

/******************************************************************************
 * general utility functions for TCc2420
 * a lot of them are based on TSpi.
 * only some of them are hardware related.
 *****************************************************************************/

void radio_init( TiSpiAdapter * spi );
void radio_reset(  TiSpiAdapter * spi );
void radio_settxpower( TiSpiAdapter * spi );
void radio_setchannel( TiSpiAdapter * spi );
void radio_get_lqi(  TiSpiAdapter * spi, int8 rssidbm, uint8 cor );


/******************************************************************************
 * general utility functions for TCc2420
 * a lot of them are based on TSpi.
 * only some of them are hardware related.
 *****************************************************************************/

#define cc2420_spi_strobe(spi,cmd) FAST2420_STROBE(spi,cmd)


void FAST2420_RX_GARBAGE(TiSpiAdapter * spi,char *pc)  ;
void FAST2420_TX_ADDR(TiSpiAdapter * spi,uint8 a);
void FAST2420_RX_ADDR(TiSpiAdapter * spi,uint8 a);
uint8 FAST2420_STROBE(TiSpiAdapter * spi,uint8 s);
void FAST2420_SETREG(TiSpiAdapter * spi,uint8 a,uint16 v);
void FAST2420_GETREG(TiSpiAdapter * spi,uint8 a,uint16 *v);
void FAST2420_UPD_STATUS(TiSpiAdapter * spi,uint8 *s);
void FAST2420_WRITE_FIFO(TiSpiAdapter * spi,uint8 *p,uint8 c);
void FAST2420_READ_FIFO(TiSpiAdapter * spi,uint8 *p,uint8 c);
void FAST2420_READ_FIFO_BYTE(TiSpiAdapter * spi,uint8 *b);
void FAST2420_READ_FIFO_NO_WAIT(TiSpiAdapter * spi,uint8 *p, uint8 c);
void FAST2420_READ_FIFO_GARBAGE(TiSpiAdapter * spi,uint8 c);
void FAST2420_WRITE_RAM_LE(TiSpiAdapter * spi,uint16 *p,uint16 a,uint8 c);
void FAST2420_READ_RAM_LE(TiSpiAdapter * spi,uint8 *p,uint16 a,uint8 c);
void FAST2420_WRITE_RAM(TiSpiAdapter * spi,uint16 *p,uint16 a,uint8 c);
void FAST2420_READ_RAM(TiSpiAdapter * spi,uint8 *p, uint16 a, uint8 c);
void cc2420_reset( TiSpiAdapter * spi );

// @TODO 200703 for huanghuan
// 如何在两个SPI之间实现代码的Port，请在代码中给与说明
// 以下函数应增加一个参数 TiSpiAdapter * spi 或者是 uint8 id
// 尽管以下函数中没有用到spi, 但是可以用spi->id
// 不要用macro区分，用spi->id或者id区分
//
void CC2420_SPI_ENABLE( void );
void CC2420_SPI_DISABLE( void );

/******************************************************************************
 * general utility functions for TCc2420
 * these functions are a lot of them are based on TSpi.
 * only some of them are hardware related.
 *****************************************************************************/

void SET_RESET_INACTIVE( void ); // // The CC2420 reset pin
void SET_RESET_ACTIVE( void );
void SET_VREG_ACTIVE( void ); // CC2420 voltage regulator enable pin
void SET_VREG_INACTIVE( void );
BOOL VALUE_OF_FIFO( void );
BOOL VALUE_OF_CCA( void );
BOOL VALUE_OF_FIFOP( void );
BOOL VALUE_OF_SFD( void );
// BOOL VALUE_OF_RESET( void );    // not used in this file
// BOOL VALUE_OF_VREG(void);     // not used in this file


#endif
