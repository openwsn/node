/***********************************************************************************
    Filename:     hal_button.c

    Description:  HAL button implementation for CCSoC + SmartRF04EB

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "../../hal_comp_foundation.h"
#include "hal_board.h"
#include "../interface/hal_button.h"
#include "../../common/cc8051/hal_cc8051.h"


/******************************************************************************
* @fn  halButtonInit
*
* @brief
*      Initializes the button functionality
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void halButtonInit(void)
{
    // Button push input
    MCU_IO_INPUT(HAL_BOARD_IO_BTN_1_PORT, HAL_BOARD_IO_BTN_1_PIN, MCU_IO_TRISTATE);
}


/******************************************************************************
* @fn  halButtonPushed
*
* @brief
*      This function detects if 'S1' is being pushed. The function
*      implements software debounce. Return true only if previuosly called
*      with button not pushed. Return true only once each time the button
*      is pressed.
*
* Parameters:
*
* @param  void
*
* @return uint8
*          HAL_BUTTON_1:    Button is being pushed
*          HAL_BUTTON_NONE: Button is not being pushed
*
******************************************************************************/
uint8 halButtonPushed(void)
{
    uint8 i;
    uint8 value;
    static uint8 prevValue;

    if (value = HAL_BUTTON_1_PUSHED()){
        for(i = 0; i < 10; i++) {
            if(!HAL_BUTTON_1_PUSHED()){
                value = HAL_BUTTON_NONE;
                break;
            }
        }
    }

    if (value){
        if (!prevValue){
            value = prevValue = HAL_BUTTON_1;
            halMcuWaitMs(50);
        }
        else {
            value = HAL_BUTTON_NONE;
        }
    }
    else{
        prevValue = HAL_BUTTON_NONE;
    }

    return value;
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
