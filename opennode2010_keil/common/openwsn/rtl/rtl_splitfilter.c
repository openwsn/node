/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
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

#define CONFIG_SPLITER_RXBUF_SIZE 255
#define CONFIG_SPLITER_TXBUF_SIZE 255

#include "rtl_configall.h"
#include <string.h>
#include <stdlib.h>
#include "rtl_foundation.h"
#include "rtl_splitfilter.h"

/* Frame Format
 * 
 * Data in the buffer:
 *	[Binary Data nB]
 *   data length is returned as the funtion return value
 *
 * Frame format
 *	[Start of Frame SOF 1B] [Frame Length 1B] [Encoded Frame]
 * 
 * the encoding rule is as the following:
 *  [0xAB 1B] hex representation => ['A' 1B] ['B' 1B]
 *
 * so the encoded length is twice of its original format.
 */

TiSplitFilter * split_create( uintx rxbufsize, uintx txbufsize )
{
	uintx size = sizeof(TiSplitFilter) + IOBUF_HOPESIZE(rxbufsize) + IOBUF_HOPESIZE(txbufsize);
	void * mem = malloc( size );
	return split_construct( mem, size, rxbufsize, txbufsize );
}

void split_free( TiSplitFilter * split )
{
	split_destroy( split );
	free( split );
}

TiSplitFilter * split_construct( void * mem, uintx size, uintx rxbufsize, uintx txbufsize )
{
	TiSplitFilter * split = (TiSplitFilter *)mem;
	memset( mem, 0x00, size );
	split->state = 0;
	split->rxlen = 0;
	iobuf_construct( (char *)mem + sizeof(TiSplitFilter), IOBUF_HOPESIZE(rxbufsize) );
	iobuf_construct( (char *)mem + sizeof(TiSplitFilter) + IOBUF_HOPESIZE(rxbufsize),
		IOBUF_HOPESIZE(txbufsize) );
	return split;
}

void split_destroy( TiSplitFilter * split )
{
	return;
}

TiIoBuffer * split_rxbuf( TiSplitFilter * split )
{
	return (TiIoBuffer *)((char *)split + sizeof(TiSplitFilter));
}

TiIoBuffer * split_txbuf( TiSplitFilter * split )
{
	uintx rxsize = sizeof(TiIoBuffer) + iobuf_size(split_rxbuf(split));
	return (TiIoBuffer *)((char *)split + sizeof(TiSplitFilter) + rxsize);
}

/* this function should be called to deal with received data stream. if this
 * function finds an entire frame in the buffer, then it will return the frame
 * through the output buffer pointer.
 *
 * param
 *	split		split oject
 *  input       input buffer
 *  output      output buffer
 *
 * return
 *  output      an enire frame. if found, then return length.
 *  return how many values in the output buffer. negative means failed.
 *
 */
uintx split_rxhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output )
{
	TiIoBuffer * rxbuf = split_rxbuf(split);
	uintx cur = 0;
	bool found = false;

	while ((cur < input->length) && (!found))
	{
		switch (split->state)
		{
		case 0:
			// state 0 is the initial state. in this state, the program only do
			// state transfer when finding an SOF byte. in all the other cases,
        	// the input byte should be ignored.
        	//
			if (iobuf_ptr(input)[cur] == SPLITFILTER_SOF)
			{
				iobuf_clear( split_rxbuf(split) );
				split->state = 1;
			}
			break;

		case 1:
			// state 1 means we had just find the start flag of a frame. and hope
			// the current byte should be the frame length. iobuf_ptr(input)[cur]
			// is the current byte.
			//
			split->rxlen = iobuf_ptr(input)[cur];
			// if the frame payload length is 0, then we should restart the frame
			// SOF identification process again.
			if (split->rxlen == 0)
			{
				split->state = 0;
				output->length = 0;
			}
			else
            	split->state = 2;
			break;

		case 2:
			// this shouldnot happen. this means the frame terminated earlier than
			// expected. this means the frame maybe incomplete. in this case, i
			// suggest you discard the frame just temporarily put inside the rxbuf.
			//
			if (iobuf_ptr(input)[cur] == SPLITFILTER_SOF)
			{
				if (split->rxlen == input->length)
				{
					iobuf_copyfrom( output, split_rxbuf(split) );
					split->state = 0;
					found = true;
				}
				else{
					// discard the incomplete frame received in "rxbuf" and restart
					// the SOF search process.
					iobuf_clear( split_rxbuf(split) );
					split->state = 1;
				}
			}
			// else if the current byte isn't SOF byte
			// (cur == split->rxlen) means we have already received all the bytes
			// of the current frame.
			//
			else{
				iobuf_pushback( output, iobuf_ptr(input)+cur, 1 );
				// todo: we did NOT thinking about how the application behave
				// if the frame exceeds the buffer size. here the condition should
				// be ==, i changed it to >= for safety reasons.
				if (cur >= split->rxlen)
				{
					iobuf_copyfrom( output, rxbuf );
					split->state = 0;
					found = true;
				}
			}
			break;

		default:
			break;
		}

		cur ++;
	}

	// remove the first "cur" bytes from the input buffer because we had already
	// processed them. attention there may still some data left in the input buffer.
	// they will be processed the next time when calling this function. so don't
	// destroy the data left inside the input buffer. you should only append the
	// new data into the input buffer.
	//
	iobuf_popfront( input, cur );

	// if find an entire frame
	if (found)
	{
		iobuf_clear( split_rxbuf(split) );
		return iobuf_length(output);
	}
	else
		return 0;
}

/* parameter
 * 	input       input is an TiIoBuffer object holdiing the data to be encapsulated
 *              as a frame. it contains the data only without SOF byte and length
 *              byte.
 *  output      input is an TiIoBuffer object holdiing the final frame. it contains
 *              the SOF byte and frame length byte.
 */
uintx split_txhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output )
{
	char c;
	iobuf_clear( output );
	c = SPLITFILTER_SOF;
	iobuf_pushback( output, &c, 1 );
	c = iobuf_length(input);
	iobuf_pushback( output, &c, 1 );
	iobuf_pushback( output, iobuf_ptr(input), iobuf_length(input) );
	return iobuf_length(output);
}

