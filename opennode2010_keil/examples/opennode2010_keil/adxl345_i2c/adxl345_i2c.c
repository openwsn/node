

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include <avr/io.h> 
#include <avr/iom128.h>
#include <stdio.h> 
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_i2c.h"
#include "../../common/openwsn/hal/hal_adxl345.h"

#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

//i2c频率设置为100khz；adxl345模块支持标准（100khz）和快速（400khz）数据传输模式
#define I2C_RATE   0x22      

static TiI2cAdapter m_i2c;
static TiAdxl345Adapter m_adxl;
static TiAdxl345XyzValue m_3dvalue[20];

int test1(void);
int test2(void);


void  getarray(int16 valx[],int16 n)
{
    TiI2cAdapter * i2c;
    TiAdxl345Adapter * adxl;
    char tempbuf[6];
	
	short int t1;
	t1=(short int)(1000*T);


    i2c = i2c_open( &m_i2c, 0 ,I2C_RATE);
    adxl = adxl345_open( &m_adxl, 0, i2c );

    memset( (char*)&valx[0], 0x00, sizeof(valx) );

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti;

	
	timeradapter = timer_construct( (void *)(&g_timeradapter), sizeof(g_timeradapter) );
	vtm = vtm_construct( (void*)&g_vtm, sizeof(g_vtm) );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );
	vti = vtm_apply( vtm );
	
	vti_open( vti, NULL, NULL );
	vti_setscale( vti, 1);
	vti_setinterval( vti, t1, 0x01 );
	vti_start( vti );
	hal_enable_interrupts();
	

	int i=0;
	
	int16 * p=(int16 *)tempbuf;//修改

	while (i<LENGTH)
	{
		if(vti_expired(vti))
		{
			vti_setscale( vti, 1);
	        vti_setinterval( vti, t1, 0x01 );
	        vti_start( vti );

            // todo: 2010.09.10 by zhangwei
            // why not adxl345_read( adxl, &valx[i], 6, 0x00 ) here and eliminate 
            // the tempbuf?
            //
			if (adxl345_read( adxl, &tempbuf[0], 6, 0x00) > 0)
        	{
                valx[i] = *p;//修改
				//valx[i] = (int16)(((uint16)tempbuf[0]) | (((uint16)(tempbuf[1]) << 8)));
            }
			i++;
		}
	}
   
	vti_close(vti );
	

    adxl345_close( adxl );
    i2c_close( i2c );

}

int main(void)
{
    test1();
    test2();
    return 0;
}

int test1(void)
{
    TiI2cAdapter * i2c;
    TiAdxl345Adapter * adxl;
    TiAdxl345XyzValue * value = &m_3dvalue[0];
    uint16 i;
	char * msg = "welcome to adxl345 demo...\r\n";

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_write( msg, strlen(msg) );

	i2c = i2c_open( &m_i2c, 0 ,I2C_RATE);
    adxl = adxl345_open( &m_adxl, 0, i2c );


    for (i=0; i<sizeof(m_3dvalue)/sizeof(TiAdxl345XyzValue); i++)
    {
        if (adxl345_readvalue( adxl, value, 0x00) > 0)
        {
            dbc_write( (char *)value, sizeof(TiAdxl345XyzValue) );
        }
        hal_delay( 1000 );
    }

    adxl345_close( adxl );
    i2c_close( i2c );
    // while (1) {};

	return 0;
}	 

int test2(void)
{
    const int BUF_CAPACITY = 256;
    TiI2cAdapter * i2c;
    TiAdxl345Adapter * adxl;
    uint16 i;
    char tempbuf[6];
    int16 valx[BUF_CAPACITY];
    int16 valy[BUF_CAPACITY];
    int16 valz[BUF_CAPACITY];

	char * msg = "welcome to adxl345 demo...\r\n";

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_write( msg, strlen(msg) );

	i2c = i2c_open( &m_i2c, 0 ,I2C_RATE);
    adxl = adxl345_open( &m_adxl, 0, i2c );

    memset( (char*)&valx[0], 0x00, sizeof(valx) );
    memset( (char*)&valy[0], 0x00, sizeof(valy) );
    memset( (char*)&valz[0], 0x00, sizeof(valz) );

    for (i=0; i<BUF_CAPACITY; i++)
    {
        if (adxl345_read( adxl, &tempbuf[0], 6, 0x00) > 0)
        {         
			valx[i] = (int16)((tempbuf[0]) | (uint16)(tempbuf[1] << 8));
            valy[i] = (int16)((tempbuf[2]) | (uint16)(tempbuf[3] << 8));
            valz[i] = (int16)((tempbuf[4]) | (uint16)(tempbuf[5] << 8));
        }
        hal_delay( 1000 );
    }
    adxl345_close( adxl );
    i2c_close( i2c );

    dbc_write( (char *)valx[0], sizeof(valx) );
    dbc_write( (char *)valy[0], sizeof(valy) );
    dbc_write( (char *)valz[0], sizeof(valz) );
    
    while (1) {};

	return 0;
}	 


