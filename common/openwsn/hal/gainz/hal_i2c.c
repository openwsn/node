/*
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.07.10
 * - revision
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.09.10
 * - tested ok.
 */
#include "../hal_configall.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "../hal_foundation.h"
#include "../hal_i2c.h"
#include "../hal_cpu.h"
#include "../hal_assert.h"
#include "hpl_atmega128.h"
#include "hpl_atmega128_pin.h"

// I2C 状态定义
// MT 主方式传输 MR 主方式接受
#define START    0x08
#define RE_START   0x10
#define MT_SLA_ACK   0x18
#define MT_SLA_NOACK 0x20
#define MT_DATA_ACK   0x28
#define MT_DATA_NOACK 0x30
#define MR_SLA_ACK   0x40
#define MR_SLA_NOACK 0x48
#define MR_DATA_ACK   0x50
#define MR_DATA_NOACK 0x58  

#define _i2c_delayms(ms) cpu_delay(ms)


//常用TWI操作(主模式写和读)
#define Start()  (TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN))   //启动I2C
#define Stop()   (TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN))   //停止I2C
#define Wait()    {while(!(TWCR&(1<<TWINT)));}     //等待中断发生
#define TestAck()   (TWSR&0xF8)          //观察返回状态
#define SetAck()    (TWCR|=(1<<TWEA))        //做出ACK应答
#define SetNoAck()  (TWCR&=~(1<<TWEA))        //做出Not Ack应答


//#define SET_READ_CMD(value) ((value) |= 0x01)
//#define SET_WRITE_CMD(value) ((value) &= 0xFE)


/**
 * open TiI2cAdapter for putchar/getchar
 */
TiI2cAdapter * i2c_open( TiI2cAdapter * i2c, uint8 id, uint8 rate )
{  
    i2c->id = id;
    // i2c->rate = rate;

    switch (i2c->id)
    {
    case 0:
        TWBR = rate;     // 设置比特率，根据SCL频率公式设定，主机模式下不小于10//
        TWDR = 0xFF;     // Default content = SDA released//
        TWCR = 0x04;     // TWEN置位，使能TWI接口；Disable Interupt and No Signal requests//
        break;
    case 1:
        // not supported now
        hal_assert( false );
        break;
    default:
        i2c = NULL;
        break;
    }
    return i2c;
}    

void  i2c_close( TiI2cAdapter * i2c )
{
    return;
}


/**
 * prepare to start i2c transmission
 */
int8 i2c_start( TiI2cAdapter * i2c )
{
    // TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    Start();       //I2C启动
    Wait();
    if(TestAck()!=START)
    {
        return -1;              //-1代表启动失败
    }
	return 0;
}

/**
 * stop i2c transmission
 */
void i2c_stop( TiI2cAdapter * i2c )
{
    // TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
  	Stop();                     //I2C停止
}

uint8 i2c_state( TiI2cAdapter * i2c )
{
    // Q: is it right?
    return TWCR;
}

int8 i2c_putchar_write_addr( TiI2cAdapter * i2c, uint8 addr )
{
    TWDR = addr;               //写I2C从器件地址和写方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        return -2;              //-2代表地址写失败
    }
	return 0;
}


// wait for interrupt occuring
// Q: is the orginal comment right? --- zhangwei
void i2c_wait( TiI2cAdapter * i2c )
{
    while(!(TWCR&(1<<TWINT)))
    {
    }
}

//观察返回状态
bool i2c_testack( TiI2cAdapter * i2c )
{
    return (TWSR&0xF8);
}

//做出ACK应答
// formerly known as SetAck()
void i2c_replyack( TiI2cAdapter * i2c )
{
    TWCR|=(1<<TWEA);
}

//做出Not Ack应答
// formerly known as SetNoAck
void i2c_replynack( TiI2cAdapter * i2c )
{
    TWCR&=~(1<<TWEA);
}

/**
 * Put a char into the local i2c hardware for transmission. Before you call this 
 * function, you should call i2c_start() first.
 *
 * @return
 *  >= 0 success
 *  < 0 failed
 */
//可以是数据也可以是从器件内部寄存器地址，通用
int8 _i2c_putchar( TiI2cAdapter * i2c, uint8 ch )
{
    TWDR = ch;                  //写数据到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        return -3;              //-3代表数据写失败
    }
	return 0;   
}

int8 i2c_restart( TiI2cAdapter * i2c )
{
    Start();                     
    Wait();
    if (TestAck()!=RE_START) 
    {
        return -4;              //-4代表重新启动失败
    }
	return 0;   
}

int8 i2c_putchar_read_addr( TiI2cAdapter * i2c, uint8 addr )
{
    TWDR = addr;               //写I2C从器件地址和读方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        return -5;              //-5代表地址读失败
    }
	return 0;
}

/**
 * get a char into the local i2c hardware for transmission. before you call this 
 * function, you should call i2c_start() first. 
 * 
 * @return
 *  >= 0 success
 *  < 0 failed
 */
int8 _i2c_getchar( TiI2cAdapter * i2c, uint8 * ch )
{
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);    //启动主I2C读方式
    Wait();
    if(TestAck()!=MR_DATA_ACK) 
    {
        return -7;              //-7代表ACK应答错误
    }
    * ch = TWDR;                //读取I2C接收数据
	return 0;
}

int8 i2c_getchar_last_byte( TiI2cAdapter * i2c, uint8 * ch )
{
    TWCR = (1<<TWINT) | (1<<TWEN);  //启动主I2C读方式
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        return -6;              //-6代表NACK应答错误
    }
    * ch = TWDR;                //读取I2C接收数据
	return 0;
}


/**
 * @param
 *  dev_id: device id
 */
int8 i2c_connect( TiI2cAdapter * i2c, uint8 dev_id )
{
    i2c_start( i2c );
    _i2c_putchar( i2c, dev_id );
    i2c->devid = dev_id;
    return 0;
}

void i2c_disconnect( TiI2cAdapter * i2c )
{
    i2c_stop( i2c );
}

/**
 * transmit a char to the target device
 */
int8 i2c_putchar( TiI2cAdapter * i2c, uint8 ch )
{
    // current operation will be transmitted to device i2c->device
    return _i2c_putchar( i2c, ch );
}


/**
 * get a char from the target device
 */
int8 i2c_getchar( TiI2cAdapter * i2c, uint8 * ch )
{
    return _i2c_getchar( i2c, ch );
}

/**
 * transmit a data block to to the target device
 */
int8 i2c_write( TiI2cAdapter * i2c, char * buf, uint8 len )
{
    return len;
}

int8 i2c_read( TiI2cAdapter * i2c, char * buf, uint8 size )
{
    return 0;
}












//i2c写一个字节
int8 i2c_putchar_old( TiI2cAdapter * i2c,unsigned char regaddr, unsigned char ch, unsigned char sla_w )
{
    int temp = 0;
    Start();       //I2C启动
    Wait();
    if(TestAck()!=START)
    {
        temp = -1;              //-1代表启动失败
        return temp;
    }
    
    TWDR = sla_w;               //写I2C从器件地址和写方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2代表地址写失败
        return temp;
    }

    TWDR = regaddr;             //写从设备内部寄存器地址到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3代表数据写失败
        return temp;
    }

    TWDR = ch;                  //写数据到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3代表数据写失败
        return temp;
    }   
    Stop();                     //I2C停止
    temp++;                     //成功写一个字节，temp+1
    return temp;
    _i2c_delayms(100);            //延时     
}



//i2c读一个字节
int8 i2c_getchar_old( TiI2cAdapter * i2c,unsigned char regaddr, char * pc, unsigned char sla_w, unsigned char sla_r )
{
    int temp = 0;
    Start();
    Wait();
    if (TestAck()!=START) 
    {
        temp = -1;               //-1代表启动失败
        return temp; 
    }

    TWDR = sla_w;               //写I2C从器件地址和写方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2代表地址写失败
        return temp;
    }

    TWDR = regaddr;             //写从设备内部寄存器地址到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3代表数据写失败
        return temp;
    }

    Start();                     //I2C重新启动
    Wait();
    if (TestAck()!=RE_START) 
    {
        temp = -4;              //-4代表重新启动失败
        return temp;
    }   

    TWDR = sla_r;               //写I2C从器件地址和读方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        temp = -5;              //-5代表地址读失败
        return temp;
    }

    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);        //启动主I2C读方式
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        temp = -6;              //-6代表NACK应答错误
        return temp;
    }

    * pc = TWDR;                //读取I2C接收数据
    temp++;                     //成功读一个字节，temp+1
    Stop();                     //I2C停止
    /*return temp;*/
	return TWDR;
}

/*
//i2c写多个字节
unsigned char i2c_write( TiI2cAdapter * i2c,unsigned char regaddr, char *pc, uint8 len, unsigned char sla_w )
{
    int temp = 0;
    Start();       //I2C启动
    Wait();
    if(TestAck()!=START)
    {
        temp = -1;              //-1代表启动失败
        return temp;
    }
    
    TWDR = sla_w;               //写I2C从器件地址和写方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2代表地址写失败
        return temp;
    }

    TWDR = regaddr;             //写从设备内部寄存器地址到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3代表数据写失败
        return temp;
    }


    //写数据
    for (;len>1;len--,pc++)
    {
        TWDR = * pc;                  //将内存中的数据写到TWDR中
        TWCR=(1<<TWINT)|(1<<TWEN);
        Wait();
        if(TestAck()!=MT_DATA_ACK)
        {
            temp = -3;                //-3代表数据写失败
            return temp;
        }   
        temp++;                       //成功写一个数据，temp值+1
    }    

    Stop();                     //I2C停止
    _i2c_delayms(10000);            //延时   
    return temp;                //temp返回值能观察到成功写了多少数据  
}

//i2c读多个字节
unsigned char i2c_read(TiI2cAdapter * i2c,unsigned char regaddr, char * pc,uint8 size, unsigned char sla_w, unsigned char sla_r)
{
    int temp = 0;
    Start();                     //I2C启动
    Wait();
    if (TestAck()!=START) 
    {
        temp = -1;               //-1代表启动失败
        return temp; 
    }

    TWDR = sla_w;               //写I2C从器件地址和写方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2代表地址写失败
        return temp;
    }

    TWDR = regaddr;             //写从设备内部寄存器地址到TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3代表数据写失败
        return temp;
    }

    Start();                     //I2C重新启动
    Wait();
    if (TestAck()!=RE_START) 
    {
        temp = -4;              //-4代表重新启动失败
        return temp;
    }   

    TWDR = sla_r;               //写I2C从器件地址和读方式
    TWCR=(1<<TWINT)|(1<<TWEN);  //写入值时，TWINT位要置位
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        temp = -5;              //-5代表地址读失败
        return temp;
    }

    //读数据，发送ACK
    for (;size>1;size--,pc++)
    {
        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);    //启动主I2C读方式
        Wait();
        if(TestAck()!=MR_DATA_ACK) 
        {
            temp = -7;              //-7代表ACK应答错误
            return temp;
        }
        * pc = TWDR;                //读取I2C接收数据 
        temp++;                     //成功读取一个数据，temp+1
    }
    
    //读最后一个数据，发送NACK
    TWCR = (1<<TWINT) | (1<<TWEN);  //启动主I2C读方式
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        temp = -6;              //-6代表NACK应答错误
        return temp;
    }

    * pc = TWDR;                //读取I2C接收数据
    temp++;
    Stop();                     //I2C停止
    return temp;                //temp返回值能观察到成功读取了多少数据
}
*/



























