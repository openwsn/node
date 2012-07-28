/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 
#ifndef _HAL_ADXL345_H_5632_
#define _HAL_ADXL345_H_5632_

/*******************************************************************************
 * @modified by Lu Weihui (luweihui163@163.com), He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.07.10
 * - revision
 * @modified by Lu Weihui (luweihui163@163.com), He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.09.10
 * - tested ok.
 * @modified by openwsn in 2011.08
 * - Revision
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_i2c.h"

/** 
 * CONFIG_ADXL345_I2C
 * CONFIG_ADXL345_SPI
 * Configure whether using I2C or SPI to communicate with ADXL345 sensor 
 */
#undef  CONFIG_ADXL345_I2C
#define CONFIG_ADXL345_I2C

#define CONFIG_ADXL345_SPI
#undef  CONFIG_ADXL345_SPI

#define CONFIG_ADXL345_DEVICE_ADDRESS 0x53

typedef union
{
   unsigned char data[6];
   struct
   {
      int x;      // little endian will swap axis_data[0 & 1]
      int y;      // little endian will swap axis_data[2 & 3]
      int z;      // little endian will swap axis_data[4 & 5]
   } xyz;
}TiAdxl345XyzValue;

typedef struct{
    uint8 id;
    TiI2cAdapter * i2c;
    //TiAdxl345XyzValue value;
}TiAdxl345Adapter;


#ifdef __cplusplus
extern "C"{
#endif

TiAdxl345Adapter * adxl345_open( TiAdxl345Adapter * adxl, uint8 id, TiI2cAdapter * i2c );
void adxl345_close( TiAdxl345Adapter * adxl );

/**
 * Reconfigure the ADXL345 sensor. Attention that adxl345_open() will give default
 * configurations.
 * 
 * @warning The prototype of this function will be changed in the futre.
 */
void adxl345_configure( TiAdxl345Adapter * adxl );

/**
 * Read the value from a 8bit register. The i2c adapter object should have already
 * opened before calling this function.
 *
 * @attention: Before calling this function, you should have already call i2c_open()
 * because this function will try to connect target device during the open process.
 */
uint8 adxl345_readregister8( TiAdxl345Adapter * adxl, uint8 regaddr );

void adxl345_writeregister8( TiAdxl345Adapter * adxl, uint8 regaddr, uint8 value );

uint16 adxl345_readregister16( TiAdxl345Adapter * adxl, uint8 regaddr );

void adxl345_writeregister16( TiAdxl345Adapter * adxl, uint8 regaddr, uint16 value );

/**
 * Read the register space of target ADXL345 component sequentially and put the 
 * results into buf. 
 * 
 * Assume the buffer is large enough to hold all returned values. It's the caller's
 * responsiblity to maintain the buffer.
 * 
 * @return How many bytes in the buffer. If error occurs, then negative value returned.
 */
int8 adxl345_readregisterspace( TiAdxl345Adapter * adxl, uint8 regstart, uint8 regcount, uint8 * buf );

int8 adxl345_writeregisterspace( TiAdxl345Adapter * adxl, uint8 regstart, uint8 regcount, uint8 * buf );

int8 adxl345_read( TiAdxl345Adapter * adxl, char * buf, uint8 size, uint8 option );

int8 adxl345_readvalue( TiAdxl345Adapter * adxl, TiAdxl345XyzValue * value, uint8 option );

#ifdef __cplusplus
}
#endif

#endif /* _HAL_ADXL345_H_5632_ */
