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

#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/rtl/rtl_random.h"
#include "openwsn/rtl/rtl_dumpframe.h"
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_targetboard.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_timer.h"
#include "openwsn/svc/svc_timer.h"
#include "openwsn/svc/svc_nio_acceptor.h"

#define CONFIG_TEST_ACCEPTOR_RXHANDLER

#define CONFIG_DEBUG

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02  
#define REMOTE_ADDRESS		0x01
#define DEFAULT_CHANNEL     11

#define BROADCAST_ADDRESS			0xFFFF

/* The value is 128 */
#define MAX_IEEE802FRAME154_SIZE    I802F154_PSDU_SIZE

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

/* Define the states of the echo application itself. */ 
#define ECHO_STATE_RECV           	0
#define ECHO_STATE_FRAME_ARRIVED  	1
#define ECHO_WAITFOR_SEND    		2

#define ECHO_VTM_RESOLUTION         5

static TiCc2520Adapter		    m_cc;
static TiFrameRxTxInterface     m_rxtx;
static TiTimerAdapter           m_timer;
static TiTimerAdapter           m_timer2;
//static TiTimerManager 		  m_vtm;
static char                     m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                     m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char 					m_nacmem[NAC_SIZE];


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
//	TiTimerManager * vtm;
//	TiTimer * vti;
	TiFrame * rxbuf;
	TiFrame * txbuf;
	char * msg = "welcome to nioacceptor based echo test...";
	int len=0;
	int count;
	uint8 state;
    halUartInit(9600,0);

    target_init();

	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    timer = timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
	timer2 = timer_construct(( char *)(&m_timer2),sizeof(TiTimerAdapter));
	
	cc = cc2520_open( cc, 0, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);

//	vtm  = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );

	#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
	timer2 = timer_open( timer2, 0, NULL, NULL, 0x01 ); 
    cc = cc2520_open( cc, 0, 0x00 );
    rxtx = cc2520_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);


	cc2520_setrxfilter(cc,nac_rxfilter_for_transceiver,nac);
//    rxtx->setrxhandler(rxtx->provider, nac_rxfilter_for_transceiver, nac);
    nac_setrxhandler(nac, _apl_rxhandler_for_acceptor, nac); 
	#endif

//    #ifndef CONFIG_TEST_ACCEPTOR_RXHANDLER
//	timer = timer_open( timer, 0, vtm_inputevent, vtm, 0x01 ); 
//	vtm   = vtm_open( vtm, timer,  ECHO_VTM_RESOLUTION );
//	vti   = vtm_apply( vtm );
//	vti   = vti_open( vti, NULL, NULL );
//    cc = cc2520_open( cc, 0, 0x00 );
//    rxtx = cc2520_interface( cc, &m_rxtx );
//	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
//	#endif
 
    cc2520_setchannel( cc, DEFAULT_CHANNEL );
    cc2520_rxon( cc );							    // Enable RX
    //cc2520_enable_addrdecode( cc );					// enable address decoding and filtering
    cc2520_setpanid( cc, PANID );					// set network identifier 
    cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// set node identifier in a sub-network
    cc2520_enable_autoack( cc );

	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2520_enable_addrdecode( cc );
	#else	
	cc2520_disable_addrdecode( cc );
	#endif

/*
	#ifdef CONFIG_TEST_ACK
	cc2520_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK; 
    #else
	cc2520_disable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	#endif
*/ 
    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    txbuf = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    hal_enable_interrupts();

	/* Wait for the rxhandler to be invoked. The rxhandler will be called by the acceptor
	 * object when a frame accepted into the rxque. */

	#ifdef CONFIG_TEST_ACCEPTOR_RXHANDLER
	USART_Send(0xFF);
	while (1) 
    {
        nac_evolve(nac,NULL);
    }
	#endif
    
	/* Polling the acceptor to retrieve the frame arrived */
	#ifndef CONFIG_TEST_ACCEPTOR_RXHANDLER
	USART_Send(0xFF);
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
	} */
    #endif

    
    frame_close( rxbuf );
	frame_close( txbuf);
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
int _apl_rxhandler_for_acceptor(void * owner, TiFrame * input, TiFrame * output, uint8 option )
{
	TiNioAcceptor * nac = ( TiNioAcceptor * )owner;
    int len;
    static TiIEEE802Frame154Descriptor m_desc;    
    TiIEEE802Frame154Descriptor * desc;  
//	if(input==NULL)	 //JOE 0803
//		return 0;
	 
	// This is the REQUEST frame received
	USART_Send( 0xF1 );
    led_toggle( LED_RED );
    len = frame_length(input);
    hal_assert(len > 0);
	ieee802frame154_dumpframe(input);
    
    // PROCESS the frame and then sending the RESPONSE frame out
 
 	USART_Send( 0xF2 );    
    desc = ieee802frame154_open( &m_desc );
    if (ieee802frame154_parse(desc, frame_startptr(input), frame_length(input)))		  //input+1
																						//frame_capacity(input)
    {
        USART_Send( 0xF3 );
        ieee802frame154_swapaddress(desc);

//        vti_setscale( vti, 1 );
//        vti_setinterval( vti, 101, 0x00 );
//        vti_start( vti );
//        while (!vti_expired(vti))
//        {
//            if (nac_send(nac, input) > 0)
//                break;
//        }
//        vti_stop(vti);

        /* An simple version without timer delay control is as the following. The problem 
         * is that if the sending failed, then the node will still continue sending
         * so the failure can block the node behavior. 
         */
        
        while (1)
        {
			input->option = 0x00;
            if (nac_send(nac, input,0x00) > 0)
                break;
        }
        
    }
    else
	{
        USART_Send( 0xF4 );
    }
}
#endif
