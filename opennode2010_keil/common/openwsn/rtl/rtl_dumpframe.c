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

#include "rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_iobuf.h"
#include "rtl_frame.h"
#include "rtl_debugio.h"
#include "rtl_ieee802frame154.h"
#include "rtl_dumpframe.h"
#include "../hal/opennode2010/hal_debugio.h"

static TiIEEE802Frame154Descriptor m_desc;

void ieee802frame154_dumpframe( TiFrame * frame )
{
	frame_movelowest(frame);
	ieee802frame154_dumpmembuf(frame_startptr(frame), frame_length(frame));
}

void ieee802frame154_dumpiobuf( TiIoBuf * iobuf )
{
	ieee802frame154_dumpmembuf(iobuf_ptr(iobuf), iobuf_length(iobuf));
}

void ieee802frame154_dumpmembuf( char * buf, int len )
{
    TiIEEE802Frame154Descriptor * desc;

	 if (len > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( len );

        desc = ieee802frame154_open( &m_desc );
        if (ieee802frame154_parse(desc, buf, len))
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
			dbo_write( buf, len );
		}
		else{
	        // if the frame received is parsed failed, then output the error frame
            // to the computer through debugging channel

	        dbc_putchar( 'X' );
			dbc_putchar( ':' );
			dbo_write( buf, len );
		}
		dbc_putchar( '\n' );
	}
}


