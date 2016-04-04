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

/*******************************************************************************
 * @history
 * - modified by zhangwei on 2011.09.23
 *   add hal_enable_interrupts() before while loop. This is mandatory.
 *   Revised and tested ok.
 ******************************************************************************/

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_interrupt.h"
#include "apl_ehv_rtc.h"
#include "apl_ehv_sensor.h"
#include "apl_ehv_wireless.h"
#include "apl_ehv_energyharvest.h"

#define UART_ID 1

#define STATE_INIT          0
#define STATE_ACTIVE        1
#define STATE_SLEEP         2
#define STATE_POWERDOWN     3

static TiUartAdapter        m_uart;      
static char m_state 		= STATE_INIT;

int main(void)
{
    short value = 0;
	TiUartAdapter * uart = NULL;
	char * msg = "welcome to energy harvesting sensor sender...";
    
	// Initialize GPIO for led, UART for debugging output, and the transceiver for
	// wirelesss communication. 

	sendnode1(&m_uart);
/*
	target_init();

	led_open(LED_ALL);
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );

	// Currently, USART 1 is configured to be the debug input/output. 
	// It's the UART nearby the JTAG interface. When the application starts, 
	// you'll see the welcome message. This means the application can be 
	// be started successfully and at least the UART and the basic hardware
	// can work properly.
	//
	hal_debug_init(1, 9600);
	dbc_mem( msg, strlen(msg) );

	// The following UART is for other use. If the UART_ID override the default
	// debug UART, then the uart hardware will use the new settings. The debug
	// input/output can still work but the parameter changed.
	//
    uart = uart_construct((void *)(&m_uart), sizeof(m_uart));
    uart = uart_open(uart, UART_ID, 9600, 8, 1, 0);

    //ehv_init();
    //sensor_init();
    wls_open(uart);
    //rtclock_init();
	wls_startup();
    
    hal_enable_interrupts();


	sendnode1();


	// Wait for the module to be wakeup through external interrupt after charged 
	// enough energy. The CPU will be wakeup automatically when an external interrupt
	// reach it.

    while (1)
    {
        switch (m_state)
        {
        case STATE_INIT:
            m_state = STATE_ACTIVE;
			// continue;
            break;
            
        case STATE_ACTIVE:
            //value = sensor_getvalue16();

			#ifdef DEBUG_WIRELESS_ONLY
			led_toggle(LED_RED);
			hal_delayms(800);
            //wls_startup();
            wls_send(value);
            //wls_shutdown();
			// continue;
			#endif
                        
			
			#ifndef DEBUG_WIRELESS_ONLY
            // set the time for the next wakeup supported by the RTC hardware. 
            // The RTC will raise an external interrupt request to the CPU and 
            // wakeup the CPU to work. The CPU will continue their former work
            // if it is in sleep or startup(boot the CPU) if it is power down.
            //
            rtclock_restart(10);
            m_state = STATE_POWERDOWN;
            ehv_set_taskdone(1);
            mcu_powerdown();
			#endif

            break;
            
        case STATE_SLEEP:
        case STATE_POWERDOWN:
            break;
        }
    }

	wls_close();
*/

	return 0;
}
