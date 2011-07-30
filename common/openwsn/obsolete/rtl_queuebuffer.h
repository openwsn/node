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

#ifndef _RTL_IOBUF_H_7686_ 
#define _RTL_IOBUF_H_7686_
/******************************************************************************
 * TiIoBuffer
 * usually used as frame buffer. 
 *
 * @author zhangwei on 20090514
 *	- revision
 *
 *****************************************************************************/


#include "rtl_configall.h"
#include "rtl_foundation.h"

/******************************************************************************
 * TiIoBuffer
 * usually used as frame buffer. 
 *
 *	size        the maximum possible character count in the buffer
 *	skiplen 	header size
 *	len         data length        
 *  index       index for read and write
 * 
 *  buf_data    returns the data pointer in the buffer, including header and body
 *  buf_header  returns the header of the frame
 *  buf_body    returns the body of the frame 
 *  buf_tail    points to the next byte space after the last character
 *
 *****************************************************************************/

#define buf_size(iobuf) (iobuf->size)
#define buf_skiplen(iobuf) (iobuf->skiplen)
#define buf_data(iobuf) ((char*)(iobuf) + sizeof(TiIoBuffer))
#define buf_header(iobuf) ((char*)(iobuf) + sizeof(TiIoBuffer))
#define buf_body(iobuf) ((char*)iobuf + sizeof(TiIoBuffer) + iobuf->skiplen)
#define buf_tail(iobuf) ((char*)iobuf + sizeof(TiIoBuffer) + iobuf->skiplen + iobuf->len)
#define buf_datalen(iobuf) (iobuf->len)
#define buf_full(iobuf) (iobuf->skiplen + iobuf->len == iobuf->size)
#define buf_empty(iobuf) (iobuf->len == 0)
#define buf_rewind(iobuf) {iobuf->index = iobuf->skiplen;}

typedef struct{
	uintx size;
	uintx skiplen;
	uintx len;
	uintx index;
}TiIoBuffer;

TiIoBuffer * buf_construct( void * mem, uintx size );
void buf_destroy( TiIoBuffer * buf );
uintx buf_read( TiIoBuffer * buf, char * mem, uintx size );
uintx iobuf_write( TiIoBuffer * buf, char * mem, uintx len );
uintx iobuf_append( TiIoBuffer * buf, char * mem, uintx len );
uintx iobuf_setskiplen( TiIoBuffer * buf, uintx value );
uintx iobuf_clear( TiIoBuffer * buf );

/* attention
 * we don't recommend to use buf_repace() functions because they don't modify 
 * the data length and index when doing replace. this may cause some inconsistent
 * problem
 */

void buf_replace( TiIoBuffer * buf, uintx start, char * newdata, uintx len );

#define buf_replace_header(iobuf,start,newdata,len) \
	assert(start+len <= iobuf->skiplen); buf_replace(iobuf,start,newdata,len)

#define buf_replace_body(iobuf,start,newdata,len) \
	assert(iobuf->skiplen+start+len <= iobuf->size); buf_replace(iobuf,iobuf->skiplen+start,newdata,len)


#endif