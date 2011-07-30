/*******************************************************************************
 * ALOHA_recv
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
#define CONFIG_ALOHA_RECV           0
#define CONFIG_ALOHA_FRAME_ARRIVED  1
#define CONFIG_ALOHA_WAITTO_SEND    2

#define ECHO_VTM_RESOLUTION         5


static TiCc2420Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;
//static TiALOHA                       m_ALOHA;
static TiAloha                      m_aloha;
static TiTimerAdapter               m_timer;
static TiTimerAdapter               m_timer2;
static TiTimerManager 		        m_vtm;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nacmem[NAC_SIZE];



#ifdef CONFIG_TEST_LISTENER
static void _ALOHA_listener( void * ccptr, TiEvent * e );
#endif

static void recvnode(void);

/**
 * 
 *
 */

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
	TiTimerAdapter   *timer2;
	TiTimerManager * vtm;
	TiTimer * vti;
	TiFrame * rxbuf;
	TiFrame * txbuf;
	char * msg = "welcome to ALOHA recv test...";
	int len=0;
	int count;
	uint8 state;//todo
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
	//dbo_open( 38400 );
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_putchar( 0xF0 );
    dbc_mem( msg, strlen(msg) );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    //mac = ALOHA_construct( (char *)(&m_ALOHA), sizeof(TiALOHA) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer = timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
	timer2 = timer_construct(( char *)(&m_timer2),sizeof(TiTimerAdapter));
	vtm  = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );

	#ifdef CONFIG_TSET_LISTENER
    cc = cc2420_open( cc, 0, aloha_evolve, mac, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);

	//mac = ALOHA_open( mac, rxtx, nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        //timer, _ALOHA_listener, NULL );
	timer2 = timer_open( timer2, 0, NULL, NULL, 0x01 ); 
	aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS,timer2, NULL, NULL, 0x00 );
	#endif

    #ifndef CONFIG_TSET_LISTENER
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );

	timer = timer_open( timer, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm   = vtm_open( vtm, timer,  ECHO_VTM_RESOLUTION );
	vti   = vtm_apply( vtm );
	vti   = vti_open( vti, NULL, NULL );

	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	//mac = ALOHA_open( mac, rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID, CONFIG_ALOHA_LOCAL_ADDRESS, 
        //timer, NULL, NULL );

	mac = aloha_open( mac,rxtx,nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS,timer, NULL, NULL, 0x00 );
	#endif
 
	cc2420_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
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
    txbuf = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

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


    state = CONFIG_ALOHA_RECV;//todo 


	while ( 1)
	{
		switch ( state)
		{
		case CONFIG_ALOHA_RECV:
            
			 len = 0;
             frame_reset( rxbuf, 3, 20, 0 );
             len = aloha_recv( mac, rxbuf, 0x00 );
			 if ( len>0)
			 {
				 frame_moveouter( rxbuf );
				 ieee802frame154_dump( rxbuf);
				 frame_skipinner( rxbuf,12,2);
				 frame_reset( txbuf,3,20,0);
				 frame_totalcopyto( rxbuf,txbuf);
				 frame_setlength( txbuf,frame_capacity( txbuf));
				 led_toggle( LED_GREEN );
				 state = CONFIG_ALOHA_FRAME_ARRIVED;
			 }

			break;
      
		case CONFIG_ALOHA_FRAME_ARRIVED:
			 if ( !frame_empty( txbuf))
			 {
				 if ( aloha_send(mac,CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
				 {
					 led_toggle( LED_YELLOW);
					 state = CONFIG_ALOHA_RECV;
				 } 
				 else
				 {
					 state = CONFIG_ALOHA_WAITTO_SEND;
				 }

			 } 
			 else
			 {
				 state = CONFIG_ALOHA_RECV;
			 }

			 break;
        
		case CONFIG_ALOHA_WAITTO_SEND:
             
			if ( !frame_empty( txbuf))
			{   
				vti_setscale( vti, 1 );
				vti_setinterval( vti, 101, 0x00 );
				vti_start( vti );
				while ( !vti_expired(vti))
			    {
				    len = 0;
					frame_reset( rxbuf, 3, 20, 0 );
				    len = aloha_recv( mac, rxbuf, 0x00 );
				    if ( len>0)
				    {   
					    frame_moveouter( rxbuf );
					    ieee802frame154_dump( rxbuf);
					    frame_skipinner( rxbuf,12,2);
					    frame_reset( txbuf,3,20,0);
					    frame_totalcopyto( rxbuf,txbuf);
					    frame_setlength( txbuf,frame_capacity( txbuf));
					    led_toggle( LED_GREEN );
					    state = CONFIG_ALOHA_FRAME_ARRIVED;
					    vti_close( vti);
						break;
				    }
			     }

				if ( len>0)
				{   
					break;
				} 
				else
				{   
					if ( aloha_send(mac,CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, 0x01) > 0)
					{   
						led_toggle( LED_YELLOW);
						state = CONFIG_ALOHA_RECV;
					} 
					else
					{
						state = CONFIG_ALOHA_WAITTO_SEND;
					}
				}
				

			} 
			else
			{
				state = CONFIG_ALOHA_RECV;
			}
			break;
        
		default:
			state = CONFIG_ALOHA_RECV;
			break;
			 
		}
	}
    #endif

    

	/*
	while(1) 
	{	
		count = 0;
        frame_reset( rxbuf, 3, 20, 0 );
		//len = ALOHA_recv( mac, rxbuf, 0x00 );  
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
				if (aloha_send(mac,CONFIG_ALOHA_REMOTE_ADDRESS, rxbuf, 0x01) > 0)//if (ALOHA_send(mac,CONFIG_ALOHA_REMOTE_ADDRESS, rxbuf, 0x01) > 0)
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

		//ALOHA_evolve(mac, NULL );
		aloha_evolve(mac, NULL );
	}
	#endif
     */

    frame_close( rxbuf );
	frame_close( txbuf);
    //ALOHA_close( mac );
    aloha_close( mac );
    cc2420_close( cc );
}

#ifdef CONFIG_TEST_LISTENER
void _ALOHA_listener( void * owner, TiEvent * e )
{
	TiAloha * mac = &m_aloha;
    TiFrame * frame = (TiFrame *)m_rxbufmem;
    uint8 len;

	dbc_putchar( 0xF4 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = aloha_recv( mac, frame, 0x00 );
		if (len > 0)
		{    
            frame_moveouter( frame );
            //_output_frame( frame, NULL );
			ieee802frame154_dump( frame);
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


