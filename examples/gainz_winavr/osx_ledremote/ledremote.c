/* ledremote
 * 该application将接收来自wireless luminance sensor的指令，调整led亮度
 * 形成一个环境光=>lum sensor => led light => 环境光的闭环演示
 * 
 * 该演示将用到简单的aloha mac协议，所以它也是一个演示如何使用mac层协议的例子
 *
 * 
 * @author zhangwei, yanshixing(TongJi University) on 20091012
 *	- first developed
 */


#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/osx/osx.h"

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define MAKEWORD(low,high) ((low & 0xFF) | (((uint16)(high)) << 8))

TiCc2420Adapter             g_cc;
TiUartAdapter	            g_uart;
char                        g_opfmem[OPF_SUGGEST_SIZE];


static void _ledremote(void);
static void _led_evolve( void * ccptr, TiEvent * e );
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );
void _led_tune( uint16 env_lum );

/*******************************************************************************
 * main
 ******************************************************************************/

int main(void)
{
	_ledremote();
}

void _ledremote(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
	char * msg = "welcome to _ledremote...";

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	osx_init();
	hal_init( osx_eventlistener, g_osx );
	//hal_setlistener( osx_post );

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	cc2420_open( cc, 0, _cc2420_listener, cc, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_enable_addrdecode( cc );				    //使能地址译码
	cc2420_setpanid( cc, PANID );				    //网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );    //网内标识
	cc2420_enable_autoack(cc);

    opf = opf_construct( (void *)(&g_opfmem), sizeof(g_opfmem) );
    opf_open( opf, OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	osx_attach( 1, _led_evolve, NULL );
	hal_enable_interrupts();
 	osx_execute();

	//while(1) {};
}

/*******************************************************************************
 * TiLedService
 * This's an standard service which can be attached to osx kernel.
 ******************************************************************************/

typedef struct{
}TiLedService;

TiLedService g_ledsvc;

void _led_evolve( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
    TiOpenFrame * opf = (TiOpenFrame *)(&g_opfmem[0]);
    uint8 len=0;

	while (1)
	{
		// If there're more than 1 frames pending inside cc2420 adapter object, then
		// you should repeat to call cc2420_read() until it returns 0. 
		//
		len = cc2420_read(cc, (char*)opf_buffer(opf), opf_size(opf), 0x00);
		if (len > 0)
			_output_openframe( opf , NULL );
		else 
			break;
	}
}

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	// if parse frame failed, then output the frame content through UART and dbo
	// and return immediately
	//
	if (!opf_parse(opf, 0))
	{
        dbo_n8toa( *opf->sequence );
		dbo_putchar( ':' );
		dbo_write( (char*)&(opf->buf[0]), opf->buf[0] );
		return;

	}

	_led_tune( MAKEWORD(opf->msdu[0], opf->msdu[1]) );
}

// 调节LED灯的亮度
// Input
//	env_lum   luminance of the environment
// Output 
//	None
//
void _led_tune( uint16 env_lum )
{
	if (env_lum < 0x010	0)
	{
        // todo
		led_off(LED_ALL);
		led_on(LED_RED);
		led_on(LED_YELLOW);
		led_on(LED_GREEN);
	}
	else if (env_lum < 0x0190)
	{
        // todo
		led_off(LED_ALL);
		led_on(LED_YELLOW);
		led_on(LED_GREEN);
	}
	else
	{
        // todo
		led_off(LED_ALL);
		led_on(LED_YELLOW);
	}

}












