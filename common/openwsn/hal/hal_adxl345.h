#ifndef _HAL_ADXL345_H_5632_
#define _HAL_ADXL345_H_5632_

/*
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.07.10
 * - revision
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.09.10
 * - tested ok.
 */
 

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
void adxl345_configure( TiAdxl345Adapter * adxl );

/**
 * Read the value from a 8bit register. The i2c adapter object should have already
 * opened before calling this function.
 *
 * @attention: Before calling this function, you should have already call i2c_open()
 * because this function will try to connect target device during the open process.
 */
int8 adxl345_readregister8( TiAdxl345Adapter * adxl, uint8 regaddr, uint8 * value );

int8 adxl345_writeregister8( TiAdxl345Adapter * adxl, uint8 regaddr, uint8 value );

int8 adxl345_readregister16( TiAdxl345Adapter * adxl, uint8 regaddr, uint16 * value );

int8 adxl345_writeregister16( TiAdxl345Adapter * adxl, uint8 regaddr, uint16 value );

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
