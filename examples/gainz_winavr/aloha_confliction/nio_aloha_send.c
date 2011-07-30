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

#define CONFIG_NAC_RXQUE_CAPACITY 2
#define CONFIG_NAC_TXQUE_CAPACITY 1

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
#include "../../common/openwsn/hal/hal_timer.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"

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

#define CONFIG_MIN_SENDING_INTERVAL 1000
#define CONFIG_MAX_SENDING_INTERVAL 1000
#define CONFIG_RESPONSE_TIMEOUT 500

static TiTimerAdapter                          	m_timeradapter;
static TiTimerManager                          	m_vtm;

static TiCc2420Adapter		                    m_cc;
static TiFrameRxTxInterface                     m_rxtx;
static char m_nacmem[NAC_SIZE];
static TiAloha                                  m_aloha;
static char                                     m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                                     m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

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
    TiCc2420Adapter * cc;
    TiFrameRxTxInterface * rxtx;;
	TiNioAcceptor * nac;
	TiAloha * mac;
	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * mac_timer;
	TiTimer * tm;
	TiFrame * txbuf;
	TiFrame * rxbuf;

	char * msg = "welcome to aloha sendnode...";
	uint8 seqid=0, option,len;
	uint8 failed =0;
	uint8 state;
	uint16 sendcount, succeed;

	// Initialize the hardware. You can observe the LED flash and UART welcome
	// string to decide whether the application is started successfully or not.
	
	target_init();
	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );

	// Construct objects on specified memory blocks. This step should be success 
	// or you may encounter unexpected mistake or behaviors.
	
	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	timeradapter = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	
	hal_assert((cc != NULL) && (nac != NULL) && (mac != NULL));
	hal_assert((timeradapter != NULL ) && (vtm != NULL));

	// Initialize the virtual timer 0, 1 and 2. Virtual timer 0 is used by the mac
	// component. By default we use hardware timer2 here. But you should be able 
	// to switch to timer0.
	//
	// @attention: To avoid timer conflictions. Generally, the RTC timer is often 
	// used by the osx kernel (not mandatory).
	
	timeradapter = timer_open( timeradapter, 2, NULL, NULL, 0x01 );	
    vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );
	mac_timer = vtm_apply( vtm );
	vti_open( mac_timer, NULL, NULL );
	tm = vtm_apply( vtm );
	vti_open( tm, NULL, NULL );
	hal_assert((vtm != NULL) && (mac_timer != NULL) && (tm != NULL));

	// Open the transceiver object for further R/W operation.
	
	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	
    rxtx = cc2420_interface( cc, &m_rxtx );
    hal_assert( rxtx != NULL );

	// Open the mac object for further R/W operation.
	
    nac_open( nac, rxtx, CONFIG_NAC_RXQUE_CAPACITY, CONFIG_NAC_TXQUE_CAPACITY);
	aloha_open( mac, rxtx, nac, CONFIG_ALOHA_CHANNEL, CONFIG_ALOHA_PANID,
		CONFIG_ALOHA_LOCAL_ADDRESS, mac_timer, NULL, NULL, 0x00 );

    cc2420_setchannel( cc, CONFIG_ALOHA_CHANNEL );
	cc2420_setrxmode( cc );							            // enable RX mode
	cc2420_setpanid( cc, CONFIG_ALOHA_PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, CONFIG_ALOHA_LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_enable_autoack( cc );
    cc2420_enable_addrdecode( cc );

    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
    
	sendcount = 0;
	succeed = 0;
	seqid = 1;
	option = 0x01; // request ack

	state = INIT_STATE;

	vti_setscale( tm, 1 );
	vti_setinterval( tm, CONFIG_SENDING_INTERVAL, 0x00 );
	vti_start( tm );
	
	hal_enable_interrupts();
	while (1)
	{
		switch (state)  
		{
		case INIT_STATE:
			if (vti_expired(tm))
			{  
				vti_restart( tm, CONFIG_SENDING_INTERVAL,1, 0x00 );
				
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
					ieee802frame154_dump( rxbuf);
					succeed ++;
					state = INIT_STATE;
					led_toggle(LED_GREEN);
				}
				
			}
			
			if (vti_expired(tm))
			{
				vti_restart( tm, CONFIG_SENDING_INTERVAL,1, 0x00 );
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
    cc2420_close( cc );
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





