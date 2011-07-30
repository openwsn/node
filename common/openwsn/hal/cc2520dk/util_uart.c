/***********************************************************************************

  Filename:     util_uart.c

  Description:  Utility library for UART.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include "hal_uart.h"
#include "util_uart.h"
#include "hal_defs.h"
#include "string.h"


/***********************************************************************************
* LOCAL FUNCTIONS
*/


/***********************************************************************************
* @fn      printChar
*
* @brief   Print a character on the console.
*
* @param   c - character to print
*
* @return  none
*/
static void printChar(char c)
{
    halUartWrite((uint8*)&c, 1);
}



/***********************************************************************************
* @fn      printStrW
*
* @brief   Print a string padded with blanks.
*
* @param   sz - string to display
*          len - total length of the string
*
* @return  none
*/
static void printStrW(char *sz, uint8 nBytes)
{
    printStr((char *)sz);
    if (strlen(sz)<nBytes) {
        nBytes-= strlen(sz);
        while(--nBytes>0)
            printChar(' ');
    }
}


/***********************************************************************************
* @fn      printHex
*
* @brief   Print an integer as hexadecimal.
*
* @param   v - the value to be displayed
*          s - size (8,16,32) | bit 7 to omit 'h' at the end
*
* @return  none
*/
static void printHex(uint32 v, uint8 s)
{
    char buf[16];
    uint8 w;

    w= (s&~NO_HEX_INDICATION)>>2;        // String length

    if (s&NO_HEX_INDICATION) {
        buf[w]= '\0';
    } else {
        buf[w]= 'h';       // HEX
        buf[w+1]= '\0';    // String terminator
    }

    // Convert number
    do {
        w--;
        buf[w]= "0123456789ABCDEF"[v&0xF];
        v>>= 4;
    } while(w>0);

    // Print
    printStr(buf);
}


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      printStr
*
* @brief   Print a zero-terminated string on the console
*
* @param   sz - string to send to the console
*
* @return  none
*/
void printStr(char* sz)
{
    char lineFeed = '\r';
    halUartWrite((uint8*)sz, (uint16) strlen(sz));
    // if last character is newline, add carriage return
    if(*(sz+(strlen(sz)-1)) == '\n')
        halUartWrite((uint8*)&lineFeed,1);
}


/***********************************************************************************
* @fn      printKvpHex
*
* @brief   Print a Key-Value pair hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHex(char *szKey, uint16 v)
{
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,16);
    printChar('\n');
    printChar('\r');
}


/***********************************************************************************
* @fn      printKvpHex8bit
*
* @brief   Print a Key-Value pair hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHex8bit(char *szKey, uint8 v)
{
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,8);
    printChar('\n');
    printChar('\r');
}

/***********************************************************************************
* @fn      printKvpHexIEEE
*
* @brief   Print a Key-Value  hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHexIEEE(char *szKey, uint8 *pIEEEAddress)
{
    uint8 i = 8;
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    while (i > 0)
    {
        printHex(pIEEEAddress[--i], 8 | NO_HEX_INDICATION);
    }
    printStr("h\n\r");
}



/***********************************************************************************
  Copyright 2008 Texas Instruments Incorporated. All rights reserved.

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
