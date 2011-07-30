/*****************************************************************************
 * delaytest modified from cc2420tx used in cc2420echo
 * This module will send txbufs periodically and try to receive the replied one.
 * The sending and receiving information will be output through UART.
 *
 * @author zhangwei in 2009.07
 *
 * @modified by zhangwei on 20070418
 *	- just modify the format of the source file and including files
 * 
 * @modified by zhangwei in 2009.07
 *	- ported to ICT GAINZ platform (based on atmega128)
 * @modified by Yan-Shixing in 2009.07
 *	- correct bugs and tested. now the two send functions sendnode1() and sendnode2()
 *    are all work success.
 * @modified by zhangwei in 200908
 *	- improve the simple sender in the past. Now the program will switch to RX 
 *    mode after TX in order to receive the txbuf replyied by echo node.
 * @modified by sunqiang on 20091125
 *  - timer added to measure the transmit delay
 ****************************************************************************/ 
#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_timer.h"
#include "../../common/openwsn/svc/svc_nio_acceptor.h"
#include "../../common/openwsn/rtl/rtl_dumpframe.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"

#ifdef CONFIG_DEBUG
    #define GDEBUG
#endif

#define CONFIG_ACK_ENABLE
//#undef  CONFIG_ACK_ENABLE

#define PANID				0x0001
//todo往两个节点里烧程序时，LOCAL_ADDRESS与REMOTE_ADDRESS应互换，这样才能收到彼此发送的信息。
#define LOCAL_ADDRESS		0x0001   
#define REMOTE_ADDRESS		0x0002
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define VTM_RESOLUTION      7
#define VTI_INTERVAL        500
#define VTI_SCALE_COUNTER   2
#define VTI_OPTION          0x01

#define MAX_IEEE802FRAME154_SIZE                128//todo

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)//todo

static TiCc2420Adapter		g_cc;
static TiUartAdapter		g_uart;
static TiTimerAdapter       g_timeradapter;
static TiFrameRxTxInterface                       m_rxtx;//todo
static TiTimerManager       g_vtm;
static char m_nacmem[NAC_SIZE];//todo
static char                                     n_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];//todo
static char                                     n_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];//todo



TiIEEE802Frame154Descriptor desc;//todo

static void delaytest(void);
//static void _output_opentxbuf( TiOpenFrame * opf, TiUartAdapter * uart );
void vti_listener( void * vtmptr, TiEvent * e );
void _put_delay();

int main(void)
{
	delaytest();
}

void delaytest(void)
{
    //char opfmem[OPF_SUGGEST_SIZE];
    TiCc2420Adapter * cc;
	//TiUartAdapter * uart;
	TiNioAcceptor * nac;
	//TiOpentxbuf * opf;
	TiFrame * txbuf;
	TiFrame * rxbuf;
	TiFrameRxTxInterface * rxtx;

	char * pc;

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti;

	char * msg = "welcome to delay test...";
	uint8 i, total_length, seqid=0, option, len;
    uint16 fcf;
	uint16 scale_counter_now, interval_now, delay_time;
	
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );
	//dbo_open(0, 38400);

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	//uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	//uart_open( uart, 0, 38400, 8, 1, 0x00 );
	//uart_write( uart, msg, strlen(msg), 0x00 );
	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
	dbc_putchar( 0xF0 );
	dbc_mem( msg, strlen(msg) );

   // opf = opf_construct( (void *)(&opfmem), sizeof(opfmem),  OPF_DEF_txbufCONTROL_DATA, OPF_DEF_OPTION );//todo
    //opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_DEF_txbufCONTROL_DATA, OPF_DEF_OPTION );//todo
	txbuf = frame_open( (char*)(&n_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 25 );
	rxbuf = frame_open( (char*)(&n_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );


	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_enable_addrdecode( cc );				 //使能地址译码
	cc2420_setpanid( cc, PANID );				 //网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS ); //网内标识

	#ifdef CONFIG_ACK_ENABLE
	cc2420_enable_autoack( cc );
	#endif

	hal_enable_interrupts();

	//Initialization of timer
	timeradapter = timer_construct( (void *)(&g_timeradapter), sizeof(g_timeradapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	vtm = vtm_construct( (void*)&g_vtm, sizeof(g_vtm) );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );

	vti = vtm_apply( vtm );
	hal_assert( vti != NULL );

	nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);//todo
	ieee802frame154_open( &(desc) );//todo

	vti_open( vti, vti_listener, vti );
	vti_setscale( vti, VTI_SCALE_COUNTER );
	vti_setinterval( vti, VTI_INTERVAL, VTI_OPTION );

	while(1) 
	{  
		frame_reset(txbuf, 3, 20, 25);

#define TEST1

#ifdef TEST1
		pc = frame_startptr( txbuf );
		for (i=0; i<frame_capacity(txbuf); i++)
			pc[i] = i;
#endif


/**********************************************************************************************************///todo
     	TiIEEE802Frame154Descriptor * desc_1;
		frame_skipouter( txbuf, 12, 2 );

		desc_1 = ieee802frame154_format( &(desc), frame_startptr(txbuf), frame_capacity(txbuf), FRAME154_DEF_FRAMECONTROL_DATA  );
		rtl_assert( desc_1 != NULL );

		ieee802frame154_set_sequence( desc_1, seqid++ );
		ieee802frame154_set_panto( desc_1, PANID  );
		ieee802frame154_set_shortaddrto( desc_1, REMOTE_ADDRESS );
		ieee802frame154_set_panfrom( desc_1, PANID  );
		ieee802frame154_set_shortaddrfrom( desc_1, LOCAL_ADDRESS);

		
		frame_moveinner( txbuf );

/************************************************************************************************************/
		TiIEEE802Frame154Descriptor * desc_2;
		frame_skipouter( rxbuf, 12, 2 );

		desc_2 = ieee802frame154_format( &(desc), frame_startptr(rxbuf), frame_capacity(rxbuf), FRAME154_DEF_FRAMECONTROL_DATA  );
		rtl_assert( desc_1 != NULL );

		ieee802frame154_set_sequence( desc_2, seqid++ );
		ieee802frame154_set_panto( desc_2, PANID  );
		ieee802frame154_set_shortaddrto( desc_2, REMOTE_ADDRESS );
		
		ieee802frame154_set_panfrom( desc_2, PANID  );
		ieee802frame154_set_shortaddrfrom( desc_2, LOCAL_ADDRESS);
		


		frame_moveinner( rxbuf );
/**************************************************************************************************************/
		#ifdef CONFIG_ACK_ENABLE
		option = 0x01;
		#else
		option = 0x00;
		#endif
        txbuf->option = option;//todo
		rxbuf->option = option;//todo
		//start the timer
		vti_start( vti );
		dbo_putchar('A');

		// try send a txbuf
		while (1)
        {
            //todo len = cc2420_send(cc, (char*)(opf_buffer(opf)), opf_datalen(opf), option);

			len = nac_send( nac,txbuf,txbuf->option );
			if (len > 0)
            {
				dbc_putchar(0xA0);
				led_toggle( LED_RED );
				              
				
				dbc_putchar( 'T' );
				//_output_opentxbuf( txbuf, NULL );
				 ieee802frame154_dump( txbuf);
                break;
            }
			else
			{
				led_toggle( LED_GREEN);
				nac_evolve( nac,NULL);
			}
            hal_delay(1000);
        }

		dbc_putchar('B');

		// try receive the txbuf replied from the echo node
		while (1)
        {
            //todo len = cc2420_recv(cc, (char*)(opf_buffer(opf)), 127, 0x00 );
			len = nac_recv( nac, rxbuf,rxbuf->option);
			if (len > 0)
            {
				led_on( LED_YELLOW );
				hal_delay( 500 );               
				led_off( LED_YELLOW );
				dbc_putchar( 0xff );
				dbc_putchar( 0xff);//todo for testing
				//_output_opentxbuf( opf, NULL );
				 ieee802frame154_dump( rxbuf);
				 dbc_putchar( 0xee);//todo for testing
				 dbc_putchar( 0xcc);
                break;
            }
            hal_delay(10);
        }
		
		cc2420_evolve( cc );
		
		dbo_putchar('C');

		hal_enable_interrupts();
		//while(1){}//todo这一句有什么用？

		//Put the delay time
		scale_counter_now=vti->scale_counter;
		interval_now=vti->interval;

		delay_time=VTM_RESOLUTION*(VTI_INTERVAL*VTI_SCALE_COUNTER-scale_counter_now*interval_now);
		dbo_uint16(delay_time);

		// attention
        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value and eliminate all other hal_delay().
		hal_delay( 500 );
	}
}

/*
void _output_opentxbuf( TiOpenFrame * opf, TiUartAdapter * uart )
{
	if (opf->datalen > 0)
	{
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (!opf_parse(opf, 0))
		{
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0] );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\n' );
	}
}
*/
void vti_listener( void * vtmptr, TiEvent * e )
{
	//led_toggle( LED_RED );
}


/*
		#ifdef CONFIG_ACK_ENABLE
        //fcf = OPF_DEF_FRMECONTROL_DATA;             // 0x8821; 
		txbuf->option = OPF_DEF_FRAMECONTROL_DATA;
		#else
        //fcf = OPF_DEF_txbufCONTROL_DATA_NOACK;       // 0x8801;
		txbuf->option = OPF_DEF_FRAMECONTROL_DATA_NOACK;
		#endif

		total_length = 30;					    
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, PANID );
		opf_set_shortaddrto( opf, REMOTE_ADDRESS );
		opf_set_panfrom( opf, PANID );
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );
		for (i=0; i<opf->msdu_len; i++)
			opf->msdu[i] = i;


*/


