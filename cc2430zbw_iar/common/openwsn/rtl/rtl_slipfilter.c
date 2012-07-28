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

/* @section history
 * @modified by zhangwei on 2011.08.17
 * - add support to judge output buffer full. If the output buffer is full, then
 *   rxhandler will return -1.
 */

/* This module implements a framing mechanism named SLIP protocol. It's used to 
 * split the continuous byte stream into consecutive frames. 
 * 
 * This module can be used with any USART adapter, byte-oriented wireless adapter 
 * such as the Chipcon/TI's cc1000 transceiver, or byte stream transmitting drivers
 * (such as the TCP/IP socket byte stream).
 * 
 * Frame format is as the following:
 * 
 * {END 1B} [DATA nB] [END 1B]
 * 
 * In the early version of SLIP, there's no frame start flag byte. Phil Karn suggests
 * to add this flag byte to flush any erroneous bytes which have been caused by 
 * line noise. We require the sender to issue an END byte to indicate the beginning
 * of a new frame though it's therectically unnecessary.
 * 
 * Please refer to RFC 1055 for more explanation of SLIP framing protocol.
 * http://tools.ietf.org/html/rfc1055
 * 
 * PROTOCOL
 * 
 * The SLIP protocol defines two special characters: END and ESC. END is
 * octal 300 (decimal 192) and ESC is octal 333 (decimal 219) not to be
 * confused with the ASCII ESCape character; for the purposes of this
 * discussion, ESC will indicate the SLIP ESC character.  To send a
 * packet, a SLIP host simply starts sending the data in the packet.  If
 * a data byte is the same code as END character, a two byte sequence of
 * ESC and octal 334 (decimal 220) is sent instead.  If it the same as
 * an ESC character, an two byte sequence of ESC and octal 335 (decimal
 * 221) is sent instead.  When the last byte in the packet has been
 * sent, an END character is then transmitted.
 *
 * Phil Karn suggests a simple change to the algorithm, which is to
 * begin as well as end packets with an END character.  This will flush
 * any erroneous bytes which have been caused by line noise.  In the
 * normal case, the receiver will simply see two back-to-back END
 * characters, which will generate a bad IP packet.  If the SLIP
 * implementation does not throw away the zero-length IP packet, the IP
 * implementation certainly will.  If there was line noise, the data
 * received due to it will be discarded without affecting the following
 * packet.
 *
 * Because there is no 'standard' SLIP specification, there is no real
 * defined maximum packet size for SLIP.  It is probably best to accept
 * the maximum packet size used by the Berkeley UNIX SLIP drivers: 1006
 * bytes including the IP and transport protocol headers (not including
 * the framing characters).  Therefore any new SLIP implementations
 * should be prepared to accept 1006 byte datagrams and should not send
 * more than 1006 bytes in a datagram.
 *
 * DEFICIENCIES
 *
 * There are several features that many users would like SLIP to provide
 * which it doesn't.  In all fairness, SLIP is just a very simple
 * protocol designed quite a long time ago when these problems were not
 * really important issues.  The following are commonly perceived
 * shortcomings in the existing SLIP protocol:
 *
 *    - addressing:
 *
 *        both computers in a SLIP link need to know each other's IP
 *       addresses for routing purposes.  Also, when using SLIP for
 *       hosts to dial-up a router, the addressing scheme may be quite
 *       dynamic and the router may need to inform the dialing host of
 *
 *     the host's IP address.  SLIP currently provides no mechanism
 *       for hosts to communicate addressing information over a SLIP
 *       connection.
 *
 *    - type identification:
 *
 *       SLIP has no type field.  Thus, only one protocol can be run
 *       over a SLIP connection, so in a configuration of two DEC
 *       computers running both TCP/IP and DECnet, there is no hope of
 *       having TCP/IP and DECnet share one serial line between them
 *       while using SLIP.  While SLIP is "Serial Line IP", if a serial
 *       line connects two multi-protocol computers, those computers
 *       should be able to use more than one protocol over the line.
 *
 *     - error detection/correction:
 *
 *       noisy phone lines will corrupt packets in transit. Because the
 *       line speed is probably quite low (likely 2400 baud),
 *       retransmitting a packet is very expensive.  Error detection is
 *       not absolutely necessary at the SLIP level because any IP
 *       application should detect damaged packets (IP header and UDP
 *       and TCP checksums should suffice), although some common
 *       applications like NFS usually ignore the checksum and depend on
 *       the network media to detect damaged packets.  Because it takes
 *       so long to retransmit a packet which was corrupted by line
 *       noise, it would be efficient if SLIP could provide some sort of
 *       simple error correction mechanism of its own.
 *
 *    - compression:
 *
 *       because dial-in lines are so slow (usually 2400bps), packet
 *       compression would cause large improvements in packet
 *       throughput. Usually, streams of packets in a single TCP
 *       connection have few changed fields in the IP and TCP headers,
 *       so a simple compression algorithms might just send the changed
 *       parts of the headers instead of the complete headers.
 *
 * Some work has been done by various groups to design and implement a
 * successor to SLIP which will address some or all of these problems.
 * such as PPP. but we still use it in OpenWSN project for its simplicity.
 *
 * Here's a Chinese translation about the key idea of SLIP protocol: 
 *
 * 下面的规则描述了SLIP协议定义的帧格式：
 * 
 * 1) IP数据报以一个称作END（0 x c 0）的特殊字符结束。同时，为了防止数据报到来之前的
 * 线路噪声被当成数据报内容，大多数实现在数据报的开始处也传一个E N D字符（如果有线路
 * 噪声，那么E N D字符将结束这份错误的报文。这样当前的报文得以正确地传输，而前一个
 * 错误报文交给上层后，会发现其内容毫无意义而被丢弃）。
 *
 * 2) 如果I P报文中某个字符为END，那么就要连续传输两个字节0 x d b和0 x d c来取代它。
 * 0 x d b这个特殊字符被称作SLIP的ESC字符，但是它的值与ASCII码的E S C字符（0 x 1 b）不同。
 *
 * 3) 如果I P报文中某个字符为SLIP的ESC字符，那么就要连续传输两个字节0 x d b和0 x d d来
 * 取代它。
 * 
 * SLIP是一种简单的帧封装方法，还有一些值得一提的缺陷：
 * 1) 每一端必须知道对方的I P地址。没有办法把本端的I P地址通知给另一端。
 * 2) 数据帧中没有类型字段（类似于以太网中的类型字段）。如果一条串行线路用于SLIP，
 * 那么它不能同时使用其他协议。
 * 3 ) SLIP没有在数据帧中加上检验和（类似于以太网中的CRC字段）。如果SLIP传输的报文
 * 被线路噪声影响而发生错误，只能通过上层协议来发现（另一种方法是，新型的调制解调器
 * 可以检测并纠正错误报文）。这样，上层协议提供某种形式的CRC就显得很重要。 
 * 
 * Reference
 * - Serial Line Internet Protocol(SLIP), 
 *   http://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol
 * - Point-to-Point Protocol (PPP)
 *   http://en.wikipedia.org/wiki/Point-to-Point_Protocol
 * - A NONSTANDARD FOR TRANSMISSION OF IP DATAGRAMS OVER SERIAL LINES: SLIP
 *   http://tools.ietf.org/html/rfc1055 (Good!)
 * - The Point-to-Point Protocol (PPP)，
 *   http://tools.ietf.org/html/rfc1661
 * - PPP in HDLC-like Framing, http://tools.ietf.org/html/rfc1662
 * - PPP in a Real-time Oriented HDLC-like Framing,
 *   http://tools.ietf.org/html/rfc2687
 * - PPP协议规范(zh), http://www.longen.com/l-r/detaill-r/ppp.htm 
 * - 协议基础: PPP协议详解，http://www.router.net.cn/Article/43575.html
 * - PPP Serial Data Transport Protocol (SDTP),
 *   http://tools.ietf.org/html/rfc1963
 * - PPP Reliable Transmission
 *   http://tools.ietf.org/html/rfc1663
 * - SLIP on Baike, http://baike.baidu.com/view/32775.htm (Good!)
 * - The Point-to-Point Protocol (PPP): An Overview,
 *   http://technet.microsoft.com/en-us/library/cc768082.aspx
 */
 
#include "rtl_configall.h"
#include "rtl_foundation.h"
#include <string.h>
#ifdef CONFIG_DYNA_MEMORY
#include <stdlib.h>
#endif
#include "rtl_slipfilter.h"
#include "rtl_iobuf.h"

/* SLIP special character codes */

#define END             0xC0 // 192 // 0300    /* indicates end of packet */
#define ESC             0xDB // 219 // 0333    /* indicates byte stuffing */
#define ESC_END         0xDC // 220 // 0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         0xDD // 221 // 0335    /* ESC ESC_ESC means ESC data byte */

#define SLIP_STATE_IDLE			0
#define SLIP_STATE_RECVING		1
#define SLIP_STATE_RECV_ESCAPE	2
#define SLIP_STATE_ACCEPTTED	3

/**
 * Initialize the TiSlipFilter object.
 */
#ifdef CONFIG_DYNA_MEMORY
TiSlipFilter * slip_filter_create()
{
	TiSlipFilter * slip;

	slip = (TiSlipFilter *)malloc(sizeof(TiSlipFilter));
	if (slip != NULL)
	{
		memset( slip, 0x00, sizeof(TiSlipFilter) );
		slip->rx_state = SLIP_STATE_IDLE;
	}

	return slip;
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void slip_filter_free( TiSlipFilter * slip )
{
	if (slip)
	{
		free(slip);
	}
}
#endif

TiSlipFilter * slip_filter_open( TiSlipFilter * slip, uintx size )
{
	rtl_assert( sizeof(TiSlipFilter) <= size );
	memset( slip, 0x00, sizeof(TiSlipFilter) );
	slip->rx_state = SLIP_STATE_IDLE;
	return slip;
}

void slip_filter_close( TiSlipFilter * slip )
{
	slip = slip;
}


/**
 * Process the packet to be sent. 
 *
 * Currently, the output buffer must be large enough to hold all temperarily output
 * or else the possible truncate may lead to unexpected error.
 *
 * Attention: How many bytes the TiIoBuf object can hold depends on "uintx" macro.
 * This type limits the maximum capacity of the TiIoBuf object. In most of the systems,
 * uintx is defined as uint16 or uint32.
 *
 * @attention
 * Negative "count" value indicates the output buffer is full and the frame
 * inside the output buffer is still not complete. This should never happen.
 * However, the TiSlipFilter cannot able to process it. The master program
 * should deal with it, for example, clear the buffer.
 *
 * @param input Contains the packet to be sent. iobuf_ptr() is the data address and
 *		iobuf_length() is the data length.
 * @param output Contains the frame for sending.
 * @return The length of the frame successfully processed in the input buffer.
 *		May return 0 or negetive value if failed. Attention the data inside input
 *      buffer will be cleared if they were processed successfully into output buffer.
 *
 * @return
 *      - 1     indicate buffer full and failed to place more characters in the output buffer.
 *              if the buffer is full and the frame is complete, then still return non negative.
 *
 * @modified by zhangwei on 2011.08.17
 *  - Add support to negative return value. If this function returns -1, then indicate
 * the frame in output buffer isn't complete but the buffer is already full. The
 * master program should deal with it.
 */
int slip_filter_txhandler( TiSlipFilter * slip, TiIoBuf * input, TiIoBuf * output )
{
	int count = 0;
	unsigned char c;

    rtl_assert(iobuf_capacity(output) >= 2);

    /* send an initial END character to flush out any data that may
     * have accumulated in the receiver due to line noise
     */
    iobuf_putchar( output, END );

    /* for each byte in the packet, send the appropriate character
     * sequence
     */
	while (!iobuf_empty(input))
	{
		if (iobuf_full(output))
        {
            count = -1;
			break;
        }
		iobuf_getchar(input, (char *)&c );
		count ++;

        switch(c){
        /* if it's the same code as an END character, we send a
         * special two character code so as not to make the
         * receiver think we sent an END
         */
        case END:
			iobuf_putchar(output, ESC);
			// @warning
			// @attention
			// @todo
			// You must guarantee the output buffer is enough to hold the char
			// or else iobuf_putchar will drop the char
            if (iobuf_full(output))
            {
                count = -1;
                break;
            }
            iobuf_putchar(output, ESC_END);
            break;

        /* if it's the same code as an ESC character,
         * we send a special two character code so as not
         * to make the receiver think we sent an ESC
         */
        case ESC:
            iobuf_putchar(output, ESC);
			// @warning
			// @attention
			// @todo
			// You must guarantee the output buffer is enough to hold the char
			// or else iobuf_putchar will drop the char
            if (iobuf_full(output))
            {
                count = -1;
                break;
            }
            iobuf_putchar(output, ESC_ESC);
            break;

        /* otherwise, we just send the character */
        default:
            iobuf_putchar(output, (char)c);
        }
	}

    /* tell the receiver that we're done sending the packet */
    if (iobuf_full(output))
        count = -1;
    else
        iobuf_putchar(output, END);

    // @attention
    // Negative "count" value indicates the output buffer is full and the frame
    // inside the output buffer is still not complete. This should never happen.
    // However, the TiSlipFilter cannot able to process it. The master program
    // should deal with it, for example, clear the buffer.
    //
    if (count < 0)
    {
        // May error processing here. But this should be done by the master program
        // instead of the TiSlipFilter itself.
    }

	return count;
}

/**
 * Process input stream can do framing with it. The founded frame is placed inside
 * output buffer.
 *
 * attention the output buffer should be large enough to hold an entire frame
 * or else this function will drop these long frames to enable the program continue
 * running. However, this should not happen for high reliable applications.
 *
 * @attention
 * - This function affect input, output queue and slip->rs_state.
 * - Before you calling this function, attention there may data already in input
 *   and output queue.
 *
 * @return
 *      > 0				Indicate there's an entire frame inside output buffer.
 *		0               not found yet. but there maybe data already pending inside output buffer.
 */
int slip_filter_rxhandler( TiSlipFilter * slip, TiIoBuf * input, TiIoBuf * output )
{
	/* @modified by Jiang Ridong on 2011.08.09
     * - Bug fixed. In the past, we use signed char, so the comparison between a
     * signed char and END character (0xC0) is always false. This is at least occured
     * in MSVC DotNET 2010 because VC will truncate 0xC0 as a signed char which value
     * is 0x40(64). This bug is fixed by define variable c as unsigned type.
     */
	unsigned char c = 0x00;
	char done = 0;
    while ((!iobuf_empty(input)) && (!done))
	{
		/* Output buffer full means the frame is too long. We had no better idea but to
		 * clear the output buffer and restart the framing process from IDLE state again.
		 */
		if (iobuf_full(output))
		{
			iobuf_clear(output);
			slip->rx_state = SLIP_STATE_IDLE;
			break;
		}

		/* get a character from input buffer to process */
        iobuf_getchar( input, (char *)&c );

		switch (slip->rx_state)
		{
		/* This's the default state of receiving mechanism. */
		case SLIP_STATE_IDLE:
			if (c != ((unsigned char)END))
			{
				iobuf_putchar(output, c);
			}
			slip->rx_state = SLIP_STATE_RECVING;
			break;

		/* Receiving data from input buffer and output results into output buffer */
		case SLIP_STATE_RECVING:
			switch (c)
			{
			case ESC:
				slip->rx_state = SLIP_STATE_RECV_ESCAPE;
				break;
			case END:
				slip->rx_state = SLIP_STATE_ACCEPTTED;
				done = 1;
				break;
			default:
				iobuf_putchar(output, c);
			}
			break;

		/* If received an ESC character, the SLIP will come into SLIP_STATE_RECV_ESCAPE state */
		case SLIP_STATE_RECV_ESCAPE:
			switch (c)
			{
			case ((unsigned char)ESC_END):
				iobuf_putchar(output, END);
				slip->rx_state = SLIP_STATE_RECVING;
				break;
			case ((unsigned char)ESC_ESC):
				iobuf_putchar(output, ESC);
				slip->rx_state = SLIP_STATE_RECVING;
				break;
			default:
				// actually, this should NOT accure. This case violate the protocol.
				// our processing is to regard c as an normal data byte.
				//assert( false );
				iobuf_putchar(output, ESC);
				iobuf_putchar(output, c);
				slip->rx_state = SLIP_STATE_RECVING;
			}
			break;

		default:
			slip->rx_state = SLIP_STATE_IDLE;
			break;
		}

		
	}

	/* If there're two END characters consecutively, then the state machine will
	 * mistaken it as an valid frame. We should ignore those frame whose length is 0.
	 *
	 * @attention
	 * The following judgement assume every frame is start at 0 in the output buffer.
	 * The caller of this function should guarantee this.
	 */
	if (slip->rx_state == SLIP_STATE_ACCEPTTED)
    {
        slip->rx_state = SLIP_STATE_IDLE;
		return iobuf_length(output); /* may return 0 */
	}
    else
		return 0;
}


