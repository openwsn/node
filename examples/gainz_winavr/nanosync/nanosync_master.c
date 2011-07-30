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
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_nanosync.h"

#define CONFIG_PANID				        0x0001
#define CONFIG_LOCAL_ADDRESS		        0x01
#define CONFIG_REMOTE_ADDRESS		        0x02
#define CONFIG_CHANNEL                      11
#define option                              0x01

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
	TiRtcAdapter * rtc;
	char local_time[9];

	char * msg = "welcome to timesync ...";
	uint8 i, seqid=0;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	//led_on( LED_RED );
	dbo_open( 38400 );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_mem( msg, strlen(msg) );

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	sync = nanosync_construct( (char *)(&m_sync), sizeof(TiNanoSync) );
	rtc = rtc_construct(( char *)(&m_rtc),sizeof(TiRtc));

	rtc_open(rtc,NULL,NULL,0x01);
	rtc_setinterval(rtc,0,2,0x01);

	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nanosync_open( sync,NANOSYNC_MASTER,rtc,rxtx, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, 65535, option);

	hal_enable_interrupts();
	rtc_start(rtc);

	sync->shortaddrto = CONFIG_REMOTE_ADDRESS;

	while (1)
	{
		
		if (_nanosync_broadcast(sync, option) > 0)
		{
		    dbc_write( "done",4);
			led_toggle(LED_GREEN);//新加的
			//break;//新删的
		}
			
		hal_delay(1000);

	}

	return 0;
}
