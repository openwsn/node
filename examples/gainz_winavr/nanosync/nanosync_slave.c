#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_rtc.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/svc/svc_nanosync.h"


#define CONFIG_PANID				        0x0001
#define CONFIG_LOCAL_ADDRESS		        0x01
#define CONFIG_REMOTE_ADDRESS		        0x02
#define CONFIG_CHANNEL                      11
#define OPTION                              0x01
#define MAX_IEEE802FRAME154_SIZE                128

TiIEEE802Frame154Descriptor                     m_meta;


static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;;
static TiNanoSync                               m_sync;
static TiUartAdapter		                    m_uart;
static TiRtcAdapter                             m_rtc;




int main()
{
	TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiNanoSync * sync;
	TiRtc   * rtc;
	char rxbuf[128];
	
	char * msg = "welcome to timesync test...";
	char * msgt = "just for test...";//测试用的
    int len=0;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );
	dbc_mem( msgt, strlen(msgt) );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
    sync = nanosync_construct((char *)(&m_sync), sizeof(TiNanoSync));
    rtc = rtc_construct((char *)(&m_rtc),sizeof(TiRtc));
    
    
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
    nanosync_open( sync,NANOSYNC_MASTER,rtc,rxtx, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, OPTION);
	rtc_open(rtc,NULL,NULL,0x01);
	rtc_setinterval(rtc,0,2,0x01);//定时间一秒
	
 
	cc2420_setchannel( cc, CONFIG_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );//这一句需不需要？
/*********************************************************
	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2420_enable_addrdecode( cc );
	#else	
	cc2420_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif

	//这一块需不需要？
*************************************************************/
	
   
    hal_enable_interrupts();
	rtc_start(rtc);
	
	while(1)
	{   
		uintx i = 0;
		i = nanosync_recv( sync,  rxbuf,sizeof(rxbuf),OPTION );//i = nanosync_recv( sync,  rxbuf,OPTION );
		//@todo 测试时间同步，让slave节点先启动几分钟，再启动master节点，观察串口输出min的变化
		//串口先输出sec，再输出min
		dbc_putchar(0xaa);//测试用的
		dbc_putchar(sync->rtc->curtime.sec);//测试用的
		dbc_putchar(0xbb);//测试用的
		dbc_putchar(sync->rtc->curtime.min);//测试用的
		dbc_putchar(0xcc);//测试用的
		hal_delay(1000);//测试用的
	   
		if(i)
		{
			switch(rxbuf[12])
				case CONFIG_NANOSYNC_IDENTIFIER:
					while(1)
					{
						
						uintx j = 0;
						j = nanosync_slave_evolve( sync, rxbuf, sizeof(rxbuf),OPTION );

						if(j)
						{
							//dbc_write("done",4);
							
							break;
						}

						
					}

                   
        
					led_toggle(LED_RED);
				
		}

	}
}
	
	
	
	

