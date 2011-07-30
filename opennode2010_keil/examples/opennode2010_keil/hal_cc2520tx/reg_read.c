
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
  Filename: reg_read.c

  Description: This application reads the register values of CC2520. The values
  are output on the serial port. 

  Configure the PC HyperTerminal for baudrate 38400 bps, 8 data bits
  no parity, and 1 stop bit. 

***********************************************************************************/

/***********************************************************************************
 * INCLUDES
 */
#include <hal_lcd.h>
#include <hal_led.h>
#include <hal_board.h>
#include <hal_uart.h>
#include <hal_rf.h>
#include <hal_assert.h>
#include "util.h"
#include "hal_cc2520.h"


/***********************************************************************************
 * CONSTANTS
 */

// Define SREG or FREG for output to serial port
#define SREG            

#define KVP(v)	{ v, #v }

typedef struct {
    const uint8 iRegNum;
    const char *szRegName;
} regKvp_t;



/***********************************************************************************
 * LOCAL VARIABLES
 */

#ifdef SREG
// CC2520 SREG names
static regKvp_t regLookup[]=
{
    KVP(CC2520_CHIPID),
    KVP(CC2520_VERSION),
    KVP(CC2520_EXTCLOCK),
    KVP(CC2520_MDMCTRL0),
    KVP(CC2520_MDMCTRL1),
    KVP(CC2520_FREQEST),
    KVP(CC2520_RXCTRL),
    KVP(CC2520_FSCTRL),
    KVP(CC2520_FSCAL0),
    KVP(CC2520_FSCAL1),
    KVP(CC2520_FSCAL2),
    KVP(CC2520_FSCAL3),
    KVP(CC2520_AGCCTRL0),
    KVP(CC2520_AGCCTRL1),
    KVP(CC2520_AGCCTRL2),
    KVP(CC2520_AGCCTRL3),
    KVP(CC2520_ADCTEST0),
    KVP(CC2520_ADCTEST1),
    KVP(CC2520_ADCTEST2),
    KVP(CC2520_MDMTEST0),
    KVP(CC2520_MDMTEST1),
    KVP(CC2520_DACTEST0),
    KVP(CC2520_DACTEST1),
    KVP(CC2520_ATEST),
    KVP(CC2520_DACTEST2),
    KVP(CC2520_PTEST0),
    KVP(CC2520_PTEST1),
    KVP(CC2520_DPUBIST),
    KVP(CC2520_ACTBIST),
    KVP(CC2520_RAMBIST),
    KVP(0xFF),
};
#elif defined FREG
// CC2520 FREG names
static regKvp_t regLookup[]=
{
    KVP(CC2520_FRMFILT0),
    KVP(CC2520_FRMFILT1),
    KVP(CC2520_SRCMATCH),
    KVP(CC2520_SRCSHORTEN0),
    KVP(CC2520_SRCSHORTEN1),
    KVP(CC2520_SRCSHORTEN2),
    KVP(CC2520_SRCEXTEN0),
    KVP(CC2520_SRCEXTEN1),
    KVP(CC2520_SRCEXTEN2),
    KVP(CC2520_FRMCTRL0),
    KVP(CC2520_FRMCTRL1),
    KVP(CC2520_RXENABLE0),
    KVP(CC2520_RXENABLE1),
    KVP(CC2520_EXCFLAG0),
    KVP(CC2520_EXCFLAG1),
    KVP(CC2520_EXCFLAG2),
    KVP(CC2520_EXCMASKA0),
    KVP(CC2520_EXCMASKA1),
    KVP(CC2520_EXCMASKA2),
    KVP(CC2520_EXCMASKB0),
    KVP(CC2520_EXCMASKB1),
    KVP(CC2520_EXCMASKB2),
    KVP(CC2520_EXCBINDX0),
    KVP(CC2520_EXCBINDX1),
    KVP(CC2520_EXCBINDY0),
    KVP(CC2520_EXCBINDY1),
    KVP(CC2520_GPIOCTRL0),
    KVP(CC2520_GPIOCTRL1),
    KVP(CC2520_GPIOCTRL2),
    KVP(CC2520_GPIOCTRL3),
    KVP(CC2520_GPIOCTRL4),
    KVP(CC2520_GPIOCTRL5),
    KVP(CC2520_GPIOPOLARITY),
    KVP(CC2520_GPIOCTRL),
    KVP(CC2520_DPUCON),
    KVP(CC2520_DPUSTAT),
    KVP(CC2520_FREQCTRL),
    KVP(CC2520_FREQTUNE),
    KVP(CC2520_TXPOWER),
    KVP(CC2520_TXCTRL),
    KVP(CC2520_FSMSTAT0),
    KVP(CC2520_FSMSTAT1),
    KVP(CC2520_FIFOPCTRL),
    KVP(CC2520_FSMCTRL),
    KVP(CC2520_CCACTRL0),
    KVP(CC2520_CCACTRL1),
    KVP(CC2520_RSSI),
    KVP(CC2520_RSSISTAT),
    KVP(CC2520_TXFIFO_BUF),
    KVP(CC2520_RXFIRST),
    KVP(CC2520_RXFIFOCNT),
    KVP(CC2520_TXFIFOCNT),
    KVP(0xFF),
};
#endif


/***********************************************************************************
 * LOCAL FUNCTIONS
 */
static void appPrintMenu(void);

/******************************************************************************
 * @fn          appPrintMenu
 *
 * @brief       Prints menu on UART 
 *
 * @param       none
 *
 * @return      none
 */
static void appPrintMenu(void)
{
  printStr("------------------------------\n");
  printStr("CC2520 Application Example\n");
  printStr("Read Registers\n");
  printStr("------------------------------\n\n");
}


/*********************************************************************
 * @fn      appPrintRfRegs
 *
 * @brief   Print radio registers
 *
 * @param   none
 *
 * @return  none
 */
static void appPrintRfRegs(void)
{
    regKvp_t *p;

    p= regLookup;

    while ( p->iRegNum != 0xFF) {

        uint16 iRegVal;
        uint8 j;

        // Read radio registers
        #ifdef SREG
        iRegVal = CC2520_MEMRD8(p->iRegNum);
        #elif defined FREG
        iRegVal = CC2520_REGRD8(p->iRegNum);
        #endif

        // Print name and contents
        printKvpHex((char*)p->szRegName,iRegVal);
        
        p++;
    }
}


/***********************************************************************************
 * @fn          main
 *
 * @brief       This is the main entry of the "Read Regs" application
 *
 * @param       none
 *
 * @return      none
 */
void main (void)
{ 
  // Initialise board peripherals
  halBoardInit();
  
  // Initialise hal_rf
  if(halRfInit()==FAILED) {
    HAL_ASSERT(FALSE);
  }
  
  // Indicate that device is powered
  halLedSet(1);
  
  // Initialise UART
  halUartInit(HAL_UART_BAUDRATE_38400, 0);
  
  // Print Logo and splash screen on LCD
  utilPrintLogo("Reg Read");
  appPrintMenu();
  
  // Puts MCU in endless loop
  while(TRUE) {
    // Wait for user to press S1 to enter menu
    while (!BUTTON_1_PUSHED());
    halMcuWaitMs(350);
    
    // Print radio registers
    appPrintRfRegs();
  }
}


