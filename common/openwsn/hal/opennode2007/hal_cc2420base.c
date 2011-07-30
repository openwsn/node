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

#include "hal_foundation.h"
#include "hal_target.h"
#include "hal_spi.h"
#include "hal_cc2420def.h"
#include "hal_cc2420.h"
#include "hal_cc2420base.h"
#include "hal_led.h"

#ifdef CONFIG_TARGET_DEFAULT
#undef CONFIG_TARGET_DEFAULT
#endif

#if ((!defined(CONFIG_TARGET_OPENNODE_10)) && (!defined(CONFIG_TARGET_OPENNODE_20)) \
	&& (!defined(CONFIG_TARGET_WLSMODEM_11)))
#define CONFIG_TARGET_DEFAULT
#endif

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * TiCc2420Adapter
 * this file is part of the TiCc2420Adapter
 * this function contains the CC2420 manipulation functions.
 * 大部分TiCc2420Adapter代码都应该放在该文件中，hal_cc2420.* 中主要实现接口和配置逻辑。
 *
 * @history
 * @modified by zhangwei on 2006-07-24
 * 	zhangwei combine the old "basic_rf_init.c", "basic_rf_receive.c" and
 * "basic_rf_send_packet.c" provided by Huanghuan into this unique file.
 *
 *****************************************************************************/


void radio_init( TiSpiAdapter * spi )
{
}

void radio_reset( TiSpiAdapter * spi )
{
}

void radio_settxpower( TiSpiAdapter * spi )
{
}

void radio_setchannel( TiSpiAdapter * spi )
{
}

void radio_get_lqi( TiSpiAdapter * spi, int8 rssidbm, uint8 cor )
{
}

void radio_set_panid( TiSpiAdapter * spi, uint16 panid )
{
}

void radio_set_shortaddr( TiSpiAdapter * spi, uint16 shortaddr )
{
}

// Set the IEEE address on the radio.
// IEEEAddr - pointer to array holding 64 bit IEEE address; array must be little
//                          endian format (starts with lowest signficant byte)
void radio_set_ieeeaddr( TiSpiAdapter * spi, uint8 * addr )
{
}


/******************************************************************************
 * some utility functions
 *****************************************************************************/

#define FAST2420_RX_GARBAGE(spi,pc) spi_get((spi), (pc))
#define FAST2420_TX_ADDR(spi,a) spi_put((spi),(a))
#define FAST2420_RX_ADDR(spi,a) spi_put((spi),(a)|0x40)

//#define cc2420_spi_enable(spi) spi_open(spi)
//#define cc2420_spi_disable(spi) spi_close(spi)

/******************************************************************************
 * FAST SPI: Register access
 * 		s = command strobe
 * 		a = register address
 * 		v = register value
 *****************************************************************************/

// @modified by zhangwei on 20070701
// to support return  value
//
uint8 FAST2420_STROBE(TiSpiAdapter * spi,uint8 s)
	{
		uint8 status;
        CC2420_SPI_ENABLE();
		//spi_open( spi, 0 );
        status = FAST2420_TX_ADDR(spi,s);
		//spi_close( spi );
        CC2420_SPI_DISABLE();
		return status;
    }

void FAST2420_SETREG(TiSpiAdapter * spi,uint8 a,uint16 v)
    {

        CC2420_SPI_ENABLE();
		//led_on( LED_YELLOW ); // debug only
        FAST2420_TX_ADDR(spi,a);
        FASTSPI_TX_WORD(spi, v);
        CC2420_SPI_DISABLE();

    }

void FAST2420_GETREG(TiSpiAdapter * spi,uint8 a,uint16 *v)
    {
        CC2420_SPI_ENABLE();
        FAST2420_RX_ADDR(spi,a);
        FASTSPI_RX_WORD(spi,v);
        CC2420_SPI_DISABLE();

    }

// Updates the SPI status byte
void FAST2420_UPD_STATUS(TiSpiAdapter * spi,uint8 *s)
    {
        CC2420_SPI_ENABLE();
        spi_get(spi,(char*)s);
        CC2420_SPI_DISABLE();
    }

//-------------------------------------------------------------------------------------------------------
//  FAST SPI: FIFO access
//      p = pointer to the byte array to be read/written
//      c = the number of bytes to read/write
//      b = single data byte

void FAST2420_WRITE_FIFO(TiSpiAdapter * spi,uint8 *p,uint8 c)
    {
        uint8 n = 0;
        CC2420_SPI_ENABLE();
        FAST2420_TX_ADDR(spi,CC2420_TXFIFO);
        for (n = 0; n < (c); n++) {
            spi_put(spi,((BYTE*)(p))[n]);
        }
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_FIFO(TiSpiAdapter * spi,uint8 *p,uint8 c)
    {
        uint8 n = 0;

        CC2420_SPI_ENABLE();
        FAST2420_RX_ADDR(spi,CC2420_RXFIFO);
        for (n = 0; n < (c); n++) {
            while (!VALUE_OF_FIFO());
            spi_get(spi,(char*)(p + n));
        }
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_FIFO_BYTE(TiSpiAdapter * spi,uint8 *b)
   {
        CC2420_SPI_ENABLE();
        FAST2420_RX_ADDR(spi,CC2420_RXFIFO);
        spi_get(spi,(char*)b);
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_FIFO_NO_WAIT(TiSpiAdapter * spi,uint8 *p, uint8 c)
    {
        uint8 n = 0;
        CC2420_SPI_ENABLE();
        FAST2420_RX_ADDR(spi,CC2420_RXFIFO);
        for (n = 0; n < (c); n++) {
            spi_get(spi,(char*)(p + n));
        }
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_FIFO_GARBAGE(TiSpiAdapter * spi,uint8 c)
    {
        uint8 n = 0;
        char  value;
        CC2420_SPI_ENABLE();
        FAST2420_RX_ADDR(spi,CC2420_RXFIFO);
        for (n = 0; n < (c); n++) {
            FAST2420_RX_GARBAGE(spi, &value);
        }
        CC2420_SPI_DISABLE();
    }

//-------------------------------------------------------------------------------------------------------
//  FAST SPI: CC2420 RAM access (big or little-endian order)
//      p = pointer to the variable to be written
//      a = the CC2420 RAM address
//      c = the number of bytes to write
//      n = counter variable which is used in for/while loops (uint8)
//
//  Example of usage:
//      uint8 n;
//      UINT16 shortAddress = 0xBEEF;
//      FASTiSpiAdapter_WRITE_RAM_LE(&shortAddress, CC2420RAM_SHORTADDR, 2);

void FAST2420_WRITE_RAM_LE(TiSpiAdapter * spi,uint16 *p,uint16 a,uint8 c)
    {
    	uint8 n;
        CC2420_SPI_ENABLE();
        spi_put(spi,0x80 | (a & 0x7F));
        spi_put(spi,(a >> 1) & 0xC0);
        for (n = 0; n < (c); n++) {
            spi_put(spi,((BYTE*)(p))[n]);
        }
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_RAM_LE(TiSpiAdapter * spi,uint8 *p,uint16 a,uint8 c)
   {
   	uint8 n;
        CC2420_SPI_ENABLE();
        spi_put(spi,0x80 | (a & 0x7F));
        spi_put(spi,((a >> 1) & 0xC0) | 0x20);
        for (n = 0; n < (c); n++) {
            spi_get(spi,(char*)(p + n));
        }
        CC2420_SPI_DISABLE();
    }

void FAST2420_WRITE_RAM(TiSpiAdapter * spi,uint16 *p,uint16 a,uint8 c)
   {
   	uint8 n;
        CC2420_SPI_ENABLE();
        spi_put(spi,0x80 | (a & 0x7F));
        spi_put(spi,(a >> 1) & 0xC0);
        n = c;
        do {
            spi_put(spi,((BYTE*)(p))[--n]);
        } while (n);
        CC2420_SPI_DISABLE();
    }

void FAST2420_READ_RAM(TiSpiAdapter * spi,uint8 *p,uint16 a,uint8 c)
    {
    	uint8 n;
        CC2420_SPI_ENABLE();
        spi_put(spi,0x80 | (a & 0x7F));
        spi_put(spi,((a >> 1) & 0xC0) | 0x20);
        n = c;
        do {
            spi_get(spi,(char*)(p + (--n)));
        } while (n);
        CC2420_SPI_DISABLE();
    }

//-------------------------------------------------------------------------------------------------------
// Other useful SPI macros
void cc2420_reset( TiSpiAdapter * spi )
    {
        FAST2420_SETREG( spi, CC2420_MAIN, 0x0000);
        FAST2420_SETREG( spi, CC2420_MAIN, 0xF800);
    }

void CC2420_SPI_ENABLE( void )
  {
        uint16 i = 0;

        #if  CSN_PORT == 0
        IO0CLR  = BM(CSN);
        #endif
        #if CSN_PORT == 1
        IO1CLR = BM(CSN);
        #endif

        // @modified by huanghuan 2006.11.16
        // the delay is to construct enough setup time of csn
        // @TODO replace with hal_delay. you should not count on while for this delay
		#pragma optimize=none
        while(i < 500)
        	i++;

  }

void CC2420_SPI_DISABLE( void )
  {
        uint16 i = 0;

        while(i < 1500) i++;   //the delay is to construct enough holdup time of csn
                               //by huanghuan 2006.11.16
        #if CSN_PORT == 0
        IO0SET  = BM(CSN);
        #endif
        #if CSN_PORT == 1
        IO1SET = BM(CSN);
        #endif

  }

// The CC2420 reset pin
void SET_RESET_INACTIVE( void )
  {
        //PINSEL0 = 0x00000000;
        //IO0DIR  = IO1DIR | BM(RESET_N);
        #if  RESET_N_PORT == 0
        IO0SET  = BM(RESET_N);
        #endif

        #if  RESET_N_PORT == 1
        IO1SET  = BM(RESET_N);
        #endif
     }

void SET_RESET_ACTIVE( void )
  {
        #if  RESET_N_PORT == 0
        IO0CLR  = BM(RESET_N);
        #endif

        #if  RESET_N_PORT == 1
        IO1CLR  = BM(RESET_N);
        #endif
     }


// CC2420 voltage regulator enable pin
void SET_VREG_ACTIVE( void )
   {
        //PINSEL0 = 0x00000000;
        //IO0DIR  = IO0DIR | BM(VREG_EN);
        #if VREG_EN_PORT == 0
        IO0SET  = BM(VREG_EN);
        #endif

        #if VREG_EN_PORT == 1
        IO1SET  = BM(VREG_EN);
        #endif
     }

void SET_VREG_INACTIVE( void )
  {
        #if VREG_EN_PORT == 0
        IO0CLR  = BM(VREG_EN);
        #endif

        #if VREG_EN_PORT == 1
        IO1CLR  = BM(VREG_EN);
        #endif
     }


BOOL VALUE_OF_FIFO( void )
    {
    	BOOL  result;
        //PINSEL0 = 0x00000000;
        //IO0DIR  = IO0DIR & (~BM(FIFO));
        #if FIFO_PORT == 0
        if(IO0PIN & BM(FIFO)) result = 1;
        else                  result = 0;
        #endif

        #if FIFO_PORT == 1
        if(IO1PIN & BM(FIFO)) result = 1;
        else                  result = 0;
        #endif

        return(result);
     }


BOOL VALUE_OF_CCA( void )
    {
    	BOOL  result;
        //PINSEL0 = 0x00000000;
        //IO0DIR  = IO0DIR & (~BM(CCA));
        #if CCA_PORT == 0
        if(IO0PIN & BM(CCA))  result = 1;
        else                  result = 0;
        #endif

        #if CCA_PORT == 1
        if(IO1PIN & BM(CCA))  result = 1;
        else                  result = 0;
        #endif

        return(result);
     }

BOOL VALUE_OF_FIFOP( void )
    {
    	BOOL  result;
        //PINSEL0 = 0x00000000;
        //IO0DIR  = IO0DIR & (~BM(FIFOP));
        #if FIFOP_PORT == 0
        if(IO0PIN & BM(FIFOP)) result = 1;
        else                  result = 0;
        #endif

        #if FIFOP_PORT == 1
        if(IO1PIN & BM(FIFOP)) result = 1;
        else                  result = 0;
        #endif

        return(result);
     }

BOOL VALUE_OF_SFD( void )
    {
    	BOOL  result;
        //PINSEL1 = 0x00000000;
        //IO0DIR  = IO0DIR & (~BM(SFD));
        #if SFD_PORT == 0
        if (IO0PIN & BM(SFD)) result = 1;
        else                  result = 0;
        #endif

        #if SFD_PORT == 1
        if (IO1PIN & BM(SFD)) result = 1;
        else                  result = 0;
        #endif
        return(result);
     }

