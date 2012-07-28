
/**
 * Test TiSioAcceptor which contains the framing mechanism based on serial communication.
 */
/*
#include "apl_configall.h"
#include <stdlib.h>
#include <string.h>
#include "apl_foundation.h"
*/
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../../common/openwsn/hal/hal_foundation.h"
#include "../../../common/openwsn/hal/hal_cpu.h"
#include "../../../common/openwsn/hal/hal_led.h"
#include "../../../common/openwsn/hal/hal_assert.h"
#include "../../../common/openwsn/hal/hal_cc2520.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/hal_debugio.h"
#include "../../../common/openwsn/rtl/rtl_slipfilter.h"
#include "../../../common/openwsn/svc/svc_sio_acceptor.h"
//
#include "../../../common/openwsn/rtl/rtl_ieee802frame154.h"

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif

#define MAX_IEEE802FRAME154_SIZE 128

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define FRAME_MEMSIZE (FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE))

static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

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

static void sio_acceptor_sender(void);
static void sio_acceptor_echo(void);
static void process( TiFrame * request, TiFrame * response );
static void init_request( TiFrame * f );
static void dumpframe( TiFrame * f );

int main(void)
{
    sio_acceptor_sender();
    //sio_acceptor_echo();
    return 0;
}

/**
 * This function periodically sends frame to the received through serial connection.
 */
void sio_acceptor_sender(void)
{
    //char m_txbuf[FRAME_MEMSIZE];
    //TiUartAdapter m_uart;
    //TiSioAcceptor m_sioacceptor;

	char type_id=0x55;	   
	char dev_id[4]={0x00,0x01,0x02,0x03};
	int seq_id=1;
    
	char tempmsg[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char powermsg[8]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
	float temmsg=27.1;
	float hummsg=80.1;

    TiFrame * txbuf;
    TiUartAdapter * uart;
    TiSioAcceptor * sio;
    TiSlipFilter       * slip;

    TiIoBuf *sio_buf_tx;
    TiIoBuf *sio_buf_rx;
    TiIoBuf *sio_buf_tmpx;
    TiIoBuf *sio_buf_rmpx;

    /* step 1: target board initialization */
     
    // targetboard_init();
    led_open();
    led_on( LED_ALL);
    hal_delay( 250 );
    led_off( LED_ALL );
    
    /* step 2: create necessary components, including TiUartAdapter, TiSioAcceptor, 
     * and TiFrame 
     */
    
    uart = uart_construct( ( void*)(&m_uart), sizeof(m_uart));
    //uart = uart_open(uart, 2, 9600, 8, 1, 0);
    uart = uart_open(uart, 0, 9600, 8, 1, 0);
    rtl_init( (void *)uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar, hal_assert_report );
    
    sio_buf_tx = iobuf_construct(( void *)(&txbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY) );
    sio_buf_rx = iobuf_construct( (void *)(&rxbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY) );
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio_buf_tmpx = iobuf_construct( (void *)(&tmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
    sio_buf_rmpx = iobuf_construct( (void *)(&rmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
#endif

    //sio = sac_open((void *)(&m_sioacceptor), sizeof(TiSioAcceptor), uart);

    slip = slip_filter_open( (void *)(&m_slip),sizeof( m_slip));

    sio = sac_construct( (void *)(&m_sac),sizeof(m_sac));
    sio = sac_open( sio,slip,uart);

    sio->txbuf = sio_buf_tx;
    sio->rxbuf = sio_buf_rx;

#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio->rmpbuf = sio_buf_rmpx;
    sio->tmpbuf = sio_buf_tmpx;
#endif

    txbuf = frame_open( (char*)(&m_txbuf), FRAME_MEMSIZE, 3, 20, 2 );
    
    while(1)  
    {
        // init_request( txbuf );
        //init_request( txbuf );

		iobuf_pushbyte(txbuf,type_id);
		iobuf_pushbyte(txbuf,dev_id,4,0);
		iobuf_pushbyte(txbuf,(char *)(&seq_id),sizeof(seq_id),0);
		seq_id++;
		iobuf_pushbyte(txbuf,tempmsg,8,0);
		iobuf_pushbyte(txbuf,(char *)(&temmsg),sizeof(float),0);
		iobuf_pushbyte(txbuf,(char *)(&hummsg),sizeof(float),0);

		for(i=0;i<5;i++)
		{
			iobuf_pushbyte(txbuf,(char *)(powermsg),8,0);
		}

        sac_send( sio, txbuf, 0x00 );
        
        #ifdef CONFIG_DEBUG
        dumpframe( txbuf );
        #endif
        
        // @modified by zhangwei on 2011.08.07
        // The following evolve is unnecessary for TiSioAcceptor sending in current 
        // implementation.
        sac_evolve( sio, NULL );
        hal_delayms( 500 );
    }
}

/**
 * This function sends every frame received from the serial connection back to their
 * original sender.
 */
void sio_acceptor_echo(void)
{
    //char m_txbuf[FRAME_MEMSIZE];
    //char m_rxbuf[FRAME_MEMSIZE];
    //TiUartAdapter m_uart;
    //TiSioAcceptor m_sioacceptor;
    
    TiFrame * rxbuf;
    TiFrame * txbuf;
    TiUartAdapter * uart;
    TiSioAcceptor * sio;
    TiSlipFilter  * slip;

    TiIoBuf *sio_buf_tx;
    TiIoBuf *sio_buf_rx;
    TiIoBuf *sio_buf_tmpx;
    TiIoBuf *sio_buf_rmpx;

    /* step 1: target board initialization */
     
    // targetboard_init();
    led_open();
    led_on( LED_ALL);
    hal_delay( 250 );
    led_off( LED_ALL );
    
    /* step 2: create necessary components, including TiUartAdapter, TiSioAcceptor, 
     * and TiFrame 
     */
    
    uart = uart_construct( ( void*)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, 2, 9600, 8, 1, 0);
    rtl_init( (void *)uart, (TiFunDebugIoPutChar)uart_putchar, (TiFunDebugIoGetChar)uart_getchar, hal_assert_report );

    sio_buf_tx = iobuf_construct(( void *)(&txbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY) );
    sio_buf_rx = iobuf_construct( (void *)(&rxbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY) );
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio_buf_tmpx = iobuf_construct( (void *)(&tmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
    sio_buf_rmpx = iobuf_construct( (void *)(&rmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
#endif


    //sio = sac_open((void *)(&m_sioacceptor), sizeof(TiSioAcceptor), uart);

    slip = slip_filter_open( (void *)(&m_slip),sizeof( m_slip));

    sio = sac_construct( (void *)(&m_sac),sizeof(m_sac));
    sio = sac_open( sio,slip,uart);

    sio->txbuf = sio_buf_tx;
    sio->rxbuf = sio_buf_rx;

#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio->rmpbuf = sio_buf_rmpx;
    sio->tmpbuf = sio_buf_tmpx;
#endif

    rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_MEMSIZE, 0, 0, 0 );
    txbuf = frame_open( (char*)(&m_txbuf), FRAME_MEMSIZE, 3, 20, 2 );
    
    while(1)  
    {
        if (frame_empty(rxbuf))
        {
            if (sac_recv(sio, rxbuf, 0x00) > 0)
            {
                process( rxbuf, txbuf );
                led_toggle( LED_RED);
            }
        }
        
        if (!frame_empty(txbuf))
        {
            if (sac_send(sio, txbuf, 0x00) > 0)
            {
                frame_clear(txbuf);
                frame_clear(rxbuf);
            }
        }
        
        sac_evolve( sio, NULL );
        hal_delayms( 5 );
    }
}

void process( TiFrame * request, TiFrame * response )
{
    #ifdef CONFIG_DEBUG
    dumpframe( request );
    #endif
    
    frame_totalcopyto( request, response );
    
    #ifdef CONFIG_DEBUG
    dumpframe( response );
    #endif
}

void init_request( TiFrame * f )
{
    const int CUR_HEADER_SIZE = 12; 
    const int CUR_TAIL_SIZE = 2;
    
    const int REQUEST_LENGTH = 6;

	TiIEEE802Frame154Descriptor m_desc, *desc;
	char * ptr;
	uint8 seqid;
	int i;

    rtl_assert(f != NULL);
    
    if (f != NULL)
    {
        // @modified by zhangwei on 2011.08.07
        // @attention
        // In the past, we usually do the following: 
        //      frame_reset(f, 2, 20, 0);
        // However, this will lead to frame_skipouter() later failure because there
        // are no space to allocate the frame tail (used for CRC). In previous versions, 
        // we doesn't check this so rigorously. Be sure to correct the frame_reset()
        // parameter values to guarantee the lower layer can be created successfully.
        
        // @attention: The frame must be opened before.
        // layer = 2, layer_start = 20, layer_capacity = REQUEST_LENGTH
        frame_reset(f, 2, 20, REQUEST_LENGTH);
        
        // place some test data into the frame
        ptr = frame_startptr(f);
        for ( i=0; i<frame_capacity(f); i++)
            ptr[i] = i;
        frame_setlength(f, frame_capacity(f));
        
        // move to the lower layer. if the layer doesn't exist, then create it.
        // frame_skipouter(...) equals frame_assemble(f, headersize, tailsize );
        
        #ifdef CONFIG_DEBUG        
        rtl_assert(frame_skipouter( f, CUR_HEADER_SIZE, CUR_TAIL_SIZE ) == true);
        #else
        frame_skipouter( f, CUR_HEADER_SIZE, CUR_TAIL_SIZE );
        #endif
        
        desc = ieee802frame154_open( &m_desc );
        desc = ieee802frame154_format( desc, frame_startptr(f), frame_capacity(f), 
            FRAME154_DEF_FRAMECONTROL_DATA ); 
        rtl_assert( desc != NULL );
        
        ieee802frame154_set_sequence( desc, seqid); 
        ieee802frame154_set_panto( desc,  PANID );
        ieee802frame154_set_shortaddrto( desc, REMOTE_ADDRESS );
        ieee802frame154_set_panfrom( desc,  PANID );
        ieee802frame154_set_shortaddrfrom( desc, LOCAL_ADDRESS );
        frame_setlength(f, frame_capacity(f));

        ieee802frame154_close(desc);
        frame_movefirst(f);
    }
}

void dumpframe( TiFrame * f )
{
    // dump the frame out for debugging
    return;
}
