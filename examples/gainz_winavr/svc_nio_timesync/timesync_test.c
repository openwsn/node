#include <stdlib.h>
#include <string.h>

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "../../common/openwsn/hal/hal_configall.h"  
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/rtl/rtl_time.h"
#include "../../common/openwsn/hal/hal_ticker.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_adc.h"
#include "../../common/openwsn/hal/hal_luminance.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/svc/svc_configall.h"  
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/rtl/rtl_dumpframe.h"

#include "../../common/openwsn/osx/osx_timer.h"
#include "../../common/openwsn/svc/svc_nio_timesync.h"
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_REMOTE_PANID				0x0002
#define CONFIG_REMOTE_ADDR				0x0001

#define MAX_IEEE802FRAME154_SIZE        128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)
#define GATW_VTM_RESOLUTION 7

static TiOsxTimer                           m_systimer; 
static TiNioTimeSync                        m_tmsync;
static TiTimerAdapter 		m_hwtimer0;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiFrameRxTxInterface m_rxtx;
static TiAloha              m_aloha;
static char                 m_nacmem[NAC_SIZE];

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
uint8                      count = 0;

void on_timer_expired( void * object, TiEvent * e );

int main()
{
	char * msg = "welcome to gateway node...";
	TiSystemTime interval = 60000;
	uint16 period = 2000;
	TiTimerAdapter * hwtimer0;
	TiTimerManager * vtm;
	TiTimer * vti;
	TiTimer *mac_timer;
	TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiAloha * mac;
	TiNioAcceptor * nac;
	TiFrame * rxbuf;
	TiNioTimeSync * tmsync;
	TiOsxTimer * systimer;
	char * ptr;
	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_write( msg, strlen(msg) );

	hwtimer0        = timer_construct( (void *)(&m_hwtimer0), sizeof(m_hwtimer0) );
	systimer        =  _osx_timer_construct( (void *)(& m_systimer), sizeof( m_systimer) );
	vtm             = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc              = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac             = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac             = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	tmsync          = timesync_construct( (void *)(&m_tmsync), sizeof(m_tmsync));

	rxbuf           = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

	cc              = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx            = cc2420_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nac             = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	hal_assert( nac != NULL );
	hwtimer0	   	= timer_open( hwtimer0, 0, vtm_inputevent, vtm, 0x01 );
	//timer_disable( hwtimer0);
	dbc_putchar( 0xa1);//todo for testing
	//systimer      =  _osx_timer_open( systimer, interval, on_timer_expired, NULL );//systimer 在这一句应该就被启动了
	systimer      =  _osx_timer_open( systimer, interval, NULL, NULL );//systimer 在这一句应该就被启动了
    dbc_putchar( 0xa2);//todo for testing
	vtm             = vtm_open( vtm, hwtimer0, GATW_VTM_RESOLUTION );
	vti             = vtm_apply( vtm );
	vti             = vti_open( vti, NULL, vti );
	mac_timer       = vtm_apply( vtm);
	mac_timer       = vti_open( vti,NULL,vti);
	dbc_putchar( 0xa3);//todo for testing
	mac             = aloha_open( mac, rxtx,nac, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,mac_timer, NULL, NULL,0x01);
	dbc_putchar( 0xa4);//todo for testing
	tmsync          = timesync_open( tmsync, systimer, NULL, period );//tmsync          = timesync_open( tmsync, systimer, mac, period );
	dbc_putchar( 0xa5);//todo for testing
	cc2420_setchannel( cc, CONFIG_NODE_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_NODE_PANID  );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_NODE_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );
	cc2420_settxpower( cc, CC2420_POWER_1);//cc2420_settxpower( cc, CC2420_POWER_2);CC2420_POWER_1
	cc2420_enable_autoack( cc );
	dbc_putchar( 0xa6);//todo for testing

	//todo 只要下面的中断一打开程序就不在往下运行，立即重启---飞掉了
	//hal_enable_interrupts();//
	dbc_putchar( 0xa7);//todo for testing
   /* 
	vti_setscale( vti, 1 );
	vti_setinterval( vti, 3000, 0x00 );
	vti_start( vti );
    */
	dbc_putchar( 0xa0);//todo for testing
	while ( 1)
	{
		frame_reset( rxbuf,3,20,0);
		ptr = frame_startptr( rxbuf);
		ptr[0] = 13;
		ptr[1] = 14;
		ptr[2] = 15;

		frame_setlength( rxbuf,3);
		//aloha_broadcast( mac,rxbuf,0x00);
		hal_delay( 1000);
		/*
		if ( vti_expired( vti))
		{
			led_toggle( LED_YELLOW);
			timesync_start(  tmsync);
			vti_setscale( vti, 1 );
			vti_setinterval( vti, 3000, 0x00 );
			vti_start( vti );
		}*/
		//timesync_evolve( tmsync, NULL );
		//aloha_evolve( tmsync->mac,NULL);
	}
}

void on_timer_expired( void * object, TiEvent * e )
{
	count++;
	led_toggle( LED_RED );
	TiSystemTime time;
	time = m_tmsync.systimer->time;
	//time = m_systimer.time;
	if ( count >=50)
	{
		dbc_mem( &time, sizeof(TiSystemTime) );
		count = 0;
		led_toggle( LED_GREEN);
	}
}      
