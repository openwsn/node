
/* the following are global macro definitions and should be effective for all header files */


#define CONFIG_TEST_LISTENER  
#undef  CONFIG_TEST_LISTENER  

#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

#define CONFIG_PANID			0x0001
#define CONFIG_LOCAL_ADDRESS	0x02
#define CONFIG_REMOTE_ADDRESS	0x00
#define CONFIG_CHANNEL		11

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 3
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 1


#include "apl_foundation.h"
#include "../../common/openwsn/hal/opennode2010/cm3/core/core_cm3.h"
#include "../../common/openwsn/hal/opennode2010/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_lightqueue.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../common/openwsn/svc/svc_nio_tinymac.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"

/* the following are private macro definitions and should be effective only inside this module */


#define BROADCAST_ADDRESS			0xFFFF

#define MAX_IEEE802FRAME154_SIZE    128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)

TiIEEE802Frame154Descriptor m_meta;

static TiFrameRxTxInterface         m_rxtx;
static TiTinyMAC                    m_tinymac;
static char                         m_rxbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                         m_nacmem[NAC_SIZE];



static void tinymac_recvnode(void);

int main(void)
{
	tinymac_recvnode();
}

void tinymac_recvnode(void)
{
    TiCc2520Adapter * cc;
    TiFrameRxTxInterface * rxtx;
    TiTinyMAC * mac;
	TiNioAcceptor * nac;
	TiFrame * rxbuf;
	char * pc;
	int i;
	int len;
	char * msg = "welcome to svc_nio_tinymac recv test...";
    
	len = 0;
//	bool ret ;
	__disable_irq();
	halUartInit( 9600,0);
	led_open();
	led_on( LED_RED);
	hal_delay( 500 );
	led_off( LED_RED );
    
	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	mac = tinymac_construct( (char *)(&m_tinymac), sizeof(TiTinyMAC) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );

	cc = cc2520_open(cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2520_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );

	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	tinymac_open( mac, rxtx, nac, CONFIG_CHANNEL, CONFIG_PANID, CONFIG_LOCAL_ADDRESS,NULL, NULL, 0x01);
	hal_assert( nac != NULL ); 

	rxbuf = frame_open( (char*)(&m_rxbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	__enable_irq();

	/* Wait for listener action. The listener function will be called by the TiCc2420Adapter
	 * object when a frame arrives */
    
	/* Query the TiCc2420Adapter object if there's no listener */
	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{	
	    frame_reset( rxbuf, 3, 20, 0 );
		len = tinymac_recv( mac, rxbuf, 0x00 );
		//len = nac_recv(  mac->nac, rxbuf, 0x00);

		//len = mac->rxtx->recv( rxtx->provider, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00 );
		if (len > 0)
		{
			frame_moveouter( rxbuf);
			pc = frame_startptr( rxbuf);
			for ( i=0;i< len;i++)
			{
				USART_Send( (uint8)( pc[i]));
			}
			frame_moveinner( rxbuf);
			led_toggle( LED_RED );
		}

		nac_evolve( mac->nac, NULL);
		//hal_delay( 1000);	
	}
	#endif

    //frame_close( rxbuf );
    //tinymac_close( mac );
   // cc2420_close( cc );
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



