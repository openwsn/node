/*******************************************************************************
 * aloha_send
 * This project implements a frame sending node based on TiAloha component. This
 * component is in module "svc_nio_aloha". 
 * 
 * @state
 *  - compile passed. tested. released.
 * @modified by openwsn on 2010.10.12
 *  - complied successfully. The sniffer have received the data successfully
 * @modified by Jiang Ridong and ZhangWei(TongJi University) in 2011.04
 *  - Revised
 ******************************************************************************/

#define CONFIG_NAC_RXQUE_CAPACITY 1
#define CONFIG_NAC_TXQUE_CAPACITY 1

#include "apl_foundation.h"
#include "openwsn/hal/hal_mcu.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_timer.h"
#include "openwsn/svc/svc_nio_acceptor.h"
#include "openwsn/svc/svc_nio_aloha.h"

#define CONFIG_DEBUG

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define CONFIG_ALOHA_PANID				        0x0001
#define CONFIG_ALOHA_LOCAL_ADDRESS		        0x01
#define CONFIG_ALOHA_REMOTE_ADDRESS		        0x02
#define CONFIG_ALOHA_CHANNEL                    11

#define CONFIG_SENDING_INTERVAL           		1000

#define MAX_IEEE802FRAME154_SIZE                128

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NAC_RXQUE_CAPACITY,CONFIG_NAC_TXQUE_CAPACITY)

// Define the states of this application
#define  INIT_STATE                            	0
#define  WAIT_RESPONSE_STATE                    1

#define VTM_RESOLUTION 5

#define UART_ID 1

#define CONFIG_MIN_SENDING_INTERVAL 1000
#define CONFIG_MAX_SENDING_INTERVAL 1000
#define CONFIG_RESPONSE_TIMEOUT 500

static TiUartAdapter m_uart;                                     
static TiTimerAdapter m_timer2;
static TiTimerAdapter m_timer3;

static TiFrameRxTxInterface m_rxtx;
static char m_nacmem[NAC_SIZE];
static TiAloha m_aloha;
static char m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_mactxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiCc2520Adapter m_cc;


static void _aloha_sendnode(void);
static void _init_request( TiFrame * txbuf, uint16 addrfrom, uint16 addrto, uint8 seqid );
static bool _match( uint16 addrfrom, uint16 addrto, uint8 seqid, TiFrame * response );

int main(void)
{
	_aloha_sendnode();
    return 0;
}

void _aloha_sendnode(void)
{   
    char * msg = "welcome to aloha sendnode...\r\n";
    TiCc2520Adapter * cc;
    TiUartAdapter * uart;
    TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
	TiAloha * mac;
	TiTimerAdapter * timer2;
    TiTimerAdapter * timer3;

	TiFrame * txbuf;
	TiFrame * rxbuf;
    TiFrame *mactxbuf;

	uint8 seqid, option,len;
	uint8 failed ;
	uint8 state;
	uint16 sendcount, succeed;
    int i;
    char * pc;

	// Initialize the hardware. You can observe the LED flash and UART welcome
	// string to decide whether the application is started successfully or not.
    seqid =0;
    failed = 0;
	
	target_init();
	led_open();
	led_on( LED_ALL );
	hal_delayms( 500 );
	led_off( LED_ALL );

    //halUartInit(9600,0);
    uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);
	rtl_init( uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar_wait, hal_assert_report );
	dbc_mem( msg, strlen(msg) );

	// Construct objects on specified memory blocks. This step should be success 
	// or you may encounter unexpected mistake or behaviors.
	
	cc = cc2520_construct( (char *)(&m_cc), sizeof(TiCc2520Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	timer2 = timer_construct( (void *)(&m_timer2), sizeof(m_timer2) );
    timer3 = timer_construct( (void *)(&m_timer3), sizeof(m_timer3) );
	
	
	// Initialize the virtual timer 0, 1 and 2. Virtual timer 0 is used by the mac
	// component. By default we use hardware timer2 here. But you should be able 
	// to switch to timer0.
	//
	// @attention: To avoid timer conflictions. Generally, the RTC timer is often 
	// used by the osx kernel (not mandatory).
	
	timer2 = timer_open( timer2, 2, NULL, NULL, 0x00 );
    timer3 = timer_open( timer3, 3, NULL, NULL, 0x00 );	
	
	cc2520_open( cc, 0, NULL, NULL, 0x00 );
	
    rxtx = cc2520_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );

	// Open the mac object for further R/W operation.
	
    nac = nac_open( nac, rxtx, CONFIG_NAC_RXQUE_CAPACITY, CONFIG_NAC_TXQUE_CAPACITY);
	mac = aloha_open( mac, rxtx, nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS,timer2, NULL, NULL, 0x00 );

    mactxbuf = frame_open( (char*)(&m_mactxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

    mac->txbuf = mactxbuf;

    cc2520_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2520_rxon( cc );							            // enable RX mode
	cc2520_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2520_enable_autoack( cc );
    //cc2420_enable_addrdecode( cc );

    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    
	sendcount = 0;
	succeed = 0;
	seqid = 1;
	option = 0x01; // request ack

	state = INIT_STATE;

	
	timer_setinterval( timer3, 1000, 7999 );
	timer_start( timer3 );
	
	while (1)
	{
		switch (state)  
		{
		case INIT_STATE:
			if (timer_expired(timer3))
			{  
				timer_start( timer3 );
				
				_init_request( txbuf, CONFIG_ALOHA_LOCAL_ADDRESS, CONFIG_ALOHA_REMOTE_ADDRESS, seqid );
				if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, option) > 0)
				{  
					//seqid ++;
					sendcount ++;
					state = WAIT_RESPONSE_STATE;
				}
				seqid ++;

			}
			break;
		
		case WAIT_RESPONSE_STATE:
			frame_reset( rxbuf,3,20,0);
			len = aloha_recv( mac, rxbuf, 0x00 );
			if (len > 0)
			{   
				if (_match(CONFIG_ALOHA_LOCAL_ADDRESS, CONFIG_ALOHA_REMOTE_ADDRESS, (seqid-1), rxbuf))
				{
					succeed ++;
                    pc = frame_startptr( rxbuf);
                    for ( i=0;i<len;i++)
                    {
                        uart_putchar(uart, pc[i]);
                    }
					state = INIT_STATE;
					led_toggle(LED_RED);
				}
				
			}
			
			if (timer_expired(timer3))
			{
				timer_start( timer3 );
				_init_request( txbuf, CONFIG_ALOHA_LOCAL_ADDRESS, CONFIG_ALOHA_REMOTE_ADDRESS, seqid );
				if (aloha_send(mac, CONFIG_ALOHA_REMOTE_ADDRESS, txbuf, option) > 0)
				{
					//seqid ++;
					sendcount ++;
					state = WAIT_RESPONSE_STATE;
				}
				seqid ++;
			}
			break;
		}
		/*
		dbc_string( " seqid = " );
		dbc_putchar( seqid );

		dbc_string( " total sendcount = " );
		dbc_putchar( sendcount );
		// dbc_n16toa( sendcount );
		
		dbc_string( " success count = " );
		dbc_putchar( succeed );*/

		aloha_evolve( mac, NULL );
	}
	
    frame_close( txbuf );
	aloha_close( mac );
    cc2520_close( cc );
}

void _init_request( TiFrame * txbuf, uint16 addrfrom, uint16 addrto, uint8 seqid )
{
	char * ptr;
	uint8 i;
	// todo
	//因为aloha_recv（）函数中的skipinner会使前两个字节清零，所以先将前两个字节保留起来。
	frame_reset( txbuf, 3, 20, 0 );
	ptr = frame_startptr( txbuf );

   //*ptr ++ = seqid;

	ptr[3] = seqid;

	for (i=4; i < 6; i++)//for (i=4; i<frame_capacity(txbuf); i++)
		ptr[i] = 9;
	frame_setlength( txbuf,6);		
	#ifdef CONFIG_DEBUG
	// _output_frame( frame );
	#endif
}

bool _match( uint16 addrfrom, uint16 addrto, uint8 seqid, TiFrame * response )
{
    TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;
	bool ret;
	char * ptr;

	ret = false;
	frame_moveouter( response );
	desc = ieee802frame154_open( &(m_desc) );
	if (ieee802frame154_parse( desc, frame_startptr(response), frame_capacity(response)))
	{
		if (ieee802frame154_shortaddrfrom(desc) != addrto) 
		{
			ret = false;
		}
		else if (ieee802frame154_shortaddrto(desc) != addrfrom)
		{
			ret = false;
		}
		else
		{
			ret = true;
		}

		//这样得到的seqid应该是echo中aloha的seqid而不是send的seqid
		/*
		else if (*((char*)ieee802frame154_msdu(desc)) != seqid)
		{
			ret = false;
		}
		*/
		frame_moveinner( response);
		ptr = frame_startptr( response);
		if ( ptr[3]!= seqid)
		{
			ret = false;
		}
	}

	return ret;
}





