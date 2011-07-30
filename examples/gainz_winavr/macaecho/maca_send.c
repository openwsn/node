/**
 * maca test
 * build the project and download the binary file to your target node, then switch 
 * the addresses and create another node. the program contains both the sending and 
 * receiving functionalities.
 */

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_frame_transceiver.h"
#include "../../common/openwsn/svc/svc_maca.h"
#include "apl_output_frame.h"

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

/* @attention
 * switch the node local/remote address for transmition experiment 
 */

#define CONFIG_PANID			0x0001
#define CONFIG_LOCALADDRESS		0x0001
#define CONFIG_REMOTEADDRESS	0x0002
#define CONFIG_CHANNEL          11

static TiCc2420Adapter		    m_cc;
static TiMACA                   m_maca;
static TiFrameRxTxInterface     m_rxtx;
static TiUartAdapter		    m_uart;
static TiTimerAdapter           m_timer;
static char                     m_txmem[FRAME_HOPESIZE(128)];
static char                     m_rxmem[FRAME_HOPESIZE(128)];

static int maca_node();
static void init_request( TiFrame * frame );
static uint8 read_sensor( uint8 input );

int main()
{
    maca_node();
    return 0;
}

int maca_node()
{
    TiCc2420Adapter * cc;
    TiMACA * mac;
	TiUartAdapter * uart;
	TiTimerAdapter * timer;
    TiFrame * txbuf;
    TiFrame * rxbuf;


    char * msg = "welcome to csma node(sending+recving)...\n";
    uint16 send_duration = 0;
    uint8 send_counter, retry;
	uint8 option;

    /* initialize the target board. */

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );

    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );


    /* construct required objects */

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = maca_construct( (char *)(&m_maca), sizeof(TiMACA) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
    timer= timer_construct( (char *)(&m_timer),sizeof(TiTimerAdapter) );
    	
    /* output hello message through the uart. if you can see the hello message from 
     * the uart, then it means the node program is successfully started to run. */

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    /* open the TiCc2420Adapter object for configure/send/recv */
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	mac = maca_open( mac, cc2420_interface(cc, &m_rxtx ), CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCALADDRESS, timer, NULL, NULL );
    rxbuf = frame_open( (char *)(&m_rxmem[0]), sizeof(m_rxmem),3 , 20, 25); 
    txbuf = frame_open( (char *)(&m_txmem[0]), sizeof(m_txmem),3 , 20, 25); 

    hal_enable_interrupts();

	for (send_counter=0; send_counter<5; send_counter++)
	{
        // controls the sending rate
        // send_duration controls the send interval, namely, how fast the sending process
        // is. we should query the transceiver by calling recv() more frequently than 
        // send() to avoid frame loss.
        //
        send_duration ++;
        if (send_duration > 100)
        {
            init_request( txbuf );
            option = 0x01;

            for (retry = 0; retry < 10; retry++)
            {
                if (maca_send(mac, txbuf, option) > 0)
                {			
                    led_toggle( LED_YELLOW );
                    break;
                }
                hal_delay(10);
            }
            send_duration = 0;
        }

        // if a frame arrived, then output it through the uart 
        //
        if (maca_recv(mac, rxbuf, option) > 0)
        {
            _output_frame( rxbuf, uart );
        }
		
        // drive the state machine in the adaptive aloha protocol to run
		maca_evolve( mac, NULL );

        // controls the sending rate and receive check frequency. the value shouldn't 
        // be too large or else the arrival frame may be lost. it shouldn't be too small
        // too or else the sending rate may be too high.
        //
        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay(10);
	}

    hal_disable_interrupts();
    cc2420_destroy( cc );
    maca_destroy( mac );
    timer_destroy( timer );
    uart_destroy( uart );
    frame_close( rxbuf );
    frame_close( txbuf );

    return 0;
}

void init_request( TiFrame * frame )
{
    TiIEEE802Frame154Descriptor desc;
    uint16 fcf;
    uint8 len=0, i;

    fcf = FRAME154_DEF_FRAMECONTROL_DATA; 
    // fcf = FRAME154_DEF_FRAMECONTROL_DATA_ACK
    frame_clear( frame );

    // the mac protocol should process the sequence id inside it. so we shouldn't 
    // care about it
    ieee802frame154_open( &desc );
    ieee802frame154_format( &desc, frame_startptr(frame), 50, fcf );
    ieee802frame154_set_panto( &desc, CONFIG_PANID );
    ieee802frame154_set_shortaddrto( &desc, CONFIG_REMOTEADDRESS );
    ieee802frame154_set_panfrom( &desc, CONFIG_PANID);
    ieee802frame154_set_shortaddrfrom( &desc, CONFIG_LOCALADDRESS );

    // msdu points to the payload part of this frame
    for (i=0; i<ieee802frame154_msdu_len(&desc); i++)
        desc.msdu[i] = i;

    desc.msdu[0] = read_sensor( 0xAB );
    desc.msdu[1] = read_sensor( 0xCD );
    desc.msdu[2] = read_sensor( 0xEF );

    ieee802frame154_assemble( &desc, &len );
    ieee802frame154_close( &desc );
    frame_setlength( frame, len );

    #ifdef GDEBUG
    //output_frame( &m_uart, frame );
    #endif
}

/* get the current sensor output */
uint8 read_sensor( uint8 input )
{
    return input;
}
