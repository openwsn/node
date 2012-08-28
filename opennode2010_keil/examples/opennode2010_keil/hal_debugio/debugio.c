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

/******************************************************************************
 * degio.c
 * @author Shi Zhirong on 20120808
 *	- first created
******************************************************************************/
 
#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif 


#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include "openwsn/hal/hal_mcu.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/rtl/rtl_configall.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_foundation.h"
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"

void debugio(void);

int main(void)
{
	debugio();
}

void debugio()
{
	char * msg = "welcome to debug world";

	target_init();

	led_open(LED_ALL);
	led_on(LED_ALL);
	hal_delayms( 1000 );
	led_off( LED_ALL );
	while(1)
	{
		dbc_mem(msg, strlen(msg));
	}
}
