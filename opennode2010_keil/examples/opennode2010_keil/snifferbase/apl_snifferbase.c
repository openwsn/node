/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
 * 
 * OpenWSN is a free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 * 
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi 
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 * 
 ******************************************************************************/ 

/*******************************************************************************
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 * 
 * @status
 *	- Tested by Jiang Ridong in 2011.05. Released.
 *
 * @author zhangwei(TongJi University) on 20070423
 * 	- revision today.
 * @modified by yan-shixing(TongJi University) on 20090725
 *  - revision. tested ok
 *
 * @modified by zhangwei(TongJi University) on 20091030
 *  - upgrade the frame output function. both the listener mode and non-listener 
 *    mode are using _output_openframe() now.
 * @modified by shi-miaojing(TongJi University) on 20091031
 *	- tested ok.
 * @modified by zhangwei in 2010
 *	- support both the binary format and text ascii format. You can use macro
 *    CONFIG_ASCII_OUTPUT to configure it.
 * @modified by zhangwei on 2011.03.04
 * 	- revision.
 * @modified by zhangwei on 2011.05.06
 * 	- revised.
 * @modified by jiang ridong, zhangwei in 2011.08
 * 	- revised.
 ******************************************************************************/ 

/** 
 * If You want to enable the interrupt drive mode of the UART, you should define
 * the following macro. This will improve performance. However, you should guarantee
 * the data can be really sent out. You should check the return value of uart_write()
 * in this case.
 */

//#define CONFIG_UART_INTERRUPT_DRIVEN
//#undef CONFIG_UART_INTERRUPT_DRIVEN

#include "apl_foundation.h"

/**
 * This macro controls the apl_ieee802frame154_dump module to output
 * the frame in ascii mode.
 */
#define CONFIG_ASCII_OUTPUT
#undef CONFIG_ASCII_OUTPUT

#undef CONFIG_ACTIVE_SENDING_MODE
#define CONFIG_ACTIVE_SENDING_MODE

/**
 * CONFIG_LISTENER
 * If you define this macro, the sniffer will be driven by the listener function. 
 * attention currently, the listener of cc2420 adapter is actually called in the 
 * interrupt mode. so you cannot do complicated time consumption operations inside
 * the listener.
 * 
 * CONFIG_ASCII_OUTPUT
 * To choose which kind of format to output. Binary format or Text ASCII format.
 * to decide the output format of the sniffer. If you define this macro, the sniffer
 * will convert the binary frame values into hex ascii format. each byte will be 
 * represented as two ascii characters.
 */

#define PANID						0x0001
#define LOCAL_ADDRESS				0x05
#define REMOTE_ADDRESS				0x04
#define SIO_RXBUF_SIZE				128
#define DEFAULT_CHANNEL     		11

#define MAX_IEEE802FRAME154_SIZE    128

#define SNIFFER_FMQUE_HOPESIZE  	FRAMEQUEUE_HOPESIZE(4)

#define CMD_DATA_REQUEST            0x01
#define CMD_DATA_RESET              0x02

typedef struct{
  uint32 received;
  uint32 dropped;
}TiSnifferStatistics;

TiCc2520Adapter m_cc; 
TiUartAdapter m_uart;
TiSioAcceptor m_sac;

static char m_nio_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_sio_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nio_rxque[SNIFFER_FMQUE_HOPESIZE];


static void _nss_execute(void);
static void _nss_send_response( TiSioAcceptor *sac,TiFrameQueue * fmque, TiSnifferStatistics * stat, TiUartAdapter * uart );
static void _active_send_test(void);
static void _init_test_response( TiFrame * frame );

/*******************************************************************************
 * functions 
 ******************************************************************************/ 

int main(void)
{
    _nss_execute();
    // _active_send_test();
}

/** 
 * Sniffer base station. All frames found will be firstly push into an frame queue
 * and then wait for the GUI program's REQUEST, and then send the frame as RESPONSE 
 * to the GUI. 
 * @return None.
 */
void _nss_execute(void)
{
	char * msg = "welcome to snifferbase ...";
    TiCc2520Adapter * cc;
    TiFrame * nio_rxbuf;
    TiFrame * sio_rxbuf;
	TiFrameQueue * nio_rxque;
	TiUartAdapter * uart;

    TiSioAcceptor * sio;

	int len = 0;
	TiSnifferStatistics stat;

	#ifndef CONFIG_ACTIVE_SENDING_MODE
	uint16 sio_addr;
    char *pc;
	#endif

    target_init();
    rtl_init( (void *)dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );

    led_open();
    led_on( LED_ALL );
    hal_delayms( 500 );
    led_off( LED_ALL );
    dbc_write( msg, strlen(msg) );

    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart,1, 9600, 8, 1, 0 );
    hal_assert( uart != NULL );

    cc = cc2520_construct((char *)(&m_cc), sizeof(TiCc2520Adapter));
    cc2520_open(cc, 0, NULL, NULL, 0x00 );
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );								// enable RX mode
	cc2520_setpanid( cc, PANID );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2520_disable_addrdecode( cc );				// disable address decoding
	cc2520_disable_autoack( cc );

    sio = sac_open(&m_sac, sizeof(m_sac), uart);

	nio_rxque = fmque_construct( &m_nio_rxque[0], sizeof(m_nio_rxque) );
    nio_rxbuf = frame_open( (char*)(&m_nio_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    #ifndef CONFIG_ACTIVE_SENDING_MODE
    sio_rxbuf = frame_open( (char*)(&m_sio_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    #endif
	
	memset( &stat, 0x00, sizeof(stat) );

	while(1) 
	{
		// Query for arrived frames from the radio adapter component, and push it
		// into the frame queue for sending later.

        frame_reset( nio_rxbuf, 0, 0, 0 );
        #ifndef CONFIG_ACTIVE_SENDING_MODE
        frame_reset( sio_rxbuf, 0, 0, 0 );
        #endif
        
        len = cc2520_read( cc, frame_startptr(nio_rxbuf), frame_capacity(nio_rxbuf), 0x00 );
        if (len > 0)
        {
            frame_setlength( nio_rxbuf, len );
			stat.received ++;
			
			// @attention: The supervisory upper computer should retrieve the frames
			// out as soon as possible. If there're too many frames inside the queue,
			// then the later frames will failed to be queued, and it will be dropped
			// naturely by the fmque_pushback() function. 
			//
			if (fmque_pushback(nio_rxque, nio_rxbuf) == 0)
				stat.dropped ++;
			
			led_toggle( LED_RED );
        }

		#ifdef CONFIG_ACTIVE_SENDING_MODE
		// In active mode, the snifferbase will send the frame out through serial
		// communication immediately without waiting for the request.
		//
		_nss_send_response(sio, nio_rxque, &stat, uart);
		#endif
		
		#ifndef CONFIG_ACTIVE_SENDING_MODE
		// In passive mode, the snifferbase will query for arrived request from the
		// serial communication, and then parse and execute the command.
		//
        len = sac_framerecv(sio, sio_rxbuf, 0x00);
		if (len > 0)
		{
            pc = frame_startptr(sio_rxbuf);
			switch (pc[0])
			{
			case CMD_DATA_REQUEST:
				_nss_send_response(sio,nio_rxque, &stat, uart);
				break;

			case CMD_DATA_RESET:
				memset(&stat, 0x00, sizeof(stat));
				break;
					
			default:
				break;
			}
		}
		#endif // CONFIG_ACTIVE_SENDING_MODE 
		
		// Simulate the running the cc2420 internal task
		cc2520_evolve( cc );
	}
} 	

/**
 * Pick an frame from the framequeue and encapsulate it as an response and send it 
 * back to the monitoring computer.
 * 
 * @param sac Serial I/O acceptor.
 * @param fmque The frame queue containing the 802.15.4 frame to be sent.
 * @param stat Statistic data
 * @param uart 
 * @return None.
 */
void _nss_send_response( TiSioAcceptor *sac, TiFrameQueue * fmque, TiSnifferStatistics * stat, TiUartAdapter * uart )
{
	TiFrame * f;
    
	// @attention If you want to output the statistics information, you can uncomment
	// the following lines.
	// dbc_uint16( stat->received );
	// dbc_uint16( stat->dropped );
	
	memset( &stat, 0x00, sizeof(stat) );
	 
	f = fmque_front( fmque );
	while (f != NULL)
	{
		//dbc_write( frame_startptr(f), frame_length(f) );
		// uart_write( uart, frame_startptr(f), frame_length(f), 0x00 );
		
		// Insert a little delay between each transmitting. This seems useless. So 
		// I finally eliminate it from the released source code.
		// hal_delay( 2 );

        if (sac_framesend(sac, f, 0) > 0)
		{
			fmque_popfront( fmque );
			f = fmque_front( fmque );
		}
	}
}

#ifdef CONFIG_ACTIVE_SENDING_MODE
/** 
 * Actively sending frames through the serial communication port. The frame is 
 * generated in this function. 
 */
void _active_send_test()
{
    char * msg = "welcome to sniffer ...";
	TiFrame * txbuf = (TiFrame *)(m_sio_rxbuf[0]);
	TiUartAdapter * uart;
    TiSioAcceptor * sio;

	target_init();
	led_open();
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );

	// #ifndef CONFIG_UART_INTERRUPT_DRIVEN
	// rtl_init(dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report);
    // dbc_mem(msg, strlen(msg));
    // #endif

    uart = uart_construct((void *)&m_uart, sizeof(TiUartAdapter));
    uart = uart_open(uart, 1, 9600, 8, 1, 0);
    hal_assert(uart != NULL);
    uart_write(uart, msg, strlen(msg), 0x00);

	rtl_init(uart, uart_putchar, uart_getchar_wait, hal_assert_report);
    dbc_mem(msg, strlen(msg));

    sio = sac_open(&m_sac, sizeof(m_sac), uart); 

	while(1) 
	{
        _init_test_response(txbuf);
		if (sac_framesend(sio, txbuf, 0) > 0)
			frame_clear(txbuf);

        sac_evolve( sio, NULL );
        hal_delayms(1000);
		led_toggle( LED_RED );
    }
    
    uart_close(uart);
    uart_destroy(uart);
}
#endif

/**
 * This function will fill an response frame. It's used for testing purpose only.
 * @param frame Containing the frame. You can get the data through frame_startptr(f)
 *      and frame_length(f).
 * @return None. 
 */
void _init_test_response( TiFrame * frame )
{
    int i;
	char * ptr;
	TiIEEE802Frame154Descriptor m_desc, *desc;
	uint8 seqid=6;
    										 
    frame_open( (char *)frame, FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 16, 50 );
	//frame_reset(frame, 3, 16, 0);
    ptr = frame_startptr(frame);

    for (i=0; i<6; i++)
        ptr[i] = '0' + i;
    frame_skipouter(frame, 12, 2);

    ieee802frame154_open(&m_desc);
    desc = ieee802frame154_format(&m_desc, frame_startptr(frame), frame_capacity(frame), 
        FRAME154_DEF_FRAMECONTROL_DATA ); 
    rtl_assert( desc != NULL );

	seqid ++;
    
    ieee802frame154_set_sequence(desc, seqid); 
    ieee802frame154_set_panto(desc, PANID);
    ieee802frame154_set_shortaddrto(desc, REMOTE_ADDRESS);
    ieee802frame154_set_panfrom(desc, PANID);
    ieee802frame154_set_shortaddrfrom(desc, LOCAL_ADDRESS);
    frame_setlength(frame, 30);
	frame_movefirst(frame);
}
