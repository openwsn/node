
#include "apl_foundation.h"
#include "apl_ieee802frame154_dump.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"


/* attention: You can replace dbc_xxx with sio_write. so that we can identify the frame 
 * in the byte stream in the PC side.
 */
#ifdef CONFIG_ASCII_OUTPUT
void ieee802frame154_dump( TiFrame * f )
{
    int8 len;
    TiIEEE802Frame154Descriptor meta;

    len = frame_length(f);
	if (len > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( len );
		 ieee802frame154_open( &meta );
        if (ieee802frame154_parse(&meta, frame_startptr(f), frame_length(f)))
        {
			dbc_putchar( ':' );
			dbc_write_n8toa( frame_startptr(f), len );
			dbc_putchar( 0xFC );
			dbc_putchar( '\r' );
			dbc_putchar( '\n' );
			}
		else{
            // if parsing failed, then we also output the data
            dbc_putchar( 0xFC );
			dbc_write_n8toa( frame_startptr(f), len );
			dbc_putchar( '\r' );
			dbc_putchar( '\n' );
			}
		}
		 else{
        // If the f doesn't contain any data, then still output some flag for indication.
        dbc_putchar( 0xFB );
        dbc_putchar( 0xFB );
    }
}
#endif

#ifndef CONFIG_ASCII_OUTPUT
void ieee802frame154_dump( TiFrame * f )
{
    int8 len;
    TiIEEE802Frame154Descriptor meta;

    len = frame_length(f);
	if (len > 0)
	{   
		
		dbc_putchar( 0xFA );
		dbc_putchar( 0xFA );
	    dbc_putchar( frame_curlayer(f) );
	 	dbc_putchar( len );

        // if the frame received is parsed successfully, then output it to the
        // computer through debugging channel
        //
        ieee802frame154_open( &meta );
        if (ieee802frame154_parse(&meta, frame_startptr(f), frame_length(f)))
        {
            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

			dbc_putchar( 0xFB );
			dbc_write( frame_startptr(f), len );
			dbc_putchar( 0xFC );
		}
		else{
            // if parsing failed, then we also output the data
            dbc_putchar( 0xFC );
			dbc_write( frame_startptr(f), len );
		}
	}
    else{
        // If the f doesn't contain any data, then still output some flag for indication.
        dbc_putchar( 0xFB );
        dbc_putchar( 0xFB );
    }
}
#endif
