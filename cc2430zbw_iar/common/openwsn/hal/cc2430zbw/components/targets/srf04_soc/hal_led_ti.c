/***********************************************************************************
    Filename: hal_led.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "../../hal_comp_foundation.h"
#include "../interface/hal_led_ti.h"
#include "hal_board.h"

/***********************************************************************************
* @fn      halLedSet
*
* @brief   Turn LED on.
*
* @param   uint8 id - id of LED
*
* @return  none
*/
void halLedSet(uint8 id)
{
    if (id==1)
        HAL_LED_SET_1();
    if (id==3)
        HAL_LED_SET_3();
}

/***********************************************************************************
* @fn      halLedClear
*
* @brief   Turn LED off.
*
* @param   uint8 id - id of LED
*
* @return  none
*/
void halLedClear(uint8 id)
{
    if (id==1)
        HAL_LED_CLR_1();
    if (id==3)
        HAL_LED_CLR_3();
}

/***********************************************************************************
* @fn      halLedToggle
*
* @brief   Change state of LED. If on, turn it off. Else turn on.
*
* @param   uint8 id - id of LED
*
* @return  none
*/
void halLedToggle(uint8 id)
{
    if (id==1)
        HAL_LED_TGL_1();
    if (id==3)
        HAL_LED_TGL_3();
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

