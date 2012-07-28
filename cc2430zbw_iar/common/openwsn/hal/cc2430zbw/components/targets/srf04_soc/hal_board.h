/***********************************************************************************

  Filename:     hal_board.h

  Description:  SmartRF04 board with CC2430EM.

***********************************************************************************/

#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#include "../../hal_comp_foundation.h"

#if (chip==2430)
#include "ioCC2430.h"
#elif (chip==2431)
#include "ioCC2431.h"
#elif (chip==2530 || chip==2531)
#include "ioCC2530.h"
#endif

#include "../../common/cc8051/hal_cc8051.h"
#include "../interface/hal_mcu_ti.h"

/***********************************************************************************
 * CONSTANTS
 */


// LEDs
#define HAL_BOARD_IO_LED_1_PORT             1   // Green
#define HAL_BOARD_IO_LED_1_PIN              0
#define HAL_BOARD_IO_LED_3_PORT             1   // Yellow
#define HAL_BOARD_IO_LED_3_PIN              3
// LED2 and LED4 are not connected to CC2430


// Buttons
#define HAL_BOARD_IO_BTN_1_PORT             0   // Button S1
#define HAL_BOARD_IO_BTN_1_PIN              1

// Potmeter
#define HAL_POTMETER_ADC_PORT               0
#define HAL_POTMETER_ADC_CH                 7

// Joystick
#define HAL_BOARD_IO_JOYSTICK_PUSH_PORT     0
#define HAL_BOARD_IO_JOYSTICK_PUSH_PIN      5

#define HAL_BOARD_IO_JOYSTICK_ADC_PORT      0
#define HAL_BOARD_IO_JOYSTICK_ADC_CH        6

/***********************************************************************************
 * MACROS
 */

// LEDs
#define HAL_LED_SET_1() MCU_IO_SET_LOW(HAL_BOARD_IO_LED_1_PORT, \
    HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_SET_3() MCU_IO_SET_LOW(HAL_BOARD_IO_LED_3_PORT, \
    HAL_BOARD_IO_LED_3_PIN)

#define HAL_LED_CLR_1() MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_1_PORT, \
    HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_CLR_3() MCU_IO_SET_HIGH(HAL_BOARD_IO_LED_3_PORT, \
    HAL_BOARD_IO_LED_3_PIN)

#define HAL_LED_TGL_1() MCU_IO_TGL(HAL_BOARD_IO_LED_1_PORT, \
    HAL_BOARD_IO_LED_1_PIN)
#define HAL_LED_TGL_3() MCU_IO_TGL(HAL_BOARD_IO_LED_3_PORT, \
    HAL_BOARD_IO_LED_3_PIN)


// Buttons
#define HAL_BUTTON_1_PUSHED() (!MCU_IO_GET(HAL_BOARD_IO_BTN_1_PORT, \
    HAL_BOARD_IO_BTN_1_PIN))


// Joystick
#define HAL_JOYSTICK_PUSHED() (MCU_IO_GET(HAL_BOARD_IO_JOYSTICK_PUSH_PORT, \
    HAL_BOARD_IO_JOYSTICK_PUSH_PIN))

#define HAL_JOYSTICK_LEVEL()  (MCU_IO_GET(HAL_BOARD_IO_JOYSTICK_ADC_PORT, \
    HAL_BOARD_IO_JOYSTICK_ADC_CH_PIN)

#define HAL_JOYSTICK_UP() ( halJoystickGetDir()==HAL_JOYSTICK_EVT_UP )
#define HAL_JOYSTICK_DOWN() ( halJoystickGetDir()==HAL_JOYSTICK_EVT_DOWN )
#define HAL_JOYSTICK_LEFT() ( halJoystickGetDir()==HAL_JOYSTICK_EVT_LEFT )
#define HAL_JOYSTICK_RIGHT() ( halJoystickGetDir()==HAL_JOYSTICK_EVT_RIGHT )


/***********************************************************************************
 * FUNCTION PROTOTYPES
 */
void halBoardInit(void);

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
