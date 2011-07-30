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

#ifndef _RTL_FRAME_H_5768_
#define _RTL_FRAME_H_5768_

#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"


layer[i] means the layer i's index in the buffer


typedef struct{
    uintx layers[]
        package_layer
        wrap_layer

}TiFrame;

setlayer
moveinner
moveouter













/** 
 * CONFIG_FRAME_SEGMENT_COUNT 
 * defines the maximum header count. the most complicate frame may has the following
 * headers: 
 *  o PHY header
 *  o MAC header
 *  o LLC header
 *  o NET header
 *  o Transportation layer header
 *  o Middleware layer header such as toplogy control, time synchronization
 *  o App layer header
 * so the maximum header level is defined to 8. the last item is used to save the
 * tail section information.
 */

#define CONFIG_FRAME_SEGMENT_COUNT 8

/* atention: if you use Dynamic C compiler from rabbit semiconductor to compile 
 * this module, you should attention that the "size" is an keyword in dyanmic C.
 * so I had to replace size with "capacity". however, you should check your other
 * source codes to avoid possible side affect about this macro.
 */

// #ifdef CONFIG_COMPILER_DYNAMICC
// ##define size capacity
// #endif

#define FRAME_HOPESIZE(capacity) (sizeof(TiFrame)+capacity)

/** 
 * TiFrame
 * TiMioBuffer contains multiple sections. each section can be regarded as a separate 
 * buffer. After initialization, it's default to section 0. In this case, it can simply
 * replace the TiIoBuf. All the input/output (R/W) operations are on current section.
 */
typedef struct{
    uintx memsize;
    uintx start;
    uintx length;
    uintx cur_segment;
    uintx cur_size;
    uintx cur_length;
    uintx segments[CONFIG_FRAME_SEGMENT_COUNT];
}TiFrame;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx capacity );
void frame_free( TiFrame * frame );
#endif

TiFrame * frame_construct( char * buf, uintx memsize )
void frame_destroy( TiFrame * frame );

#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame )
#endif

uintx frame_capacity( TiFrame * frame );
uintx frame_length( TiFrame * frame );
uintx frame_start( TiFrame * frame );
uintx frame_end( TiFrame * frame );

char * frame_startptr( TiFrame * frame );
char * frame_endptr( TiFrame * frame );
char * frame_dataptr_to( TiFrame * frame, uintx idx );

bool frame_full( TiFrame * frame );
bool frame_empty( TiFrame * frame );

uintx frame_reserve_front( TiFrame * frame, uintx segcount, uintx skiplen );
uintx frame_reserve_back( TiFrame * frame, uintx segcount, uintx skiplen );

uintx frame_segcount( TiFrame * frame );
uintx frame_setsegment( TiFrame * frame, uintx segidx, uintx capacity );
uintx frame_getsegment( TiFrame * frame, uintx segidx );
uintx frame_prev_segment
frame_next_segment
frame_first_segment
frame_last_segment
frame_current
frame_setcurrent
frame_adjustsegment
frame_shrink

uintx frame_segfull
uintx frame_segempty
frame_seg

frame_read
frame_write
frame_pushfront




mbuf_setsection( mbuf, start, size );
mbuf_prevsection
mbuf_nextsection
mbuf_firstsection
mbuf_lastsection
mbuf_cursection
mbuf_changecurrent( m_buf, start );

mbuf_readsection
mbuf_writesection

mbuf_reserve_front( mbuf, sec_count, len );
mbuf_reserve_back( mbuf, sec_count, len );





TiMIoBuf
TiMioBuffer
TiFrame


TiFrame * mbuf_construct( char * buf, uintx memsize )
void mbuf_destroy( TiFrame * mbuf );
TiFrame * mbuf_create( uintx size );
void mbuf_free( TiFrame * mbuf );

mbuf_setsection( mbuf, start, size );
mbuf_prevsection
mbuf_nextsection
mbuf_firstsection
mbuf_lastsection
mbuf_cursection
mbuf_changecurrent( m_buf, start );

mbuf_readsection
mbuf_writesection



mbuf_dataptr
mbuf_start
mbuf_end

mbuf_secptr
mbuf_secstart
mbuf_secend
mbuf_sec



mbuf_size( TiFrame * mbuf );
mbuf_dataptr( TiFrame * mbuf );
mbuf_datalength( TiFrame * mbuf );
mbuf_datastart( TiFrame * mbuf );
mbuf_dataend( TiFrame * mbuf );

mbuf_section_count( TiFrame * mbuf );
mbuf_get_section( TiFrame * mbuf, uintx idx );
mbuf_movefirst
mbuf_movelast
mbuf_moveprev
mbuf_movenext

mbuf_findsection
mbuf_nextsection
mbuf_locatesection( TiFrame * mbuf, uintx sidx, 

/** 
 * set current section
 */
mbuf_set_current( TiFrame * mbuf, uintx idx );



/** 
 * set section length
 * there're totally CONFIG_FRAME_SEGMENT_COUNT sections in the total buffer. each section
 * can represent protocol header, protocol payload or protocol tail.
 */
mbuf_set_section( TiFrame * mbuf, uintx sidx, uintx size )
mbuf_move_section_forward( TiFrame * mbuf, uintx len );
mbuf_move_section_backward( TiFrame * mbuf, uintx len );

/**
 * set the current section
 * attention: all the I/O (or read/write operations) are based on current sections
 */
mbuf_set_current
mbuf_gotonext
mbuf_gotoprev
mbuf_gotofirst
mbuf_gotolast


/** 
 * mbuf_adjustsection
 * adjust section start position and size
 */
mbuf_adjustsection
mbuf_resetsection

mbuf_clearsection
/**
 * shrink sections. this function will eliminate the possible space between two nearby
 * sections.
 */
mbuf_shrink



mbuf_free_section( TiFrame * mbuf, uintx sidx );
mbuf_reset_section( TiFrame * mbuf, uintx sidx );
mbuf_




mbuf_header_count( TiFrame * mbuf );

mbuf_movefirst( TiFrame * mbuf );
mbuf_movelast( TiFrame * mbuf );
mbuf_moveinner( TiFrame * mbuf );
mbuf_moveouter( TiFrame * mbuf );
mbuf_moveto( TiFrame * mbuf, uintx hidx );

mbuf_cur_ptr
mbuf_cur_header_ptr
mbuf_cur_payload_ptr
mbuf_cur_capacity
mbuf_cur_header_length
mbuf_cur_payload_length

mbuf_header_ptr( TiFrame * mbuf, uintx hidx );
mbuf_payload_ptr( TiFrame * mbuf, uintx hidx );
mbuf_capacity( TiFrame * mbuf, uintx hidx );
mbuf_header_length( TiFrame * mbuf, uintx hidx );
mbuf_payload_length( TiFrame * mbuf, uintx hidx ):

mbuf_reserve( TiFrame * mbuf, uintx header_count, uintx bytespace );
char mbuf_alloc_outer_header( TiFrame * mbuf, uintx hsize );

mbuf_set_header( TiFrame * mbuf, 
mbuf_set_cur_header_payload( TiFrame * mbuf, uintx );


mbuf_write_header( TiFrame * mbuf, char * buf, uintx len );
mbuf_write_payload( TiFrame * mbuf, char * buf, uintx len );

mbuf_write_cur_header( TiFrame * mbuf, char * buf, uintx len );
mbuf_write_cur_payload( TiFrame * mbuf, char * buf, uintx len );

mbuf_set_header_length( TiFrame * mbuf, uintx hidx, uintx hdrlen );
mbuf_set_cur_header_length( TiFrame * mbuf, uintx hdrlen );





typedef struct{
    uintx memsize;
    uintx size;
    uintx length;
    uintx start;
    uintx end;
    uintx curoffset;
    uintx curheader;
    uintx header[CONFIG_FRAME_SEGMENT_COUNT];
}TiFrame;

TiFrame * mbuf_construct( char * buf, uintx memsize )
void mbuf_destroy( TiFrame * mbuf );
TiFrame * mbuf_create( uintx size );
void mbuf_free( TiFrame * mbuf );

mbuf_size( TiFrame * mbuf );
mbuf_dataptr( TiFrame * mbuf );
mbuf_datalength( TiFrame * mbuf );
mbuf_datastart( TiFrame * mbuf );
mbuf_dataend( TiFrame * mbuf );

mbuf_header_count( TiFrame * mbuf );

mbuf_movefirst( TiFrame * mbuf );
mbuf_movelast( TiFrame * mbuf );
mbuf_moveinner( TiFrame * mbuf );
mbuf_moveouter( TiFrame * mbuf );
mbuf_moveto( TiFrame * mbuf, uintx hidx );

mbuf_cur_ptr
mbuf_cur_header_ptr
mbuf_cur_payload_ptr
mbuf_cur_capacity
mbuf_cur_header_length
mbuf_cur_payload_length

mbuf_header_ptr( TiFrame * mbuf, uintx hidx );
mbuf_payload_ptr( TiFrame * mbuf, uintx hidx );
mbuf_capacity( TiFrame * mbuf, uintx hidx );
mbuf_header_length( TiFrame * mbuf, uintx hidx );
mbuf_payload_length( TiFrame * mbuf, uintx hidx ):

mbuf_reserve( TiFrame * mbuf, uintx header_count, uintx bytespace );
char mbuf_alloc_outer_header( TiFrame * mbuf, uintx hsize );

mbuf_set_header( TiFrame * mbuf, 
mbuf_set_cur_header_payload( TiFrame * mbuf, uintx );


mbuf_write_header( TiFrame * mbuf, char * buf, uintx len );
mbuf_write_payload( TiFrame * mbuf, char * buf, uintx len );

mbuf_write_cur_header( TiFrame * mbuf, char * buf, uintx len );
mbuf_write_cur_payload( TiFrame * mbuf, char * buf, uintx len );

mbuf_set_header_length( TiFrame * mbuf, uintx hidx, uintx hdrlen );
mbuf_set_cur_header_length( TiFrame * mbuf, uintx hdrlen );






mbuf_setheader( TiFrame * mbuf, uintx hidx, 

mbuf_format( header_count );

mbuf_skipheader( TiFrame * mbuf, uintx skiplen );
mbuf_skipforward
mbuf_skipbackward

mbuf_reserve( TiFrame * mbuf, uintx reserved );
mbuf_addheader
mbuf_removeheader
mbuf_reserveheader

mbuf_addheader
mbuf_addheader
mbuf_

mbuf_setskiplen
mbuf_read
mbuf_write
mbuf_header_ptr
mbuf_skipinner
mbuf_skipouter
mbuf_inner
mbuf_outer
mbuf_setheader
mbuf_addheader
mbuf_appendtail



//---------------------------------------------------------------------------
// TxFrameBuffer
// to help manipluate frame generation and analysis. provide buffer management
// when add frame header and frame tail or vice versa.
//---------------------------------------------------------------------------
/*
class TxFrameBuffer{
private:
	char * m_buf;
	uint m_capacity;
	uint m_start;
	uint m_len;
public:
	TxFrameBuffer( uint capacity, uint skiplen=0 )
	{
		m_buf = new char[capacity];
		m_capacity = capacity;
		m_start = skiplen;
		m_len = 0;
	}

	~TxFrameBuffer()
	{
		if (m_buf)
		{
			delete m_buf;
			m_buf = NULL;
		}
	}

	void setskiplen( uint skiplen ) {m_start = skiplen;};

	// read the data out from the internal buffer. the results will be place into
	// the buffer indicated by pointer "buf"
	// @param
	//	buf:  	the memory block to receive the data
	//	capacity: the maximum bytes can be put into the buf
	// @return
	//  how many bytes put into the buf succesfully
	//
	uint read( char * buf, uint capacity )
	{
		uint count = min(capacity, m_len);
		if (count > 0)
		{
			memmove( buf, m_buf+m_start, count );
			m_len -= count;
		}
		return count;
	}

	// write the data into internal buffer
	// @param
	//	data:  	where the data is put
	//	len:	data length
	// @result
	//	how many bytes successfully write to the internal buffer
	//
	uint write( char * data, uint len )
	{
		uint count = min(m_capacity - m_start, len);
		if (count > 0)
		{
			memmove( m_buf+m_start, data, count );
			m_len += count;
		}
		return count;
	}

	// in the version, the space before the body part of the frame must be larger
	// than required
	//
	uint addheader( char * data, uint len )
	{
		assert( m_start >= len );
		m_start -= len;
		memmove( m_buf + m_start, data, len );
		return len;
	}

	uint appendtail( char * data, uint len )
	{
		uint count = min(m_capacity - m_start - m_len, len);
		if (count > 0)
		{
			memmove( data+m_start+m_len, data, count );
			m_len += count;
		}
		return count;
	}

	void removeheader( uint hdrlen )
	{
		assert( m_len >= hdrlen );
		m_start += hdrlen;
	}

	void removeheader( char * buf, unit hdrlen )
	{
		assert( m_len >= hdrlen );
		memmove( buf, m_buf + m_start, hdrlen );
    }

	void removetail( uint taillen )
	{
		assert( m_len >= taillen );
		m_len -= taillen;
	}

	void removetail( char * buf, uint taillen )
	{
		assert( m_len >= taillen );
		memmove(
	}

	char * buffer() {return m_buf;};
	uint capacity() {return m_capacity;}
	char * data() {return m_buf + m_start;}
	uint datalen() {return m_len;}

	uint moveto( TxFrameBuffer * fbuf, uint skiplen=0 )
	{
		fbuf->setskiplen( skiplen );
		fbuf->write( this->data(), this->datalen() );
	};
};
*/

#ifdef __cplusplus
}
#endif

#endif /* _RTL_FRAME_H_5768_ */

