/***********************************************************************************
  Filename:     assy_ccmsp2618_cc2520em.c

  Description:  Defines connections between the CCMSP-EM430FG2618 and the CC2520EM

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_ccmsp2618_cc2520em.h"


/***********************************************************************************
* GLOBAL VARIABLES
*/
const digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};


/***********************************************************************************
* FUNCTIONS
*/
static void halRadioSpiInit(uint32 divider);
static void halMcuRfInterfaceInit(void);


/***********************************************************************************
* @fn          halRadioSpiInit
*
* @brief       Initalise Radio SPI interface
*
* @param       none
*
* @return      none
*/
static void halRadioSpiInit(uint32 divider)
{
    UCB1CTL1 |= UCSWRST;                          // Put state machine in reset
    UCB1BR0 = LO_UINT32(divider);
    UCB1BR1 = HI_UINT32(divider);
    P5DIR |= 0x01;
    P5SEL |= 0x0E;                               // P7.3,2,1 peripheral select (mux to ACSI_A0)
    UCB1CTL1 = UCSSEL0 | UCSSEL1;                // Select SMCLK
    UCB1CTL0 |= UCCKPH | UCSYNC | UCMSB | UCMST; // 3-pin, 8-bit SPI master, rising edge capture
    UCB1CTL1 &= ~UCSWRST;                        // Initialize USCI state machine
}


/***********************************************************************************
* @fn      halMcuRfInterfaceInit
*
* @brief   Initialises SPI interface to CC2520 and configures reset and vreg
*          signals as MCU outputs.
*
* @param   none
*
* @return  none
*/
static void halMcuRfInterfaceInit(void)
{
    // Initialize the CC2520 interface
    CC2520_SPI_END();
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(0);
    CC2520_BASIC_IO_DIR_INIT();
}


/***********************************************************************************
* @fn      halAssyInit
*
* @brief   Initialize interfaces between radio and MCU
*
* @param   none
*
* @return  none
*/
void halAssyInit(void)
{
    halRadioSpiInit(0);
    halMcuRfInterfaceInit();
#ifndef MRFI_CC2520
    halDigioConfig(&pinRadio_GPIO0);
#endif
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
