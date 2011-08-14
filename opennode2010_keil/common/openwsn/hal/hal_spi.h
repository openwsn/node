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


#ifndef _SPIADAPTER_H_4289_
#define _SPIADAPTER_H_4289_

/*****************************************************************************
 * @author zhangwei on 2006-07-20
 * TiSpiAdapter
 *
 * @modified by zhangwei on 20061010
 * based on Huanghuan's mature code.
 * support multiple SPI channel.
 *
 * @modified by zhangwei on 20070509
 * add two function spi_open() and spi_close()
 * these two functions can be used to adjust detail timing when transmitting
 * multi-byte streams. when you want to transmit multi-byte stream, you must
 * call open() and close(). for single byte transmission, also recommend call
 * to open() and close().
 *
 * when you call open() a spi, the code will also select the device.
 ****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"

#define TSpi TiSpiAdapter

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  uint8 id;
}TiSpiAdapter;

TiSpiAdapter * spi_construct( char * buf, uint8 size );
void  spi_destroy( TiSpiAdapter * spi );
//void  spi_configure( TiSpiAdapter * spi );
TiSpiAdapter * spi_open( TiSpiAdapter * spi, uint8 id );
void  spi_close( TiSpiAdapter * spi );

uint8 spi_put( TiSpiAdapter * spi, char ch );
int8  spi_get( TiSpiAdapter * spi, char * pc );
uint8 spi_status( TiSpiAdapter * spi );
void  spi_wait( TiSpiAdapter * spi );
uint8 spi_read( TiSpiAdapter * spi, char * buf, uint8 capacity, uint8 opt );
uint8 spi_write( TiSpiAdapter * spi, char * buf, uint8 len, uint8 opt );


/* @attention
 * obsolete functions. pls use the above function instead
 */
void FASTSPI_TX_WORD_LE(TiSpiAdapter * spi, uint16 x) ;
void FASTSPI_TX_WORD(TiSpiAdapter * spi, uint16 x);
void FASTSPI_RX_WORD_LE(TiSpiAdapter * spi, uint16 *x);
void FASTSPI_RX_WORD(TiSpiAdapter * spi, uint16 *x);

#ifdef __cplusplus
}
#endif

#endif /*_SPI_H_*/
