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
 * TiFrame
 * TiFrame manipulates nested frame structure widely used in communication protocols.
 * It manages different layers. All these layer shares same memory space. 
 * Furthermore, each layer can be manipulated as if they were an independent TiIoBuf
 * object.
 *  *
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
 * @modified by zhangwei on 2011.08.03
 *	- improved frame_close(). We needn't to memset() here. The old code will lose
 *    the memsize value, which will lead to unexpected failed if calling frame_open()
 *    to initialize it again.
 * @modified by zhangwei on 2011.08.04
 * 	- add frame_putchar() and frame_getchar()
 ******************************************************************************/
 
 /**
 * Q: Why TiFrame?
 * R: 网络协议栈是一个有层次的软件结构，层与层之间通过预定的接口传递网络报文。
 * 网络报文中包含了在协议栈各层使用到的各种信息。网络报文的长度是不固定的，因此采用什么样的数据结构来存储这些
网络报文就显得非常重要。在BSD的实现中，采用的数据结构是 mbuf，它所
能存储的数据的长度是固定的，如果一个网络报文需要多个 mbuf，这些 mbuf
链接成一个链表。所以同一个网络报文里的数据在内存中的存储可能是不连续
的。在LINUX的实现中，同一个网络报文的数据在内存中是连续存放的，每个
网络报文都有一个控制结构，叫做sk_buff。
 *
 * 	In openwsn architecture, the equivalent object to mbuf or sk_buff is TiFrame.
 * We use TiFrame to management the frame/packet description and it's data together.
 */

#include "rtl_configall.h"
#ifdef CONFIG_DEBUG
#include <stdlib.h>
#endif
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_frame.h"
#ifdef CONFIG_DEBUG
#include "rtl_debugio.h"
#endif

#define _frame_layer_start(frame,idx) ((frame)->layerstart[idx])
#define _frame_layer_end(frame,idx) ((frame)->layerstart[idx] + (frame)->layerlength[idx] - 1)
#define _frame_layer_length(frame,idx) ((frame)->layerlength[idx])
#define _frame_layer_capacity(frame,idx) ((frame)->layercapacity[idx])

static void _frame_initlayer( TiFrame * frame, uint8 layer, uintx layerstart, uintx layercapacity );

/** 
 * create an TiFrame object with required capacity.
 * @return An memory pointer to the TiFrame object created. Or return NULL when failed.
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx buffercapacity, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
    char * buf;

    uintx memsize = FRAME_HOPESIZE(buffercapacity);
    buf = (char *)malloc( memsize );
    rtl_assert( buf != NULL );
    if (buf != NULL);
        frame_open( buf, buffercapacity, init_layerindex, init_layerstart, init_layercapacity );

    return (TiFrame *)buf;
}
#endif

/** free a frame created by frame_create() */
#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame )
{
	if (frame != NULL)
	{
		frame_close( frame );
		free( frame );
	}
}
#endif

/**
 * Construct an TiFrame object on specified memory block and initialized it for R/W 
 * operations. This memory block must be allocatedly before.
 * 
 * After open, the frame "curlayer" will be the "initlayer".
 * 
 * @param buf Memory block start address
 * @param uintx Memory block size.
 * @param init_layerindex Indicate the index of the initial layer. 
 * @param init_layerstart Indicate where the initial layer start in the frame internal buffer.
 * @param init_layercapacity The capacity of the initial layer.
 */
TiFrame * frame_open( char * buf, uintx memsize, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
    TiFrame * frame;

    /**
     * @attention Since the TiFrame object requires a large memory block, we strongly
     * suggest you define "uintx" as "uint16" in "configall.h" even in 8-bit architecture 
     * systems. You should be very caution when "uintx" is defined as "uint8" if 
     * TiFrame is used in such projects.
	 * 
	 * You can comment the assertion related to this check in frame_open() function.
	 */
	
    rtl_assert( sizeof(uintx) >= 2 );

	/* guarantee the initial layer can be placed in the frame */
    rtl_assert( init_layercapacity <= memsize - sizeof(TiFrame) - init_layerstart );

    memset( buf, 0x00, memsize );
    frame = (TiFrame *)(buf);
    frame->memsize = memsize;
    frame_reset( frame, init_layerindex, init_layerstart, init_layercapacity );

    return frame;
}

/**
 * Destroy an TiFrame object. After you close an frame, you should call frame_open()
 * or frame_reset() again to make it usable.
 * 
 * @see frame_open(), frame_reset()
 */
void frame_close( TiFrame * frame )
{
	frame->firstlayer = 0;
	frame->curlayer = 0;
	frame->layercount = 0;
    return;
}

/**
 * Duplicate an TiFrame object. this function will allocated memory used, so you 
 * should free the memory after using the frame. 
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

/**
 * Reset the TiFrame object. After resetting, the frame object is the same as it
 * is just initialized by calling frame_open(). There will be only one layer existed 
 * after resetting for reading and writing.
 * 
 * frame_reset() will also set the frame's "curlayer" to the "initlayer".
 * 
 * @attention
 * When you want to reuse a TiFrame object without re-open it again, you can call
 * this function. However, this function assumes the frame object has been opened 
 * at least once before. (or else this function will not know frame->memsize. This
 * will lead to unexpected error.
 * 
 * @see TiIoBuf object, iobuf_clear(), frame_open(), frame_totalclear(), frame_bufferclear()
 * 
 * @param frame The pointer to the frame object to be reset
 * @param init_layerindex Indicate the index of the initial layer. 
 * @param init_layerstart Indicate where the initial layer start in the frame internal buffer.
 * @param init_layercapacity The capacity of the initial layer. If this value is 0,
 *		then the currently layer will occupy all the memory left in the buffer.
 * @return None
 */
void frame_reset( TiFrame * frame, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity )
{
	uintx memsize;
	
	rtl_assert( frame != NULL );

	/* clear the memory block while still keeping the "memsize" property */
	memsize = frame->memsize;
	memset( (char*)frame, 0x00, memsize );
	frame->memsize = memsize;

	/* guarantee the layer can be placed inside frame buffer */
    rtl_assert( init_layercapacity <= memsize - sizeof(TiFrame) - init_layerstart );

	frame->option = 0x00;
    frame->firstlayer = init_layerindex;
    frame->curlayer = init_layerindex;
    frame->layercount = 1;
    frame->layerstart[init_layerindex] = init_layerstart;
    frame->layerlength[init_layerindex] = 0;
    frame->layercapacity[init_layerindex] = ((init_layercapacity == 0) ? (frame->memsize - sizeof(TiFrame) - init_layerstart ) : init_layercapacity);
}

/**
 * This function will initialize an specified layer item in the layer table inside
 * an frame object. 
 * 
 * @attention
 * 	- This function doesn't check whether the specified layer is already existed 
 * 	  or not.
 *  - This function doesn't affect the "curlayer" property of the frame object, 
 * but it will blindly increase the "layercount" property. This may lead to inconsistent 
 * frame settings if cannot used peroperly. Suggest to use this function in the 
 * TiFrame implementation only.
 *  - It doesn't affect the layercount property. 
 * 
 * @modified by zhangwei on 2011.08.03
 * 	- For simplicity and efficiency reasons, this function doesn't check whether 
 *    the specified layer exists or not. In the past, this function will check 
 * 	  whether the specified layer exists.
 *  - Return type is changed from "bool" to "void".
 */
void _frame_initlayer( TiFrame * frame, uint8 layer, uintx layerstart, uintx layercapacity )
{
	/* the input layerindex must inside range [0,CONFIG_FRAME_LAYER_CAPACITY-1] */
	rtl_assert( layer < CONFIG_FRAME_LAYER_CAPACITY );
	rtl_assert( layerstart + layercapacity < frame_totalcapacity(frame) );

    frame->layerstart[layer] = layerstart;
    frame->layerlength[layer] = 0;
    frame->layercapacity[layer] = layercapacity;
        
    if (frame->layercount == 0)
    {
		frame->firstlayer = layer;
        // frame->curlayer = layerindex;
    }
            
    // frame->layercount ++;
}

/** 
 * Returns the TiFrame's internal buffer which is used to save data 
 * @return An memory pointer to the internal buffer.
 */
char * frame_totalbuffer( TiFrame * frame )
{
	return ((char*)frame) + sizeof(TiFrame);
}

/**
 * Clear the TiFrame buffer. This function will reset all layer's "length" property
 * to 0. Namely, it will clear the data in all existed layers, but it will not affect
 * layer structures.
 *
 * Q: what's the difference between frame_reset() and frame_totalclear()?
 * R: frame_reset() will remove all layers and re-create an initial layer again. 
 * frame_totalclear() only clears the data in all layers, but will not affect the 
 * layer structure. It will not remove any layers.
 * 
 * @modified by zhangwei on 2010.08.07
 * - in the past, this function will automatically create the first layer. however,
 *   this may cause problems if you start the frame at the the application layer.
 *   now it will clear all layers. you need to create the your first layer manually.
 * @modified by zhangwei on 2011.08.03
 * - Change function name from frame_bufferclear() to frame_totalclear()
 * - The behavior of this function is redefined.
 */
void frame_totalclear( TiFrame * frame )
{
	uintx i;
	for (i=frame->firstlayer; i<frame->firstlayer+frame->layercount; i++)
		frame->layerlength[i] = 0;
}

/**
 * Total capacity of the TiFrame object. It's biggest frame size that TiFrame can 
 * accepted. It's also the maximum size of the lowest layer frame.
 * 
 * @modified by openwsn on 2011.08.03
 * - In the previous version, the frame_totalcapacity is defined as macro to the capacity
 * of the first existed layer. But this isn't convenient in some cases. So it was finally
 * changed to the total possible space length of the entire frame.
 */
uintx frame_totalcapacity( TiFrame * frame )
{
    return (uintx)((frame->memsize) - sizeof(TiFrame));
}

/** Copy an TiFrame entirely from one to another */
uintx frame_totalcopyfrom( TiFrame * frame, TiFrame * from )
{
	uintx memsize = frame->memsize;
	rtl_assert( frame->memsize >= from->memsize );
	memmove( frame, from, from->memsize );
	frame->memsize = memsize;
	return from->memsize;
}

/** Copy an TiFrame entirely from one to another */
uintx frame_totalcopyto( TiFrame * frame, TiFrame * to )
{
	return frame_totalcopyfrom(to, frame);
}

uintx frame_layerstart( TiFrame * frame, uint8 layer )
{
	rtl_assert(frame_layerexists(frame,layer));
    return frame->layerstart[layer];
}

char * frame_layerstartptr( TiFrame * frame, uint8 layer )
{
	rtl_assert(frame_layerexists(frame,layer));
    return (char*)frame + sizeof(TiFrame) + (frame->layerstart[layer]);
}

uintx frame_layerend( TiFrame * frame, uint8 layer )
{
	rtl_assert(frame_layerexists(frame,layer));
    return frame->layerstart[layer] + frame->layerlength[layer] - 1;
}

uintx frame_layerlength( TiFrame * frame, uint8 layer )
{
	rtl_assert(frame_layerexists(frame,layer));
    return frame->layerlength[layer];
}

uintx frame_layercapacity( TiFrame * frame, uint8 layer )
{
	rtl_assert(frame_layerexists(frame,layer));
    return frame->layercapacity[layer];
}

/** 
 * Set data length of the current layer.
 * This function will check the layer length value against layer capacity. The length
 * should be smaller than or equal to layer capacity. 
 * 
 * @warning
 * This function doesn't check with the above layer length or the below layer length.
 * So there maybe inconsistence between two adjacent layers.
 */
void frame_setlayerlength( TiFrame * frame, uint8 layer, uintx len )
{
	rtl_assert(frame_layerexists(frame,layer));

	/* assert the layer length not exceeds the layer capacity previously configured */
	rtl_assert(len < frame->layercapacity[layer]);
	
    frame->layerlength[layer] = min(len, frame->layercapacity[layer]);
}

/**
 * Set the capacity of current layer. The new capacity should larger than data length.
 * 
 * @warning
 * Since this function doesn't check the above/below layer data length or capacity 
 * restrictions, the incorrect value of new capacity may lead to inconsistent between 
 * difference layers.
 * 
 * @modified by zhangwei on 2011.08.03
 * - In the past, this function may also change adjacent layer capacities. In order
 *   to make the behavior more definite, I removed these source codes (moved into 
 *   frame_totalshrink() and frame_totalexpand() function) and now this function
 *   only affect the specified layer capacity.
 */
bool frame_setlayercapacity( TiFrame * frame, uint8 layer, uintx capacity )
{
	bool ret=false;

	rtl_assert(frame_layerexists(frame,layer) && (frame_layerlength(frame,layer) < capacity));
    

    if (frame->layercount > 0)
    {
		rtl_assert((frame->layerlength[layer] <= capacity) && (capacity <= frame_totalcapacity(frame)));
		
		if ((frame->layerlength[layer] <= capacity) && (capacity <= frame_totalcapacity(frame)))
		{
			frame->layercapacity[layer] = capacity;
			ret = true;
		}
	}
			
	return ret;
}

/**
 * Shrink the layer capacity. 
 * 
 * @param choice Decide the behavior of the shrink process.
 *      0   Shrink the current layer only. 
 *		1	All above layers including the current one. 
 *		2   All below layers including the current one. 
 *		3 	All existed layers.
 *		other values are invalid.
 *		
 * @modified by zhangwei on 2011.08.04
 * - First added. Originally, frame_setcapacity() can do part of this work. Now, frame_setcapacity()
 *   has changed its behavior and split as frame_setcapacity(), frame_shrinklayer()
 *   and frame_expandlayer().
 */
void frame_shrinklayer( TiFrame * frame, uint8 layer, uintx newcapacity, uint8 choice )
{
    intx delta;
    uintx i;
    uintx from, to;

    if (frame_layerexists(frame,layer))
    {
		rtl_assert((frame->layerlength[layer] <= newcapacity) && (newcapacity <= frame->layercapacity[layer]));
        frame->layercapacity[layer] = newcapacity;

        delta = frame->layercapacity[layer] - newcapacity;
		
		rtl_assert( choice < 4 );
		from=0; 
		to=0;
		from = (((choice == 2) || (choice == 3)) ? frame->firstlayer : layer);
		to = (((choice == 1) || (choice == 3)) ? (frame->firstlayer + frame->layercount-1) : layer);
		
        for (i=from; i<=to; i++)
        {
            rtl_assert( frame->layercapacity[i] >= delta );

            if (frame->layercapacity[i] >= delta)
				frame->layercapacity[i] -= delta;
			else
				frame->layercapacity[i] = 0;
        }
	}

	return;
}

/**
 * Expand the layer capacity. 
 * 
 * @param choice Decide the behavior of the shrink process.
 *      0   Shrink the current layer only. 
 *		1	All above layers including the current one. 
 *		2   All below layers including the current one. 
 *		3 	All existed layers.
 *		other values are invalid.
 * 
 * @warning
 *  Even though this function have an assertion to guarantee the new capacity should 
 * be smaller than frame_totalcapacity(frame), it still possible to lead to buffer 
 * overrun(the setting capacity is larger than the real buffer capacity). So be careful
 * with the value of "newcapacity" parameter.
 *		
 * @modified by zhangwei on 2011.08.04
 * - First added. Originally, frame_setcapacity() can do part of this work. Now, 
 *   frame_setcapacity() has changed its behavior and split as frame_setcapacity(), 
 *   frame_shrinklayer() and frame_expandlayer().
 */
void frame_expandlayer( TiFrame * frame, uint8 layer, uintx newcapacity, uint8 choice )
{
    intx delta;
	uintx i;
    uintx from, to;

    if (frame_layerexists(frame,layer))
    {
		rtl_assert((frame->layercapacity[layer] <= newcapacity) && (newcapacity <= frame_totalcapacity(frame)));
        frame->layercapacity[layer] = newcapacity;

        delta = newcapacity - frame->layercapacity[layer];
		
		rtl_assert( choice < 4 );
		from=0; to=0;
		from = (((choice == 2) || (choice == 3)) ? frame->firstlayer : layer);
		to = (((choice == 1) || (choice == 3)) ? (frame->firstlayer + frame->layercount-1) : layer);
		
        for (i=from; i<=to; i++)
        {
			frame->layercapacity[i] += delta;

			rtl_assert(frame->layerstart[i] + frame->layercapacity[i] <= frame_totalcapacity(frame));

			if (frame->layerstart[i] + frame->layercapacity[i] > frame_totalcapacity(frame))
				frame->layercapacity[i] = frame_totalcapacity(frame);
        }
	}

	return;
}

/** Whether the specified layer exists */
bool frame_layerexists( TiFrame * frame, uint8 layer )
{
	return ((frame->layercount > 0) && (layer >= frame->firstlayer) && (layer < frame->firstlayer + frame->layercount));
}

/**
 * Returns the index of the current layer. The lowest level (outer most) layer is with 
 * index 0. the TiFrame currently support at most 8 layers. this can be reconfigured
 * by macro CONFIG_FRAME_LAYER_CAPACITY.
 */
uintx frame_curlayer( TiFrame * frame )
{
    return frame->curlayer;
}

/** Switch the current layer. Nothing changed if failed. */
bool frame_setcurlayer( TiFrame * frame, uintx layer )
{
    bool ret = false;
	if (frame_layerexists(frame, layer))
	{
        frame->curlayer = layer;
        ret = true;
    }
    return ret;
}

/** Returns the highest most layer id */
uintx frame_highestlayer( TiFrame * frame )
{
    return frame->firstlayer + frame->layercount - 1;
}

/** Returns the lowest layer id */
uintx frame_lowestlayer( TiFrame * frame )
{
    return frame->firstlayer;
}

/** Change the current layer to the higher one */
#define frame_moveinner(f) frame_movehigher(f)
bool frame_movehigher( TiFrame * frame )
{
    bool ret = false;
    if (frame->curlayer < frame->firstlayer + frame->layercount - 1)
    {
        frame->curlayer ++;
        ret = true;
    }
    return ret;
}

/** Change the current layer to the lower one */
#define frame_moveouter(f) frame_movelower(f)
bool frame_movelower( TiFrame * frame )
{
    bool ret = false;
    if (frame->curlayer > frame->firstlayer)
    {
        frame->curlayer --;
        ret = true;
    }
    return ret;
}

bool frame_movelowest( TiFrame * frame )
{
    frame->curlayer = frame->firstlayer;
    return true;
}

bool frame_movehighest( TiFrame * frame )
{
    frame->curlayer = frame->firstlayer + frame->layercount - 1;
    return true;
}

/**
 * Add a higher level layer. 
 * Attention this function assumes there's already an layer exists, or else it doesn't 
 * know where to place the new layer.
 * 
 * Different to frame_skipinner and frame_skipouter, this function doesn't affect the curlayer settings.
 * 
 * frame_skipinner = frame_addlayerinner + frame_movehigher
 * 
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the outer layer.
 */
bool frame_addlayerinterior( TiFrame * frame, uintx offset, uintx left )
{
	uintx last = frame->firstlayer + frame->layercount - 1;
	
	rtl_assert( frame_layerexists(frame,last) );
	
	if (last+1 < CONFIG_FRAME_LAYER_CAPACITY)
	{
		/* assert the current highest layer frame is enough to keep the new 
		 * layer frame */
		rtl_assert( frame->layercapacity[last] - offset - left > 0 );
		
		_frame_initlayer( frame, last+1, frame->layerstart[last] + offset,
			frame->layercapacity[last] - offset - left ); 
		frame->layercount++;
		return true;
	}
	else
		return false;
}

/**
 * Add a lower level layer. 
 * Attention this function assumes there's already an layer exists, or else it doesn't 
 * know where to place the new layer.
 * 
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the new layer.
 */
bool frame_addlayerexterior( TiFrame * frame, uintx offset, uintx left )
{
	uintx first = frame->firstlayer;
	if (first > 0)
	{
		rtl_assert( frame->layerstart[first] > offset );
		rtl_assert( frame->layercapacity[first] + offset + left <= frame_totalcapacity(frame) );
		_frame_initlayer( frame, first-1, frame->layerstart[first] - offset,
			frame->layercapacity[first] + offset + left ); 
		frame->layercount ++;
		return true;
	}
	else
		return false;
}

/**
 * Remove the highest level layer from the frame object.
 *
 * @attention Generally this function doesn't affect the "curlayer" property of 
 *   the frame object. However, the "curlayer" will decrease by 1 if the most inner
 *   layer(last layer) is the "curlayer". Furthermore, if there's no layer after
 *   the remove, then the "firstlayer", "curlayer" and "layercount" property are 
 *   all reset to 0.
 * 
 * @return true when success.
 */
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

/**
 * Remove the lowest level layer from the frame object.
 *
 * @attention Generally this function doesn't affect the "curlayer" property of 
 *   the frame object. However, the "curlayer" will decrease by 1 if the most inner
 *   layer(last layer) is the "curlayer". Furthermore, if there's no layer after
 *   the remove, then the "firstlayer", "curlayer" and "layercount" property are 
 *   all reset to 0.
 * 
 * @return true when success.
 */
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

/** 
 * Add an inner layer and also change the current layer to it. If the destination layer
 * is already exist, then simply switch the "curlayer" property to that one.
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
		_frame_initlayer( frame, 0, skiplen, frame_totalcapacity(frame) - skiplen - left);
		frame->firstlayer = 0;
		frame->curlayer = 0;
		frame->layercount = 1;
	}
	else{
        cur = frame->curlayer;
        if (cur < CONFIG_FRAME_LAYER_CAPACITY - 1)
        {
            if (cur == (frame->firstlayer + frame->layercount - 1))
            {
                rtl_assert(frame->layercapacity[cur] >= (skiplen + left));
                _frame_initlayer( frame, cur+1, frame->layerstart[cur] + skiplen, frame->layercapacity[cur] - skiplen - left);
                frame->curlayer ++;
                frame->layercount ++;
                ret = true;
            }
            else{
                frame->curlayer ++;
                ret = true;
            }
        }
    }

	/*
    if (frame->layercount == 0)
    {
		TiFrame * frame, uint8 layer, uintx layerstart, uintx layercapacity )
	
        frame->firstlayer = 0;
        frame->curlayer = 0;
        frame->layercount = 1;
        frame->layerstart[0] = skiplen;
        frame->layerlength[0] = 0;
        frame->layercapacity[0] = frame_totalcapacity(frame) - skiplen - left;
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
	*/

    return ret;
}

/**
 * Add an outer layer and also change the current layer to it. If the destination layer
 * is already exist, then simply switch the "curlayer" property to that one.
 */
bool frame_skipouter( TiFrame * frame, uintx skiplen, uintx left )
{
    bool ret = false;
    uintx cur;

    if (frame->layercount == 0)
    {
		_frame_initlayer( frame, CONFIG_FRAME_LAYER_CAPACITY-1, skiplen, frame_totalcapacity(frame) - skiplen - left);
        frame->firstlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->curlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->layercount = 1;
    }
    else{
        cur = frame->curlayer;
        if (cur > 0)
        {
            if (cur == frame->firstlayer)
            {
                /* assert: the empty part before this layer must be enough to hold the low
                 * layer's header length which is specified by parameter "skiplen" */

                rtl_assert( skiplen <= frame->layerstart[cur] );

                _frame_initlayer( frame, cur-1, frame->layerstart[cur] - skiplen, frame->layercapacity[cur] + skiplen + left);

                /* assert: the tail of the lower layer must not exceed the limit of the frame object's memory size */
                rtl_assert( frame->layercapacity[cur-1] + frame->layerstart[cur-1] <= frame->memsize );

                frame->firstlayer = cur - 1;
                frame->curlayer --;
                frame->layercount ++;
                ret = true;
            }
            else{
                frame->curlayer --;
                ret = true;
            }
        }
    }
/*
    if (frame->layercount == 0)
    {
        frame->firstlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->curlayer = CONFIG_FRAME_LAYER_CAPACITY-1;
        frame->layercount = 1;
        frame->layerstart[frame->curlayer] = skiplen;
        frame->layerlength[frame->curlayer] = 0;
        frame->layercapacity[frame->curlayer] = frame_totalcapacity(frame) - skiplen - left;
    }
    else{
        cur = frame->curlayer;
        if (cur > 0)
        {
            */ /* assert: the empty part before this layer must be enough to hold the low
            * layer's header length which is specified by parameter "skiplen" */ /*

            rtl_assert( skiplen <= frame->layerstart[cur] );

            frame->layerstart[cur-1] = frame->layerstart[cur] - skiplen;
            frame->layerlength[cur-1] = 0;
            frame->layercapacity[cur-1] = frame->layercapacity[cur] + skiplen + left;

            */ /* assert: the tail of the low layer must not exceed the limit of the frame object's memory size */ /*
            rtl_assert( frame->layercapacity[cur-1] + frame->layerstart[cur-1] <= frame->memsize );

            frame->firstlayer = cur - 1;
            frame->curlayer --;
            frame->layercount ++;
            ret = true;
        }
    }
*/	
    return ret;
}

/**
 * Clear the data of the current layer inside the frame object.
 * @return none
 */
void frame_clear( TiFrame * frame )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
	frame->layerlength[frame->curlayer] = 0;
}

/**
 * resize the current layer frame to specified new size. this function is usually
 * called after an frame object is constructed because the default frame will occupy
 * all the memmory available in the buffer.
 * 
 * This function is simpler than frame_shrinklayer() and frame_expandlayer() because
 * it only affects the current layer. 
 * 
 * @return the new size of the current layer frame is succeed. 0 if failed.
 * 
 * @warning This function doesn't check the higher/lower level layer restrictions.
 * Incorrect newsize parameter values will lead to inconsistent status of the TiFrame
 * layer information. *
 */
uintx frame_resize( TiFrame * frame, uintx newsize )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    frame_setlayercapacity( frame, frame->curlayer, newsize );
    return 0;
}

uintx frame_start( TiFrame * frame )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    return frame->layerstart[frame->curlayer];
}

uintx frame_end( TiFrame * frame )
{
    uintx cur;
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    cur = frame->curlayer;
    return frame->layerstart[cur] + frame->layerlength[cur] - 1;
}

uintx frame_length( TiFrame * frame )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    return frame->layerlength[frame->curlayer];
}

uintx frame_capacity( TiFrame * frame )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    return frame->layercapacity[frame->curlayer];
}

/**
 * Return an memory pointer to the first byte of the current item's data. if there's
 * no data in the current item, then return NULL. it's almost the same as frame_startptr()
 * except it will return NULL when the item is empty.
 */
char * frame_dataptr( TiFrame * frame )
{
    char * ptr;
    uintx cur = frame->curlayer;
	
    rtl_assert(frame_layerexists(frame, cur));
    if (frame->layerlength[cur] == 0)
        ptr = NULL;
    else
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[cur];

    return ptr;
}

char * frame_startptr( TiFrame * frame )
{
    rtl_assert(frame_layerexists(frame, frame->curlayer));
    return (char *)frame + sizeof(TiFrame) + frame->layerstart[frame->curlayer];
}

/**
 * Get the memory pointer to the last byte in the data buffer. if there's no data
 * in the bufer then returns NULL.
 */ 
char * frame_endptr( TiFrame * frame )
{
    char * ptr;
    uintx cur = frame->curlayer;

    rtl_assert(frame_layerexists(frame, cur));
    if (frame->layerlength[cur] == 0)
        ptr = NULL;
    else
        ptr = (char *)frame + sizeof(TiFrame) + frame->layerstart[cur] + frame->layerlength[cur] - 1;

    return ptr;
}

/**
 * Returns the memory pointer to the frame header. 
 * @param psize *pSize will be the header size. 
 */
char * frame_header( TiFrame * frame, uintx * psize )
{
    uintx cur;
	
    if (psize != NULL)
    {
        cur = frame->curlayer;
		rtl_assert(frame_layerexists(frame, cur));
		
        if (cur < frame_lastlayer(frame))
        {
            *psize = frame->layerstart[cur+1] - frame->layerstart[cur];
        }
        else{
            *psize = 0;
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

	rtl_assert(frame_layerexists(frame, cur));
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

/**
 * Get the tail of current layer.
 */
char * frame_tail( TiFrame * frame, uintx * psize )
{
    uintx next;
    char * ptr;
	uint8 cur = frame->curlayer;

	rtl_assert(frame_layerexists(frame, cur));
	
    if (cur < frame->layercount - 1)
    {
        next = cur + 1;
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
	rtl_assert(frame_layerexists(frame, cur));
    return (frame->layerlength[cur] == frame->layercapacity[cur]);
}

bool frame_empty( TiFrame * frame )
{
	rtl_assert(frame_layerexists(frame, frame->curlayer));
    return (frame->layerlength[frame->curlayer] == 0);
}

uintx frame_available( TiFrame * frame )
{
    uintx cur = frame->curlayer;
	rtl_assert(frame_layerexists(frame, cur));
    return (frame->layercapacity[cur] - frame->layerlength[cur]);
}

uintx frame_read( TiFrame * frame, char * buf, uintx size )
{
	uintx count = min( size, frame_length(frame) );
	rtl_assert(frame_layerexists(frame, frame->curlayer));
	memmove( buf, frame_startptr(frame), count );
	return count;
}

uintx frame_write( TiFrame * frame, char * data, uintx len )
{
	uintx count = min( frame_capacity(frame), len );
	rtl_assert(frame_layerexists(frame, frame->curlayer));
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
	rtl_assert(frame_layerexists(frame, frame->curlayer));
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

	rtl_assert(frame_layerexists(frame, cur));
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
	rtl_assert(frame_layerexists(frame, frame->curlayer));
	memmove( buf, frame_startptr(frame), count );
	return count;
}

void frame_popfront( TiFrame * frame, uintx count )
{
	rtl_assert(frame_layerexists(frame, frame->curlayer));
	
	if (count < frame_length(frame))
	{
		memmove( frame_startptr(frame), frame_startptr(frame)+count, frame_length(frame) - count );
		frame->layerlength[frame->curlayer] -= count;
	}
	else
		frame->layerlength[frame->curlayer] = 0;
}

uintx frame_getchar( TiFrame * frame, char * pc )
{
	uintx count = 0;
	count = frame_front(frame, pc, 1);
	if (count > 0)
	{
		frame_popfront(frame, count);
	}
	return count;
}

bool frame_set( TiFrame * frame, uintx idx, char c )
{
	rtl_assert(frame_layerexists(frame, frame->curlayer));
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
	rtl_assert(frame_layerexists(frame, frame->curlayer));
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
	rtl_assert(frame_layerexists(frame1, frame1->curlayer));
    rtl_assert(frame_layerexists(frame2, frame2->curlayer));
	memmove( frame_startptr(frame1), frame_startptr(frame2), count );
	frame1->layerlength[frame1->curlayer] = count;
	return count;
}

uintx frame_copyto( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame2), frame_length(frame1) );
	rtl_assert(frame_layerexists(frame1, frame1->curlayer));
    rtl_assert(frame_layerexists(frame2, frame2->curlayer));
	memmove( frame_startptr(frame2), frame_startptr(frame1), count );
	frame2->layerlength[frame2->curlayer] = count;
	return count;
}

/**
 * Move data from one frame object to another. After move, the data in source frame 
 * will be cleared.
 */
uintx frame_movefrom( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame1), frame_length(frame2) );
	rtl_assert(frame_layerexists(frame1, frame1->curlayer));
    rtl_assert(frame_layerexists(frame2, frame2->curlayer));
	memmove( frame_startptr(frame1), frame_startptr(frame2), count );
	frame1->layerlength[frame1->curlayer] = count;
	frame_popfront(frame2, count);
	return count;

}

uintx frame_moveto( TiFrame * frame1, TiFrame * frame2 )
{
	uintx count = min( frame_capacity(frame2), frame_length(frame1) );
	rtl_assert(frame_layerexists(frame1, frame1->curlayer));
    rtl_assert(frame_layerexists(frame2, frame2->curlayer));
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
	rtl_assert(frame_layerexists(frame1, frame1->curlayer));
	rtl_assert(frame_layerexists(frame2, frame2->curlayer));

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
	rtl_assert(frame_layerexists(frame, frame->curlayer));
	frame->layerlength[frame->curlayer] = count;
}

void frame_adjustlength( TiFrame * frame, int delta )
{
	rtl_assert(frame_layerexists(frame, frame->curlayer));
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
