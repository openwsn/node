/*******************************************************************************
 * acceptor_echo
 * This is used to test the svc_nio_acceptor module. The difference between this one
 * and the test in "svc_nio_acceptor" is that: this test contains the state transfer
 * between RX and TX. 
 * 
 * In this test, the node will listen to incoming frames. And it will send the frame 
 * back. You can regard this process as a simple REQUEST/PROCESS/RESPONSE pair.
 * So it's valuable 
 * 
 * @state
 *  - Released. Tested Ok.
 *
 * @author zhangwei on 2012.08.02
 *  - First version. This version is based on the csma echo test which is firstly 
 *    developed by Xu Fuzhen on 2012.10.12.
 * @modified by Shi Zhironng on 2012.08.03
 *  - Compiled successfully and tested Ok. 
 * 
 ******************************************************************************/

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>

#ifdef CONFIG_TARGETBOARD_GAINZ
#include <avr/wdt.h>
#endif

#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/rtl/rtl_random.h"
#include "../../common/openwsn/rtl/rtl_dumpframe.h"
#include "../../common/openwsn/hal/hal_foundation.h"
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
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
//#include "nio_apl_output_frame.h"

#define CONFIG_DEBUG

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define APC_PANID			0x0001
#define APC_LOCAL_ADDRESS	0x02
#define APC_REMOTE_ADDRESS	0x01
#define APC_CHANNEL		    11

#define BROADCAST_ADDRESS			0xFFFF

/* The value is 128 */
#define MAX_IEEE802FRAME154_SIZE    I802F154_PSDU_SIZE

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

/* Define the states of the echo application itself. */ 
#define ECHO_STATE_RECV           0
#define ECHO_STATE_FRAME_ARRIVED  1
#define ECHO_WAITFOR_SEND    2

#define ECHO_VTM_RESOLUTION         5

static TiCc2520Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;
static TiTimerAdapter               m_timer;
static TiTimerAdapter               m_timer2;
static TiTimerManager 		        m_vtm;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nacmem[NAC_SIZE];


#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
int _apl_rxhandler_for_acceptor(void * owner, TiFrame * inputbuf, TiFrame * outputbuf, uint8 option );
#endif

static void echonode(void);

/**
 * 
 *
 */

int main(void)
{
	echonode();
}

void echonode(void)
{
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
	TiTimerAdapter   *timer;
	TiTimerAdapter   *timer2;
	TiTimerManager * vtm;
	TiTimer * vti;
	TiFrame * rxbuf;
	TiFrame * txbuf;
	char * msg = "welcome to nioacceptor based echo test...";
	int len=0;
	int count;
	uint8 state;
    
    // initialize the target hardware board
    target_init();

    // flash the led to indicate the software is running successfully now.
    //
	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

    // initialize the runtime library for debugging input/output and assertion
    // hal_assert_report is defined in module "hal_assert"
    //
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_putchar( 0xF0 );
    dbc_mem( msg, strlen(msg) );

	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    timer = timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
	timer2 = timer_construct(( char *)(&m_timer2),sizeof(TiTimerAdapter));
	vtm  = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );

	#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
	timer2 = timer_open( timer2, 0, NULL, NULL, 0x01 ); 
    cc = cc2520_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2520_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);

    rxtx->setrxhandler(rxtx->provider, nac_rxfilter_for_transceiver, nac);
    nac_setrxhandler(nac, apl_rxhandler_for_acceptor, NULL); 
	#endif

    #ifndef CONFIG_TEST_ACCEPTOR_RXHANDLER
	timer = timer_open( timer, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm   = vtm_open( vtm, timer,  ECHO_VTM_RESOLUTION );
	vti   = vtm_apply( vtm );
	vti   = vti_open( vti, NULL, NULL );
    cc = cc2520_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2520_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	#endif
 
	cc2520_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2520_setrxmode( cc );							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier  
	cc2520_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// node identifier in the network

	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2520_enable_addrdecode( cc );
	#else	
	cc2520_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2520_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK; 
    #else
	cc2520_disable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	#endif
 
    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    txbuf = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    #ifdef CONFIG_TEST_ACK
	#else
	#endif

    hal_enable_interrupts();

	/* Wait for the rxhandler to be invoked. The rxhandler will be called by the acceptor
	 * object when a frame accepted into the rxque. */

	#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
	while (1) 
    {
        nac_evolve();
    }
	#endif
    
	/* Polling the acceptor to retrieve the frame arrived */
	#ifndef CONFIG_TEST_ACCEPTOR_RXHANDLER

    /* By default, the application is running in the receiving mode. */
    state = ECHO_STATE_RECV;
/*
	while (1)
	{
		switch ( state)
		{
		case ECHO_STATE_RECV:
            len = 0;
            frame_reset( rxbuf, 3, 20, 0 );
            len = nac_recv( mac, rxbuf, 0x00 );
			if ( len>0)
			{
				 frame_moveouter( rxbuf );
				 ieee802frame154_dump( rxbuf);
				 frame_skipinner( rxbuf,12,2);
				 frame_reset( txbuf,3,20,0);
				 frame_totalcopyto( rxbuf, txbuf);
				 frame_setlength( txbuf,frame_capacity(txbuf));
				 led_toggle( LED_GREEN );
				 state = ECHO_STATE_FRAME_ARRIVED;
			 }
			break;
      
		case ECHO_STATE_FRAME_ARRIVED:
			if (!frame_empty(txbuf))
			{
				if (nac_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				{
					led_toggle(LED_YELLOW);
					state = ECHO_STATE_RECV;
				} 
				else{
					state = ECHO_WAITFOR_SEND;
				}
			} 
			else{
				state = ECHO_STATE_RECV;
			}
			break;
        
		case ECHO_WAITFOR_SEND:
			if (!frame_empty(txbuf))
			{   
				vti_setscale( vti, 1 );
				vti_setinterval( vti, 101, 0x00 );
				vti_start( vti );
				while (!vti_expired(vti))
			    {
				    len = 0;
					frame_reset( rxbuf, 3, 20, 0 );
				    len = nac_recv( mac, rxbuf, 0x00 );
				    if (len > 0)
				    {   
					    frame_moveouter( rxbuf );
					    ieee802frame154_dump( rxbuf);
					    frame_skipinner( rxbuf,12,2);
					    frame_reset(txbuf,3,20,0);
					    frame_totalcopyto(rxbuf, txbuf);
					    frame_setlength( txbuf,frame_capacity( txbuf));
					    led_toggle( LED_GREEN );
					    state = ECHO_STATE_FRAME_ARRIVED;
					    vti_close( vti);
						break;
				    }
			    }

				if (len > 0)
				{   
					break;
				} 
				else{
					if (nac_send(mac,CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
					{   
						led_toggle( LED_YELLOW);
						state = ECHO_STATE_RECV;
					} 
					else{
						state = ECHO_WAITFOR_SEND;
					}
				}
			} 
			else{
				state = ECHO_STATE_RECV;
			}
			break;
        
		default:
			state = ECHO_STATE_RECV;
			break;
		}
	}
    #endif
*/
    
    frame_close( rxbuf );
	frame_close( txbuf);
    nac_close( mac );
    cc2520_close( cc );
}

#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
/**
 * The RX frame handler in the application layer. It will firstly inspect the frame
 * and switch the source and destination address, and finally sent the frame back
 * to its sender. 
 * 
 * This function can be regarded as a simple version of the wireless data server. 
 * We can learn how to implement a simple REQUEST-PROCESS_RESPONSE workflow. 
 * 
 * @todo: 
 * The failed sending will block the whole process in the current version. You should
 * give it some time.
 */ 
int _apl_rxhandler_for_acceptor(void * owner, TiFrame * inputbuf, TiFrame * outputbuf, uint8 option )
{
    int len;
    
    // This is the REQUEST frame received
	dbc_putchar( 0xF1 );
    led_toggle( LED_RED );
    len = frame_length(inputbuf);
    hal_assert(len > 0);
	ieee802frame154_dump(inputbuf);
    
    // PROCESS the frame and then sending the RESPONSE frame out
	dbc_putchar( 0xF2 );
    static TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;
    
    desc = ieee802frame154_open( &m_desc );
    if (ieee802frame154_parse(desc, frame_startptr(input), frame_capacity(input)))
    {
        dbc_putchar( 0xF3 );
        ieee802frame154_swapaddress(desc);

        vti_setscale( vti, 1 );
        vti_setinterval( vti, 101, 0x00 );
        vti_start( vti );
        while (!vti_expired(vti))
        {
            if (nac_send(nac, input) > 0)
                break;
        }
        vti_stop(vti);

        /* An simple version without timer delay control is as the following. The problem 
         * is that if the sending failed, then the node will still continue sending
         * so the failure can block the node behavior. 
         */
        /*
        while (1)
        {
            if (nac_send(nac, input) > 0)
                break;
        }
        */
    }
    else{
        dbc_putchar( 0xF4 );
    }
}
