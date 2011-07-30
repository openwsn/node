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
#include "../../common/openwsn/svc/svc_tinymac.h"


#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_PANID				        0x0001
#define CONFIG_LOCAL_ADDRESS		        0x01
#define CONFIG_REMOTE_ADDRESS		        0x02
#define CONFIG_CHANNEL                    11

#define MAX_IEEE802FRAME154_SIZE                128

static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;;
static TiTinyMAC                                m_tiny;
//static TiUartAdapter		                    m_uart;
static TiTimerAdapter                           m_timer;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

/*
#define TiAloha TiSimpleAloha
#define aloha_construct(buf,msize) saloha_construct((buf),(msize))
#define aloha_open(mac,rxtx,chn,panid,addr,timer,listener,lisobject,opt) saloha_open((mac),(rxtx),(chn),(panid),(addr),(timer),(listener),(lisobject),(opt))
#define aloha_close(mac) saloha_close((mac))
#define aloha_send(a,f,opt) saloha_send((a),(f),(opt))
#define aloha_recv(a,f,opt) saloha_recv((a),(f),(opt))
#define aloha_evolve(a,e) saloha_evolve((a),(e))
*/

void tinymac_sendnode(void);

int main(void)
{
	tinymac_sendnode();
    return 0;
}

void tinymac_sendnode(void)
{   
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
    TiTinyMAC * mac;
	TiTimerAdapter   *timer;
	TiFrame * txbuf;
	char * pc;

	char * msg = "welcome to simplealoha sendnode...";
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
	mac = tinymac_construct( (char *)(&m_tiny), sizeof(TiTinyMAC) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
    rxtx = cc2420_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );
    tinymac_open( mac,rxtx, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, 0x01);

    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );
    
	hal_enable_interrupts();
	while(1) 
	{
        tinymac_setremoteaddress( mac, CONFIG_REMOTE_ADDRESS );

        dbc_putchar(0x24);
        frame_reset(txbuf, 3, 20, 25);

	    #define TEST1

        #ifdef TEST1
        pc = frame_startptr( txbuf );
        for (i=0; i<frame_capacity(txbuf); i++)
            pc[i] = i;
        #endif

        #ifdef TEST2
        frame_pushback( txbuf, "01234567890123456789", 20 ); 
        #endif


        // if option is 0x00, then aloha send will not require ACK from the receiver. 
        // if you want to debugging this program alone without receiver node, then
        // suggest you use option 0x00.
        // the default setting is 0x01, which means ACK is required.
        //
		option = 0x01;

        while (1)
        {
            if (tinymac_send(mac, txbuf, option) > 0)
            {			
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
   
        tinymac_evolve( mac, NULL );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
        // attention: this long delay will occupy the CPU and it may lead to frame lossing.
        
		hal_delay(1000);

		//break;
	}

    frame_close( txbuf );
    tinymac_close( mac );
    cc2420_close( cc );
}

