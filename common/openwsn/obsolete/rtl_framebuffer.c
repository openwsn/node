#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_framebuffer.h"

typedef struct{
    uintx memsize;
    uintx size;
    uintx length;
    uintx start;
    uintx end;
    uintx curoffset;
    uintx curheader;
    uintx header[CONFIG_FBUF_HEADER_LEVEL];
}TiFrameBuffer;

TiFrameBuffer * mbuf_construct( char * buf, uintx memsize )
{
}

void mbuf_destroy( TiFrameBuffer * mbuf )
{
}

TiFrameBuffer * mbuf_create( uintx size )
{
}

void mbuf_free( TiFrameBuffer * mbuf )
{
}





