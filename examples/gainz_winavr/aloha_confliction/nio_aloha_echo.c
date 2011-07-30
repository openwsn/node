/*******************************************************************************
 * aloha_recv
 * The receiving test program based on ALOHA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * The ALOHA_recv is frist written by xufuzhen in 2010/10/12
 * Complied successfully
 *  
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/rtl/rtl_random.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/svc/svc_timer.h"
//#include "../../common/openwsn/svc/svc_nio_ALOHA.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/rtl/rtl_dumpframe.h"
//#include "nio_apl_output_frame.h"

#define CONFIG_DEBUG


#define CONFIG_TEST_LISTENER  
#undef  CONFIG_TEST_LISTENER  

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define CONFIG_ALOHA_PANID			0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS	0x02
#define CONFIG_ALOHA_REMOTE_ADDRESS	0x01
#define CONFIG_ALOHA_CHANNEL		11

#define BROADCAST_ADDRESS			0xFFFF

#define MAX_IEEE802FRAME154_SIZE    128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

//todo define the states of the echo
#define INIT_STATE           0
#define CONFIG_ALOHA_FRAME_ARRIVED  1
#define RETRY_SEND_STATE    2

#define ECHO_VTM_RESOLUTION         5


static TiCc2420Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;
static TiAloha                      m_aloha;
static TiTimerAdapter               m_timer;
static TiTimerManager 		        m_vtm;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nacmem[NAC_SIZE];



#ifdef CONFIG_TEST_LISTENER
static void _aloha_listener( void * ccptr, TiEvent * e );
#endif

static void recvnode(void);
static void _process( TiFrame * request, TiFrame * response );

int main(void)
{
	recvnode();
}

void recvnode(void)
{
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    //TiALOHA * mac;
	TiAloha  * mac;
	TiTimerAdapter   *timer;
	TiTimerManager * vtm;
	TiTimer * mac_timer;
	TiTimer * tm;
	TiFrame * rxbuf;
	TiFrame * txbuf;
	char * msg = "welcome to ALOHA recv test...";
	int len=0;
	uint16 recvcount, sendcount, sendfailed;
	uint8 state;

    target_init();

    // flash the led to indicate the software is successfully running now.
    //
	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	//led_on( LED_RED );

    // initialize the runtime library for debugging input/output and assertion
    // hal_assert_report is defined in module "hal_assert"
    //
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_putchar( 0xF0 );
    dbc_mem( msg, strlen(msg) );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer = timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
	vtm  = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );

    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
	
    rxtx = cc2420_interface( cc, &m_rxtx );

	timer = timer_open( timer, 0, NULL, NULL, 0x01 );
	vtm = vtm_open( vtm, timer, ECHO_VTM_RESOLUTION );
	mac_timer = vtm_apply( vtm );
	vti_open( mac_timer, NULL, mac_timer );
	tm = vtm_apply( vtm );
	tm = vti_open( tm, NULL, tm );
	hal_assert((vtm != NULL) && (mac_timer != NULL) && (tm != NULL));

	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac = aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS, mac_timer, NULL, NULL, 0x00 );
   

	cc2420_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );
	cc2420_enable_addrdecode( cc );

    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    txbuf = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    hal_enable_interrupts();

	recvcount = 0;
	sendcount = 0;
	sendfailed = 0;
    state = INIT_STATE; 
    /*
	while ( 1)
	{   
		len = 0;
		frame_reset( rxbuf, 3, 20, 0 );
		len = aloha_recv( mac, rxbuf, 0x00 );
		if ( len>0)
		{   ieee802frame154_dump( rxbuf);
			led_toggle( LED_GREEN);
		}

	}
*/
/*
    while ( 1)
    {
		frame_reset( rxbuf, 3, 20, 0 );
		len = aloha_recv( mac, rxbuf, 0x00 );
		if (len > 0)
		{   
			led_toggle( LED_RED);
			recvcount ++;
			_process( rxbuf, txbuf );
			if ( aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
			{
				sendcount ++;
				led_toggle( LED_YELLOW);
			} 
			
		}
    }
*/

	while ( 1)
	{
		switch ( state)
		{
		case INIT_STATE:
            frame_reset( rxbuf, 3, 20, 0 );
            len = aloha_recv( mac, rxbuf, 0x00 );
			if (len > 0)
			{   
			    led_toggle( LED_RED);
				recvcount ++;
				_process( rxbuf, txbuf );
				if ( aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				{
					sendcount ++;
					led_toggle( LED_YELLOW);
				} 
				else{
					vti_restart( tm, 101, 1,0x00 );
					sendfailed ++;
					state = RETRY_SEND_STATE;
				}
			}
			break;
			
		case RETRY_SEND_STATE:
			hal_assert( !frame_empty(txbuf) );
			
			if ( vti_expired(tm))
			{   
				if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				{
					sendcount ++;
					sendfailed --;
					led_toggle( LED_YELLOW);
				} 
				state = INIT_STATE;
			}
			else{
				frame_reset( rxbuf, 3, 20, 0 );
				len = aloha_recv( mac, rxbuf, 0x00 );
				if (len > 0)
				{
					recvcount ++;
					_process( rxbuf, txbuf );
					if ( aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
					{
						sendcount ++;
						led_toggle( LED_YELLOW);
						state = INIT_STATE;
					} 
					else{
						vti_restart( tm, 101,1, 0x00 );
						sendfailed ++;
					}
				}
			}
			break;
		
		default:
			state = INIT_STATE;
		}
		/*
		dbc_putchar( recvcount );
		dbc_putchar( sendcount );
		dbc_putchar( sendfailed );*/
        
		aloha_evolve( mac, NULL );
	}
	
    frame_close( rxbuf );
	frame_close( txbuf);
    aloha_close( mac );
    cc2420_close( cc );
}

void _process( TiFrame * request, TiFrame * response )
{
	frame_moveouter( request );
	ieee802frame154_dump( request);
	frame_skipinner( request, 12, 2);
	
	frame_reset( response, 3, 20, 0 );
	frame_totalcopyto( request, response );
	frame_setlength( response,frame_capacity(response));
    
	
}




