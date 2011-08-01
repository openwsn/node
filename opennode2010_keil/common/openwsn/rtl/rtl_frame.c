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

/*******************************************************************************
 * rtl_frame
 * TiFrame object to help manipulating nested layer frame structures.
 *
 * @author zhangwei in 2005
 * @modified by zhangwei in 200604
 *  - revision
 * @modified by openwsn in 20100706
 *  - full revised. compile passed. not fully tested yet.
 * @modified by zhangwei on 2010.08.07
 *  - revision
 * @modified by zhangwei on 2010.12.27
 *  - add a new member variable option. This variable can be used to keep some 
 *    information when passing the frame between different objects.
 * @modified by zhangwei in 2011.02
 *  - add member variable "option" to save some configurations when passing between
 *    different layers. 
 * @modified by zhangwei in 2011.03
 *  - add four new functions: frame_addlayerinterior, frame_addlayerexterior, 
 *    frame_removelayerinterior, frame_removelayerexterior.
 ******************************************************************************/
 
#include "rtl_configall.h"
#include "../hal/opennode2010/hal_uart.h"
#ifdef CONFIG_DEBUG
#include <stdlib.h>
#endif
#include <string.h>

#include "rtl_foundation.h"
#include "rtl_frame.h"
#ifdef CONFIG_DEBUG
#include "rtl_debugio.h"
#endif

#define _frame_layer_start(frame,idx) (frame->layerstart[idx])
#define _frame_layer_end(frame,idx) (frame->layerstart[idx] + frame->layerlength[idx] - 1)
#define _frame_layer_length(frame,idx) (frame->layerlength[idx])
#define _frame_layer_capacity(frame,idx) (frame->layercapacity[idx])

/** 
 * create an TiFrame object with capacity specfied 
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx buffercapacity, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
    char * buf;

    uintx memsize = FRAME_HOPESIZE(buffercapacity);
    buf = (char *)malloc( memsize );
    rtl_assert( buf != NULL );
    if (buf != NULL);
        frame_open( buf, buffercapacity, init_layerindex, init_layerstart, layer_layercapacity );

    return (TiFrame *)buf;
}
#endif

/**
 * free an TiFrame object 
 */
#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame )
{
    frame_close( frame );
    free( frame );
}
#endif

/**
 * construct an TiFrame object on a pre-allocated memory block
 */
TiFrame * frame_open( char * buf, uintx memsize, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
    TiFrame * frame;
    /* for GAINZ platform, you should define "uintx" as "uint16" in configall.h */
#ifdef CONFIG_TARGET_GAINZ
    rtl_assert( sizeof(uintx) == 2 );
#endif
    rtl_assert( init_layercapacity <= memsize - sizeof(TiFrame) - init_layerstart );
    memset( buf, 0x00, memsize );
    frame = (TiFrame *)(buf);
    frame->memsize = memsize;
    frame_reset( frame, init_layerindex, init_layerstart, init_layercapacity );
    return frame;
}

/**
 * destroy an TiFrame object
 */
void frame_close( TiFrame * frame )
{
    memset( frame, 0x00, frame->memsize );
    return;
}

/**
 * duplicate an TiFrame object. this function will allocated memory used, so you 
 * should free the memory after using it. 
 *
 * suggest using frame_create() and frame_copyfrom() apparently.
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame )
{
    char * buf;

	rtl_assert( frame != NULL );
	buf = (char *)malloc( frame->memsize );
    if (buf != NULL)
        memmove( buf, frame, frame->memsize );

    return (TiFrame *)buf;
}
#endif

void frame_reset( TiFrame * frame, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
	uintx memsize;
	
	rtl_assert( frame != NULL );

	// clear the memory block while still keeping the "memsize" property
	memsize = frame->memsize;
	memset( (char*)frame, 0x00, memsize );
	frame->memsize = memsize;

    rtl_assert( init_layercapacity <= memsize - sizeof(TiFrame) - init_layerstart );

	frame->option = 0x00;
    frame->firstlayer = init_layerindex;
    frame->curlayer = init_layerindex;
    frame->layercount = 1;
    frame->layerstart[init_layerindex] = init_layerstart;
    frame->layerlength[init_layerindex] = 0;
    frame->layercapacity[init_layerindex] = ((init_layercapacity == 0) ? (frame->memsize - sizeof(TiFrame) - init_layerstart ) : init_layercapacity);
}

char * frame_buffer( TiFrame * frame )
{
	return ((char*)frame) + sizeof(TiFrame);
}

/**
 * clear the TiFrame object. after the clear, there's only one item inside the frame
 * and it occupies all the memory available.
 * 
 * Q: what's the difference between frame_reset() and frame_bufferclear()?
 * R: frame_reset() can choose where the initial layer is and frame_bufferclear assumes
 * layer 0 is the initial layer.
 * 
 * @modified by zhangwei on 2010.08.07
 *  - in the past, this function will automatically create the first layer. however,
 *    this may cause problems if you start the frame at the the application layer.
 *    now the bufferclear means clear all layers. you need to create the your first
 *    layer manually.
 */
void frame_bufferclear( TiFrame * frame )
{
	uintx memsize;
    
	rtl_assert( frame != NULL );
	memsize = frame->memsize;
	memset( (char*)frame, 0x00, memsize );
	frame->memsize = memsize;

	/*
    memset( (char*)frame + sizeof(TiFrame), 0x00, frame->memsize - sizeof(TiFrame) );
    
    frame->firstlayer = 0;
    frame->curlayer = 0;
    frame->layercount = 0;
    frame->layerstart[0] = 0;
    frame->layerlength[0] = 0;
    frame->layercapacity[0] = 0;
	*/
}

/**
 * total capacity of the TiFrame object. it's usually the maximum size of the lowest 
 * layer frame.
 */
uintx frame_buffercapacity( TiFrame * frame )
{
    return frame->memsize - sizeof(TiFrame);
}

uintx frame_totalcopyfrom( TiFrame * frame, TiFrame * from )
{
	//USART_Send( 0xb0);//todo for testing
	//USART_Send( from->memsize);//todo for testing
	rtl_assert( frame->memsize == from->memsize );
    //uint32 memsize = frame->memsize;
	memmove( frame, from, from->memsize );
    //frame->memsize = memsize;
	//USART_Send( 0xc0);//todo for testing
	return from->memsize;
}

uintx frame_layerstart( TiFrame * frame, uint8 layer )
{
   rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
	
    return frame->layerstart[layer];
}

char * frame_layerstartptr( TiFrame * frame, uint8 layer )
{
    rtl_assert((layer+1 > frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
	
    return (char*)frame + sizeof(TiFrame) + (frame->layerstart[layer]);
}

uintx frame_layerend( TiFrame * frame, uint8 layer )
{
    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
    return frame->layerstart[layer] + frame->layerlength[layer] - 1;
}

uintx frame_layerlength( TiFrame * frame, uint8 layer )
{
    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
    return frame->layerlength[layer];
}

uintx frame_layercapacity( TiFrame * frame, uint8 layer )
{
    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
	
    return frame->layercapacity[layer];
}

// set the data length of the current layer
void frame_setlayerlength( TiFrame * frame, uint8 layer, uintx count )
{
    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
    frame->layerlength[layer] = count;
}

// set the capacity of current layer. attention the difference between setlayerlength 
// and setlayercapacity
// - length, and capacity
// - setlayercapacity will adjust upper layer's capacity.
//
void frame_setlayercapacity( TiFrame * frame, uint8 layer, uintx count )
{
    intx delta;
    uint8 i;

    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));

    if (frame->layercount > 0)
    {
        delta = count - frame->layercapacity[layer];
        frame->layercapacity[layer] = count;
        rtl_assert( frame->layercapacity[layer] >= 0 );

        for (i=1; i<frame->layercount; i++)
        {
            layer++;
            frame->layercapacity[layer] += delta;
            rtl_assert( frame->layercapacity[layer] >= 0 );
        }
    }
}

uintx frame_curlayer( TiFrame * frame )
{
    return frame->curlayer;
}

/**
 * change the current item
 * if failed, then nothing changed. 
 *
 * @warning
 * this function can change the current layer index inside the TiFrame object. It
 * doesn't check whether the current the layer is existed or not!
 */
bool frame_setcurlayer( TiFrame * frame, uintx layer )
{
    bool ret = false;
    rtl_assert((layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));

    if (frame->layercount > 0)
    {
        frame->curlayer = layer;
        ret = true;
    }
    return ret;
}

/** 
 * Set the inner item as the current one. If there's no inner item then returns false.
 */
bool frame_moveinner( TiFrame * frame )
{
    bool ret = false;
    if (frame->curlayer < frame->firstlayer + frame->layercount - 1)
    {
        frame->curlayer ++;
        ret = true;
    }
    return ret;
}

/** 
 * Set the outer item as the current one. If there's no outer item then returns false.
 */
bool frame_moveouter( TiFrame * frame )
{
    bool ret = false;
    if (frame->curlayer > frame->firstlayer)
    {
        frame->curlayer --;
        ret = true;
    }
    return ret;
}

bool frame_moveoutermost( TiFrame * frame )
{
    frame->curlayer = frame->firstlayer;
    return true;
}

bool frame_moveinnermost( TiFrame * frame )
{
    frame->curlayer = frame->firstlayer + frame->layercount - 1;
    return true;
}

uintx frame_innermost( TiFrame * frame )
{
    return frame->firstlayer + frame->layercount - 1;
}

uintx frame_outermost( TiFrame * frame )
{
    return frame->firstlayer;
}

bool frame_initlayer( TiFrame * frame, uint8 layerindex, uintx layerstart, uintx layercapacity )
{
	bool ret;
	// the input layerindex must inside range [0,CONFIG_FRAME_LAYER_CAPACITY-1]
	rtl_assert( layerindex < CONFIG_FRAME_LAYER_CAPACITY );
	
	// if the specified layer is already exist, then simply return false
	if (frame->layercapacity[layerindex] != 0)
	{
		rtl_assert( layerstart + layercapacity < frame_buffercapacity(frame) );

		frame->layerstart[layerindex] = layerindex;
		frame->layerlength[layerindex] = 0;
		frame->layercapacity[layerindex] = layercapacity;
		
		if (frame->layercount == 0)
		{
			frame->firstlayer = layerindex;
			frame->curlayer = layerindex;
		}
			
		frame->layercount ++;
		ret = true;
	}
	else
		ret = false;
	
	return ret;
}

bool frame_addlayerinterior( TiFrame * frame, uintx offset, uintx left )
{
	uintx last = frame->firstlayer + frame->layercount - 1;
	if (last+1 < CONFIG_FRAME_LAYER_CAPACITY)
	{
		rtl_assert( frame->layercapacity[last] - offset - left > 0 );
		return frame_initlayer( frame, last+1, frame->layerstart[last] + offset,
			frame->layercapacity[last] - offset - left ); 
	}
	else
		return false;
}

bool frame_addlayerexterior( TiFrame * frame, uintx offset, uintx left )
{
	uintx first = frame->firstlayer;
	if (first > 0)
	{
		rtl_assert( frame->layerstart[first] - offset >= 0 );
		rtl_assert( frame->layercapacity[first] + offset + left <= frame_buffercapacity(frame) );
		return frame_initlayer( frame, first-1, frame->layerstart[first] - offset,
			frame->layercapacity[first] + offset + left ); 
	}
	else
		return false;
}

/** 
 * add inner item and also change the current to it
 * 
 * @param skiplen: header size of the current item
 * @param left: tail size of the current item
 */
bool frame_skipinner( TiFrame * frame, uintx skiplen, uintx left )
{
    bool ret = false;
    uintx cur;

    if (frame->layercount == 0)
    {
        frame->firstlayer = 0;
        frame->curlayer = 0;
        frame->layercount = 1;
        frame->layerstart[0] = skiplen;
        frame->layerlength[0] = 0;
        frame->layercapacity[0] = frame_buffercapacity(frame) - skiplen - left;
    }
    else{
        cur = frame->curlayer;
        if (cur < CONFIG_FRAME_LAYER_CAPACITY - 1)
        {
            frame->layerstart[cur+1] = frame->layerstart[cur] + skiplen;
            frame->layerlength[cur+1] = 0;
            frame->layercapacity[cur+1] = frame->layercapacity[cur] - skiplen - left;
            rtl_assert( frame->layercapacity[cur] >= (skiplen + left) );

            frame->curlayer ++;
            frame->layercount ++;
            ret = true;
        }
    }

    return ret;
}

bool frame_skipouter( TiFrame * frame, uintx skiplen, uintx left )
{
    bool ret = false;
    uintx cur;

    if (frame->layercount == 0)
    {
        frame->firstlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->curlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->layercount = 1;
        frame->layerstart[frame->curlayer] = skiplen;
        frame->layerlength[frame->curlayer] = 0;
        frame->layercapacity[frame->curlayer] = frame_buffercapacity(frame) - skiplen - left;
    }
    else{
        cur = frame->curlayer;
        if (cur > 0)
        {
            /* assert: the empty part before this layer must be enough to hold the low
            * layer's header length which is specified by parameter "skiplen" */

            rtl_assert( skiplen <= frame->layerstart[cur] );

            frame->layerstart[cur-1] = frame->layerstart[cur] - skiplen;
            frame->layerlength[cur-1] = 0;
            frame->layercapacity[cur-1] = frame->layercapacity[cur] + skiplen + left;

            /* assert: the tail of the low layer must not exceed the limit of the frame object's memory size */
            rtl_assert( frame->layercapacity[cur-1] + frame->layerstart[cur-1] <= frame->memsize );

            frame->firstlayer = cur - 1;
            frame->curlayer --;
            frame->layercount ++;
            ret = true;
        }
    }
    return ret;
}

bool frame_removelayerinterior( TiFrame * frame )
{
	bool ret = false;
	uintx last;
	
	if (frame->layercount > 0)
	{
		last = frame->firstlayer + frame->layercount - 1;
		frame->layerstart[last] = 0;
		frame->layerlength[last] = 0;
		frame->layercapacity[last] = 0;
		
		frame->layercount --;
		if (frame->layercount == 0)
		{
			frame->firstlayer = 0;
			frame->curlayer = 0;
		}
		else{
			if (frame->curlayer == last)
				frame->curlayer --;
		}
		
		ret = true;
	}
	
	return ret;
}

bool frame_removelayerexterior( TiFrame * frame )
{
	bool ret = false;
	uintx first;
	
	if (frame->layercount > 0)
	{
		first = frame->firstlayer;
		frame->layerstart[first] = 0;
		frame->layerlength[first] = 0;
		frame->layercapacity[first] = 0;
		
		frame->layercount --;
		if (frame->layercount == 0)
		{
			frame->firstlayer = 0;
			frame->curlayer = 0;
		}
		else{
			frame->firstlayer++;
			if (frame->curlayer == first)
				frame->curlayer --;
		}
		
		ret = true;
	}
	
	return ret;
}

void frame_clear( TiFrame * frame )
{
    rtl_assert( frame->curlayer > 0 );
    frame->layerlength[frame->curlayer] = 0;
}

/**
 * resize the current layer frame to specified new size. this function is usually
 * called after an frame object is constructed because the default frame will occupy
 * all the memmory available in the buffer.
 * 
 * @return the new size of the current layer frame is succeed. 0 if failed.
 * 
 * @warning
 *
 */
uintx frame_resize( TiFrame * frame, uintx newsize )
{
    frame_setlayercapacity( frame, frame->curlayer, newsize );
    return 0;
}

uintx frame_start( TiFrame * frame )
{
    rtl_assert( frame->curlayer > 0 );
    return frame->layerstart[frame->curlayer];
}

uintx frame_end( TiFrame * frame )
{
    uintx cur = frame->curlayer;
    return frame->layerstart[cur] + frame->layerlength[cur] - 1;
}

uintx frame_length( TiFrame * frame )
{
    return frame->layerlength[frame->curlayer];
}

uintx frame_capacity( TiFrame * frame )
{
    return frame->layercapacity[frame->curlayer];
}

/**
 * return an memory pointer to the first byte of the current item's data. if there's
 * no data in the current item, then return NULL. it's almost the same as frame_startptr()
 * except it will return NULL when the item is empty.
 */
char * frame_dataptr( TiFrame * frame )
{
    char * ptr;
    uintx cur = frame->curlayer;

    if (frame->layerlength[cur] == 0)
        ptr = NULL;
    else
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[cur];

    return ptr;
}

char * frame_startptr( TiFrame * frame )
{
    return (char *)frame + sizeof(TiFrame) + frame->layerstart[frame->curlayer];
}

char * frame_endptr( TiFrame * frame )
{
    uintx cur;
    char * ptr;

    cur = frame->curlayer;
    if (frame->layerlength[cur] == 0)
        ptr = NULL;
    else
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[cur] + frame->layerlength[cur] - 1;

    return ptr;
}

char * frame_header( TiFrame * frame, uintx * psize )
{
    uintx cur;
    if (psize != NULL)
    {
        cur = frame->curlayer;
        if (cur < frame_lastlayer(frame))
        {
            *psize = frame->layerstart[cur+1] - frame->layerstart[cur];
        }
        else{
            *psize = frame->layercapacity[cur];
        }
    }
    return frame_startptr(frame);
}

/**
 * returns the body pointer of current item. the pointer value is actually pointed 
 * to the next item's start address.
 * 
 * @return
 *  if the current layer is already the last layer, then returns NULL.
 *  otherwise it returns an pointer to the payload section in current layer, which 
 *  is also the startptr or headerptr of the inner layer frame.
 * 
 * Q: what's the difference between the following two kinds of code lets?
 *  method 1:
 *      ptr = frame_interior(f, &size);
 *  method 2:
 *      frame_moveinner(f);
 *      ptr = frame_startptr(f)
 * 
 * R: the second method will affect the internal current index, while the first one
 *  doesn't.
 * 
 */
char * frame_interior( TiFrame * frame, uintx * psize )
{
    char * ptr;
    uintx cur = frame->curlayer;

    if (cur < frame_lastlayer(frame))
    {
        if (psize != NULL)
        {
            *psize = frame->layercapacity[cur+1];
        }
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[cur+1];
    }
    else{
        if (psize != NULL)
        {
            *psize = 0;
        }
        ptr = NULL;
    }
    return ptr;
}

char * frame_tail( TiFrame * frame, uintx * psize )
{
    uintx next;
    char * ptr;
	uint8 cur = frame->curlayer;

    if (frame->curlayer < frame->layercount - 1)
    {
        next = frame->curlayer+1;
        if (psize != NULL)
        {
            *psize = frame->layerstart[cur] + frame->layercapacity[cur] - frame->layerstart[next] - frame->layercapacity[next];
        }
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[next] + frame->layercapacity[next];
    }
    else{
        if (psize != NULL)
        {
            *psize = 0;
        }
        ptr = NULL;
    }
    return ptr;
}

bool frame_full( TiFrame * frame )
{
    uintx cur = frame->curlayer;
    return (frame->layerlength[cur] == frame->layercapacity[cur]);
}

bool frame_empty( TiFrame * frame )
{
    return (frame->layerlength[frame->curlayer] == 0);
}

uintx frame_available( TiFrame * frame )
{
    uintx cur = frame->curlayer;
    return (frame->layercapacity[cur] - frame->layerlength[cur]);
}

uintx frame_read( TiFrame * frame, char * buf, uintx size )
{
	uintx count = min( size, frame_length(frame) );
	memmove( buf, frame_startptr(frame), count );
	return count;
}

uintx frame_write( TiFrame * frame, char * data, uintx len )
{
	uintx count = min( frame_capacity(frame), len );
	memmove( frame_startptr(frame), data, count );
	frame->layerlength[frame->curlayer] = count;
	return count;
}

/* frame_pushback
 * append the data at the end of the original data. If there's not enough space,
 * then only the (size-length) characters will be padded.
 */
 uintx frame_pushback( TiFrame * frame, char * data, uintx len )
{
	uintx count = min(frame_available(frame), len);
	memmove( frame_endptr(frame), data, count );
	frame->layerlength[frame->curlayer] += count;
	return count;
}

/* frame_pushbyte
 * append a single byte at the end of the original data
 */
uintx frame_pushbyte( TiFrame * frame, unsigned char value )
{
    char * ptr = frame_startptr(frame) + frame_length(frame);
    uintx cur = frame->curlayer;

    if (frame->layerlength[cur] < frame->layercapacity[cur])
    {
	    *ptr = value;
	    frame->layerlength[cur] ++;
        return 1;
    }
    else 
        return 0;
}

uintx frame_front( TiFrame * frame, char * buf, uintx len )
{
	uintx count = min( len, frame_length(frame) );
	memmove( buf, frame_startptr(frame), count );
	return count;
}

void frame_popfront( TiFrame * frame, uintx count )
{
	if (count < frame_length(frame))
	{
		memmove( frame_startptr(frame), frame_startptr(frame)+count, frame_length(frame) - count );
		frame->layerlength[frame->curlayer] -= count;
	}
	else
		frame->layerlength[frame->curlayer] = 0;
}

bool frame_set( TiFrame * frame, uintx idx, char c )
{
	rtl_assert( idx < frame_capacity(frame) );
	if (idx < frame_capacity(frame))
	{
		frame_startptr(frame)[idx] = c;
		return true;
	}
	else
		return false;
}

bool frame_get( TiFrame * frame, uintx idx, char * c )
{
	rtl_assert( idx < frame_length(frame) );
	if (idx < frame_length(frame))
	{
		*c = frame_startptr(frame)[idx];
		return true;
	}
	else
		return false;
}

uintx frame_copyfrom( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame1), frame_length(frame2) );
	memmove( frame_startptr(frame1), frame_startptr(frame2), count );
	frame1->layerlength[frame1->curlayer] = count;
	return count;
}

uintx frame_copyto( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame2), frame_length(frame1) );
	memmove( frame_startptr(frame2), frame_startptr(frame1), count );
	frame2->layerlength[frame2->curlayer] = count;
	return count;
}

uintx frame_movefrom( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame1), frame_length(frame2) );
	memmove( frame_startptr(frame1), frame_startptr(frame2), count );
	frame1->layerlength[frame1->curlayer] = count;
	frame_popfront(frame2, count);
	return count;

}

uintx frame_moveto( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame2), frame_length(frame1) );
	memmove( frame_startptr(frame2), frame_dataptr(frame1), count );
	frame2->layerlength[frame2->curlayer] = count;
	frame_popfront(frame1, count);
	return count;
}

// append frame2 to the end of frame1. it's quite similar to frame_pushback
uintx frame_append( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count=0;

	rtl_assert( (frame1 != NULL) && (frame2 != NULL) );
	count = min(frame_available(frame1), frame_length(frame2));
	if (count > 0)
	{
		memmove( frame_endptr(frame1), frame_startptr(frame2), count );
		frame_adjustlength( frame1, count );
	}
	return count;
}

void frame_setlength( TiFrame * frame, uintx count )
{
	frame->layerlength[frame->curlayer] = count;
}

void frame_adjustlength( TiFrame * frame, int delta )
{
	frame->layerlength[frame->curlayer] += delta;
}

#ifdef CONFIG_DEBUG
void frame_dump( TiFrame * frame )
{
    uintx i;

    dbc_string( "frame_dump(frame): " );
    dbc_string( "frame_memsize = " );
    dbc_n16toa( frame->memsize );
    dbc_string( "frame->firstlayer = " );
    dbc_n16toa( frame->firstlayer );
    dbc_string( "frame->curlayer = " );
    dbc_n16toa( frame->curlayer );
    dbc_string( "frame->layercount = " );
    dbc_n16toa( frame->layercount );
    dbc_putchar( '\n' );

    for (i=0; i<CONFIG_FRAME_LAYER_CAPACITY; i++)
    {
        dbc_string( "layer " );
        dbc_n8toa( i );
        dbc_putchar( ':' );
        dbc_putchar( ' ' );
        dbc_n16toa( frame->layerstart[i] );
        dbc_putchar( ',' );
        dbc_n16toa( frame->layerlength[i] );
        dbc_putchar( ',' );
        dbc_n16toa( frame->layercapacity[i] );
        dbc_putchar( '\n' );
    }

    dbc_string( "content: " );
    dbc_mem( frame_totalstartptr(frame), frame_totallength(frame) );
    dbc_putchar( '\n' );
}
#endif
