/*******************************************************************************
 * csma_recv
 * The receiving test program based on CSMA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * The csma_recv is frist written by xufuzhen in 2010/10/12
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
//#include "../../common/openwsn/svc/svc_nio_csma.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/rtl/rtl_dumpframe.h"
//#include "nio_apl_output_frame.h"

#define CONFIG_DEBUG


#define CONFIG_TEST_LISTENER  
#undef  CONFIG_TEST_LISTENER  

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define CONFIG_CSMA_PANID			0x0001
#define CONFIG_CSMA_LOCAL_ADDRESS	0x02
#define CONFIG_CSMA_REMOTE_ADDRESS	0x01
#define CONFIG_CSMA_CHANNEL		11

#define BROADCAST_ADDRESS			0xFFFF

#define MAX_IEEE802FRAME154_SIZE    128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiCc2420Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;
static TiAloha                      m_aloha;
static TiTimerAdapter               m_timer;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nacmem[NAC_SIZE];



#ifdef CONFIG_TEST_LISTENER
static void _csma_listener( void * ccptr, TiEvent * e );
#endif

static void recvnode(void);

int main(void)
{
	recvnode();
}

void recvnode(void)
{
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
	TiAloha  * mac;
	TiTimerAdapter   *timer;
	TiFrame * rxbuf;
	char * msg = "welcome to csma recv test...";
	int len=0;
	int count;
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
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	#ifdef CONFIG_TSET_LISTENER
    cc = cc2420_open( cc, 0, aloha_evolve, mac, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);

	//mac = csma_open( mac, rxtx, nac, CONFIG_CSMA_CHANNEL, CONFIG_CSMA_PANID, CONFIG_CSMA_LOCAL_ADDRESS, 
        //timer, _csma_listener, NULL );

	aloha_open( mac,rxtx,nac, CONFIG_CSMA_CHANNEL, CONFIG_CSMA_PANID,
		CONFIG_CSMA_LOCAL_ADDRESS,timer, NULL, NULL, 0x00 );
	#endif

    #ifndef CONFIG_TSET_LISTENER
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	//mac = csma_open( mac, rxtx,nac, CONFIG_CSMA_CHANNEL, CONFIG_CSMA_PANID, CONFIG_CSMA_LOCAL_ADDRESS, 
        //timer, NULL, NULL );

	mac = aloha_open( mac,rxtx,nac, CONFIG_CSMA_CHANNEL, CONFIG_CSMA_PANID,
		CONFIG_CSMA_LOCAL_ADDRESS,timer, NULL, NULL, 0x00 );
	#endif
 
	cc2420_setchannel( cc, CONFIG_CSMA_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_CSMA_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_CSMA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2420_enable_addrdecode( cc );
	#else	
	cc2420_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif
 
    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    dbc_putchar(0x11);

    #ifdef CONFIG_TEST_ACK
    //fcf = OPF_DEF_FRAMECONTROL_DATA_ACK; 
	#else
    //fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	#endif


    hal_enable_interrupts();

	/* Wait for listener action. The listener function will be called by the TiCc2420Adapter
	 * object when a frame arrives */
	#ifdef CONFIG_TEST_LISTENER	
	while (1) {}
	#endif
    
	/* Query the TiCc2420Adapter object if there's no listener */
	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{	
		count = 0;
        frame_reset( rxbuf, 3, 20, 0 );
		//len = csma_recv( mac, rxbuf, 0x00 );  
		len = aloha_recv( mac, rxbuf, 0x00 );
		if (len > 0)
		{   
			dbc_putchar( 0xF3 );


            frame_moveouter( rxbuf );
			ieee802frame154_dump( rxbuf);
			frame_skipinner( rxbuf,12,2);
			led_toggle( LED_GREEN );

			while (1)
			{   
				if (aloha_send(mac,CONFIG_CSMA_REMOTE_ADDRESS, rxbuf, 0x01) > 0)//if (csma_send(mac,CONFIG_CSMA_REMOTE_ADDRESS, rxbuf, 0x01) > 0)
				{

					led_toggle( LED_YELLOW );

					break;
				}
				else{
					dbo_putchar(0x22);

					//todo 如果没有下面两行代码，那么受到坏小子节点干扰后，程序将一直执行dbo_putchar(0x22);这一句，即使关掉坏小子节点，程序也没办法恢复。
					count ++;
					if ( count > 0x10)
					{
						break;
					}
					
					
				}
				hal_delay(1000);
			}
			
        }

		//csma_evolve(mac, NULL );
		aloha_evolve(mac, NULL );
	}
	#endif

    frame_close( rxbuf );
    //csma_close( mac );
    aloha_close( mac );
    cc2420_close( cc );
}

#ifdef CONFIG_TEST_LISTENER
void _csma_listener( void * owner, TiEvent * e )
{
	TiCsma * mac = &m_csma;
    TiFrame * frame = (TiFrame *)m_rxbufmem;
    uint8 len;

	dbc_putchar( 0xF4 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = csma_recv( mac, frame, 0x00 );
		if (len > 0)
		{    
            frame_moveouter( frame );
            _output_frame( frame, NULL );
            frame_moveinner( frame );
			led_toggle( LED_RED );

			/* warning: You cannot wait too long in the listener. Because in the 
			 * current version, the listener is still run in interrupt mode. 
			 * you may encounter unexpect error at the application error in real-time
			 * systems. However, the program should still work properly even the 
			 * delay time is an arbitrary value here. No error are allowed in this case. 
			 *
			 * => That's why we cannot use hal_delay() to control the LED to make
			 * it observable for human eye. 
			 */
			// hal_delay( 500 );
			break;
        }
	}
}
#endif


