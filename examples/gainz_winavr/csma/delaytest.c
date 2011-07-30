/*****************************************************************************
 * delaytest modified from cc2420tx used in cc2420echo
 * This module will send frames periodically and try to receive the replied one.
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
 *    mode after TX in order to receive the frame replyied by echo node.
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
#include "../../common/openwsnn/hal/hal_targetboard.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_timer.h"

#ifdef CONFIG_DEBUG
    #define GDEBUG
#endif

#define CONFIG_ACK_ENABLE
//#undef  CONFIG_ACK_ENABLE

#define PANID				0x0001
#define LOCAL_ADDRESS		0x0002   
#define REMOTE_ADDRESS		0x0001
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define VTM_RESOLUTION      7
#define VTI_INTERVAL        500
#define VTI_SCALE_COUNTER   2
#define VTI_OPTION          0x01

static TiCc2420Adapter		g_cc;
static TiUartAdapter		g_uart;
static TiTimerAdapter       g_timeradapter;
static TiTimerManager       g_vtm;

static void delaytest(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );
void vti_listener( void * vtmptr, TiEvent * e );
void _put_delay();

int main(void)
{
	delaytest();
}

void delaytest(void)
{
    char opfmem[OPF_SUGGEST_SIZE];
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
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
	dbo_open(0, 38400);

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    opf = opf_construct( (void *)(&opfmem), sizeof(opfmem),  OPF_DEF_FRAMECONTROL_DATA, OPF_DEF_OPTION );
    opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_DEF_FRAMECONTROL_DATA, OPF_DEF_OPTION );


	cc2420_open( cc, 0, NULL, NULL, 0x00 );
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
	vtm = vtm_construct( (void*)&g_vtm, sizeof(g_vtm) );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );

	vti = vtm_apply( vtm );
	hal_assert( vti != NULL );
	vti_open( vti, vti_listener, vti );
	vti_setscale( vti, VTI_SCALE_COUNTER );
	vti_setinterval( vti, VTI_INTERVAL, VTI_OPTION );

	while(1) 
	{
		#ifdef CONFIG_ACK_ENABLE
        fcf = OPF_DEF_FRAMECONTROL_DATA;             // 0x8821;    
		#else
        fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK;       // 0x8801;  
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

		#ifdef CONFIG_ACK_ENABLE
		option = 0x01;
		#else
		option = 0x00;
		#endif

		//start the timer
		vti_start( vti );
		dbo_putchar('A');

		// try send a frame
		while (1)
        {
            len = cc2420_send(cc, (char*)(opf_buffer(opf)), opf_datalen(opf), option);
			if (len > 0)
            {
				dbo_putchar(0xA0);
				led_on( LED_RED );
				hal_delay( 500 );               
				led_off( LED_RED );
				dbo_putchar( 'T' );
				_output_openframe( opf, &g_uart );
                break;
            }
            hal_delay(10);
        }

		dbo_putchar('B');

		// try receive the frame replied from the echo node
		while (1)
        {
            len = cc2420_recv(cc, (char*)(opf_buffer(opf)), 127, 0x00 );
			if (len > 0)
            {
				led_on( LED_YELLOW );
				hal_delay( 500 );               
				led_off( LED_YELLOW );
				dbo_putchar( 'R' );
				_output_openframe( opf, &g_uart );
                break;
            }
            hal_delay(10);
        }
		
		cc2420_evolve( cc );
		
		dbo_putchar('C');

		hal_enable_interrupts();
		while(1){}

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


void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
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

void vti_listener( void * vtmptr, TiEvent * e )
{
	led_toggle( LED_RED );
}

