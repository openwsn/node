


#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../../common/openwsn/rtl/rtl_slipfilter.h"
#include "../../../common/openwsn/svc/svc_sio_acceptor.h"

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif
//#define TEST_ACK_REQUEST

#define MAX_IEEE802FRAME154_SIZE                128


#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiIEEE802Frame154Descriptor m_desc;
TiUartAdapter               m_uart;
TiSioAcceptor               m_sac;
TiSlipFilter                m_slip;

char txbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY)];
char rxbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY)];
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
char tmpbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY)];
char rmpbuf_block[ IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY)];
#endif

void sendnode1(void);
//void sendnode2(void);

int main(void)
{
    sendnode1();
}

void sendnode1(void)
{
    TiCc2520Adapter * cc;
    TiFrame * txbuf;
    TiIEEE802Frame154Descriptor * desc;
    TiUartAdapter      * uart;
    TiSioAcceptor      * sac;
    TiSlipFilter       * slip;

    uint8 i, first, seqid, option, len;
    char * ptr;

    TiIoBuf *buf_1;
    TiIoBuf *buf_2;
    TiIoBuf *buf_3;
    TiIoBuf *buf_4;
    seqid = 0;

    led_open();

    led_on( LED_ALL);
    hal_delay( 500 );
    led_off( LED_ALL );
    /*
    halUartInit( 9600,0);
    cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
    cc2520_open( cc, 0, NULL, NULL, 0x00 );
    cc2520_setchannel( cc, DEFAULT_CHANNEL );
    cc2520_rxon( cc );							//Enable RX
    cc2520_enable_addrdecode( cc );					//使能地址译码
    cc2520_setpanid( cc, PANID );					//网络标识
    cc2520_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识
    cc2520_enable_autoack( cc );
     */
    buf_1 = iobuf_construct(( void *)(&txbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY) );
    buf_2 = iobuf_construct( (void *)(&rxbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY) );
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    buf_3 = iobuf_construct( (void *)(&tmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
    buf_4 = iobuf_construct( (void *)(&rmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
#endif
    uart = uart_construct( ( void*)(&m_uart),sizeof(m_uart));
    uart = uart_open(uart,2,9600,8,1,0);

    slip = slip_filter_open( (void *)(&m_slip),sizeof( m_slip));

    sac = sac_construct( (void *)(&m_sac),sizeof(m_sac));//todo for testing
    sac = sac_open( sac,slip,uart);//sac = sac_open( (void *)(&m_sac),sizeof( m_sac),slip,uart);

    sac->txbuf = buf_1;
    sac->rxbuf = buf_2;

#ifdef SIO_ACCEPTOR_SLIP_ENABLE
   sac->rmpbuf = buf_4;
   sac->tmpbuf = buf_3;
#endif

    desc = ieee802frame154_open( &m_desc );
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

    option = 0x00;

    while(1)  
    {
        
        frame_reset( txbuf,3,20,0);
      
        ptr = frame_startptr( txbuf);

        for ( i = 0;i< 6;i++)
            ptr[i] = i;
        frame_skipouter( txbuf,12,2);
        desc = ieee802frame154_format( desc, frame_startptr( txbuf), frame_capacity( txbuf ), 
            FRAME154_DEF_FRAMECONTROL_DATA ); 
        rtl_assert( desc != NULL );
        ieee802frame154_set_sequence( desc, seqid); 
        ieee802frame154_set_panto( desc,  PANID );
        ieee802frame154_set_shortaddrto( desc, REMOTE_ADDRESS );
        ieee802frame154_set_panfrom( desc,  PANID );
        ieee802frame154_set_shortaddrfrom( desc, LOCAL_ADDRESS );
        frame_setlength( txbuf,20);
        first = frame_firstlayer( txbuf);

        //len = cc2420_write(cc, frame_layerstartptr(txbuf,first), frame_layercapacity(txbuf,first), option);
       // len = cc2520_write(cc, frame_layerstartptr(txbuf,first), frame_length( txbuf), option);
/*
        if( len)
        {
            led_toggle( LED_RED);
            seqid++;
        }
        */

        //uart_write(uart,frame_layerstartptr(txbuf,first),frame_length( txbuf),0);

        //led_toggle( LED_RED);
        len = sac_recv(sac,txbuf,0);
       if ( len)//todo for testing
       {
           // ptr = frame_startptr(txbuf);

          // for ( i=0;i<len;i++)
          // {
               // USART_Send(ptr[i]);
          // }

           // USART_Send( 0xff);
            // USART_Send(0xff);
          //sac->txbuf->length = 0;//todo for testing
          //sac->rxbuf->length =0;//todo for testing
          led_toggle( LED_RED);
           sac_send( sac,txbuf,0);
         
       }
       
        sac_evolve(sac,NULL);

        //len = uart_read( sac->device,buf,40,0);
        //for ( i=0;i<len;i++)
        //{
           // USART_Send( buf[i]);
       // }


    }
}
