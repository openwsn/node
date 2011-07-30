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

#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/rtl/rtl_frame.h"
#include "output_frame.h"

/* todo: you should replace dbo_xxx with sio_write. so that we can identify the frame 
 * in the byte stream in the PC side.
 */
#ifdef CONFIG_ASCII_OUTPUT
void output_openframe( TiFrame * opf )
{
    // if the opf structure contains an frame, then output it.
	if ( frame_capacity(opf) > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( frame_length( opf) );

		if ( !frame_empty( opf))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbc_n8toa( opf[2] );//seqid
			dbc_putchar( ':' );
			_dbc_write( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			_dbc_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\r' );
		dbo_putchar( '\n' );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( '.' );
    }
}
#endif

#ifndef CONFIG_ASCII_OUTPUT
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( 0x88 );
		dbo_putchar( 0x88 );
	 	dbo_putchar( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_putchar( *opf->sequence );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
            dbo_putchar( 0x00 );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( 0x88 );
        dbo_putchar( 0x88 );
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
    }
}
#endif



