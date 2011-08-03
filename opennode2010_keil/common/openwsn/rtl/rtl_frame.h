#ifndef _RTL_FRAME_H_5768_
#define _RTL_FRAME_H_5768_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2015 zhangwei(TongJi University)
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
 * @see rtl_iobuf module. Most of the interface functions of TiFrame is the same 
 * as TiIoBuf, except the layer management.
 ******************************************************************************/

#include "rtl_configall.h"
#include "rtl_foundation.h"

/** 
 * CONFIG_FRAME_LAYER_CAPACITY 
 * defines the maximum layers the frame support. currently, the most complicate frame 
 * may be the following:
 * 
 *  PHY Frame[ MAC Frame [LLC Frame [NET Frame [TCP Frame [Middleware Frame [App Frame]]]]]]
 * 
 * so this macro is defined to 8 by default. you can decrease value to save memory space.
 */

#ifndef CONFIG_FRAME_LAYER_CAPACITY
#define CONFIG_FRAME_LAYER_CAPACITY 8
#endif

/* atention: if you use Dynamic C compiler from rabbit semiconductor to compile 
 * this module, you should attention that the "size" is an keyword in dyanmic C.
 * so I had to replace size with "capacity". however, you should check your other
 * source codes to avoid possible side affect about this macro.
 */

/* #ifdef CONFIG_COMPILER_DYNAMICC
 * ##define size capacity
 * #endif
 */

#define FRAME_HOPESIZE(capacity) (sizeof(TiFrame)+(capacity))

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

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * TiFrame
 * TiMioBuffer contains multiple sections. each section can be regarded as a separate 
 * buffer. After initialization, it's default to section 0. In this case, it can simply
 * replace the TiIoBuf. All the input/output (R/W) operations are on current section.
 * 
 * frame layered structure
 * 
 *                          [App Frame .......................................]
 *                      [App Support Frame such as one2many ..................]
 *                  [Middleware such as TimeSync Frame, Localization Frame ...]
 *              [NET Frame ...................................................]
 *          [LLC Frame........................................................]
 *      [MAC Frame ...........................................................]
 *  [PHY Frame ...............................................................]
 * 
 * each higher layer item is also the payload of the lower item.
 * 
 *  member variables
 *  memsize: the memory block size the current frame object occupied
 *  curlayer: current item
 *  layercount: active items currently.
 *  bottomlayer: outermost layer.
 *  origin: the initial start position in the buffer. skipinner() and skipouter()
 *      will adjust the index according to this origin.
 *  item_start[idx]: offset of each item. item size is estimated by item_start[current+1]
 *      - item_start[current]
 *  item_length[idx]: data length of the specified layer's interior data.
 */
typedef struct{
    uintx memsize;
	uint8 option;
    uintx firstlayer;
    uintx curlayer;
    uintx layercount;
    uintx layerstart[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layerlength[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layercapacity[CONFIG_FRAME_LAYER_CAPACITY];
}TiFrame;

/**
 * create a frame
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx capacity );
#endif

/**
 * free a frame created by frame_create()
 */
#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame );
#endif

TiFrame * frame_open( char * buf, uintx memsize, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );
void frame_close( TiFrame * frame );

#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame );
#endif

/**
 * Clear the frame. After reset, the frame object is the same as it is just opened.
 * There will be only one layer existed after reset for reading and writing.
 * 
 * @param frame The pointer to the frame object to be reset
 * @param init_layerindex Indicate the index of the initial layer. 
 * @param init_layerstart Indicate where the initial layer start in the frame internal buffer.
 * @param init_layercapacity The capacity of the initial layer.
 * @return None
 */
void frame_reset( TiFrame * frame, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );

char * frame_buffer( TiFrame * frame );

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
void frame_bufferclear( TiFrame * frame );
#define frame_totalclear(frame) frame_bufferclear(frame)

uintx frame_buffercapacity( TiFrame * frame );

#define frame_totalstart(frame) frame_layerstart(frame,(frame)->firstlayer)
#define frame_totalstartptr(frame) frame_layerstartptr(frame,(frame)->firstlayer)
#define frame_totalend(frame) frame_layerend(frame,(frame)->firstlayer)
#define frame_totallength(frame) frame_layerlength(frame,(frame)->firstlayer)
#define frame_settotallength(frame,count) frame_setlayerlength(frame,(frame)->firstlayer,count)
#define frame_totalcapacity(frame) frame_layercapacity(frame,(frame)->firstlayer)
#define frame_settotalcapacity(frame,count) frame_setlayerlength(frame,(frame)->firstlayer,count)

uintx frame_totalcopyfrom( TiFrame * frame, TiFrame * from );
uintx frame_totalcopyto( TiFrame * frame, TiFrame * to );

uintx frame_layerstart( TiFrame * frame, uint8 layer );
char * frame_layerstartptr( TiFrame * frame, uint8 layer );
uintx frame_layerend( TiFrame * frame, uint8 layer );
uintx frame_layerlength( TiFrame * frame, uint8 layer );
void frame_setlayerlength( TiFrame * frame, uint8 layer, uintx count );
uintx frame_layercapacity( TiFrame * frame, uint8 layer );
bool frame_setlayercapacity( TiFrame * frame, uint8 layer, uintx count );

// todo
void frame_shrinklayer( TiFrame * frame, uint8 layer, uintx count );
void frame_layerexpand( TiFrame * frame, uint8 layer, uintx count );
// end

/**
 * return the current layer index in the byte buffer. the outer most layer is with 
 * index 0. the TiFrame currently support at most 8 layers. this can be reconfigured
 * by macro CONFIG_FRAME_LAYER_CAPACITY.
 */
uintx frame_curlayer( TiFrame * frame );

bool frame_layerexists( TiFrame * frame, uint8 layer );

/**
 * change the current item
 * if failed, then nothing changed. 
 */
bool frame_setcurlayer( TiFrame * frame, uintx cur );

/** 
 * set the inner item as the current one
 */
bool frame_moveinner( TiFrame * frame );

/** 
 * set the inner item as the current one
 */
bool frame_moveouter( TiFrame * frame );

#define frame_movefirst(f) frame_moveoutermost(f)
#define frame_movelast(f) frame_moveinnermost(f)
#define frame_movemostoutner(f) frame_moveoutermost(f)
#define frame_movemostinner(f) frame_moveinnermost(f)

bool frame_moveinnermost( TiFrame * frame );
bool frame_moveoutermost( TiFrame * frame );

#define frame_firstlayer(f) frame_outermost(f)
#define frame_lastlayer(f) frame_innermost(f)
#define frame_mostouter(f) frame_outermost(f)
#define frame_mostinner(f) frame_innermost(f)

// inside/outside?
// interior/exterior
uintx frame_innermost( TiFrame * frame );
uintx frame_outermost( TiFrame * frame );

/**
 * This function will initialize an specified layer item in the layer table inside
 * an frame object. Attention it doesn't affect the "curlayer" property of the frame
 * object, but it will blindly increase the "layercount" property. Since this function
 * judges whether an layer meta item is empty or not by "layercapacity"
 * property, it may lead to wrong result if you doesn't reset the frame object first.
 * 
 * @warning This function is better to be used inside the TiFrame object. It doesn't
 * guarantee the allocate layer are nearby together in the layer table. It's totally
 * depends on the value of "layerindex" parameter input.
 */
bool frame_initlayer( TiFrame * frame, uint8 layerindex, uintx layerstart, uintx layercapacity );

/**
 * add a layer. Different to frame_skipinner and frame_skipouter, this function 
 * doesn't affect the curlayer settings.
 * 
 * frame_skipinner = frame_addlayerinner + frame_moveinner
 * 
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the outer layer.
 */
bool frame_addlayerinterior( TiFrame * frame, uintx offset, uintx left );

/*
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the new layer.
 */
bool frame_addlayerexterior( TiFrame * frame, uintx offset, uintx left );

/** 
 * add inner item and also change the current to it
 * 
 * @param skiplen: header size of the current item
 * @param left: tail size of the current item
 */
bool frame_skipinner( TiFrame * frame, uintx skiplen, uintx left );

bool frame_skipouter( TiFrame * frame, uintx skiplen, uintx left );

/**
 * Remove the most innert layer from the frame object.
 *
 * @attention Generally this function doesn't affect the "curlayer" property of 
 *   the frame object. However, the "curlayer" will decrease by 1 if the most inner
 *   layer(last layer) is the "curlayer". Furthermore, if there's no layer after
 *   the remove, then the "firstlayer", "curlayer" and "layercount" property are 
 *   all reset to 0.
 * 
 * @return true when success.
 */
bool frame_removelayerinterior( TiFrame * frame );
bool frame_removelayerexterior( TiFrame * frame );

/**
 * @section all the following functions are operated on current layer.
 */

/**
 * Clear the data of the current layer inside the frame object.
 * @return none
 */
void frame_clear( TiFrame * frame );

/**
 * resize the current layer frame to specified new size. this function is usually
 * called after an frame object is constructed because the default frame will occupy
 * all the memmory available in the buffer.
 * 
 * @return the new size of the current layer frame is succeed. 0 if failed.
 */
uintx frame_resize( TiFrame * frame, uintx newsize );

uintx frame_start( TiFrame * frame );

uintx frame_end( TiFrame * frame );

/** 
 * Return the data length in the current layer. 
 * @return The data length in the current layer. 
 */
uintx frame_length( TiFrame * frame );

/** 
 * Return the capacity of the current layer. It's based on bytes. The capacity is 
 * fixed when this layer is created.
 * 
 * @return The capacity of the current layer. It's based on bytes.
 */
uintx frame_capacity( TiFrame * frame );

/**
 * return an memory pointer to the first byte of the current item's data. if there's
 * no data in the current item, then return NULL. it's almost the same as frame_startptr()
 * except it will return NULL when the item is empty.
 */
char * frame_dataptr( TiFrame * frame );

/** Returns an char * type pointer to the first byte of the frame buffer. */
char * frame_startptr( TiFrame * frame );

/** Returns an char * type pointer to the last byte of the frame buffer. */
char * frame_endptr( TiFrame * frame );

char * frame_header( TiFrame * frame, uintx * psize );
char * frame_interior( TiFrame * frame, uintx * psize );
char * frame_tail( TiFrame * frame, uintx * psize );

/** Returns true when the frame buffer in the current layer is full */
bool frame_full( TiFrame * frame );

/** Returns true when the frame buffer in the current layer is empty */
bool frame_empty( TiFrame * frame );

/** Return the avaiable space length in byte count in the buffer of current layer */
uintx frame_available( TiFrame * frame );

/** 
 * @brief Reads the data out from the frame buffer and place them into the buf.
 * @param buf Where the data will be placed.
 * @param size The maximum length of the buf.
 * @return The data length inside buf.
 */
uintx frame_read( TiFrame * frame, char * buf, uintx size );

/** 
 * @brief Reads the data out from the frame buffer and place them into the buf.
 * @param buf Where the data will be placed.
 * @param size The maximum length of the buf.
 * @return The data length inside buf.
 */
uintx frame_write( TiFrame * frame, char * data, uintx len );

/* frame_pushback
 * append the data at the end of the original data. If there's not enough space,
 * then only the (size-length) characters will be padded.
 */
uintx frame_pushback( TiFrame * frame, char * data, uintx len );

 /* frame_pushbyte
 * append a single byte at the end of the original data
 */
uintx frame_pushbyte( TiFrame * frame, unsigned char value );

uintx frame_front( TiFrame * frame, char * buf, uintx len );
void frame_popfront( TiFrame * frame, uintx count );

bool frame_set( TiFrame * frame, uintx idx, char c );
bool frame_get( TiFrame * frame, uintx idx, char * c );

uintx frame_copyfrom( TiFrame * frame1, TiFrame * frame2 );
uintx frame_copyto( TiFrame * frame1, TiFrame * frame2 );
uintx frame_movefrom( TiFrame * frame1, TiFrame * frame2 );

uintx frame_moveto( TiFrame * frame1, TiFrame * frame2 );
uintx frame_append( TiFrame * frame1, TiFrame * frame2 );

/**
 * Set the length property of current layer. Attention if you put the data into 
 * the layer's buffer directly by memory pointer, then the frame object cannot 
 * know how many bytes you have put. You need to tell the frame object the data 
 * length by calling this function.:
 * 
 * Example: 
 * 		char * buf = frame_startptr(f);
 * 		uintx len = min( frame_available(f), count );
 * 		memmove( buf, data, len );
 *      frame_setlength(len);
 *
 * @param frame Pointer to the TiFrame object. 
 * @param count the new length of current layer.
 */
void frame_setlength( TiFrame * frame, uintx count );

/**
 * Adjust the length property of current layer. Different to frame_setlength(), 
 * this function will adjust the length property of current layer by adding an offset.
 */
void frame_adjustlength( TiFrame * frame, int delta );

#define frame_setcapacity(f,capacity) frame_setlayercapacity((f),(f)->curlayer,(capacity))

/* Dump the internal state of the TiFrame object. This function is used to help debugging.
 * Requires <link rtl_debugio.h>
 */
#ifdef CONFIG_DEBUG
void frame_dump( TiFrame * frame );
#endif


#ifdef __cplusplus
}
#endif

#endif /* _RTL_FRAME_H_5768_ */

