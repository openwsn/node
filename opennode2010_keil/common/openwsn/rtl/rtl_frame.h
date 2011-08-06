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
 * Defines the maximum layers the frame can support. currently, the most complicate frame 
 * may be the following:
 * 
 *  PHY Frame[ MAC Frame [LLC Frame [NET Frame [TCP Frame [Middleware Frame [App Frame]]]]]]
 * 
 * So this macro is defined to 8 by default currently. you can decrease value to 
 * save memory space.
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

/* Returns the memory size required given hope capacity. */
#define FRAME_HOPESIZE(capacity) (sizeof(TiFrame)+(capacity))

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * TiFrame
 * TiFrame manipulates nested frame structure widely used in communication protocols.
 * It manages different layers. All these layer shares same memory space. 
 * Furthermore, each layer can be manipulated as if they were an independent TiIoBuf
 * object.
 * 
 * Frame layered structure
 * 
 *                          [App Frame ...................................]
 *                      [App Support Frame such as one2many ..............]
 *                  [Middleware such as TimeSync Frame, Localization Frame] 
 *              [NET Frame ...............................................]
 *          [LLC Frame....................................................]
 *      [MAC Frame .......................................................][CRC]
 *  [PHY Frame ................................................................]
 * 
 * Each higher layer item is also the payload of the lower item.
 * 
 * Member variables:
 *  - memsize: the memory block size the current frame object occupied
 *  - option: for any use.
 *  - firstlayer: first layer id.
 *  - curlayer: current layer's id. The valid layer id is in range [0,..CONFIG_FRAME_LAYER_CAPACITY-1].
 *  - layercount: existed layer count. values 0 means no layer existed.
 *  - layerstart[]: each layers start index.
 *  - layerlength[]: each layers length.
 *  - layercapacity[]: each layers capacity.
 */
typedef struct{
    uintx memsize;
	uintx option;
    uintx firstlayer;
    uintx curlayer;
    uintx layercount;
    uintx layerstart[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layerlength[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layercapacity[CONFIG_FRAME_LAYER_CAPACITY];
}TiFrame;

/** 
 * create an TiFrame object with required capacity.
 * @return An memory pointer to the TiFrame object created. Or return NULL when failed.
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx capacity );
#endif

/** free a frame created by frame_create() */
#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame );
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
TiFrame * frame_open( char * buf, uintx memsize, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );

/**
 * Destroy an TiFrame object. After you close an frame, you should call frame_open()
 * or frame_reset() again to make it usable.
 * 
 * @see frame_open(), frame_reset()
 */
void frame_close( TiFrame * frame );

/**
 * Duplicate an TiFrame object. this function will allocated memory used, so you 
 * should free the memory after using the frame. 
 *
 * suggest using frame_create() and frame_copyfrom() apparently.
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame );
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
 * @see TiIoBuf object, iobuf_clear(), frame_open(), frame_totalclear()
 * 
 * @param frame The pointer to the frame object to be reset
 * @param init_layerindex Indicate the index of the initial layer. 
 * @param init_layerstart Indicate where the initial layer start in the frame internal buffer.
 * @param init_layercapacity The capacity of the initial layer.
 * @return None
 */
void frame_reset( TiFrame * frame, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );

/** 
 * Returns the TiFrame's internal buffer which is used to save data 
 * @return An memory pointer to the internal buffer.
 */
char * frame_totalbuffer( TiFrame * frame );

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
void frame_totalclear( TiFrame * frame );

/**
 * Total capacity of the TiFrame object. It's biggest frame size that TiFrame can 
 * accepted. It's also the maximum size of the lowest layer frame.
 */
uintx frame_totalcapacity( TiFrame * frame );

#define frame_totalstart(frame) frame_layerstart((frame),((frame)->firstlayer))
#define frame_totalstartptr(frame) frame_layerstartptr((frame),((frame)->firstlayer))
#define frame_totalend(frame) frame_layerend((frame),((frame)->firstlayer))
#define frame_totallength(frame) frame_layerlength((frame),((frame)->firstlayer))
#define frame_settotallength(frame,len) frame_setlayerlength((frame),((frame)->firstlayer),(len))
//#define frame_totalcapacity(frame) frame_layercapacity((frame),((frame)->firstlayer))
#define frame_settotalcapacity(frame,capacity) frame_setlayerlength((frame),((frame)->firstlayer),(capacity))

/** Copy an TiFrame entirely from one to another */
uintx frame_totalcopyfrom( TiFrame * frame, TiFrame * from );

/** Copy an TiFrame entirely from one to another */
uintx frame_totalcopyto( TiFrame * frame, TiFrame * to );

uintx frame_layerstart( TiFrame * frame, uint8 layer );
char * frame_layerstartptr( TiFrame * frame, uint8 layer );
uintx frame_layerend( TiFrame * frame, uint8 layer );
uintx frame_layerlength( TiFrame * frame, uint8 layer );
uintx frame_layercapacity( TiFrame * frame, uint8 layer );

/** 
 * Set data length of the current layer.
 * This function will check the layer length value against layer capacity. The length
 * should be smaller than or equal to layer capacity. 
 * 
 * @warning
 * This function doesn't check with the above layer length or the below layer length.
 * So there maybe inconsistence between two adjacent layers.
 */
void frame_setlayerlength( TiFrame * frame, uint8 layer, uintx count );

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
bool frame_setlayercapacity( TiFrame * frame, uint8 layer, uintx count );

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
void frame_shrinklayer( TiFrame * frame, uint8 layer, uintx newcapacity, uint8 choice );

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
void frame_expandlayer( TiFrame * frame, uint8 layer, uintx newcapacity, uint8 choice );

/** Whether the specified layer exists */
bool frame_layerexists( TiFrame * frame, uint8 layer );

/**
 * Returns the index of the current layer. The lowest level (outer most) layer is with 
 * index 0. the TiFrame currently support at most 8 layers. this can be reconfigured
 * by macro CONFIG_FRAME_LAYER_CAPACITY.
 */
uintx frame_curlayer( TiFrame * frame );

/** Switch the current layer. Nothing changed if failed. */
bool frame_setcurlayer( TiFrame * frame, uintx cur );

#define frame_firstlayer(f) frame_lowestlayer(f)
#define frame_lastlayer(f) frame_highestlayer(f)
// #define frame_outermost(f) frame_lowestlayer(f)
// #define frame_innermost(f) frame_highestlayer(f)

uintx frame_highestlayer( TiFrame * frame );
uintx frame_lowestlayer( TiFrame * frame );

// inside/outside?
// interior/exterior

#define frame_moveinner(f) frame_movehigher(f)
#define frame_moveouter(f) frame_movelower(f)
#define frame_movefirst(f) frame_movelowest(f)
#define frame_movelast(f) frame_movehighest(f)
// #define frame_moveoutermost(f) frame_movelowest(f)
// #define frame_moveinnermost(f) frame_movehighest(f)

/** Change the current layer to the higher one */
#define frame_moveinner(f) frame_movehigher(f)
bool frame_movehigher( TiFrame * frame );

/** Change the current layer to the lower one */
#define frame_moveouter(f) frame_movelower(f)
bool frame_movelower( TiFrame * frame );

bool frame_movehighest( TiFrame * frame );
bool frame_movelowest( TiFrame * frame );

/**
 * Add a higher level layer. 
 * Attention this function assumes there's already an layer exists, or else it doesn't 
 * know where to place the new layer.
 * 
 * Different to frame_skipinner and frame_skipouter, this function doesn't affect the curlayer settings.
 * 
 * frame_skipinner = frame_addlayerinner + frame_moveinner
 * 
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the outer layer.
 */
bool frame_addlayerinterior( TiFrame * frame, uintx offset, uintx left );

/**
 * Add a lower level layer. 
 * Attention this function assumes there's already an layer exists, or else it doesn't 
 * know where to place the new layer.
 * 
 * @param offset The relative position related to the outer layer's start position. 
 *   	Essentially, the value of offset is the header size of the new layer.
 */
bool frame_addlayerexterior( TiFrame * frame, uintx offset, uintx left );

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
bool frame_removelayerinterior( TiFrame * frame );

bool frame_removelayerexterior( TiFrame * frame );

/** 
 * add inner item and also change the current to it
 * 
 * @param skiplen: header size of the current item
 * @param left: tail size of the current item
 */
bool frame_skipinner( TiFrame * frame, uintx skiplen, uintx left );

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
bool frame_skipouter( TiFrame * frame, uintx skiplen, uintx left );

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
 * This function is simpler than frame_shrinklayer() and frame_expandlayer() because
 * it only affects the current layer. 
 * 
 * @return the new size of the current layer frame is succeed. 0 if failed.
 * 
 * @warning This function doesn't check the higher/lower level layer restrictions.
 * Incorrect newsize parameter values will lead to inconsistent status of the TiFrame
 * layer information. *
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

#define frame_putchar(iobuf,c) frame_pushbyte(iobuf,((char)c))

uintx frame_getchar( TiFrame * frame, char * pc );

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

