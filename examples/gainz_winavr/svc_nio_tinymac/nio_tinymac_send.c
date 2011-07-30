
#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
#define GDEBUG
#endif
#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_lightqueue.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_nio_tinymac.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/hal/hal_timer.h"



#define CONFIG_PANID				        0x0001
#define CONFIG_LOCAL_ADDRESS		        0x01
#define CONFIG_REMOTE_ADDRESS		        0x02
#define CONFIG_CHANNEL                    11

#define MAX_IEEE802FRAME154_SIZE                128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;
static TiTinyMAC                                m_tiny;

static char m_nacmem[NAC_SIZE];
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

static void tinymac_sendnode(void);
static void _output_frame( TiFrame * frame );

int main(void)
{
	tinymac_sendnode();
	return 0;
}

void tinymac_sendnode( void )
{   
	TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiTinyMAC * mac;
	TiNioAcceptor * nac;
	TiTimerAdapter   *timer;
	TiFrame * txbuf;
	char * pc;

	char * msg = "welcome to tinymac sendnode...";
	uint8 i, seqid=0, option;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	//led_on( LED_RED );
	dbo_open( 38400 );

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_mem( msg, strlen(msg) );
	dbc_putchar( 0xff);//todo for testing

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = tinymac_construct( (char *)(&m_tiny), sizeof(TiTinyMAC) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	tinymac_open( mac, rxtx, nac, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, 0x01);
	hal_assert( nac != NULL ); 
    
	txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	hal_enable_interrupts();
	while(1) 
	{
		tinymac_setremoteaddress( mac, CONFIG_REMOTE_ADDRESS );

		dbc_putchar(0x24);
		frame_reset(txbuf, 3, 20, 0);
    
#define TEST1

#ifdef TEST1
		pc = frame_startptr( txbuf );
		for (i=0; i<frame_capacity(txbuf); i++)
			pc[i] = i;
		frame_setlength( txbuf, frame_capacity(txbuf) );
#endif
		
#ifdef TEST2
		frame_pushback( txbuf, "01234567890123456789", 20 ); 
#endif

		// if option is 0x00, then mac component will send without requiring ACK from the transceiver.
		// if you want to debugging this program alone without receiver node, then
		// suggest you use option 0x00.
		// the default setting is 0x01, which means ACK is required.
		//
		option = 0x00; 
		
		while (1)
		{   
			//_output_frame( txbuf );
			if (tinymac_send(mac, txbuf, option) > 0)
			{	
				//todo 在此不执行_output_frame( txbuf );，totallength（txbuf）=0
                dbo_putchar( 0xff);
				dbo_putchar( seqid );
				break;
			}
			else{
				dbo_putchar(0x22);
				dbo_putchar( seqid );
			}
			hal_delay(100);
		}
		
		tinymac_evolve( mac, NULL );

		
		nac_evolve( mac->nac, NULL );
       
		// the following delay controls the sending rate. if you want to test the 
		// RXFIFO overflow processing you can decrease this value. 
		//
		// @attention: this long delay will occupy the CPU and the CPU will have no 
		// chance to perform other tasks.
        
		hal_delay(1000);

		led_toggle( LED_RED);//for testing
		//break;
	}

	frame_close( txbuf );
	tinymac_close( mac );
	cc2420_close( cc );
}


void _output_frame( TiFrame * frame )
{
   
	static TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;
	
	if (frame_totallength(frame) > 0)
	{   

		
		dbc_putchar( '>' );
	 	dbc_n8toa( frame_totallength(frame) );

        desc = ieee802frame154_open( &m_desc );
        // ? if (ieee802frame154_parse(desc, frame_startptr(frame), frame_length(frame)))
        if (ieee802frame154_parse(desc, frame_startptr(frame), frame_capacity(frame)))
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

            dbc_n8toa( ieee802frame154_sequence(desc) );
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


