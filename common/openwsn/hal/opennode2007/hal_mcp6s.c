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
#include "hal_mcp6s.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_global.h"

/* @param
 * 	spi		point to an outside spi driver
 * 			assume the spi driver is OK
 */
TiMcp6s26Adapter * mcp_construct(  uint8 id, char * buf, uint8 size, TiSpiAdapter * spi )
{
	TiMcp6s26Adapter * mcp;
	char* out_string = "mcp consturct succesful!\n";

	if (sizeof(TiMcp6s26Adapter) > size)
		mcp = NULL;
	else
		mcp = (TiMcp6s26Adapter *)buf;


	if (mcp != NULL)
	{
		memset( (char*)mcp, 0x00, sizeof(TiMcp6s26Adapter) );
	        mcp->id = id;
	        mcp->spi = spi;
	        uart_write(g_uart, out_string,25, 0);
	}

	return mcp;
}

void mcp_destroy( TiMcp6s26Adapter * mcp )
{
}


void mcp6s26_spi_enable()
{
    uint16 i = 0;

    #if MCP6S26_SPI_CS_PORT == 0
    IO0CLR  = BM(MCP6S26_SPI_CS_PIN);
    #endif

    #if MCP6S26_SPI_CS_PORT == 1
    IO1CLR  = BM(MCP6S26_SPI_CS_PIN);
    #endif

    while(i < 500) i++;
}

void mcp6s26_spi_disable()
{
    uint16 i = 0;

    while(i < 1500) i++;

    #if MCP6S26_SPI_CS_PORT == 0
    IO0SET  = BM(MCP6S26_SPI_CS_PIN);
    #endif

    #if MCP6S26_SPI_CS_PORT == 1
    IO1SET  = BM(MCP6S26_SPI_CS_PIN);
    #endif
}

void mcp6s26_setchannel(TiMcp6s26Adapter * mcp,uint8 channel)
{
    uint8 ch;
    if(channel <= 5 ) ch = channel;
    else              ch = 0;
    mcp6s26_spi_enable();
    spi_put(mcp->spi,0x41);
    spi_put(mcp->spi,ch);
    mcp6s26_spi_disable();
}

void mcp_setgains( TiMcp6s26Adapter * mcp, uint8 ratio )
{
     uint8 ch;
    if(ratio <= 7 ) ch = ratio;
    else            ch = 0;
    mcp6s26_spi_enable();
    spi_put(mcp->spi,0x40);
    spi_put(mcp->spi,ch);
    mcp6s26_spi_disable();
}
