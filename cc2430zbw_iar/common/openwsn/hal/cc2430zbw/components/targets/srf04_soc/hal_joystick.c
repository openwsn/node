/************************************************************************************
    Filename:     hal_joystick.c

    Description:  HAL joystick control header file

    Copyright 2007 Texas Instruments, Inc.
************************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "../../hal_comp_foundation.h"
#include "../interface/hal_joystick.h"
#include "../interface/hal_digio.h"
#include "hal_board.h"
#include "../../radios/cc2430/adc.h"
#include "../interface/hal_int.h"

/***********************************************************************************
* LOCAL CONSTANTS and MACROS
*/
#define xJOYSTICK_IRQ

/***********************************************************************************
* LOCAL VARIABLES
*/
static const digioConfig pinJoystickMove = {HAL_BOARD_IO_JOYSTICK_PUSH_PORT,
                                            HAL_BOARD_IO_JOYSTICK_PUSH_PIN,
                                            BV(HAL_BOARD_IO_JOYSTICK_PUSH_PIN),
                                            HAL_DIGIO_INPUT, 0};

#ifdef JOYSTICK_IRQ
static void halJoystickMoveISR(void);
static ISR_FUNC_PTR joystick_isr_tbl[HAL_JOYSTICK_EVT_MAX] = {0};
#endif

/************************************************************************************
* @fn  halJoystickInit
*
* @brief
*      Initializes the joystic functionality
*
* Parameters:
*
* @param  void
*
* @return void
*
************************************************************************************/
void halJoystickInit(void)
{
    // Joystick push input
    MCU_IO_INPUT(HAL_BOARD_IO_JOYSTICK_PUSH_PORT, HAL_BOARD_IO_JOYSTICK_PUSH_PIN, \
        MCU_IO_TRISTATE);

    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_JOYSTICK_ADC_PORT, HAL_BOARD_IO_JOYSTICK_ADC_CH);

    halDigioConfig(&pinJoystickMove);
#ifdef JOYSTICK_IRQ
    halDigioIntSetEdge(&pinJoystickMove, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinJoystickMove, &halJoystickMoveISR);
#endif
}

#ifdef JOYSTICK_IRQ
/***********************************************************************************
* @fn      halJoystickIntConnect
*
* @brief   Connect isr for joystick move interrupt. The parameter event tells for
*          which joystick direction this isr should be called.
*
* @param   event - Joystick direction
*          func - Pointer to function to connect
*
* @return  none
*/
uint8 halJoystickIntConnect(uint8 event, ISR_FUNC_PTR func)
{
    istate_t key;
    HAL_INT_LOCK(key);
    switch(event) {
    case HAL_JOYSTICK_EVT_UP:
        joystick_isr_tbl[HAL_JOYSTICK_EVT_UP] = func;
        break;
    case HAL_JOYSTICK_EVT_DOWN:
        joystick_isr_tbl[HAL_JOYSTICK_EVT_DOWN] = func;
        break;
    case HAL_JOYSTICK_EVT_LEFT:
        joystick_isr_tbl[HAL_JOYSTICK_EVT_LEFT] = func;
        break;
    case HAL_JOYSTICK_EVT_RIGHT:
        joystick_isr_tbl[HAL_JOYSTICK_EVT_RIGHT] = func;
        break;
    case HAL_JOYSTICK_EVT_PUSHED:
        joystick_isr_tbl[HAL_JOYSTICK_EVT_PUSHED] = func;
        break;
    default:
        HAL_INT_UNLOCK(key); return FAILED;
    }
    HAL_INT_UNLOCK(key);
    return SUCCESS;
}

/***********************************************************************************
* @fn      halJoystickIntEnable
*
* @brief   Enable joystick move interrupt
*
* @param   event - Joystick direction
*
* @return  none
*/
uint8 halJoystickIntEnable(uint8 event)
{
   // Ignore event parameter
    halDigioIntEnable(&pinJoystickMove);
    return SUCCESS;
}


/***********************************************************************************
* @fn      halJoystickIntDisable
*
* @brief   Disable joystick move interrupt
*
* @param   none
*
* @return  none
*/
void halJoystickIntDisable(void)
{
   halDigioIntDisable(&pinJoystickMove);
}

/***********************************************************************************
* @fn      halJoystickIntClear
*
* @brief   Clear pending joystick interrupts
*
* @param   event - Joystick direction
*
* @return  none
*/
void halJoystickIntClear(void)
{
    halDigioIntClear(&pinJoystickMove);
}
#endif



/************************************************************************************
* @fn  halJoystickPushed
*
* @brief
*      This function detects if the joystick is being pushed. The function
*      implements software debounce. Return true only if previuosly called
*      with joystick not pushed. Return true only once each time the joystick
*      is pressed.
*
* Parameters:
*
* @param  void
*
* @return uint8
*          1: Button is being pushed
*          0: Button is not being pushed
*
******************************************************************************/
uint8 halJoystickPushed(void)
{
    uint8 i;
    uint8 value;
    static uint8 prevValue;

    if (value = HAL_JOYSTICK_PUSHED()){
        for(i = 0; i < 10; i++){
            if(!HAL_JOYSTICK_EVT_PUSHED){
                value = 0;
                break;
            }
        }
    }

    if (value){
        if (!prevValue){
            value = prevValue = 1;
            halMcuWaitMs(50);
        }
        else {
            value = 0;
        }
    }
    else{
        prevValue = 0;
    }

    return value;
}

/******************************************************************************
* @fn  halJoystickGetDir
*
* @brief
*      This function utilizes the ADC to give an indication of the current
*      position of the joystick. Current support is for 90 degrees
*      positioning only.
*
*      The joystick control is encoded as an analog voltage.  Keep on reading
*      the ADC until two consecutive key decisions are the same.
*
*      Meassured values from the ADC:
*      ------------------------
*      |Direction | Value     |
*      ------------------------
*      |DOWN      | 0x00      |
*      |LEFT      | 0x35-0x37 |
*      |RIGHT     | 0x55-0x58 |
*      |UP        | 0x65-0x68 |
*      |CENTER    | 0x76-0x79 |
*      ------------------------
*
* Parameters:
*
* @param  void
*
* @return uint8
*          DOWN:    Joystick direction is down    (270 degrees)
*          LEFT:    Joystick direction is left    (180 degrees)
*	       RIGHT:   Joystick direction is right   (0 degrees)
*	       UP:      Joystick direction is up      (90 degrees)
*	       CENTER:  Joystick direction is centred (passive position)
*
******************************************************************************/
uint8 halJoystickGetDir(void)
{
    static uint16 adcValue;
    uint8  direction[2];

    do {
        direction[1] = direction[0];

        adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_9_BIT, \
            HAL_BOARD_IO_JOYSTICK_ADC_CH);

        // Only use 7 out of the 9 bits
        adcValue = (adcValue & 0x7FC0) >> 8;

        if (adcValue < 0x10) {
            direction[0] = HAL_JOYSTICK_EVT_DOWN;
        } else if (adcValue < 0x40) {
            direction[0] = HAL_JOYSTICK_EVT_LEFT;
        } else if (adcValue < 0x60) {
            direction[0] = HAL_JOYSTICK_EVT_RIGHT;
        } else if (adcValue < 0x70) {
            direction[0] = HAL_JOYSTICK_EVT_UP;
        } else {
            direction[0] = HAL_JOYSTICK_EVT_CENTER;
        }

    } while(direction[0] != direction[1]);

    return direction[0];
}

#ifdef JOYSTICK_IRQ

/***********************************************************************************
* @fn      halJoystickMoveISR
*
* @brief   Interrupt service routine for joystick move interrupt
*
* @param   none
*
* @return  none
*/
static void halJoystickMoveISR(void)
{
    uint8 direction = halJoystickGetDir();

    switch(direction)
    {
    case HAL_JOYSTICK_EVT_DOWN:
      if (joystick_isr_tbl[HAL_JOYSTICK_EVT_DOWN]){
        (*joystick_isr_tbl[HAL_JOYSTICK_EVT_DOWN])();
      }
        break;
    case HAL_JOYSTICK_EVT_LEFT:
      if (joystick_isr_tbl[HAL_JOYSTICK_EVT_LEFT]){
        (*joystick_isr_tbl[HAL_JOYSTICK_EVT_LEFT])();
      }
        break;
    case HAL_JOYSTICK_EVT_RIGHT:
      if (joystick_isr_tbl[HAL_JOYSTICK_EVT_RIGHT]){
        (*joystick_isr_tbl[HAL_JOYSTICK_EVT_RIGHT])();
      }
        break;
    case HAL_JOYSTICK_EVT_UP:
      if (joystick_isr_tbl[HAL_JOYSTICK_EVT_UP]){
        (*joystick_isr_tbl[HAL_JOYSTICK_EVT_UP])();
      }
        break;
    case HAL_JOYSTICK_EVT_CENTER:
      // Interrupt while in center means joystick is pushed
      if (joystick_isr_tbl[HAL_JOYSTICK_EVT_PUSHED]){
        (*joystick_isr_tbl[HAL_JOYSTICK_EVT_PUSHED])();
      }
        break;
    default:
      // no action
      break;
    }
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

#endif