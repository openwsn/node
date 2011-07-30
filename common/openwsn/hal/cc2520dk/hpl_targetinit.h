/******************************************************************************
    Filename:     hal_board.h

    Description:  HAL board peripherals header file for the
                  SmartRF05EB + CCMSP-EM430F2618 platform

******************************************************************************/
#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_mcu.h"

#define BOARD_CCMSP2618

#if   defined ASSY_CCMSP2618_CC2520
#include "hal_ccmsp2618_cc2520em.h"
#elif defined ASSY_CCMSP2618_CC2500
#include "hal_ccmsp2618_cc2500em.h"
#elif defined ASSY_CCMSP2618_CC2420
#include "hal_ccmsp2618_cc2420em.h"
#endif

/******************************************************************************
 * CONSTANTS
 */

// Board properties
#define BOARD_NAME                      "SRF05EB/CCMSP2618"
#define NUM_LEDS                        4
#define NUM_BUTTONS                     2
#define NUM_JSTKS                       1
#define NUM_POTS                        1


// Ports
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
#define HAL_BUTTON_1_PUSHED()           (!MCU_IO_GET(HAL_BOARD_IO_BTN_1_PORT, HAL_BOARD_IO_BTN_1_PIN))
#define HAL_BUTTON_2_PUSHED()           (!MCU_IO_GET(HAL_BOARD_IO_BTN_2_PORT, HAL_BOARD_IO_BTN_2_PIN))

// Joystick
#define HAL_JOYSTICK_UP()               (!!MCU_IO_GET(HAL_BOARD_IO_JOY_UP_PORT, HAL_BOARD_IO_JOY_UP_PIN))
#define HAL_JOYSTICK_DOWN()             (!!MCU_IO_GET(HAL_BOARD_IO_JOY_DN_PORT, HAL_BOARD_IO_JOY_DN_PIN))
#define HAL_JOYSTICK_LEFT()             (!!MCU_IO_GET(HAL_BOARD_IO_JOY_LT_PORT, HAL_BOARD_IO_JOY_LT_PIN))
#define HAL_JOYSTICK_RIGHT()            (!!MCU_IO_GET(HAL_BOARD_IO_JOY_RT_PORT, HAL_BOARD_IO_JOY_RT_PIN))
#define HAL_JOYSTICK_PUSHED()           (!!MCU_IO_GET(HAL_BOARD_IO_JOY_PUSH_PORT, HAL_BOARD_IO_JOY_PUSH_PIN))

#define HAL_DEBOUNCE(expr)              { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }

// LCD Access Macros

// SPI interface control
#define LCD_SPI_BEGIN()                 st( P6OUT &= ~0x04; )
#define LCD_SPI_TX(x)                   st( IFG2 &= ~UCB0RXIFG; UCB0TXBUF= x; )
#define LCD_SPI_RX()                    UCB0RXBUF
#define LCD_SPI_WAIT_RXRDY()            st( while (!(IFG2 & UCB0RXIFG)); )
#define LCD_SPI_END()                   st( NOP(); NOP(); NOP(); NOP(); P6OUT |= 0x04; )

// LCD pin control
#define LCD_DO_WRITE()                  st( P6OUT |= 0x08; )
#define LCD_DO_CONTROL()                st( P6OUT &= ~0x08; )

// LCD port initialization
// LCD mode(P6.3) and CS(P6.2)
// LCD SPI SCLK(P3.3), SPI MISO(P3.2) and SPI MOSI(P3.1)
#define LCD_CTRL_INIT_PORTS()           st( P6DIR |= 0x08; )
#define LCD_SPI_INIT_PORTS()            st( P6DIR |= 0x04; P3SEL |= 0x0E; )
#define LCD_SPI_DISABLE()               st( P6DIR &= ~0x0C; P3SEL&= ~0x0E; )


// CC2511 Access Macros for SPI master; the SPI-bus except the chip select is shared
// with the LCD

#define CC2511_SPI_BEGIN()              st( P3OUT &= ~0x01; NOP(); )
#define CC2511_SPI_TX(x)                st( IFG2 &= ~UCB0RXIFG; UCB0TXBUF= x; )
#define CC2511_SPI_RX()                 UCB0RXBUF
#define CC2511_SPI_WAIT_RXRDY()         st( while (!(IFG2 & UCB0RXIFG)); )
#define CC2511_SPI_END()                st( NOP(); P3OUT |= 0x01; )

#define HAL_PROCESS()


/***********************************************************************************
* GLOBAL VARIABLES
*/


/******************************************************************************
 * GLOBAL FUNCTIONS
 */

void halBoardInit(void);


#ifdef __cplusplus
}
#endif


/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
#endif
