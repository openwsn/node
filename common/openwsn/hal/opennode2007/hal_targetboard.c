/*****************************************************************************
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
 ****************************************************************************/
#include "hal_target.h"
#include "hal_cc2420.h"
#include "hal_led.h"
#include "hal_mcp6s.h"
#include "hal_sensor_strain.h"
#include "hal_sensor_switch.h"
#include "hal_sensor_vibration.h"

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif

#ifdef CONFIG_TARGET_OPENNODE_20
static void target_init_opennode_20( void );
#endif
#ifdef CONFIG_TARGET_OPENNODE_30
static void target_init_opennode_30( void );
#endif
#ifdef CONFIG_TARGET_DEFAULT
static void target_init_default( void );
#endif

static void target_init_uart0( uint32 bps );

/* initialize the target hardware
 * this function MUST run successfully or the later code will dead.
 */
void target_init( void )
{
	//char * msg = "hal_target_init success\r\n";
	//uint8 n;

	/* TargetPortInit()*/
	/* when CONFIG_TARGET_OPENNODE_10
	 * this is already an obsolete hardware. it may be eliminated soon */

    #ifdef CONFIG_TARGET_OPENNODE_10
    PINSEL0 = PINSEL0 | 0x000c0000;                   //FIFOP is EXTIN3
    #endif

	/* when CONFIG_TARGET_OPENNODE_20 */

	#ifdef CONFIG_TARGET_OPENNODE_20
	target_init_opennode_20();
    #endif

	/* when CONFIG_TARGET_OPENNODE_30
	 * @modified by zhangwei on 20070626
	 * add support to version 3 hardware of openwsn
	 */
    #ifdef CONFIG_TARGET_OPENNODE_30
	target_init_opennode_30();
    #endif

	/* when CONFIG_TARGET_WLSMODEM_11 */

    #ifdef CONFIG_TARGET_WLSMODEM_11
    PINSEL0 = PINSEL0| 0x80000000;                   //FIFOP are EXTIN2
    PINSEL0 = PINSEL0& 0xbfffffff;
    #endif

    #if VIBRATION_SLEEP_PORT == 0
    IO0DIR |= BM(VIBRATION_SLEEP_PIN);
    #endif

    #if     VIBRATION_SLEEP_PORT == 1
    IO1DIR |= BM(VIBRATION_SLEEP_PIN);
    #endif

    #if     LED_GREEN_PORT == 0
    IO0DIR |= BM(LED_GREEN_PIN);
    #endif

    #if     LED_GREEN_PORT == 1
    IO1DIR |= BM(LED_GREEN_PIN);
    #endif

    #if     LED_RED_PORT == 0
    IO0DIR |= BM(LED_RED_PIN);
    #endif

    #if     LED_RED_PORT == 1
    IO1DIR |= BM(LED_RED_PIN);
    #endif

    #if     LED_YELLOW_PORT == 0
    IO0DIR |= BM(LED_YELLOW_PIN);
    #endif

    #if     LED_YELLOW_PORT == 1
    IO1DIR |= BM(LED_YELLOW_PIN);
    #endif

    #if     RESET_N_PORT == 0
    IO0DIR |= BM(RESET_N);
    #endif

    #if     RESET_N_PORT == 1
    IO1DIR |= BM(RESET_N);
    #endif

    #if     VREG_EN_PORT == 0
    IO0DIR |= BM(VREG_EN);
    #endif

    #if     VREG_EN_PORT == 1
    IO1DIR |= BM(VREG_EN);
    #endif

    #if     CSN_PORT == 0
    IO0DIR |= BM(CSN);
    #endif

    #if     CSN_PORT == 1
    IO1DIR |= BM(CSN);
    #endif

    #if     CCA_PORT == 0
    IO0DIR &= (~BM(CCA));
    #endif

    #if     CCA_PORT == 1
    IO1DIR &= (~BM(CCA));
    #endif

    #if     SFD_PORT == 0
    IO0DIR &= (~BM(SFD));
    #endif

    #if     SFD_PORT == 1
    IO1DIR &= (~BM(SFD));
    #endif

    #if     FIFO_PORT == 0
    IO0DIR &= (~BM(FIFO));
    #endif

    #if     FIFO_PORT == 1
    IO1DIR &= (~BM(FIFO));
    #endif

	led_init();
    target_init_uart0( 9600 );

	// output a message through UART0
	/*
	n = 0;
	while (msg[n] != '\0')
	{
		U0THR = msg[n];
		while ((U0LSR & 0x20) == 0)
		{
			NULL;
		}
	}
	*/

    return;
}

#ifdef CONFIG_TARGET_OPENNODE_20
void target_init_opennode_20( void )
{
	PINSEL0 = PINSEL0 | 0x80000000;                   //FIFOP is EXTIN2
    PINSEL0 = PINSEL0 & 0xbfffffff;

    PINSEL1 = PINSEL1 | 0x10000000;
    PINSEL1 = PINSEL1 & 0xdfffffff;                   //P0.30 is AD0.3

    #if     AVCCS_SWITCH_PORT == 0
    IO0DIR |= BM(AVCCS_SWITCH_PIN);
    #endif

    #if     AVCCS_SWITCH_PORT == 1
    IO1DIR |= BM(AVCCS_SWITCH_PIN);
    #endif

    #if     MCP6S26_SPI_CS_PORT == 0
    IO0DIR |= BM(MCP6S26_SPI_CS_PIN);
    #endif

    #if     MCP6S26_SPI_CS_PORT == 1
    IO1DIR |= BM(MCP6S26_SPI_CS_PIN);
    #endif
}
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
/* initialization for OpenNode 3.0 hardware
 * mainly the Port initialization */
void target_init_opennode_30( void )
{
	uint32 mask, ctrl;
	// P0[0:1] UART
	// P0[4:7]: SPI0 SCLK, MISO, MOSI, SSEL (P0.7 is set to GIO)
	// P0[15]: RF-FIFOP (EXTINT2)
	// correct!
	mask = 0x3FFF00F0;
	ctrl = 0x80001505;
	PINSEL0 = PINSEL0 & mask | ctrl;

    //PINSEL0 = PINSEL0 | 0x80000000;
    //PINSEL0 = PINSEL0 & 0xbfffffff;

	// P0.16: RF-SFD(GIO or timer capture)
	mask = 0xFFFFFFFC;
	ctrl = 0x00000000;
	PINSEL1 = PINSEL1 & mask | ctrl;

    //PINSEL1 = PINSEL1 | 0x10000000;
    //PINSEL1 = PINSEL1 & 0xdfffffff;                   //P0.30 is AD0.3

	// P1[16,18,25]: LED
	// P1[17] RF-CCA
	// P1[18] RF-VREGEN
	// P1[21] RF-CS
	// P1[22] RF-FIFO
	// P1[23] RF-RESET
	mask = ~(1 << 3);
	ctrl = 0x80;
	PINSEL2 = PINSEL2 & mask | ctrl;
}
#endif

#ifdef CONFIG_TARGET_DEFAULT
void target_init_default( void )
{
}
#endif

/* initialze UART0 for startup.
 * the default configurations will be used. you may change it in the future
 * @modified by zhangwei on 20060628
 * huanghuan delete the following section.
 * but i think UART0 config should be kept here. that's better than to delete them.
 * @modified by zhangwei on 200706
 * @TODO: you should delete the following UART configuration because you've already
 * done the same thing in hal_target_reset()/BootTargetResetInit
 */
void target_init_uart0( uint32 bps )
{
    uint16 Fdiv;

    //PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* Select the pins for Uart 选择管脚为UART0 */
	PINSEL0 = (PINSEL0 & (~0x0f)) | 0x05;		/* Select the pins for Uart 选择管脚为UART0 */
	U0LCR = 0x83;                   			/* DLAB=1,允许设置波特率 */
    //U0LCR = 0x80;                               /* Enable to access the frequenc regecter 允许访问分频因子寄存器 */
    Fdiv = (Fpclk / 16) / bps;                  /* Set the baudrate设置波特率 */
    U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
    U0LCR = 0x03;                               /* Disable to access the frequenc regecter 禁止访问分频因子寄存器 */
                                                /* set to 8,1,n 且设置为8,1,n */
	U0IER = 0x00;                               /* Disable interrupt禁止中断 */
    U0FCR = 0x00;                               /* initial FIFO 初始化FIFO */
}

