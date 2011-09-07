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

#include "../../common/openwsn/hal/hal_configall.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_target.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_cpu.h"

/*******************************************************************************
 * @modified by zhangwei on 2010.05.15
 *  - ported to winavr 2009. now the project can be built successfully with portable
 *    winavr 2009 and avrstudio 4.15.
 *  - rebuild the target binary files
 *  - revision
 ******************************************************************************/

int main(void)
{
	char * msg = "hello! \r\n";
    uint8 i;

    /* initialize the target microcontroller(mcu) and the target board */
	target_init();

    /* open USRAT 0 and set baudrate to 38400. other parameters uses default values.
     * by default, this project uses the GAINZ hardware platform. for other platforms
     * you may need to modify the initial settings */

	dbo_open(0, 38400);

    /* output the message character by character */
    for (i=0; i<9; i++)
        dbo_putchar( msg[i] );

    /* output the message as a whole */
	dbo_write( msg, 9 );

    while (1) {};

    return 0;
} 
