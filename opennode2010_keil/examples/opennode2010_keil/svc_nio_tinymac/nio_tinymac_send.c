
#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
#define GDEBUG
#endif
#include "apl_foundation.h"
#include "openwsn/hal/opennode2010/cm3/core/core_cm3.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_lightqueue.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/svc/svc_nio_tinymac.h"
#include "openwsn/svc/svc_nio_acceptor.h"



#define CONFIG_PANID				        0xffff
#define CONFIG_LOCAL_ADDRESS		        0x01
#define CONFIG_REMOTE_ADDRESS		        0xffff
#define CONFIG_CHANNEL                    11

#define MAX_IEEE802FRAME154_SIZE                128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

static TiFrameRxTxInterface                     m_rxtx;
static TiTinyMAC                                m_tiny;

static char m_nacmem[NAC_SIZE];
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter                                 m_cc;

static void tinymac_sendnode(void);

int main(void)
{
	tinymac_sendnode();
	return 0;
}

void tinymac_sendnode( void )
{   
	TiCc2520Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiTinyMAC * mac;
	TiNioAcceptor * nac;
	TiFrame * txbuf;
	char * pc;

	char * msg = "welcome to tinymac sendnode...";
	uint8 i, seqid=0, option;
	__disable_irq();
	halUartInit( 9600,0);
	led_open();
	led_on( LED_RED);
	hal_delayms( 500 );
	led_off( LED_RED );
    
	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	mac = tinymac_construct( (char *)(&m_tiny), sizeof(TiTinyMAC) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
    
	cc = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	
	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	tinymac_open( mac, rxtx, nac, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, 0x01);
	hal_assert( nac != NULL ); 
    
	txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	__enable_irq();
	while(1) 
	{
		tinymac_setremoteaddress( mac, CONFIG_REMOTE_ADDRESS );

		//dbc_putchar(0x24);
		frame_reset(txbuf, 3, 20, 0);
    
#define TEST1

#ifdef TEST1
		pc = frame_startptr( txbuf );
		for (i=0; i<6; i++)
			pc[i] = i;
		frame_setlength( txbuf, 6 );
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
			if (tinymac_send(mac, txbuf, option) > 0)
			{
				USART_Send( 0xee);
				
				break;
			}
			else{
				USART_Send( 0xff);
				
			}
			hal_delayms(100);
		}
		
		tinymac_evolve( mac, NULL );

		
		nac_evolve( mac->nac, NULL );
      
        
		hal_delayms(1000);

		led_toggle( LED_RED);//for testing
	}

	frame_close( txbuf );
	tinymac_close( mac );
}

