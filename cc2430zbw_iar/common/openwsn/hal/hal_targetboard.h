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
#ifndef _HAL_TARGETBOARD_H_7C83_
#define _HAL_TARGETBOARD_H_7C83_


#include "hal_configall.h"
#include "hal_foundation.h"

/*
#if   defined ASSY_CCMSP2618_CC2520
#include "assy_ccmsp2618_cc2520em.h"
#elif defined ASSY_CCMSP2618_CC2500
#include "assy_ccmsp2618_cc2500em.h"
#elif defined ASSY_BB
#include "assy_ccmsp2618_cc2520em.h"
#warning "ASSY_BB not implemented; using ASSY_CCMSP2618_CC2520"
#else
//#error "No assembly chosen"
#endif
*/

/* Hardware Platform Layer
 * including CPU/MCU specific source codes
 * 
 * Reference 
 * Hardware Abstraction Architecture, http://www.tinyos.net/tinyos-2.x/doc/html/tep2.html
 */

/* Q: What's the differene between module hal_targetboard and hal_targetinit?
 * R: hal_targetboard provides a set of utility functions to operate the targetboard.
 * While, the hal_targetinit module provides only one function "target_init" which
 * encapsulate the source from target startup to osx kernel startup.
 * 
 * Q: What's the difference between target_startup() in hal_startup module and target_init()
 * function in hal_targetinit module?
 * R: target_startup() is called automatically when the system is powered on. It's called
 * by the hardware and depends on the CPU architecture. For a lot of CPU, the developing 
 * environment has already provides their own startup source code, so the target_startup()
 * is unnecessary.
 *    Function target_init() is usually called as the first function in main() function. 
 * So it's always after target_startup(). This function is often called by osx kernel when
 * the kernel is startup to perform initialization process. If you don't use the osx 
 * kernel in your application, you can simply call target_init() in main() function.
 */



#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 * CONSTANTS
 */

#define HAL_BOARD_IO_LED_1_PORT         4
#define HAL_BOARD_IO_LED_1_PIN          0
#define HAL_BOARD_IO_LED_2_PORT         4
#define HAL_BOARD_IO_LED_2_PIN          1
#define HAL_BOARD_IO_LED_3_PORT         4
#define HAL_BOARD_IO_LED_3_PIN          2
#define HAL_BOARD_IO_LED_4_PORT         4
#define HAL_BOARD_IO_LED_4_PIN          3

#define HAL_BOARD_IO_BTN_1_PORT         2
#define HAL_BOARD_IO_BTN_1_PIN          4
#define HAL_BOARD_IO_BTN_2_PORT         2
#define HAL_BOARD_IO_BTN_2_PIN          5

#define HAL_BOARD_IO_JOY_UP_PORT        4
#define HAL_BOARD_IO_JOY_UP_PIN         5
#define HAL_BOARD_IO_JOY_DN_PORT        4
#define HAL_BOARD_IO_JOY_DN_PIN         4
#define HAL_BOARD_IO_JOY_LT_PORT        4
#define HAL_BOARD_IO_JOY_LT_PIN         6
#define HAL_BOARD_IO_JOY_RT_PORT        4
#define HAL_BOARD_IO_JOY_RT_PIN         7
#define HAL_BOARD_IO_JOY_PUSH_PORT      6
#define HAL_BOARD_IO_JOY_PUSH_PIN       7
#define HAL_BOARD_IO_JOY_MOVE_PORT      2
#define HAL_BOARD_IO_JOY_MOVE_PIN       3
#define HAL_BOARD_IO_JOY_LEVEL_PORT     6
#define HAL_BOARD_IO_JOY_LEVEL_PIN      1

#define HAL_BOARD_IO_POT_LEVEL_PORT     6
#define HAL_BOARD_IO_POT_LEVEL_PIN      0

#define HAL_BOARD_IO_UART_RTS_PORT      2
#define HAL_BOARD_IO_UART_RTS_PIN       6

#define HAL_BOARD_IO_FLASH_CS_PORT      6
#define HAL_BOARD_IO_FLASH_CS_PIN       5


/******************************************************************************
 * MACROS
 */

// LED
#define HAL_LED_SET_1()                 MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_SET_2()                 MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_2_PORT, HAL_BOARD_IO_LED_2_PIN)
#define HAL_LED_SET_3()                 MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN)
#define HAL_LED_SET_4()                 MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_4_PORT, HAL_BOARD_IO_LED_4_PIN)

#define HAL_LED_CLR_1()                 MCU_IO_SET_LOW(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_CLR_2()                 MCU_IO_SET_LOW(HAL_BOARD_IO_LED_2_PORT, HAL_BOARD_IO_LED_2_PIN)
#define HAL_LED_CLR_3()                 MCU_IO_SET_LOW(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN)
#define HAL_LED_CLR_4()                 MCU_IO_SET_LOW(HAL_BOARD_IO_LED_4_PORT, HAL_BOARD_IO_LED_4_PIN)

#define HAL_LED_TGL_1()                 MCU_IO_TGL(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_TGL_2()                 MCU_IO_TGL(HAL_BOARD_IO_LED_2_PORT, HAL_BOARD_IO_LED_2_PIN)
#define HAL_LED_TGL_3()                 MCU_IO_TGL(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN)
#define HAL_LED_TGL_4()                 MCU_IO_TGL(HAL_BOARD_IO_LED_4_PORT, HAL_BOARD_IO_LED_4_PIN)

// UART RTS
#define HAL_RTS_SET()                   MCU_IO_SET_HIGH(HAL_BOARD_IO_UART_RTS_PORT, HAL_BOARD_IO_UART_RTS_PIN)
#define HAL_RTS_CLR()                   MCU_IO_SET_LOW(HAL_BOARD_IO_UART_RTS_PORT, HAL_BOARD_IO_UART_RTS_PIN)
#define HAL_RTS_DIR_OUT()               MCU_IO_OUTPUT(HAL_BOARD_IO_UART_RTS_PORT, HAL_BOARD_IO_UART_RTS_PIN, 0)

// Buttons
#define BUTTON_1_PUSHED()               (!MCU_IO_GET(HAL_BOARD_IO_BTN_1_PORT, HAL_BOARD_IO_BTN_1_PIN))
#define BUTTON_2_PUSHED()               (!MCU_IO_GET(HAL_BOARD_IO_BTN_2_PORT, HAL_BOARD_IO_BTN_2_PIN))

// Joystick
#define JOYSTICK_UP()                   (!!MCU_IO_GET(HAL_BOARD_IO_JOY_UP_PORT, HAL_BOARD_IO_JOY_UP_PIN))
#define JOYSTICK_DOWN()                 (!!MCU_IO_GET(HAL_BOARD_IO_JOY_DN_PORT, HAL_BOARD_IO_JOY_DN_PIN))
#define JOYSTICK_LEFT()                 (!!MCU_IO_GET(HAL_BOARD_IO_JOY_LT_PORT, HAL_BOARD_IO_JOY_LT_PIN))
#define JOYSTICK_RIGHT()                (!!MCU_IO_GET(HAL_BOARD_IO_JOY_RT_PORT, HAL_BOARD_IO_JOY_RT_PIN))
#define JOYSTICK_PUSHED()               (!!MCU_IO_GET(HAL_BOARD_IO_JOY_PUSH_PORT, HAL_BOARD_IO_JOY_PUSH_PIN))

// LCD Access Macros

// SPI interface control
#define LCD_SPI_BEGIN()                 st( P6OUT &= ~0x04; )
#define LCD_SPI_TX(x)                   st( IFG2 &= ~UCB0RXIFG; UCB0TXBUF= x; )
#define LCD_SPI_RX()                    UCB0RXBUF
#define LCD_SPI_WAIT_RXRDY()            st( while (!(IFG2 & UCB0RXIFG)); )
#define LCD_SPI_END()                   st( asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); P6OUT |= 0x04; )

// LCD pin control
#define LCD_DO_WRITE()                  st( P6OUT |= 0x08; )
#define LCD_DO_CONTROL()                st( P6OUT &= ~0x08; )
#define LCD_ACTIVATE_RESET()            st( P6OUT &= ~0x10; )
#define LCD_RELEASE_RESET()             st( P6OUT |= 0x10; )

// LCD port initialization
// LCD reset(P6.4), mode(P6.3) and CS(P6.2)
// LCD SPI SCLK(P3.3), SPI MISO(P3.2) and SPI MOSI(P3.1)
#define LCD_CTRL_INIT_PORTS()           st( P6DIR |= 0x08; P6DIR |= 0x10; )
#define LCD_SPI_INIT_PORTS()            st( P6DIR |= 0x04; P3SEL |= 0x0E; )
#define LCD_SPI_DISABLE()               st( P6DIR &= ~0x1C; P3SEL&= ~0x0E; )


// CC2511 Access Macros for SPI master; the SPI-bus except the chip select is shared
// with the LCD

#define CC2511_SPI_BEGIN()              st( P3OUT &= ~0x01; )
#define CC2511_SPI_TX(x)                st( IFG2 &= ~UCB0RXIFG; UCB0TXBUF= x; )
#define CC2511_SPI_RX()                 UCB0RXBUF
#define CC2511_SPI_WAIT_RXRDY()         st( while (!(IFG2 & UCB0RXIFG)); )
#define CC2511_SPI_END()                st( asm("NOP"); P3OUT |= 0x01; )

void target_init( void );
void target_reset( void );

#ifdef __cplusplus
}
#endif

#endif /* _HAL_TARGETBOARD_H_7C83_ */
