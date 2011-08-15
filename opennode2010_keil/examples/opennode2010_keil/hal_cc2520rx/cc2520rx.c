

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"

#define CONFIG_LISTENER    
#undef  CONFIG_LISTENER    

#define TEST_CHOICE 1
//#define TEST_ACK
//#undef  TEST_ACK


#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11
#define MAX_IEEE802FRAME154_SIZE                128

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter             m_cc;
void recvnode1(void);


/*
#if (TEST_CHOICE == 1)
static void recvnode1(void);
#endif

#if (TEST_CHOICE == 2)
static void recvnode2(void);
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif
*/

int main(void)
{
   recvnode1();
   /*
    #if (TEST_CHOICE == 1)
	recvnode1();
    #endif

    #if (TEST_CHOICE == 2)
	recvnode2();
    #endif
	*/
}

//#if (TEST_CHOICE == 1)
void recvnode1(void)
{
    TiCc2520Adapter * cc;
	TiFrame * rxbuf;
	uint8 len;
    uint8 i;
    char *pc;
	led_open();
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );
	halUartInit(9600,0);
	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );

	cc2520_open( cc, 0, NULL, NULL, 0x00 );
	
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );							//Enable RX
	cc2520_enable_addrdecode( cc );					//使能地址译

	cc2520_setpanid( cc, PANID );					//网络标识
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	// when use this scan mode to receive data, interrupt should be disable; otherwise the data will be
	// read twice and in the second time there are no data actually which leads to a assert.
 	// Attention: in this scan mode, MCU always try to read and in my  test it is faster than the transmission of data. 
	// Thus, after 4 times, there no data at all, and the MCU still want to read, which lead to an assert. So we'd better
	// not use this scan mode.
    
	while(1) 
	{
		frame_reset( rxbuf,0,0,0);
		
		cc2520_evolve( cc );

		len = cc2520_read( cc, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00 );
        if ( len)
        {
			frame_setlength( rxbuf,len);
            pc = frame_startptr(rxbuf);
            for ( i=0;i<len;i++)
            {
                USART_Send( pc[i]);
            }
			led_toggle( LED_RED);
        }
	}
}
