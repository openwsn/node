


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


#define PANID				0x0002
#define LOCAL_ADDRESS		0x03
#define REMOTE_ADDRESS		0x04
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11
#define MAX_IEEE802FRAME154_SIZE                128

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter             m_cc;
void recvnode1(void);


int main(void)
{
   recvnode1();
   
}

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
	cc2520_disable_addrdecode( cc );					//禁止地址译码
	#ifdef TEST_ACK
	cc2420_enable_autoack(cc);
	#endif

	cc2520_setpanid( cc, PANID );					//网络标识
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    
	while(1) 
	{
		frame_reset( rxbuf,0,0,0);
		
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

