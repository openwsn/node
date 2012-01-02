#ifndef _RTL_IOBUF_H_4662_
#define _RTL_IOBUF_H_4662_
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

/**
 * TiIoBuf
 * An array based high performance input/output buffer. It can also be used as a 
 * byte based queue.
 *
 * The IoBuffer is the most simple and most widely used one among the three.
 * it's quite similar to an array with some utility function enhancement.
 * 
 * You can use TiIoBuf to replace character array or byte queue.
 * 
 * Related modules:
 *	TiFrame in rtl_frame, TiLightQueue in rtl_lightqueue
 */
 
/* @state
 * 	released
 *
 * @modified by zhangwei on 2009.05.xx
 *	- created
 * @modified by zhangwei on 2010.02.19
 * 	- bug fix: correct length setting error in function iobuf_pushback()
 * @modified by zhangwei on 2010.02.20
 * 	- bug fix: correct behavior of iobuf_append()
 *  - add: iobuf_adjustlength()
 * @modified by zhangwei on 2010.02.21
 * 	- bug fix in iobuf_append()
 *  - add function iobuf_dump(). the dump function is enabled when CONFIG_DEBUG
 *    defined.
 * @modified by zhangwei on 2011.07.30
 *	- revision
 *	- revision. Eliminate some compiling warnings.
 *  - add iobuf_putchar() and iobuf_getchar()
 */

/**
 * attention
 * This module reconized the following macros:
 *
 * CONFIG_DYNA_MEMORY
 * If this macro is defined, then iobuf_create(), iobuf_free() and iobuf_duplicate()
 * will be enabled.
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

#define IOBUF_HOPESIZE(size) (sizeof(TiIoBuf)+size)

/**
 * @attention
 * @warning:
 * The maximum I/O buffer length is decided by "uintx" type, which can be configured
 * in the configuration file (usually configall.h).
 * 
 * Due to memory saving and CPU architecture restrictions, "uintx" can be defined 
 * as uint8. So you must be very caution that not to exceed the maximum limit. 
 * suggested buffer size <= 0xFF - sizeof(TiIoBuf), which is about 240.
 */

#define iobuf_putchar(iobuf,c) iobuf_pushbyte(iobuf,((char)c))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uintx memsize;
	uintx size;
	uintx length;
}TiIoBuf;

#ifdef CONFIG_DYNA_MEMORY
TiIoBuf * 	    iobuf_create( uintx size );
void 			iobuf_free( TiIoBuf * iobuf );
#endif

#define iobuf_open(iobuf,memsize) iobuf_construct((iobuf),(memsize))
#define iobuf_close(iobuf) iobuf_destroy((iobuf))

TiIoBuf * 	    iobuf_construct( void * iobuf, uintx memsize );
void 			iobuf_destroy( TiIoBuf * iobuf );

#ifdef CONFIG_DYNA_MEMORY
TiIoBuf * 	    iobuf_duplicate( TiIoBuf * iobuf );
#endif

char * 			iobuf_data( TiIoBuf * iobuf );
uintx 			iobuf_length( TiIoBuf * iobuf );
uintx			iobuf_size( TiIoBuf * iobuf );
char * 			iobuf_ptr( TiIoBuf * iobuf );
char * 			iobuf_endptr( TiIoBuf * iobuf );
void 			iobuf_clear( TiIoBuf * iobuf );
bool 			iobuf_full( TiIoBuf * iobuf );
bool 			iobuf_empty( TiIoBuf * iobuf );
uintx           iobuf_available( TiIoBuf * iobuf );

uintx 			iobuf_read( TiIoBuf * iobuf, char * otherbuf, uintx len );
uintx 			iobuf_write( TiIoBuf * iobuf, char * data, uintx len );
uintx 			iobuf_pushback( TiIoBuf * iobuf, char * data, uintx len );
uintx 			iobuf_pushbyte( TiIoBuf * iobuf, unsigned char value );
uintx           iobuf_front( TiIoBuf * iobuf, char * otherbuf, uintx len );
void            iobuf_popfront( TiIoBuf * iobuf, uintx count );
uintx           iobuf_getchar( TiIoBuf * iobuf, char * pc );
bool            iobuf_set( TiIoBuf * iobuf, uintx idx, char c );
bool            iobuf_get( TiIoBuf * iobuf, uintx idx, char * c );
uintx 			iobuf_copyfrom( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );
uintx 			iobuf_copyto( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );
uintx 			iobuf_movefrom( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );
uintx 			iobuf_moveto( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );
uintx 			iobuf_append( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );

void 			iobuf_setlength( TiIoBuf * buf, uintx count );
void	        iobuf_adjustlength( TiIoBuf * buf, int delta );

#ifdef CONFIG_DEBUG
void            iobuf_dump( TiIoBuf * buf );
#endif

#ifdef __cplusplus
}
#endif

#endif

