/*****************************************************************************
 * this test demostrate how to send a frame using TCc2420 adapter object
 *
 * @author HuangHuan in 2005.12
 * @modified by MaKun on 2007-04-18
 *	- test sending/TX function of cc2420 driver
 *
 * @modified by zhangwei on 20070418
 *	- just modify the format of the source file and including files
 * 
 * @modified by zhangwei on 20070701
 *	- port to OpenNode-3.0 hardware
 *	- and speed the transimisson rate. 
 * @modified by zhangwei in 2009.07
 *	- ported to ICT GAINZ platform (based on atmega128)
 ****************************************************************************/ 

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

#define PANID				0x0001
#define LOCAL_ADDRESS		35   // or 0
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			127
#define DEFAULT_CHANNEL     11

static TiCc2420Adapter g_cc;
static TiUartAdapter	g_uart;
static char *          g_txframebuf[130];
static TiOpenFrame *   g_txframe;

void sendnode(void);

int main(void)
{
	sendnode1();
	// sendnode2();
}

void sendnode1(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * txframe;
	char * msg = "welcome to sendnode...";
	uint8 frame[BUF_SIZE];
	uint8 len, j;
    TiDataFrameIndex * fidx;
    uint16 fcf;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	txframe = opf_construct( (void*)(&g_txframe), sizeof(g_txframe) );
	hal_enable_interrupts();
 
	while(1) 
	{
        fcf = 0x0000;
        len = 60;
        fidx = opf_as_data( txframe, len, fcf, opf_rawdata(txframe) );
        *fidx->sequence = 33;
        *fidx->panid = PANID;
        *fidx->shortaddrto = REMOTE_ADDRESS;
        *fidx->panfrom = PANID;
        *fidx->shortaddrfrom = LOCAL_ADDRESS;
        *fidx->data[0] = 0x01;
        *fidx->data[1] = 0x02;
        *fidx->data[2] = 0x03;
        *fidx->data[3] = 0x04;
        *fidx->data[4] = 0x05;
        *fidx->data[5] = 0x06;

        while (1)
        {
            if (cc2420_sendframe(cc, &frame[0], 60) > 0)
            {
                led_toggle( LED_RED );
                uart_write( &g_uart, '*' );
                break;
            }
            hal_delay(100);
        }
		
		cc2420_evolve( cc );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay(1000);


		/*
		len = cc2420_recvframe( cc, &buf[0], 127 );
		if (len > 6)
		{
			led_toggle();
			uart_write( uart, &buf[0], len, 0x01 );
		}
		*/

		/*
		len = uart_read( g_uart0, &buf, 127 );
		if (len > 0)
		{
		   if buf[0] == 0
			 cc2420_write( &buf, len );
		   else
			 execute( &buf, len );
		}
		*/
		cc2420_evolve( cc );

		hal_delay(1000);
	}
}

void sendnode2(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * txframe;
	char * msg = "welcome to sendnode...";
	uint8 frame[BUF_SIZE];
	uint8 len, j;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	hal_enable_interrupts();

	while(1) 
	{
		memset( &frame[0], 0x00, sizeof(frame) );
		frame[0]=0x88;
		frame[1]=0x41;
		frame[2]=0x01;
		frame[3]=0x00;
		frame[4]=0x01;
		frame[5]=0x00;
		frame[6]=0x01;
		frame[7]=0x00;
		frame[8]=0x01;
		frame[9]=0x00;
		frame[10]=0x01;
		for(j=11;j<128;j++)
		{
			frame[j]=0x00;
		}

        while (1)
        {
            if (cc2420_sendframe(cc, &frame[0], 60) > 0)
            {
                led_toggle( LED_RED );
                uart_write( &g_uart, '*' );
                break;
            }
            hal_delay(100);
        }

		cc2420_evolve( cc );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay(1000);
	}
}


/*
void  main(void)
{
  HAL_SET_PIN_DIRECTIONS();
  Leds_greenOn();
  Leds_redOn();
  Leds_yellowOn();
  IoInit();
  
  //HAL_sched_init();
  cc2420_init();
  hal_atomic_enable_interrupt();
  //address=my_short_address;
  
  cc2420_setchnl(11);
  
  cc2420_setrxmode();				//Enable RX
  cc2420_enableAddrDecode();		//使能地址译码
  //cc2420_setpanid(0x0001);			//网络标识
  //cc2420_setshortaddress(address);	//网内标识
  cc2420_enablefifop();				//启用fifop中断
  
  char buf[127];
  //uint16 source_addr;
  
  while(1) 
  {
  	//HAL_sleep();
	uint8_t j;
	uint8_t packet[127];
	packet[0]=0x88;
	packet[1]=0x41;
	packet[2]=0x01;
	packet[3]=0x00;
	packet[4]=0x01;
	packet[5]=0x00;
	packet[6]=0x01;
	packet[7]=0x00;
	packet[8]=0x01;
	packet[9]=0x00;
	packet[10]=0x01;
	for(j=11;j<128;j++)
	{
		packet[j]=0x00;
	}
	cc2420_sendframe(uint8_t len, uint8_t *packet);
	//
	len = cc2420_receiveframe(uint8_t *msg);
	if (len > 0)
	{
	   //uart_write( g_uart0, &buf, len );
	}
	//
	len = uart_read( g_uart0, &buf, 127 );
	if len > 0
	{
	   if buf[0] == 0
	     cc2420_write( &buf, len );
	   else
	     execute( &buf, len );
	}
}
*/

/*
void cc2420tx_test( void )
{
    char * welcome = "cc2420tx started...\r\n";
	TiUartAdapter * uart;
	TiCc2420Adapter * cc;

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	uart_construct( (void *)g_uart, sizeof(TiUartAdapter) );
	cc2420_construct( (void *)g_cc, sizeof(TiCc2420Adapter) );

	uart = uart_open( uart, 0, 38400, 8, 1, NULL, NULL );
	uart_write( uart, welcome, strlen(welcome), 0x00 );

	cc2420_open( cc, 0, cc2420_listener, cc, 0x00 );
	cc2420_setpanid( cc, PANID );
	cc2420_setshortaddr( cc, LOCAL_ADDRESS );
	

    memset( (char*)(&txframe), 0x00, sizeof(txframe) );
    memset( (char*)(&txbuf), 0x00, sizeof(txbuf) );

    for (n = 0; n < 10; n++) 
    {
        txframe.payload[n] = 2;
        txbuf[10 + n] = 2;  //?
    }
    txframe.payload[2] = 1;  

    cc2420_open( g_cc2420 );

    while (TRUE) 
    {    
        led_off( LED_RED );
		
        // test section one: 
        // transmit using TOpenFrame based interface: cc2420_write
        // every time a frame sent, the program will dump a short message to UART for display 
        // and twinkle the LED for 1 time.
        //
        #ifdef CONFIG_GENERAL_RW
        uart_write( g_uart, "sending...\r\n", 12, 0x00 );
		
        //txframe.length = 10 + 11;
        txframe.length = 50; // between 1 and 0x127
        txframe.panid = PANID;
        txframe.nodeto = REMOTE_ADDRESS; //can be arbitray address for testing the sniffer
        txframe.nodefrom = LOCAL_ADDRESS;
        txframe.payload[0] %= 8;
        txframe.payload[0]++;

        length = cc2420_write( g_cc2420, &(txframe), 0 );
        if (length > 0)
        {
            uart_write( g_uart, "sent\r\n", 6, 0x00 );
            #ifdef GDEBUG
            uart_write( g_uart, "****", 4, 0x00 );
            uart_putchar( g_uart, length );
            uart_putchar( g_uart, '\r' );
            uart_putchar( g_uart, '\n' );
            #endif
            led_twinkle( LED_RED, 500 );
        }
        #endif
	  
        // test section two: 
        // transmit using char buffer based interface
        // @modified by zhangwei on 20070418
        // huanghuan seems test the following section. i cannot guartantee whether 
        // the next char buffer based interface can work properly.
        //
        #ifdef CONFIG_RAWFRAME_RW          
		txbuf[1] = txbuf[2] = txbuf[3] = 0;
		// set PANID, REMOTE_ADDRESS, LOCAL_ADDRESS. little endium
		txbuf[4] = 0x20; txbuf[5] = 0x24; 
		txbuf[6] = 0x78; txbuf[7] = 0x56;
		txbuf[8] = 0x34; txbuf[9] = 0x12; 
    
        txbuf[10]++;
        if (txbuf[10] >= 5) 
        {
            txbuf[10] = 1;
        }
        led_twinkle(LED_RED, 500);
        cc2420_rawwrite( g_cc2420, (char *)txbuf, 10 + 11,0);
        #endif

        hal_delay( 400 );
    }
	
    global_destroy();
    return;															
}

void cc2420_listener( void * ccptr, TiEvent * e )
{
    TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;
	char * msg = "cc2420listerner received event";

	uart_write( &g_uart, msg, strlen(msg), 0x00 );
}
*/
