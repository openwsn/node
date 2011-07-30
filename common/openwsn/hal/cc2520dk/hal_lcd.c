/***********************************************************************************
  Filename:     hal_lcd.c

  Description:  Functions for controlling the LCD on SmartRF05EB

***********************************************************************************/

/***********************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "hal_lcd.h"
#include "hal_mcu.h"
#include "hal_board.h"


/***********************************************************************************
 * CONSTANTS AND DEFINES
 */

// LCD instruction set definitions

// Set power save mode
#define OSC_OFF                         0x00
#define OSC_ON                          0x01
#define POWER_SAVE_OFF                  0x00
#define POWER_SAVE_ON                   0x02
#define SET_POWER_SAVE_MODE(options)    lcdControl(0x0C | (options))

// Function Set
#define CGROM                           0x00
#define CGRAM                           0x01
#define COM_FORWARD                     0x00
#define COM_BACKWARD                    0x02
#define TWO_LINE                        0x00
#define THREE_LINE                      0x04
#define FUNCTION_SET(options)           lcdControl(0x10 | (options))

// Set Display Start Line
#define LINE1                           0x00
#define LINE2                           0x01
#define LINE3                           0x02
#define LINE4                           0x03
#define SET_DISPLAY_START_LINE(line)    lcdControl(0x18 | (line))

// Set bias control
#define BIAS_1_5                        0x00
#define BIAS_1_4                        0x01
#define SET_BIAS_CTRL(bias)             lcdControl(0x1C | (bias))

// Power control
#define VOLTAGE_DIVIDER_OFF             0x00
#define VOLTAGE_DIVIDER_ON              0x01
#define CONVERTER_AND_REG_OFF           0x00
#define CONVERTER_AND_REG_ON            0x04
#define SET_POWER_CTRL(options)         lcdControl(0x20 | (options))

// Set display control
#define DISPLAY_CTRL_ON                 0x01
#define DISPLAY_CTRL_OFF                0x00
#define DISPLAY_CTRL_BLINK_ON           0x02
#define DISPLAY_CTRL_BLINK_OFF          0x00
#define DISPLAY_CTRL_CURSOR_ON          0x04
#define DISPLAY_CTRL_CURSOR_OFF         0x00
#define SET_DISPLAY_CTRL(options)       lcdControl(0x28 | (options))

// Set DD/CGRAM address
#define SET_DDRAM_ADDR(charIndex)       lcdControl(0x80 | (charIndex))
#define SET_GCRAM_CHAR(specIndex)       lcdControl(0xC0 | ((specIndex) << 3))

// Set ICONRAM address
#define CONTRAST_CTRL_REGISTER          0x10
#define SET_ICONRAM_ADDR(addr)          lcdControl(0x40 | (addr))

// Set double height
#define LINE_1_AND_2                    0x01
#define LINE_2_AND_3                    0x02
#define NORMAL_DISPLAY                  0x00
#define SET_DOUBLE_HEIGHT(options)      lcdControl(0x08 | (options))

// LCD lines
#define LCD_LINE_COUNT                  3
#define LCD_LINE_LENGTH                 16



/***********************************************************************************
 * LOCAL VARIABLES
 */
static char pLcdLineBuffer[LCD_LINE_LENGTH];

/***********************************************************************************
 * LOCAL FUNCTIONS
 */
static void lcdControl(uint8 command);
static void lcdWrite(uint8 data);
static void lcdWriteMany(const char *pData, uint16 count);
static void lcdWriteLine(uint8 line, const char *pText);

/***********************************************************************************
 * @fn          lcdControl
 *
 * @brief       Send command to display
 *
 * @param       uint8 command
 *
 * @return      none
 */
static void lcdControl(uint8 command)
{
    LCD_SPI_BEGIN();
    LCD_DO_CONTROL();
    LCD_SPI_TX(command);
    LCD_SPI_WAIT_RXRDY();
    LCD_SPI_END();
}

/***********************************************************************************
 * @fn          lcdWrite
 *
 * @brief       Write character to display
 *
 * @param       uint8 data - character to display
 *
 * @return      none
 */
static void lcdWrite(uint8 data)
{
    LCD_SPI_BEGIN();
    LCD_DO_WRITE();
    LCD_SPI_TX(data);
    LCD_SPI_WAIT_RXRDY();
    LCD_SPI_END();
}

/***********************************************************************************
 * @fn          lcdWriteMany
 *
 * @brief       Write text to display
 *
 * @param       char *pData - text buffer
 *              uint16 count - number of bytes
 *
 * @return      none
 */
static void lcdWriteMany(const char *pData, uint16 count)
{
    LCD_SPI_BEGIN();
    LCD_DO_WRITE();
    while (count--) {
        LCD_SPI_TX(*(pData++));
        LCD_SPI_WAIT_RXRDY();
    }
    LCD_SPI_END();
}

/***********************************************************************************
 * @fn          lcdWriteLine
 *
 * @brief       Write line on display
 *
 * @param       uint8 line - display line
 *              char *pText - text buffer to write
 *
 * @return      none
 */
static void lcdWriteLine(uint8 line, const char *pText)
{
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH);
    lcdWriteMany(pText, LCD_LINE_LENGTH);
}


/***********************************************************************************
 * GLOBAL FUNCTIONS
 */

 /***********************************************************************************
 * @fn          halLcdInit
 *
 * @brief       Initalise LCD
 *
 * @param       none
 *
 * @return      none
 */
void halLcdInit(void)
{
    // Initialize I/O
    LCD_CTRL_INIT_PORTS();

    // Perform the initialization sequence
    FUNCTION_SET(CGRAM | COM_FORWARD | THREE_LINE);
    halLcdSetContrast(15);
    SET_POWER_SAVE_MODE(OSC_OFF | POWER_SAVE_ON);
    SET_POWER_CTRL(VOLTAGE_DIVIDER_ON | CONVERTER_AND_REG_ON);
    SET_BIAS_CTRL(BIAS_1_5);
    halMcuWaitMs(21); // 21 ms

    // Clear the display
    halLcdClear();
    halLcdClearAllSpecChars();
    SET_DISPLAY_CTRL(DISPLAY_CTRL_ON | DISPLAY_CTRL_BLINK_OFF | DISPLAY_CTRL_CURSOR_OFF);

}

/***********************************************************************************
 * @fn          halLcdClear
 *
 * @brief       Clear all lines on display
 *
 * @param       none
 *
 * @return      none
 */
void halLcdClear(void)
{
    uint8 n;
    SET_DDRAM_ADDR(0x00);
    for (n = 0; n < (LCD_LINE_COUNT * LCD_LINE_LENGTH); n++) {
        lcdWrite(' ');
    }
}

/***********************************************************************************
 * @fn          halLcdSetContrast
 *
 * @brief       Set display contrast
 *
 * @param       uint8 value - contrast value
 *
 * @return      none
 */
void halLcdSetContrast(uint8 value)
{
    SET_ICONRAM_ADDR(CONTRAST_CTRL_REGISTER);
    lcdWrite(value);
}

/***********************************************************************************
 * @fn          halLcdGetLineLength
 *
 * @brief       Get max number of characters on each line
 *
 * @param       none
 *
 * @return      uint8 - number of characters on a line
 */
uint8 halLcdGetLineLength(void)
{
    return LCD_LINE_LENGTH;
}

/***********************************************************************************
 * @fn          halLcdGetNumLines
 *
 * @brief       Get max number of lines in display
 *
 * @param       none
 *
 * @return      uint8 - number of characters on a line
 */
uint8 halLcdGetNumLines(void)
{
    return LCD_LINE_COUNT;
}

/***********************************************************************************
 * @fn          halLcdWriteChar
 *
 * @brief       Write single character
 *
 * @param       uint8 line - display line
 *              uint8 col - column
 *              char text - character to display
 *
 * @return      none
 */
void halLcdWriteChar(uint8 line, uint8 col, char text)
{
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH + col);
    lcdWrite(text);
}

/***********************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write line on display
 *
 * @param       uint8 line - display line
 *              char *pLine - pointer to text buffer to write
 *
 * @return      none
 */
void halLcdWriteLine(uint8 line, const char *pLine)
{
    uint8 n;
    if (pLine) {
        for (n = 0; n < LCD_LINE_LENGTH; n++) {
            if (*pLine!='\0') {
                pLcdLineBuffer[n] = *(pLine++);
            } else {
                pLcdLineBuffer[n] = ' ';
            }
        }
        lcdWriteLine(line, pLcdLineBuffer);
    }
}

/***********************************************************************************
 * @fn          halLcdWriteLines
 *
 * @brief       Write lines on display
 *
 * @param       char *pLine1 - pointer to text buffer for line 1
 *              char *pLine2 - pointer to text buffer for line 2
 *              char *pLine3 - pointer to text buffer for line 3
 *
 * @return      none
 */
void halLcdWriteLines(const char *pLine1, const char *pLine2, const char *pLine3)
{
    if (pLine1) halLcdWriteLine(1, pLine1);
    if (pLine2) halLcdWriteLine(2, pLine2);
    if (pLine3) halLcdWriteLine(3, pLine3);
}




/***********************************************************************************
 * @fn          halLcdClearAllSpecChars
 *
 * @brief       Clear special characters
 *
 * @param       none
 *
 * @return      none
 */
void halLcdClearAllSpecChars(void)
{
    uint8 n;
    SET_GCRAM_CHAR(0);
    for (n = 0; n < (8 * 8); n++) {
        lcdWrite(0x00);
    }
}

/***********************************************************************************
 * @fn          halLcdCreateSpecChar
 *
 * @brief       Create special character
 *
 * @param       uint8 index
 *              const char *p5x8Spec
 *
 * @return      none
 */
void halLcdCreateSpecChar(uint8 index, const char *p5x8Spec)
{
    uint8 n;
    SET_GCRAM_CHAR(index);
    for (n = 0; n < 8; n++) {
        lcdWrite(p5x8Spec[n]);
    }
}

/***********************************************************************************
 * @fn          halLcdWriteSpecChar
 *
 * @brief       Write special character
 *
 * @param       uint8 line  - display line
 *              uint8 col   - column
 *              uint8 index - index of spec character
 *
 * @return      none
 */
void halLcdWriteSpecChar(uint8 line, uint8 col, uint8 index)
{
    SET_DDRAM_ADDR((line - 1) * LCD_LINE_LENGTH + col);
    lcdWrite(index);
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

