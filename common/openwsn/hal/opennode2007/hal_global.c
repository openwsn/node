/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 *
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 *****************************************************************************/
//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables with initialization and finalization
//
// 	This module is very important in the whole application. It does not only
// declares and initializes all the global objects, but also create the
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//
//----------------------------------------------------------------------------
// @history
// @modified by zhangwei on 2006-10-18
// 	first created
//
//----------------------------------------------------------------------------

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_global.h"
#include "hal_led.h"
#include "hal_assert.h"

#ifdef CONFIG_DEBUG
  #define GDEBUG
#endif

#ifdef CONFIG_UART0_ENABLE
static TiUartAdapter 			m_uart0;
#endif

static TiUartAdapter 			m_uart1;
static TiSpiAdapter 			m_spi0;
static TiSpiAdapter 			m_spi1;
// @modified by zhangwei on 20070601
// zhangwei eliminate violatile before m_cc2420
static TiCc2420Adapter              m_cc2420;
static TiTimerAdapter				m_timer0;
static TiTimerAdapter				m_timer1;
static TiTimerAdapter				m_timer2;
static TiWatchdogAdapter 			m_watchdog;
static TiMcp6s26Adapter     m_mcp6s26;
static TiVibSensorAdapter     m_vibration;
static TiAdConversion        m_ad;

#ifdef CONFIG_UART0_ENABLE
TiUartAdapter *               g_uart0 = NULL;
#endif

TiUartAdapter *				g_uart1 = NULL;
TiSpiAdapter *				g_spi0 = NULL;
TiSpiAdapter *				g_spi1 = NULL;
TiCc2420Adapter *				g_cc2420 = NULL;
TiTimerAdapter * 					g_timer0 = NULL;
TiTimerAdapter * 					g_timer1 = NULL;
TiTimerAdapter * 					g_timer2 = NULL;
TiWatchdogAdapter * 				g_watchdog = NULL;
TiMcp6s26Adapter *          g_mcp6s26 = NULL;
TiVibSensorAdapter *          g_vibration = NULL;
TiAdConversion *             g_ad = NULL;

// an global variable to record whether the system is successfully initialized.
uint8 						g_hal_init = FALSE;

// @attention
//	though the debugio_xxx() used the g_debugio object, it has already declared
// in the debugio module to facilitate programming. so you need NOT declare
// g_debugio here.
//
// @return
//	0:	success
//	-1 or other negtive:	failed
//
int8 hal_global_construct( void )
{
	#ifdef GDEBUG
	char * msg = "hal_global_construct() running...\r\n";
	#endif

	g_hal_init = TRUE;

    led_init();

    #ifdef CONFIG_UART0_ENABLE
	g_uart0 	= uart_construct( 0, (char*)(&m_uart0), sizeof(TiUartAdapter) );
    #endif

    #ifdef CONFIG_UART1_ENABLE
	//g_uart1     = uart_construct( 1, (char*)(&m_uart1), sizeof(TiUartAdapter) );
	#endif

    #ifdef GDEBUG
    	uart_write( g_uart, msg, strlen(msg), 0x00 );
    #endif

        g_spi0 		= spi_construct( 0, (char*)(&m_spi0), sizeof(TiSpiAdapter) );
	//g_spi1 		= spi_construct( 1, (char*)(&m_spi1), sizeof(TiSpiAdapter) );

	g_cc2420 	= cc2420_construct( (char*)(&m_cc2420), sizeof(TiCc2420Adapter), g_spi );
	//g_mcp6s26 = mcp_construct( 0, (char*)(&m_mcp6s26), sizeof(TiMcp6s26Adapter),g_spi0 );
	//g_ad      = ad_construct( 0, (char*)(&m_ad),  sizeof(TAdConversion) );
        //g_vibration 	= vib_construct(0,(char*)(&m_vibration), sizeof(TVibrationSensor), g_mcp6s26, g_ad  );


	g_timer0 	= timer_construct( (char*)(&m_timer0), sizeof(TiTimerAdapter) );
	g_timer1 	= timer_construct((char*)(&m_timer1), sizeof(TiTimerAdapter) );
	g_timer2 	= timer_construct( (char*)(&m_timer2), sizeof(TiTimerAdapter) );
	//g_watchdog 	= watchdog_construct( (char*)(&m_watchdog), sizeof(TWatchdog), CONFIG_WATCHDOG_INTERVAL );
/*
	if ( (g_uart0 == NULL) || (g_uart1 == NULL) || (g_spi == NULL) || (g_timer0 == NULL)
		|| (g_cc2420 == NULL) || (g_watchdog == NULL) )
	{
		g_hal_init = FALSE;
		assert( FALSE );
	}

	// after constructing all the global objects successfully and link them
	// successfully, you should do more work to configure them to work.
	// here is a good place for you to check and start the hardware.
	//
	if (g_hal_init)
	{
		spi_configure( g_spi0 );
		spi_configure( g_spi1 );
		timer_configure( g_timer0, NULL, NULL, 1 );
		timer_configure( g_timer1, NULL, NULL, 1 );
		timer_configure( g_timer2, NULL, NULL, 1 );
		uart_configure( g_uart0, 9600, 8, 1, 0, 0x00 );
		uart_configure( g_uart1, 9600, 8, 1, 0, 0x00 );
	}
        */

	assert( g_hal_init );
	return (g_hal_init == TRUE) ? 0 : -1;
}

int8 hal_global_destroy( void )
{
	watchdog_destroy( g_watchdog );
	timer_destroy( g_timer0 );
	timer_destroy( g_timer1 );
	timer_destroy( g_timer2 );
	cc2420_destroy( g_cc2420 );
	spi_destroy( g_spi );
	uart_destroy( g_uart0 );
	uart_destroy( g_uart1 );

	mcp_destroy( g_mcp6s26 );
	return 0;
}
