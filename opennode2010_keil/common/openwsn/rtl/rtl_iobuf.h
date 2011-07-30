#ifndef _RTL_IOBUF_H_4662_
#define _RTL_IOBUF_H_4662_
/*******************************************************************************
 * TiIoBuf
 * an array based high performance simple input/output queue.
 *
 * @state
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
 ******************************************************************************/

/* attention
 * This module reconized the following macros:
 *
 * CONFIG_DYNA_MEMORY
 * If this macro defines, then iobuf_create() and iobuf_free() will be enabled.
 */

#include "rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_DEBUG
#include <stdio.h>
#endif
#include "rtl_foundation.h"


/*******************************************************************************
 * TiIoBuf
 * an array based high performance simple input/output queue.
 *
 * there're three kinds of buffers provided by the system:
 *	- TiIoBuf
 *	- TiQueueBuffer
 *  - TiFrameBuffer
 *
 * The IoBuffer is the most simple and most widely used one among the three.
 * it's quite similar to an array with some utility function enhancement.
 ******************************************************************************/

#define IOBUF_HOPESIZE(size) (sizeof(TiIoBuf)+size)

/* @attention
 * @warning:
 * currently, the TiIoBuf is configured to use 8 bit integers. So you must be very
 * caution that not to exceed the maximum limit. 
 * suggested buffer size <= 0xFF - sizeof(TiIoBuf), which is about 240.
 */

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

