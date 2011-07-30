
#include "../rtl/rtl_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "nio_dumpframe.h"

void ieee802frame154_dump( TiFrame * frame )
{
    static TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;

	if (frame_length(frame) > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( frame_length(frame) );

		/*
        desc = ieee802frame154_open( &m_desc );
        if (ieee802frame154_parse(desc, frame_startptr(frame), frame_length(frame)))
        {
            // if the frame received is parsed successfully, then output it to the
            // computer through debugging channel

            //ieee802frame154_sequence( desc );
		    //ieee802frame154_panto( desc );
		    //ieee802frame154_shortaddrto( desc );
		    //ieee802frame154_panfrom( desc );
		    //ieee802frame154_shortaddrfrom( desc );

            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

            dbc_n8toa( ieee802frame154_sequence(desc) );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_length(frame) );
		}
		else{
	        // if the frame received is parsed failed, then output the error frame
            // to the computer through debugging channel

	        dbc_putchar( 'X' );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_length(frame) );
		}
		*/
		
		//dbc_write( frame_startptr(frame), frame_length(frame) );
		dbc_write( frame_buffer(frame), frame_buffercapacity(frame) );
        //dbc_mem( frame_startptr(rxbuf), frame_length(rxbuf) );
		//dbc_write( frame_startptr(frame), frame_capacity(frame) );
		dbc_putchar( '\n' );
	}
}


