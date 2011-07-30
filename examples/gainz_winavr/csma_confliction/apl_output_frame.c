
#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_debugio.h"
//#include "../../common/openwsn/svc/svc_simplealoha.h"
#include "apl_output_frame.h"

TiIEEE802Frame154Descriptor m_meta;

void _output_frame( TiFrame * frame )
{
    static TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;

	if (frame_totallength(frame) > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( frame_totallength(frame) );

        desc = ieee802frame154_open( &m_desc );
        // ? if (ieee802frame154_parse(desc, frame_startptr(frame), frame_length(frame)))
        if (ieee802frame154_parse(desc, frame_startptr(frame), frame_capacity(frame)))
        {
            // if the frame received is parsed successfully, then output it to the
            // computer through debugging channel

            //ieee802frame154_set_sequence( desc, seqid ++ );
		    //ieee802frame154_set_panto( desc, CONFIG_ALOHA_DEFAULT_PANID );
		    //ieee802frame154_set_shortaddrto( desc, CONFIG_ALOHA_REMOTE_ADDRESS );
		    //ieee802frame154_set_panfrom( desc, CONFIG_ALOHA_PANID);
		    //ieee802frame154_set_shortaddrfrom( desc, CONFIG_ALOHA_LOCAL_ADDRESS );

            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

            dbc_n8toa( ieee802frame154_sequence(desc) );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_capacity(frame) );
		}
		else{
	        // if the frame received is parsed failed, then output the error frame
            // to the computer through debugging channel

	        dbc_putchar( 'X' );
			dbc_putchar( ':' );
			dbc_write( frame_startptr(frame), frame_capacity(frame) );
		}
		dbc_putchar( '\r' );
		dbc_putchar( '\n' );
	}
}


