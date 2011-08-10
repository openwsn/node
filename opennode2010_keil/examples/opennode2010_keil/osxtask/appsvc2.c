
/* appsvc2.c
 * This is the implementation file of TiAppService2. This file demonstrates how to 
 * develope an runnable service module.
 */

#include "asv_configall.h"
#include <string.h>
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "asv_foundation.h"
#include "appsvc2.h"

//TiCc2420Adapter		g_cc;
TiUartAdapter		g_uart;

TiAppService2 * asv2_open( TiAppService2 * svc, uint16 interval )
{
	memset( (void*)svc, 0x00, sizeof(TiAppService2) );
	svc->state = 0;
	svc->listener = NULL;
	svc->lisowner = NULL;
	svc->interval = interval;
    return svc;
}

void asv2_close( TiAppService2 * svc )
{
	return;
}

void asv2_setlistener( TiAppService2 * svc, TiFunEventHandler listener, void * lisowner )
{
	svc->listener = listener;
	svc->lisowner = lisowner;
}

void send(void)
{
   /*
	char opfmem[OPF_SUGGEST_SIZE];
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;


	char * msg = "send";
	uint8 i, total_length, seqid=0, option, len;
    uint16 fcf;

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    //opf = opf_construct(  );
	#ifdef TEST_ACK_REQUEST
    opf = opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_DEF_FRAMECONTROL_DATA_ACK, OPF_DEF_OPTION );
	#else
    opf = opf_open( (void *)(&opfmem), sizeof(opfmem), OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
	#endif

	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	#ifdef TEST_ACK_REQUEST
	cc2420_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK;             // 0x8821;    
	#else
	cc2420_disable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK;			// 0x8801;  
	#endif

	hal_enable_interrupts();


		total_length = 30;					     
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, PANID );
		opf_set_shortaddrto( opf, REMOTE_ADDRESS );
		opf_set_panfrom( opf, PANID );
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );

        */

		/* datalen should be opf->msdu_len */
    /*
		for (i=0; i<opf->msdu_len; i++)
			opf->msdu[i] = i;

		
		/*dbo_putchar(0x88);

		for (i=0; i<opf->buf[0]; i++)
		{
			dbo_putchar(opf->buf[i]);
		}*/
		
/*
		#ifdef TEST_ACK_REQUEST
		option = 0x01;
		#else
		option = 0x00;
		#endif

            if ((len=cc2420_write(cc, (char*)(opf_buffer(opf)), opf_datalen(opf), option)) > 0)
    
				led_off( LED_RED );
				hal_delay( 500 );
				led_on( LED_RED );
				hal_delay( 500 );     */         
            
}
void asv2_evolve( void * svcptr, TiOsxTaskHeapItem *item )
{
	USART_Send(0xff);//todo for testing
	//send();



	
}


