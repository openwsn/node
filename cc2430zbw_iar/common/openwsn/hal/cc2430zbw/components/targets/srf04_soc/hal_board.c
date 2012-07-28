
/***********************************************************************************
  Filename:     hal_board.c
    
  Description:  Initalize board peripherals
    
***********************************************************************************/

#include "../../hal_comp_foundation.h"
#include "hal_board.h"
#include "../interface/hal_lcd_ti.h"
#include "../interface/hal_mcu_ti.h"
#include "../interface/hal_button.h"
#include "../interface/hal_joystick.h"
#include "../interface/hal_int.h"


void halBoardInit(void)
{
    halMcuInit();

    // LED
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_1_PORT, HAL_BOARD_IO_LED_1_PIN, 0);
    MCU_IO_OUTPUT(HAL_BOARD_IO_LED_3_PORT, HAL_BOARD_IO_LED_3_PIN, 0);

    HAL_LED_CLR_1();
    HAL_LED_CLR_3();

    // Button
    MCU_IO_INPUT(0, 1, MCU_IO_TRISTATE);

    // Joystick level input
    MCU_IO_INPUT(HAL_BOARD_IO_JOYSTICK_ADC_PORT, HAL_BOARD_IO_JOYSTICK_ADC_CH, \
        MCU_IO_TRISTATE);

    // Joystick push input
    MCU_IO_INPUT(HAL_BOARD_IO_JOYSTICK_PUSH_PORT, HAL_BOARD_IO_JOYSTICK_PUSH_PIN, \
        MCU_IO_TRISTATE);

    halLcdInit();
    halIntOn();
}

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
