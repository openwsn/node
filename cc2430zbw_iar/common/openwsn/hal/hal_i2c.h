#ifndef _HAL_I2C_H_6784_
#define _HAL_I2C_H_6784_
/*
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.07.10
 * - revision
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.09.10
 * - tested ok.
 */

/* 
 * reference
 * [1] I2C specification
 * [2] search i2c through hackchina.com
 * [3] at24c04.c (at24c04 e2prom and stm32), http://www.hackchina.com/r/86363/at24c04.c__html, 
 *     and http://www.hackchina.com/r/86363/main.c__html;
 * [4] simple explanation about I2C, http://www.360doc.com/content/09/0428/15/72158_3300928.shtml;
 */
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"

typedef struct{
    uint8 id;
    uint8 devid;
}TiI2cAdapter;

#ifdef __cplusplus
extern "C"{
#endif

// 此处rate为TWBR的值，具体频率要求看scl公式
TiI2cAdapter * i2c_open( TiI2cAdapter * i2c, uint8 id, uint8 rate );
void i2c_close( TiI2cAdapter * i2c );
int8 i2c_start( TiI2cAdapter * i2c );
void i2c_stop( TiI2cAdapter * i2c );
#define i2c_writeselect(i2c,addr) i2c_putchar_write_addr(i2c,addr)
int8 i2c_putchar_write_addr( TiI2cAdapter * i2c, uint8 addr );
int8 i2c_putchar( TiI2cAdapter * i2c, uint8 ch );
int8 i2c_restart( TiI2cAdapter * i2c );
#define i2c_readselect(i2c,addr) i2c_putchar_read_addr(i2c,addr)
int8 i2c_putchar_read_addr( TiI2cAdapter * i2c, uint8 addr );
int8 i2c_getchar( TiI2cAdapter * i2c, uint8 * ch );
int8 i2c_getchar_last_byte( TiI2cAdapter * i2c, uint8 * ch );




/*
void i2c_start( TiI2cAdapter * i2c );
void i2c_stop( TiI2cAdapter * i2c );
uint8 i2c_state( TiI2cAdapter * i2c );
void i2c_wait( TiI2cAdapter * i2c );
bool i2c_testack( TiI2cAdapter * i2c );
void i2c_replyack( TiI2cAdapter * i2c );
void i2c_replynack( TiI2cAdapter * i2c );

int8 i2c_connect( TiI2cAdapter * i2c, uint8 dev_id );
void i2c_disconnect( TiI2cAdapter * i2c );

int8 i2c_putchar( TiI2cAdapter * i2c, uint8 ch );
int8 i2c_getchar( TiI2cAdapter * i2c, uint8 * ch );
int8 i2c_write( TiI2cAdapter * i2c, char * buf, uint8 len );
int8 i2c_read( TiI2cAdapter * i2c, char * buf, uint8 size );
*/

//i2c写一个字节
int8 i2c_putchar_old( TiI2cAdapter * i2c,unsigned char regaddr, unsigned char ch, unsigned char sla_w );

//i2c读一个字节
int8 i2c_getchar_old( TiI2cAdapter * i2c,unsigned char regaddr, char * pc, unsigned char sla_w, unsigned char sla_r );

/*
//i2c写多个字节
unsigned char i2c_write( TiI2cAdapter * i2c,unsigned char regaddr, char *pc, uint8 len, unsigned char sla_w );

//i2c读多个字节
unsigned char i2c_read(TiI2cAdapter * i2c,unsigned char regaddr, char * pc,uint8 size, unsigned char sla_w, unsigned char sla_r);
*/

#ifdef __cplusplus
};
#endif


#endif /* _HAL_I2C_H_6784_ */


