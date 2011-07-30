#include "rtl_configall.h"
#include <stdlib.h>
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_frame.h"


#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx capacity )
{
    char * buf;
    uintx memsize = FRAME_HOPESIZE(capacity);
    buf = (char *)malloc( memsize );
    rtl_assert( buf != NULL );
    return frame_construct( buf, capacity );
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame )
{
    frame_destroy( frame );
}
#endif


TiFrame * frame_construct( char * buf, uintx memsize )
{
    TiFrame * frame;
    memset( buf, 0x00, memsize );
    frame = (TiFrame *)(buf);
    frame->memsize = memsize;
    frame->start = 0;
    frame->length = 0;
    frame->cur_segment = 0;
    frame->cur_size = 0;
    frame->cur_length = 0;
}

void frame_destroy( TiFrame * frame )
{
    return;
}

#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	TiFrame * newframe = frame_create(frame_capacity(frame));
    if (newframe != NULL)
	    frame_copyfrom( newframe, frame );
    return newframe;
}
#endif

uintx frame_capacity( TiFrame * frame )
{
    return frame->memsize - sizeof(TiFrame);
}

uintx frame_length( TiFrame * frame )
{
    return frame->length;
}

uintx frame_start( TiFrame * frame )
{
    return frame->start;
}

uintx frame_end( TiFrame * frame )
{
    return frame->end;
}

char * frame_startptr( TiFrame * frame )
{
    return (char *)frame + sizeof(TiFrame) + frame->start;
}

char * frame_endptr( TiFrame * frame )
{
    return (char *)frame + sizeof(TiFrame) + frame->start + frame->length;
}

char * frame_dataptr_to( TiFrame * frame, uintx idx )
{
    rtl_assert( idx < frame_capacity(frame) );
    return (char *)frame + sizeof(TiFrame) + idx;
}

bool frame_full( TiFrame * frame )
{
    return (frame->length == frame_capacity(frame));
}

bool frame_empty( TiFrame * frame )
{
    return (frame->length == 0);
}

uintx frame_reserve_front( TiFrame * frame, uintx segcount, uintx skiplen )
{
    uintx i;

    rtl_assert( segcount < FRAME_SEGMENT_COUNT );
    for (i=0; i<segcount; i++)
        frame->segments[i] = 0;

    frame->start = skiplen;
    frame->segments[segcount] = skiplen;

    return skiplen;
}

uintx frame_reserve_back( TiFrame * frame, uintx segcount, uintx skiplen );
{
    uintx i;

    rtl_assert( segcount < FRAME_SEGMENT_COUNT );
    for (i=FRAME_SEGMENT_COUNT-segcount; i<FRAME_SEGMENT_COUNT; i++)
        frame->segments[i] = 0;

    frame->length = frame_capacity(frame) - frame->start;
    frame->segments[FRAME_SEGMENT_COUNT-segcount-1] = frame_capacity(frame) - skiplen;

    return skiplen;
}

uintx frame_segcount( TiFrame * frame )
{
    return CONFIG_FRAME_SEGMENT_COUNT;
}

void frame_setsegment( TiFrame * frame, uintx segidx, uintx start, uintx capacity )
{
    frame->cur_segment = segidx;
    frame->segments[segidx] = start;
    if (segidx < CONFIG_FRAME_SEGMENT_COUNT-1)
    {
        delta = start + capacity - frame->segments[segidx-1];
        for (i=segidx+1; i<CONFIG_FRAME_SEGMENT_COUNT; i++)
            frame->segments[i] += delta;
    }
}

uintx frame_segment( TiFrame * frame, uintx segidx )
{
    return frame->segments[segidx];
}

uintx frame_segment_capacity

uintx frame_prev_segment
frame_next_segment
frame_first_segment
frame_last_segment
frame_current
frame_setcurrent
frame_adjustsegment( TiFrame * frame, uintx segidx, uintx capacity )
{
}

frame_shrink


/**
 * return the pointer to the first byte in the current segment. 
 */
char * frame_segment_ptr( TiFrame * frame )
{
	rtl_assert( frame != NULL );

	if (frame->length > 0)
		return (char*)frame + sizeof(TiFrame);
	else
		return NULL;
}

char * frame_data( TiFrame * frame )
{
	rtl_assert( frame != NULL );

	if (frame->length > 0)
		return (char*)frame + sizeof(TiFrame);
	else
		return NULL;
}

 uintx frame_length( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	return frame->length;
}

 uintx frame_size( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	return frame->size;
}

 char * frame_ptr( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	return (char*)frame + sizeof(TiFrame);
}

 char * frame_endptr( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	return frame_ptr(frame) + frame_length(frame);
}

 void frame_clear( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	frame->length = 0;
}

 bool frame_full( TiFrame * frame )
{
	rtl_assert( frame != NULL );
	return (frame->length == frame->size);
}

 bool frame_empty( TiFrame * frame )
{
	return (frame->length == 0);
}

 uintx frame_available( TiFrame * frame )
{
	return (frame->size - frame->length);
}

 uintx frame_read( TiFrame * frame, char * databuf, uintx size )
{
	uintx count = min( size, frame->length );
	memmove( databuf, frame_ptr(frame), count );
	return count;
}

 uintx frame_write( TiFrame * frame, char * data, uintx len )
{
	uintx count = min( frame->size, len );
	memmove( frame_ptr(frame)+frame->length, data, count );
	frame->length = count;
	return count;
}

/* frame_pushback
 * append the data at the end of the original data. If there's not enough space,
 * then only the (size-length) characters will be padded.
 */
 uintx frame_pushback( TiFrame * frame, char * data, uintx len )
{
	uintx count = min(frame->size - frame->length, len);
	memmove( frame_ptr(frame)+frame->length, data, count );
	frame->length += count;
	return count;
}

/* frame_pushbyte
 * append a single byte at the end of the original data
 */
 uintx frame_pushbyte( TiFrame * frame, unsigned char value )
{
	char * ptr = frame_ptr(frame) + frame->length;
	*ptr = value;
	frame->length ++;
	return 1;
}

 uintx frame_front( TiFrame * frame, char * otherbuf, uintx len )
{
	uintx count = min( len, frame->length );
	memmove( otherbuf, frame_ptr(frame), count );
	return count;
}

 void frame_popfront( TiFrame * frame, uintx count )
{
	if (count < frame->length)
	{
		memmove( frame_ptr(frame), frame_ptr(frame)+count, frame->length - count );
		frame->length -= count;
	}
	else
		frame->length = 0;
}

 bool frame_set( TiFrame * frame, uintx idx, char c )
{
	rtl_assert( idx < frame_size(frame) );
	if (idx < frame_size(frame))
	{
		frame_ptr(frame)[idx] = c;
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
		*c = frame_ptr(frame)[idx];
		return true;
	}
	else
		return false;
}

uintx frame_copyfrom( TiFrame * iobuf1, TiFrame * iobuf2 )
{
	uintx count = min( frame_size(iobuf1), frame_length(iobuf2) );
	memmove( frame_ptr(iobuf1), frame_data(iobuf2), count );
	iobuf1->length = count;
	return count;
}

uintx frame_copyto( TiFrame * iobuf1, TiFrame * iobuf2 )
{
	uintx count = min( frame_size(iobuf2), frame_length(iobuf1) );
	memmove( frame_ptr(iobuf2), frame_data(iobuf1), count );
	iobuf2->length = count;
	return count;
}

uintx frame_movefrom( TiFrame * iobuf1, TiFrame * iobuf2 )
{
	uintx count = min( frame_size(iobuf1), frame_length(iobuf2) );
	memmove( frame_ptr(iobuf1), frame_data(iobuf2), count );
	iobuf1->length = count;
	frame_popfront(iobuf2, count);
	return count;

}

uintx frame_moveto( TiFrame * iobuf1, TiFrame * iobuf2 )
{
	uintx count = min( frame_size(iobuf2), frame_length(iobuf1) );
	memmove( frame_ptr(iobuf2), frame_data(iobuf1), count );
	iobuf2->length = count;
	frame_popfront(iobuf1, count);
	return count;
}

// append iobuf2 to the end of iobuf1. it's quite similar to frame_pushback
uintx frame_append( TiFrame * iobuf1, TiFrame * iobuf2 )
{
	uintx avail, len, count=0;

	rtl_assert( (iobuf1 != NULL) && (iobuf2 != NULL) );
	avail = frame_available(iobuf1);
	len = frame_length(iobuf2);
	count = min(avail, len);
	if (count > 0)
	{
		memmove( frame_endptr(iobuf1), frame_ptr(iobuf2), count );
		frame_adjustlength( iobuf1, count );
	}
	return count;
}

 void	frame_setlength( TiFrame * buf, uintx count )
{
	buf->length = count;
}

void frame_adjustlength( TiFrame * buf, int delta )
{
	buf->length += delta;
}

#ifdef CONFIG_DEBUG
void frame_dump( TiFrame * buf )
{
	int i;
	char * pc;
	// printf("dump frame: memsize=%d, size=%d, length=%d\n", buf->memsize, buf->size, buf->length );
	if (buf->length > 0)
	{
		// putchar('=');
		// putchar('>');
		pc = frame_ptr(buf);
		for (i=0; i<buf->length; i++)
        {
			// putchar(pc[i]);
        }
		// putchar('\n');
	}
}
#endif






