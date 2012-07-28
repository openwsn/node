/***********************************************************************************
  Filename:     util_lcd.c

  Description:  Utility library for LCD control

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_defs.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_joystick.h"
#include "hal_button.h"
#include "hal_lcd_srf04.h"

#include "util_lcd.h"
#include "hal_rf.h"
#include "string.h"
#include "stdlib.h"


/***********************************************************************************
* LOCAL VARIABLES
*/

const char ppBarGraphChar[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
};

/***********************************************************************************
* GLOBAL FUNCTIONS
*/

#ifdef ASSY_EXP4618_CC2420
extern void halLcdWriteLine7Seg(const char *text);
#endif

#ifndef WIN32
/***********************************************************************************
* @fn          utilLcdMenuSelect
*
* @brief       Show a horisontally scrolled text menu on the LCD. Text lines given in
*               an array is shown in line 2 of the LCD, these lines are browsable.
*               The menu is navigated by using the joystick left and right, and press
*               S1 button to select an option. The function then returns the element number
*               in the given menu list that was chosen by the user.
*
* @param       ppMenu - pointer to list of texts to display for menu options
*              nMenuItems - number of menu options
*
* @return      uint8 - index to which of the menu items that was chosen
*/
uint8 utilMenuSelect(const menu_t* pMenu)
{
    uint8 index=0;
    uint8 updateLCD = TRUE;
    #ifdef SRF04EB
    char pLcdLine2[LCD_LINE_LENGTH] = "    ";
    #endif

    while (halButtonPushed()!=HAL_BUTTON_1) {
        // Joystick input: Reset = UP, Decr = LEFT, Incr = RIGHT
        if (halJoystickGetDir()==HAL_JOYSTICK_EVT_RIGHT) {
            index++;
            index %= pMenu->nItems;
            updateLCD = TRUE;
        }
		else if (halJoystickGetDir()==HAL_JOYSTICK_EVT_LEFT) {
            if(index == 0)
               index = pMenu->nItems-1;
            else
              index--;
            
            updateLCD = TRUE;
        }

        if(updateLCD) {
            // Display the updated value and arrows
            #ifdef SRF04EB
             // Make space for left arrow at left end of display
            strncpy(&(pLcdLine2[1]), (char*)pMenu->pMenuItems[index].szDescr, halLcdGetLineLength()-2);
            halLcdWriteLine(HAL_LCD_LINE_2, pLcdLine2);
            halLcdWriteChar(HAL_LCD_LINE_2, 0, '<');
            halLcdWriteChar(HAL_LCD_LINE_2, halLcdGetLineLength()-1, '>');
            #elif defined(ASSY_EXP4618_CC2420)
            halLcdWriteLine7Seg((char*)pMenu->pMenuItems[index].szDescr);
            #else
            halLcdWriteLine(HAL_LCD_LINE_2, (char*)pMenu->pMenuItems[index].szDescr);
            halLcdWriteChar(HAL_LCD_LINE_3, 0, '<');
            halLcdWriteChar(HAL_LCD_LINE_3, halLcdGetLineLength()-1, '>');
            #endif

            updateLCD = FALSE;
        }
        halMcuWaitMs(150);
    }
    halMcuWaitMs(150);

    return pMenu->pMenuItems[index].value;
}



/***********************************************************************************
* @fn          utilChipIdToStr
*
* @brief       Converts a chip ID to a text string.
*
* @param       uint8 chipID
*
* @return      none
*/
const char* utilChipIdToStr(uint8 chipID)
{
    const char* szId;

    switch(chipID) {
    case HAL_RF_CHIP_ID_CC2420:
        szId= "2420";
        break;
    case HAL_RF_CHIP_ID_CC2430:
        szId= "2430";
        break;
    case HAL_RF_CHIP_ID_CC2431:
        szId= "2431";
        break;
    case HAL_RF_CHIP_ID_CC2520:
        szId= "2520";
        break;
    case HAL_RF_CHIP_ID_CC2530:
        szId= "2530";
        break;
    case HAL_RF_CHIP_ID_CC2531:
        szId= "2531";
        break;
    default:
        szId= "";
    };

    return szId;
}

/***********************************************************************************
* @fn          utilPrintLogo
*
* @brief       Prints splash screen and logo
*
* @param       szAppName - String with name of application. Length of string must be
               no longer than (LCD_LINE_LENGTH - 5)
*
* @return      none
*/
void utilPrintLogo(char* szAppName)
{
    char lcdLine1[] = "CCxxxx ( )";
    char lcdLine2[14]="";                    // Support up to 30 characters LCD line length
    uint8 lcdLineLength = halLcdGetLineLength();

    strncpy(&lcdLine1[2],utilChipIdToStr(halRfGetChipId()),4);
    lcdLine1[8] = (char)halRfGetChipVer() + '0';

    if( (strlen(szAppName)+strlen(lcdLine2)) <= lcdLineLength ) {
        strcat(lcdLine2, szAppName);
    }
    #ifdef SRF04EB
    halLcdWriteLine(HAL_LCD_LINE_1, lcdLine1);
    halLcdWriteLine(HAL_LCD_LINE_2, lcdLine2);

    #elif defined(ASSY_EXP4618_CC2420)
    halLcdWriteLine7Seg(lcdLine1+3);            // Only the chip name

    #else   // SRF05EB
    //halLcdCreateSpecChar(0, symbol1);
    //halLcdCreateSpecChar(1, symbol2);
    //halLcdCreateSpecChar(2, symbol3);
    //halLcdCreateSpecChar(3, symbol4);
    halLcdWriteLine(HAL_LCD_LINE_1, lcdLine1);
    halLcdWriteLine(HAL_LCD_LINE_2, lcdLine2);
    halLcdWriteLine(HAL_LCD_LINE_3, "   TI LPW");
    //halLcdWriteSpecChar(HAL_LCD_LINE_1, 0, 0);
    //halLcdWriteSpecChar(HAL_LCD_LINE_2, 0, 1);
    //halLcdWriteSpecChar(HAL_LCD_LINE_1, 1, 2);
    //halLcdWriteSpecChar(HAL_LCD_LINE_2, 1, 3);
    #endif
}

#endif

/***********************************************************************************
* @fn          utilPrintText
*
* @brief       Prints a text string across all lines of the display. Newlines
*              cause continuation on the next line.
*
* @param       pTxt - text to display
*
* @param       n - number of characters to print
*
* @return      0
*/
uint8 utilPrintText(uint8* pTxt, uint8 n)
{
    uint8 li[3];
    uint8  i, iLine, nChars, nLines;

    // Display properties
    nLines= halLcdGetNumLines();
    nChars= nLines*halLcdGetLineLength();

    // Split string on newlines
    i= 0;
    iLine= 0;
    li[0]= 0;
    li[1]= 0xff;
    li[2]= 0xff;

    while(i<n && i<nChars && iLine<nLines) {
        if (pTxt[i]=='\n') {
            iLine++;
            li[iLine]= i+1;
            pTxt[i]= '\0';
        }
        i++;
    }

    // Display
	for (iLine=0; iLine<nLines; iLine++) {
		if (li[iLine]!=0xFF)
			halLcdWriteLine(HAL_LCD_LINE_1+iLine, (char const*)pTxt + li[iLine] );
	}

    return 0;
}

/***********************************************************************************
* @fn         utilLoadBarGraph
*
* @brief      Load bar graph symbols on LCD. This function must be called before
*             utilLcdBarGraph can be used.
*
* @param      none
*
* @return     none
*/
void utilLoadBarGraph(void)
{
	uint8 n;

    // Load the bar graph characters
    for (n = 0; n < 8; n++) {
        //halLcdCreateSpecChar(n, ppBarGraphChar[n]);
    }
}

/***********************************************************************************
* @fn         utilDisplayBarGraph
*
* @brief      Display bar graph on LCD
*
* @param      uint8 line - line number
*             uint8 col - column number
*             uint8 min - minimum value
*             uint8 value - value to display
*
* @return     int8 - sampled RSSI value
*/
void utilDisplayBarGraph(uint8 line, uint8 col, uint8 min, uint8 value)
{
    if (value <= min) {
        halLcdWriteChar(line, col, ' ');
    } else if (value >= min + 8) {
        //halLcdWriteSpecChar(line, col, 7);
    } else {
        //halLcdWriteSpecChar(line, col, value - min - 1);
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

