/*******************************************************************************
 * maca_send
 * This project implements a frame sending node based on TiMaca component. This
 * component is in module "svc_maca". 
 * 
 * @state
 *  - compile passed. send rts successfully .
 *
 * @modified by openwsn on 2010.10.12
 *  - complied successfully. The sniffer have recv the rts successfully
 * 
 ******************************************************************************/

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
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_maca.h"

#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_MACA_PANID				        0x0001
#define CONFIG_MACA_LOCAL_ADDRESS		        0x01
#define CONFIG_MACA_REMOTE_ADDRESS		        0x02
#define CONFIG_MACA_CHANNEL                     11

#define MAX_IEEE802FRAME154_SIZE                128

static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;;
static TiMACA                                   m_maca;
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

void maca_sendnode(void);

int main(void)
{
	maca_sendnode();
    return 0;
}

void maca_sendnode(void)
{   
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiMACA * mac;
	TiTimerAdapter   *timer;
	TiFrame * txbuf;
	char * pc;

	char * msg = "welcome to maca sendnode...";
	uint8 i, seqid=0, option;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );
    dbo_open( 38400 );
    
	
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );


	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = maca_construct( (char *)(&m_maca), sizeof(TiMACA) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    	
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );

    hal_assert( rxtx != NULL );
    // attention: since timer0 is used for the osx kernel, we propose the next 16bit 
    // timer to be used in this module. If you port this example to other hardware
    // platform, you may need to adjust the timer_open parameters.
    //
    // Q: is the second parameter be 2 or 3 for atmega's 16 bit timer?

    timer = timer_open( timer, 2, NULL, NULL, 0x00 ); 

    // initialize the standard aloha component for sending/recving
	hal_assert( (rxtx != NULL) && (timer != NULL) );
    maca_open( mac, rxtx, CONFIG_MACA_CHANNEL, CONFIG_MACA_PANID, CONFIG_MACA_LOCAL_ADDRESS, 
        timer, NULL, NULL );
    
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );
	hal_enable_interrupts();

	

	while(1) 
	{
		maca_setremoteaddress( mac, CONFIG_MACA_REMOTE_ADDRESS );
        frame_reset(txbuf, 3, 20, 25);

	    #define TEST1

        #ifdef TEST1
        pc = frame_startptr( txbuf );
        for (i=0; i<frame_capacity(txbuf); i++)
            pc[i] = i;
		//frame_setlength(txbuf, i-1);
        #endif

        #ifdef TEST2
        frame_pushback( txbuf, "01234567890123456789", 20 ); 
        #endif


        // if option is 0x00, then aloha send will not require ACK from the receiver. 
        // if you want to debugging this program alone without receiver node, then
        // suggest you use option 0x00.
        // the default setting is 0x01, which means ACK is required.
        //
		option = 0x00;


        while (1)
        {   
            if (maca_send(mac, txbuf, option) > 0)
            {
			    dbc_putchar(0x22);		
                led_toggle( LED_YELLOW );
                dbo_putchar( 0x11);
                dbo_putchar( seqid );
                break;
            }
			else{
				dbo_putchar(0x22);
                dbo_putchar( seqid );
			}
            hal_delay(1000);
        }
		
		// for simple aloha, you needn't to call aloha_evolve(). it's necessary for 
        // standard aloha.
   
        maca_evolve( mac, NULL );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
        // attention: this long delay will occupy the CPU and it may lead to frame lossing.
        
		hal_delay(1000);

		//break;
	}

    frame_close( txbuf );
    maca_close( mac );
    cc2420_close( cc );
}



