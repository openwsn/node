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

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 1
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "../../common/openwsn/hal/opennode2010/hal_mcu.h"

#include "../../common/openwsn/hal/opennode2010/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/rtl/rtl_random.h"
#include "../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../common/openwsn/hal/opennode2010/hal_timer.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"

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


static TiFrameRxTxInterface         m_rxtx;
static TiAloha                      m_aloha;
static TiTimerAdapter               m_timer2;
static TiTimerAdapter               m_timer3;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_nacmem[NAC_SIZE];
static char                         m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];




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
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    //TiALOHA * mac;
	TiAloha  * mac;
	TiTimerAdapter   *timer2;
    TiTimerAdapter   *timer3;

	TiFrame * rxbuf;
	TiFrame * txbuf;
    TiFrame * mactxbuf;
	int len;
	uint16 recvcount, sendcount, sendfailed;
	uint8 state;
    len = 0;


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
    halUartInit(9600,0);
   
	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer2 = timer_construct(( char *)(&m_timer2),sizeof(TiTimerAdapter));
    timer3 = timer_construct(( char *)(&m_timer3),sizeof(TiTimerAdapter));

    cc = cc2520_open( cc, 0, NULL, NULL, 0x00 );
	
    rxtx = cc2520_interface( cc, &m_rxtx );

	timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
    timer3 = timer_open( timer3, 3, NULL, NULL, 0x00 );


	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac = aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS, timer2, NULL, NULL, 0x00 );
    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

    mac->txbuf = mactxbuf;

	cc2520_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2520_rxon( cc );							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2520_enable_autoack( cc );
	//cc2420_enable_addrdecode( cc );

    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    txbuf = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );


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

    timer_setinterval( timer3,101,7999);

	while ( 1)
	{
		switch ( state)
		{
		case INIT_STATE:
            frame_reset( rxbuf, 3, 20, 0 );
            len = aloha_recv( mac, rxbuf, 0x00 );
			if (len > 0)
			{   
				recvcount ++;
				_process( rxbuf, txbuf );
				if ( aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				{
					sendcount ++;
					led_toggle( LED_RED);
				} 
				else{
					timer_start( timer3);
					sendfailed ++;
					state = RETRY_SEND_STATE;
				}
			}
			break;
			
		case RETRY_SEND_STATE:
			hal_assert( !frame_empty(txbuf) );
			
			if ( timer_expired(timer3))
			{   
				if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				{
					sendcount ++;
					sendfailed --;
					led_toggle( LED_RED);
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
						led_toggle( LED_RED);
						state = INIT_STATE;
					} 
					else{
						timer_start( timer3);
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
    cc2520_close( cc );
}

void _process( TiFrame * request, TiFrame * response )
{
	frame_moveouter( request );
	//ieee802frame154_dump( request);
	frame_skipinner( request, 12, 2);
	
	frame_reset( response, 3, 20, 0 );
	frame_totalcopyto( request, response );
	frame_setlength( response,frame_capacity(response));
    
	
}




