/***********************************************************************************
    Filename:     hal_joystick.c

    Description:  HAL joystick control

***********************************************************************************/


/***********************************************************************************
* INCLUDES
*/
#include "hal_joystick.h"
#include "hal_board.h"

// NB! MRFI and digio interrupt vectors conflict
#ifndef MRFI
#include "hal_digio.h"
#include "hal_int.h"
#endif

/***********************************************************************************
* CONSTANTS
*/
#define JOYSTICK_UP_BM              BM(HAL_BOARD_IO_JOY_UP_PIN)
#define JOYSTICK_DOWN_BM            BM(HAL_BOARD_IO_JOY_DN_PIN)
#define JOYSTICK_LEFT_BM            BM(HAL_BOARD_IO_JOY_LT_PIN)
#define JOYSTICK_RIGHT_BM           BM(HAL_BOARD_IO_JOY_RT_PIN)


/***********************************************************************************
* LOCAL VARIABLES
*/
#ifndef MRFI
static ISR_FUNC_PTR joystick_isr_tbl[HAL_JOYSTICK_EVT_MAX];
static const digioConfig pinJoystickMove = {HAL_BOARD_IO_JOY_MOVE_PORT,
                                            HAL_BOARD_IO_JOY_MOVE_PIN,
                                            BV(HAL_BOARD_IO_JOY_MOVE_PIN),
                                            HAL_DIGIO_INPUT, 0};

static void halJoystickMoveISR(void);
#endif
/***********************************************************************************
* @fn      halJoystickInit
*
* @brief   Configure joystick move interrupt
*
* @param   none
*
* @return  none
*/
void halJoystickInit(void)
{
#ifndef MRFI
	uint8 i;
	
    for (i=0; i<HAL_JOYSTICK_EVT_MAX;i++)
    	joystick_isr_tbl[i]= 0;
#endif

    // Discrete input
    MCU_IO_INPUT(HAL_BOARD_IO_JOY_UP_PORT, HAL_BOARD_IO_JOY_UP_PIN, MCU_IO_TRISTATE);
    MCU_IO_INPUT(HAL_BOARD_IO_JOY_DN_PORT, HAL_BOARD_IO_JOY_DN_PIN, MCU_IO_TRISTATE);
    MCU_IO_INPUT(HAL_BOARD_IO_JOY_RT_PORT, HAL_BOARD_IO_JOY_RT_PIN, MCU_IO_TRISTATE);
    MCU_IO_INPUT(HAL_BOARD_IO_JOY_LT_PORT, HAL_BOARD_IO_JOY_LT_PIN, MCU_IO_TRISTATE);
    MCU_IO_INPUT(HAL_BOARD_IO_JOY_PUSH_PORT, HAL_BOARD_IO_JOY_PUSH_PIN, MCU_IO_TRISTATE);

    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_JOY_LEVEL_PORT, HAL_BOARD_IO_JOY_LEVEL_PIN);
#ifndef MRFI
    halDigioConfig(&pinJoystickMove);

    halDigioIntSetEdge(&pinJoystickMove, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinJoystickMove, &halJoystickMoveISR);
    halDigioIntEnable(&pinJoystickMove);
#endif
}


#ifndef MRFI
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
#endif

/***********************************************************************************
* @fn      halJoystickPushed
*
* @brief   Return true if the joystick is pushed
*
* @param   none
*
* @return  TRUE or FALSE
*/
uint8 halJoystickPushed(void)
{
    uint8 v;

    v= HAL_JOYSTICK_PUSHED();

    if (v)
        HAL_DEBOUNCE(!HAL_JOYSTICK_PUSHED());

    return v;
}



/***********************************************************************************
* @fn      halJoystickGetDir
*
* @brief   Return the current position/direction of the joystick
*
* @param   none
*
* @return  The current joystick direction
*/
uint8 halJoystickGetDir(void)
{
    // Read the analog value on A1 to get the joystick level
    uint16 value;

    ADC12CTL0  = SHT0_2 + ADC12ON;     // Sampling time, ADC12 on
    ADC12CTL1  = SHP;                  // Use sampling timer
    ADC12MCTL0 = 1;                    // Sample channel A1
    ADC12CTL0 |= ENC | ADC12SC;        // Enable and Start conversions
    while (!(ADC12IFG & 0x0001));      // Conversion done?
    value      = ADC12MEM0;            // Access result
    ADC12CTL0  = 0;                    // Turn ADC off

    // Use level to determine direction
    if (value > 0x0A80)
        return HAL_JOYSTICK_EVT_CENTER;
    if (value > 0x0900)
        return HAL_JOYSTICK_EVT_RIGHT;
    if (value > 0x0800)
        return HAL_JOYSTICK_EVT_LEFT;
    if (value > 0x0600)
        return HAL_JOYSTICK_EVT_DOWN;
    if (value > 0x0100)
        return HAL_JOYSTICK_EVT_UP;

    return HAL_JOYSTICK_EVT_INVALID;
}



#ifndef MRFI
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
    // Take a snapshot of the ports connected to joystick inputs
    uint8 direction = P4IN;
    uint8 pushed    = HAL_JOYSTICK_PUSHED();

    if (pushed)
    {
        if(joystick_isr_tbl[HAL_JOYSTICK_EVT_PUSHED] != 0)
            (*joystick_isr_tbl[HAL_JOYSTICK_EVT_PUSHED])();
    }
    else if (direction & JOYSTICK_UP_BM)
    {
        if(joystick_isr_tbl[HAL_JOYSTICK_EVT_UP]  != 0)
            (*joystick_isr_tbl[HAL_JOYSTICK_EVT_UP])();
    }
    else if (direction & JOYSTICK_DOWN_BM)
    {
        if(joystick_isr_tbl[HAL_JOYSTICK_EVT_DOWN]  != 0)
            (*joystick_isr_tbl[HAL_JOYSTICK_EVT_DOWN])();
    }
    else if (direction & JOYSTICK_LEFT_BM)
    {
        if(joystick_isr_tbl[HAL_JOYSTICK_EVT_LEFT]  != 0)
            (*joystick_isr_tbl[HAL_JOYSTICK_EVT_LEFT])();
    }
    else if (direction & JOYSTICK_RIGHT_BM)
    {
        if(joystick_isr_tbl[HAL_JOYSTICK_EVT_RIGHT]  != 0)
            (*joystick_isr_tbl[HAL_JOYSTICK_EVT_RIGHT])();
    }
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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
