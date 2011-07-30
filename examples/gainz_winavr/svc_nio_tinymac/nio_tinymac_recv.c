
/* the following are global macro definitions and should be effective for all header files */

#define CONFIG_DEBUG
#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 4
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 4


#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_nio_tinymac.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"

/* the following are private macro definitions and should be effective only inside this module */

#define CONFIG_TEST_LISTENER  
#undef  CONFIG_TEST_LISTENER  

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define CONFIG_PANID			0x0001
#define CONFIG_LOCAL_ADDRESS	0x02
#define CONFIG_REMOTE_ADDRESS	0x00
#define CONFIG_CHANNEL		11

#define BROADCAST_ADDRESS			0xFFFF

#define MAX_IEEE802FRAME154_SIZE    128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

TiIEEE802Frame154Descriptor m_meta;

static TiCc2420Adapter		        m_cc;
static TiFrameRxTxInterface         m_rxtx;
static TiTinyMAC                    m_tinymac;
static TiTimerAdapter               m_timer;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nacmem[NAC_SIZE];

#ifdef CONFIG_TEST_LISTENER
static void _tinymac_listener( void * ccptr, TiEvent * e );
#endif

static void tinymac_recvnode(void);
static void _output_frame( TiFrame * frame );

int main(void)
{
	tinymac_recvnode();
}

void tinymac_recvnode(void)
{
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiTinyMAC * mac;
	TiNioAcceptor * nac;
	TiTimerAdapter   *timer;
	TiFrame * rxbuf;
	char * msg = "welcome to svc_nio_tinymac recv test...";
    int len=0;
	bool ret ;

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
    mac = tinymac_construct((char *)(&m_tinymac), sizeof(TiTinyMAC));
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    timer= timer_construct((char *)(&m_timer),sizeof(TiTimerAdapter));

	#ifdef CONFIG_TSET_LISTENER
	// cc = cc2420_open( cc, 0, _aloha_listener, NULL, 0x00 );
    cc = cc2420_open( cc, 0, tinymac_evolve, mac, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
	nac = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	mac = tinymac_open( mac, rxtx,nac, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS, 
        timer, _tinymac_listener, NULL,0x00 );
	
	#endif

    #ifndef CONFIG_TSET_LISTENER
    cc2420_open( cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
    nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	tinymac_open( mac, rxtx, nac, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, 0x01);
	hal_assert( nac != NULL );
	#endif
 
	if ((cc==NULL) || (mac==NULL) || (rxtx==NULL) || (mac==NULL) || (nac==NULL))
	{
		// somoe components construct failure
		// dbc_write( warning messages )
		ret = -1;
	}

	cc2420_setchannel( cc, CONFIG_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );

	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2420_enable_addrdecode( cc );
	#else	
	cc2420_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif
    
    #ifdef CONFIG_TEST_ACK
    //fcf = OPF_DEF_FRAMECONTROL_DATA_ACK; 
	#else
    //fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK; 
	#endif
    
    rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
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
	    frame_reset( rxbuf, 3, 20, 0 );
		len = tinymac_recv( mac, rxbuf, 0x00 );
		if (len > 0)
		{
			dbc_putchar( 0xF3 );
			
			frame_moveouter( rxbuf );
            _output_frame( rxbuf );
			//todo为了测试先把上句删掉
            frame_moveinner( rxbuf );
			
			/* warning: You shouldn't wait too long in the while loop, or else 
			 * you may encounter frame loss. However, the program should still 
			 * work properly even the delay time is an arbitrary value. No error 
			 * are allowed in this case. 
			 */
			led_toggle( LED_RED );
			dbc_putchar( 0xff);
		}

		nac_evolve( mac->nac, NULL);
		hal_delay( 1000);	
	}
	#endif

    frame_close( rxbuf );
    tinymac_close( mac );
    cc2420_close( cc );
}

#ifdef CONFIG_TEST_LISTENER
void _tinymac_listener( void * owner, TiEvent * e )
{
	TiTinyMAC * mac = &m_tinymac;
    TiFrame * frame = (TiFrame *)m_rxbufmem;
    uint8 len;

	dbc_putchar( 0xF4 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = tinymac_recv( mac, frame, 0x00 );
		if (len > 0)
		{    
            frame_moveouter( frame );
            _output_frame( frame );
            frame_moveinner( rxbuf );
			led_toggle( LED_RED );

			
			// hal_delay( 500 );
			break;
        }
	}
}
#endif

void _output_frame( TiFrame * frame )
{
    static TiIEEE802Frame154Descriptor desc;

	if (frame_totallength(frame) > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( frame_totallength(frame) );

        ieee802frame154_open( &desc );
        // ? if (ieee802frame154_parse(desc, frame_startptr(frame), frame_length(frame)))
        if (ieee802frame154_parse(&desc, frame_startptr(frame), frame_capacity(frame)))
        {
            // if the frame received is parsed successfully, then output it to the
            // computer through debugging channel

            //ieee802frame154_set_sequence( desc, seqid ++ );
		    //ieee802frame154_set_panto( desc, CONFIG_ALOHA_DEFAULT_PANID );
		    //ieee802frame154_set_shortaddrto( desc, CONFIG_ALOHA_REMOTE_ADDRESS );
		    //ieee802frame154_set_panfrom( desc, CONFIG_ALOHA_PANID);
		    //ieee802frame154_set_shortaddrfrom( desc, CONFIG_ALOHA_LOCAL_ADDRESS );

            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

            dbc_n8toa( ieee802frame154_sequence(&desc) );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_capacity(frame) );
		}
		else{
	        // if the frame received is parsed failed, then output the error frame
            // to the computer through debugging channel

	        dbc_putchar( 'X' );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_capacity(frame) );
		}
		dbc_putchar( '\r' );
		dbc_putchar( '\n' );
	}
}



