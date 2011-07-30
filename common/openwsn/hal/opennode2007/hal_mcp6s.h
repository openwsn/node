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
#ifndef _HAL_MCP6S_H_3467_
#define _HAL_MCP6S_H_3467_

/******************************************************************************
 * @author huanghuan on 200612
 * @note
 * 	MCP Driver for Microchip MCP6S26
 * @history
 * @modified by zhangwei on 20070101
 * 	revise source code from MaKun
 * 	adjust the interface
 *****************************************************************************/

#include "hal_foundation.h"
#include "hal_configall.h"
#include "hal_spi.h"


#define MCP6S26_SPI_CS_PIN 24
#define MCP6S26_SPI_CS_PORT 1

typedef struct{
  uint8 id;
  TiSpiAdapter * spi;
}TiMcp6s26Adapter;

TiMcp6s26Adapter * mcp_construct(  uint8 id, char * buf, uint8 size, TiSpiAdapter * spi  );
void mcp_destroy( TiMcp6s26Adapter * mcp );

void mcp6s26_spi_enable(void);
void mcp6s26_spi_disable(void);
void mcp6s26_setchannel(TiMcp6s26Adapter * mcp,uint8 channel);
void mcp6s26_setgains( TiMcp6s26Adapter * mcp, uint8 ratio );

#endif
