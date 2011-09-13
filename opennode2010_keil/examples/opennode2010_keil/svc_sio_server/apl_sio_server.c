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
 * Demonstrate how to communicate with the host computer under the support of TiSioAcceptor. 
 * Different to the TiUartAdapter testings, these test including the framing support.
 * 
 * @status
 *	- Developing
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

#define CONFIG_ACTIVE_SENDING_MODE
//#undef CONFIG_ACTIVE_SENDING_MODE

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

#define SNIFFER_FMQUE_HOPESIZE  	FRAMEQUEUE_HOPESIZE(2)

#define CMD_DATA_REQUEST            0x01
#define CMD_DATA_RESPONSE           0x01
#define CMD_DATA_RESET              0x02

TiUartAdapter m_uart;
TiSioAcceptor m_sac;

static char m_sio_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

static void _sio_active_sender(void);
static void _sio_passive_service();
static TiFrame * _sio_init_frame(byte * buf, uint16 len, uint8 type);

/*******************************************************************************
 * functions 
 ******************************************************************************/ 

int main(void)
{
    //_sio_active_sender();
    _sio_passive_service();
}

/** 
 * Actively sending frames through the serial communication port. The frame is 
 * generated in this function. 
 */
void _sio_active_sender()
{
    char * msg = "welcome to active frame sender ...";
	TiFrame * txbuf = (TiFrame *)(&m_sio_rxbuf[0]);
	TiUartAdapter * uart;
    TiSioAcceptor * sio;

	// initialize the target board and flash welcome LED to indicate the application
    // is started successfully.

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

    // initialize the serial I/O acceptor object for frame based sending and receiving.
    sio = sac_open(&m_sac, sizeof(m_sac), uart); 

	while(1) 
	{    
        // uart_write(uart, msg, strlen(msg), 0x00);
    
        txbuf = (TiFrame *)(&m_sio_rxbuf[0]);
        txbuf = _sio_init_frame((byte*)(&m_sio_rxbuf[0]), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), CMD_DATA_RESPONSE);
		if (sac_framesend(sio, txbuf, 0) > 0)
			frame_clear(txbuf);

        sac_evolve( sio, NULL ); 
        hal_delayms(500);
		led_toggle( LED_RED );
    }
    
    sac_close(sio);
    uart_close(uart);
    uart_destroy(uart);
}

/** 
 * This is a simple frame based serial I/O server. It wills send response frame 
 * when only receiving an request frame.
 */
void _sio_passive_service(void)
{
    char * msg = "welcome to serial I/O service ...";
	TiFrame * rxbuf = (TiFrame *)(m_sio_rxbuf[0]);
    TiFrame * txbuf;
	TiUartAdapter * uart;
    TiSioAcceptor * sio;
    char * data;
    uint8 state=0;

	// initialize the target board and flash welcome LED to indicate the application
    // is started successfully.

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

    // initialize the serial I/O acceptor object for frame based sending and receiving.
    sio = sac_open(&m_sac, sizeof(m_sac), uart); 

    state = 0;
	while(1) 
	{
        switch (state)
        {
        case 0:
            rxbuf = frame_open((char *)(&m_sio_rxbuf[0]), sizeof(m_sio_rxbuf), 3, 16, 50);
    		if (sac_framerecv(sio, rxbuf, 0) > 0)
            {
        		led_toggle( LED_RED );
                state = 1;
                state = 0;
                hal_delayms(500);
            };
            break;
        case 1:
            txbuf = rxbuf;
            data = frame_startptr(txbuf);
            data[0] = CMD_DATA_RESPONSE;
	    	if (sac_framesend(sio, txbuf, 0) > 0)
            {
    			frame_clear(txbuf);
                state = 0;
            }
            break;
        }

        sac_evolve( sio, NULL );
    }
    
    sac_close(sio);
    uart_close(uart);
    uart_destroy(uart);
}

/**
 * This function will fill an response frame. It's used for testing purpose only.
 * @param frame Containing the frame. You can get the data through frame_startptr(f)
 *      and frame_length(f).
 * @return None. 
 */
TiFrame * _sio_init_frame(byte * buf, uint16 len, uint8 type)
{
    TiFrame * frame;
	char * data;
	TiIEEE802Frame154Descriptor m_desc, *desc;
	uint8 seqid=6;
    int i;
    						
    hal_assert(len >= FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE));                            				 
    // memory, memory size, init_layer, init_layerstart, init_layercapacity
    frame = frame_open( buf, len, 3, 16, 20 );
	//frame_reset(frame, 3, 16, 0);
    
    data = frame_startptr(frame);
    for (i=0; i<frame_capacity(frame); i++)
    {
        data[i] = '0' + i;
	}
    data[0] = (uint8)type;
    frame_setlength(frame, frame_capacity(frame));

    frame_skipouter(frame, 12, 2);
    desc = ieee802frame154_open(&m_desc);
    desc = ieee802frame154_format(desc, frame_startptr(frame), frame_capacity(frame), 
        FRAME154_DEF_FRAMECONTROL_DATA ); 
    hal_assert(desc != NULL);

    ieee802frame154_set_sequence(desc, seqid++); 
    ieee802frame154_set_panto(desc, PANID);
    ieee802frame154_set_shortaddrto(desc, REMOTE_ADDRESS);
    ieee802frame154_set_panfrom(desc, PANID);
    ieee802frame154_set_shortaddrfrom(desc, LOCAL_ADDRESS);
    frame_setlength(frame, 12+2+20);

	frame_movefirst(frame);
    return frame;
}
