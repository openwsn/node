
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

/***********************************************************************************
  Filename:     hal_mcu.c
    
  Description:  hal mcu library 
    
***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_board.h"
#include "hal_mcu.h"

/***********************************************************************************
* @fn          halMcuInit
*
* @brief       Turn off watchdog and set up system clock. Set system clock to 
*              8 MHz 
*
* @param       none
*             
* @return      none
*/
void halMcuInit(void)
{
/* todo
    uint16 i;
    
    // Stop watchdog
    WDTCTL = WDTPW + WDTHOLD;
    
    // Wait for xtal to stabilize
    while (IFG1 & OFIFG)
    {
        // Clear oscillator fault flag
        IFG1 &= ~OFIFG;
        for (i = 0x4800; i > 0; i--) asm("NOP");
    }
    
    // Set clock source to DCO @ 8 MHz 
    DCOCTL = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;
    BCSCTL1 |= XT2OFF;
    
    // Wait for DCO to synchronize with ACLK (at least 28*32 ACLK cycles)
    for (i = 0x1C00; i > 0; i--) asm("NOP");
*/
}


/***********************************************************************************
* @fn          halMcuWaitUs
*
* @brief       Busy wait function. Waits the specified number of microseconds. Use
*              assumptions about number of clock cycles needed for the various 
*              instructions. The duration of one cycle depends on MCLK. In this HAL
*              , it is set to 8 MHz, thus 8 cycles per usec.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 usec - number of microseconds delay
*             
* @return      none
*/
// todo	 important
//#pragma optimize=none
void halMcuWaitUs(uint16 usec) // 5 cycles for calling
{
    // The least we can wait is 3 usec:
    // ~1 usec for call, 1 for first compare and 1 for return 
    while(usec > 3)       // 2 cycles for compare
    {                     // 2 cycles for jump
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        asm("NOP");       // 1 cycles for nop
        usec -= 2;        // 1 cycles for optimized decrement
    }
}                         // 4 cycles for returning


/***********************************************************************************
* @fn          halMcuWaitMs
*
* @brief       Busy wait function. Waits the specified number of milliseconds. Use
*              assumptions about number of clock cycles needed for the various 
*              instructions.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 millisec - number of milliseconds delay
*             
* @return      none
*/
// todo
// #pragma optimize=none
void halMcuWaitMs(uint16 msec)
{
    while(msec-- > 0)
    {
        halMcuWaitUs(1000);
    }
}


/***********************************************************************************
* @fn          halMcuSetLowPowerMode
*
* @brief      Sets the MCU in a low power mode. Will turn global interrupts on at
*             the same time as entering the LPM mode. The MCU must be waken from
*             an interrupt (status register on stack must be modified).
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint8 mode - power mode
*             
* @return      none
*/
void halMcuSetLowPowerMode(uint8 mode)
{
/* todo
    switch (mode)
    {
    case HAL_MCU_LPM_0:
        __low_power_mode_0();
        break;
    case HAL_MCU_LPM_1:
        __low_power_mode_1();
        break;
    case HAL_MCU_LPM_2:
        __low_power_mode_2();
        break;
    case HAL_MCU_LPM_3:
        __low_power_mode_3();
        break;
    case HAL_MCU_LPM_4:
        __low_power_mode_4();
        break;
    }
*/
}

