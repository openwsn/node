/**************************************************************************************************
  Filename:       hal_lcd.c
  Revised:        $Date: 2007-11-01 08:44:53 -0700 (Thu, 01 Nov 2007) $
  Revision:       $Revision: 15821 $

  Description:    This file contains the interface to the HAL LCD Service.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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
**************************************************************************************************/

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "hal_lcd.h"
#include "OSAL.h"
#include "OnBoard.h"

#ifdef ZTOOL_PORT
  #include "DebugTrace.h"
#endif

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/
#define LCD_MAX_BUF 25

// General I/O definitions
#define IO_GIO  0  // General purpose I/O
#define IO_PER  1  // Peripheral function
#define IO_IN   0  // Input pin
#define IO_OUT  1  // Output pin
#define IO_PUD  0  // Pullup/pulldn input
#define IO_TRI  1  // Tri-state input
#define IO_PUP  0  // Pull-up input pin
#define IO_PDN  1  // Pull-down input pin

// LCD port/bit definitions
#define LCD_CLK_PORT  2
#define LCD_CLK_PIN   0  // P2_0
#define LCD_DATA_PORT 1
#define LCD_DATA_PIN  2  // P1_2

/* LCD Line Address */
#define LCD_LINE1_ADDR  0x80
#define LCD_LINE2_ADDR  0xC0

// LCD device definitions
#define LCD_ADDR  0x76  // SM-Bus address of the LCD controller
#define CH1_ADDR  0x08
#define LCD_RS_0  0x00  // RS = 0 => selects instruction register for write/busy flag
#define LCD_RS_1  0x40  // RS = 1 => selects the data register for both read and write

/**************************************************************************************************
 *                                           MACROS
 **************************************************************************************************/

// Removed to allow PNAME macro to expand -
// Problem: in ioCC2430.h, #define P PSW_bit.P
#undef P

  /* I/O PORT CONFIGURATION */
#define CAT1(x,y) x##y  // Concatenates 2 strings
#define CAT2(x,y) CAT1(x,y)  // Forces evaluation of CAT1

// LCD port I/O defintions
// Builds I/O port name: PNAME(1,INP) ==> P1INP
#define PNAME(y,z) CAT2(P,CAT2(y,z))
// Builds I/O bit name: BNAME(1,2) ==> P1_2
#define BNAME(port,pin) CAT2(CAT2(P,port),CAT2(_,pin))

#define LCD_SCL BNAME(LCD_CLK_PORT, LCD_CLK_PIN)
#define LCD_SDA BNAME(LCD_DATA_PORT, LCD_DATA_PIN)

// LCD port I/O defintions
#define LCD_SCL BNAME(LCD_CLK_PORT, LCD_CLK_PIN)
#define LCD_SDA BNAME(LCD_DATA_PORT, LCD_DATA_PIN)

#define IO_DIR_PORT_PIN(port, pin, dir) \
{\
  if ( dir == IO_OUT ) \
    PNAME(port,DIR) |= (1<<(pin)); \
  else \
    PNAME(port,DIR) &= ~(1<<(pin)); \
}

#define LCD_DATA_HIGH()\
{ \
  IO_DIR_PORT_PIN(LCD_DATA_PORT, LCD_DATA_PIN, IO_IN); \
}

#define LCD_DATA_LOW() \
{ \
  IO_DIR_PORT_PIN(LCD_DATA_PORT, LCD_DATA_PIN, IO_OUT); \
  LCD_SDA = 0;\
}

#define IO_FUNC_PORT_PIN(port, pin, func) \
{ \
  if( port < 2 ) \
  { \
    if ( func == IO_PER ) \
      PNAME(port,SEL) |= (1<<(pin)); \
    else \
      PNAME(port,SEL) &= ~(1<<(pin)); \
  } \
  else \
  { \
    if ( func == IO_PER ) \
      P2SEL |= (1<<(pin>>1)); \
    else \
      P2SEL &= ~(1<<(pin>>1)); \
  } \
}

#define IO_IMODE_PORT_PIN(port, pin, mode) \
{ \
  if ( mode == IO_TRI ) \
    PNAME(port,INP) |= (1<<(pin)); \
  else \
    PNAME(port,INP) &= ~(1<<(pin)); \
}

#define IO_PUD_PORT(port, dir) \
{ \
  if ( dir == IO_PDN ) \
    P2INP |= (1<<(port+5)); \
  else \
    P2INP &= ~(1<<(port+5)); \
}

/**************************************************************************************************
 *                                          TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                       GLOBAL VARIABLES
 **************************************************************************************************/
#ifdef LCD_HW
static uint8 *Lcd_Line1;
#endif

/**************************************************************************************************
 *                                       FUNCTIONS - API
 **************************************************************************************************/
#if (defined LCD_HW) && (HAL_LCD == TRUE)
static void initLcd( void );
static void initSmb( void );
static void lcdUpdateLine( uint8 line, uint8 *pLine );
static byte lcdConvertChar( byte aChar );
static void smbSend( uint8 *buffer, uint8 len );
static bool smbSendByte( uint8 dByte );
static void smbWrite( bool dBit );
static void smbClock( bool dir );
static void smbStart( void );
static void smbStop( void );
static void smbWait( void );
#endif

/**************************************************************************************************
 * @fn      HalLcdInit
 *
 * @brief   Initilize LCD Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 **************************************************************************************************/
void HalLcdInit(void)
{
#if (HAL_LCD == TRUE)

#ifdef LCD_HW
  Lcd_Line1 = NULL;
  initLcd();
#endif

#endif /* HAL_LCD */

}

/*************************************************************************************************
 *                    LCD EMULATION FUNCTIONS
 *
 * Some evaluation boards are equipped with Liquid Crystal Displays
 * (LCD) which may be used to display diagnostic information. These
 * functions provide LCD emulation, sending the diagnostic strings
 * to Z-Tool via the RS232 serial port. These functions are enabled
 * when the "LCD_SUPPORTED" compiler flag is placed in the makefile.
 *
 * Most applications update both lines (1 and 2) of the LCD whenever
 * text is posted to the device. This emulator assumes that line 1 is
 * updated first (saved locally) and the formatting and send operation
 * is triggered by receipt of line 2. Nothing will be transmitted if
 * only line 1 is updated.
 *
 *************************************************************************************************/


/**************************************************************************************************
 * @fn      HalLcdWriteString
 *
 * @brief   Write a string to the LCD
 *
 * @param   str    - pointer to the string that will be displayed
 *          option - display options
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteString ( char *str, uint8 option)
{
#if (HAL_LCD == TRUE)

#ifdef LCD_SD
  byte x;
  byte bln;
  byte sln;
  char *buf;

  if ( Lcd_Line1 == NULL )
  {
    // Set up system start-up message
    Lcd_Line1 = osal_mem_alloc( MAX_LCD_CHARS+1 );
    HalLcdWriteString( "Figure8 Wireless", HAL_LCD_LINE_1 );
  }

  sln = (byte)osal_strlen( str );

  // Check boundries
  if ( sln > MAX_LCD_CHARS )
    sln = MAX_LCD_CHARS;

  if ( option == HAL_LCD_LINE_1 ) {
    // Line 1 gets saved for later
    osal_memcpy( Lcd_Line1, str, sln );
    Lcd_Line1[sln] = '\0';
  }
  else {
    // Line 2 triggers action
    x = (byte)osal_strlen( (char*)Lcd_Line1 );
    bln = x + 1 + sln + 1;
    buf = osal_mem_alloc( bln );
    if ( buf != NULL ) {
      // Concatenate strings
      osal_memcpy( buf, Lcd_Line1, x );
      buf[x++] = ' ';
      osal_memcpy( &buf[x], str, sln );
      buf[x+sln] = '\0';
      // Send it out
#ifdef ZTOOL_PORT
      debug_str( (byte*)buf );
#endif
      osal_mem_free( buf );
    }
  }
#endif // LCD_SD

#ifdef LCD_HW
  lcdUpdateLine( option, (byte*)str );
#endif

#endif /* HAL_LCD */

}

/**************************************************************************************************
 * @fn      HalLcdWriteValue
 *
 * @brief   Write a value to the LCD
 *
 * @param   value  - value that will be displayed
 *          radix  - 8, 10, 16
 *          option - display options
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option)
{
#if (HAL_LCD == TRUE)
  uint8 buf[LCD_MAX_BUF];

  _ltoa( value, &buf[0], radix );
  HalLcdWriteString( (char*)buf, option );
#endif /* HAL_LCD */
}

/**************************************************************************************************
 * @fn      HalLcdWriteScreen
 *
 * @brief   Write a value to the LCD
 *
 * @param   line1  - string that will be displayed on line 1
 *          line2  - string that will be displayed on line 2
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteScreen( char *line1, char *line2 )
{
#if (HAL_LCD == TRUE)
  HalLcdWriteString( line1, HAL_LCD_LINE_1 );
  HalLcdWriteString( line2, HAL_LCD_LINE_2 );
#endif /* HAL_LCD */
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title  -
 *          value  -
 *          format -
 *          line   -
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line )
{
#if (HAL_LCD == TRUE)
  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  HalLcdWriteString( (char*)buf, line );		
#endif /* HAL_LCD */
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title   -
 *          value1  -
 *          format1 -
 *          value2  -
 *          format2 -
 *          line    -
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1,
                                  uint16 value2, byte format2, uint8 line )
{
#if (HAL_LCD == TRUE)
  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  if ( tmpLen )
  {
    osal_memcpy( buf, title, tmpLen );
    buf[tmpLen++] = ' ';
  }

  err = (uint32)(value1);
  _ltoa( err, &buf[tmpLen], format1 );
  tmpLen = (uint8)osal_strlen( (char*)buf );

  buf[tmpLen++] = ',';
  buf[tmpLen++] = ' ';
  err = (uint32)(value2);
  _ltoa( err, &buf[tmpLen], format2 );

  HalLcdWriteString( (char *)buf, line );		
#endif /* HAL_LCD */
}

/**************************************************************************************************
 * @fn      HalLcdDisplayPercentBar
 *
 * @brief   Display percentage bar on the LCD
 *
 * @param   title   -
 *          value   -
 *
 * @return  None
 **************************************************************************************************/
void HalLcdDisplayPercentBar( char *title, uint8 value )
{
#if (HAL_LCD == TRUE)
  uint8 percent;
  uint8 leftOver;
  uint8 buf[17];
  uint32 err;
  uint8 x;

  /* Write the title: */
  HalLcdWriteString( title, HAL_LCD_LINE_1 );

  if ( value > 100 )
    value = 100;

  /* convert to blocks */
  percent = (byte)(value / 10);
  leftOver = (byte)(value % 10);

  /* Make window */
  osal_memcpy( buf, "[          ]  ", 15 );

  for ( x = 0; x < percent; x ++ )
  {
    buf[1+x] = '>';
  }

  if ( leftOver >= 5 )
    buf[1+x] = '+';

  err = (uint32)value;
  _ltoa( err, (uint8*)&buf[13], 10 );

  HalLcdWriteString( (char*)buf, HAL_LCD_LINE_2 );
#endif /* HAL_LCD */
}

#if (defined LCD_HW) && (HAL_LCD == TRUE)
/*********************************************************************
 * @fn      initLcd
 * @brief   Initializes LCD I/O bus and LCD device
 * @param   void
 * @return  void
 */
static void initLcd( void )
{
  uint8 buffer[8];

  // Initialize the serial I/O bus
  initSmb();

  // Load LCD initialization message
  buffer[0] = LCD_ADDR;
  buffer[1] = LCD_RS_0;  // Instruction Register
  buffer[2] = 0x0C;      // Display control         D =  1:      Display On
  //                                                C =  0:      Cursor Off
  //                                                B =  0:      Cursor character blink off
  buffer[3] = 0x21;      // Function set            H =  1:      Use extended instruction set
  buffer[4] = 0xA0;      // Set DDRAM address       ADD = 0x20
  buffer[5] = 0x07;      // Display configuration   P =  1:      Column data right to left
  //                                                Q =  1:      Row data, bottom to top
  buffer[6] = 0x34;      // Function set            DL=  0:      4 bits
  //                                                M =  1:      2-line by 16 display
  //                                                SL=  0:      MUX1:18
  //                                                H =  0:      Use basic instruction set
  buffer[7] = 0x01;      // Clearing display

  // Send message to LCD device
  smbSend( buffer, 8 );

}

/*********************************************************************
 * @fn      lcdUpdateLine
 * @brief   Updates one line of the LCD display
 * @param   line - LCD line numberptr to string going to LCD line 1
 * @param   p2 - ptr to string going to LCD line 2
 * @return  void
 */
static void lcdUpdateLine( uint8 line, uint8 *pLine )
{
  uint8 i;
  uint8 chr;
  uint8 addr;
  uint8 *buffer;

  if ( line == HAL_LCD_LINE_1 )
    addr = LCD_LINE1_ADDR;
  else
    addr = LCD_LINE2_ADDR;

  // Get a buffer to work with
  buffer = osal_mem_alloc( 2+HAL_LCD_MAX_CHARS );
  if ( buffer != NULL )
  {
    // Build and send control string
    buffer[0] = LCD_ADDR;
    buffer[1] = LCD_RS_0;
    buffer[2] = addr;
    smbSend( buffer, 3 );

    // Build and send message string
    buffer[0] = LCD_ADDR;
    buffer[1] = LCD_RS_1;
    // Convert and save message bytes
    for( i = 2; i < 2+HAL_LCD_MAX_CHARS; i++ )
    {
      chr = *pLine++;
      if ( chr == '\0' )
      {
        chr = lcdConvertChar( ' ' );
        break;
      }
      else
        buffer[i] = lcdConvertChar( chr );
    }

    // Fill remainder of line with blanks
    for( ; i < 2+HAL_LCD_MAX_CHARS; i++ )
      buffer[i] = chr;

    // Put it on the display
    smbSend( buffer, 2+HAL_LCD_MAX_CHARS );

    // Give back buffer memory
    osal_mem_free( buffer );
  }
}

/*********************************************************************
 * @fn      lcdConvertChar
 * @brief   Converts an ASCII character to an LCD character (R sheet)
 * @param   aChar - ASCII character
 * @return  lChar - LCD character
 */
static byte lcdConvertChar( byte aChar )
{
  uint8 lChar;

  if ((aChar >= 'a') && (aChar <= 'z'))
    // Lower case
    lChar = aChar + ('a' - 0xE1);
  else if ((aChar >= 'A') && (aChar <= 'Z'))
    // Upper case
    lChar = aChar + ('A' - 0xC1);
  else if (((aChar >= ' ') && (aChar <= '#')) ||
           ((aChar >= '%') && (aChar <= '?')))
    // Sonme symbols
    lChar = aChar + (' ' - 0xA0);
  else
  {
    switch ( aChar )
    {
      case '$':
         lChar = 0x82;
         break;
      case '§':
         lChar = 0xDF;
         break;
      case '£':
         lChar = 0x81;
         break;
      case '@':
         lChar = 0x80;
         break;
      case '[':
         lChar = 0x8A;
         break;
      case ']':
         lChar = 0x54;
         break;
      case '_':
         lChar = 0x5A;
         break;
      case 'æ':
         lChar = 0x9D;
         break;
      case 'ø':
         lChar = 0x8C;
         break;
      case 'å':
         lChar = 0x8F;
         break;
      case 'Æ':
         lChar = 0x9C;
         break;
      case 'Ø':
         lChar = 0x8B;
         break;
      case 'Å':
         lChar = 0x8E;
         break;
      case 0x10:
         lChar = 0x10;  // Left arrow
         break;
      case 0x11:
         lChar = 0x20;  // Right arrow
         break;
      case 0x12:
         lChar = 0x12;  // Up arrow
         break;
      case 0x13:
         lChar = 0x30;  // Down arrow
         break;
      default:
         lChar = 0x30;
         break;
    }
  }

  return ( lChar );
}

/*********************************************************************
 * @fn      initSmb
 * @brief   Initializes two-wire serial I/O bus
 * @param   void
 * @return  void
 */
static void initSmb( void )
{
  // Set port pins as inputs
  IO_DIR_PORT_PIN( LCD_CLK_PORT, LCD_CLK_PIN, IO_IN );
  IO_DIR_PORT_PIN( LCD_DATA_PORT, LCD_DATA_PIN, IO_IN );

  // Set for general I/O operation
  IO_FUNC_PORT_PIN( LCD_CLK_PORT, LCD_CLK_PIN, IO_GIO );
  IO_FUNC_PORT_PIN( LCD_DATA_PORT, LCD_DATA_PIN, IO_GIO );

  // Set I/O mode for pull-up/pull-down
  IO_IMODE_PORT_PIN( LCD_CLK_PORT, LCD_CLK_PIN, IO_PUD );
  IO_IMODE_PORT_PIN( LCD_DATA_PORT, LCD_DATA_PIN, IO_PUD );

  // Set pins to pull-up
  IO_PUD_PORT( LCD_CLK_PORT, IO_PUP );
  IO_PUD_PORT( LCD_DATA_PORT, IO_PUP );
}

/*********************************************************************
 * @fn      smbSend
 * @brief   Sends buffer contents to SM-Bus device
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in buffer
 * @return  void
 */
static void smbSend( uint8 *buffer, uint8 len )
{
  uint8 i;

  smbStart();
  for ( i = 0; i < len; i++ )
  {
    while ( !smbSendByte( buffer[i] ) );  // Send until ACK received
  }
  smbStop();
}

/*********************************************************************
 * @fn      smbSendByte
 * @brief   Serialize and send one byte to SM-Bus device
 * @param   dByte - data byte to send
 * @return  ACK status - 0=none, 1=received
 */
static bool smbSendByte( uint8 dByte )
{
  uint8 i;

  for ( i = 0; i < 8; i++ )
  {
    // Send the MSB
    smbWrite( dByte & 0x80 );
    // Next bit into MSB
    dByte <<= 1;
  }
  smbClock( 0 );
  LCD_DATA_HIGH();
  smbClock( 1 );

  return ( !LCD_SDA );  // Return ACK status
}

/*********************************************************************
 * @fn      smbWrite
 * @brief   Send one bit to SM-Bus device
 * @param   dBit - data bit to clock onto SM-Bus
 * @return  void
 */
static void smbWrite( bool dBit )
{
  smbClock( 0 );
  smbWait();
  if ( dBit )
  {
    LCD_DATA_HIGH();
  }
  else
  {
    LCD_DATA_LOW();
  }
  smbClock( 1 );
  smbWait();
}

/*********************************************************************
 * @fn      smbClock
 * @brief   Clocks the SM-Bus. If a negative edge is going out, the
 *          I/O pin is set as an output and driven low. If a positive
 *          edge is going out, the pin is set as an input and the pin
 *          pull-up drives the line high. This way, the slave device
 *          can hold the node low if longer setup time is desired.
 * @param   dir - clock line direction
 * @return  void
 */
static void smbClock( bool dir )
{
  if ( dir )
  {
    IO_DIR_PORT_PIN( LCD_CLK_PORT, LCD_CLK_PIN, IO_IN );
  }
  else
  {
    IO_DIR_PORT_PIN( LCD_CLK_PORT, LCD_CLK_PIN, IO_OUT );
    LCD_SCL = 0;
  }
  smbWait();
}

/*********************************************************************
 * @fn      smbStart
 * @brief   Initiates SM-Bus communication. Makes sure that both the
 *          clock and data lines of the SM-Bus are high. Then the data
 *          line is set high and clock line is set low to start I/O.
 * @param   void
 * @return  void
 */
static void smbStart( void )
{
  // Wait for both clock and data line high
  while ( !( LCD_SCL && LCD_SDA) );
  LCD_DATA_LOW();
  smbWait();
  smbClock( 0 );
}

/*********************************************************************
 * @fn      smbStop
 * @brief   Terminates SM-Bus communication. Waits unitl the data line
 *          is low and the clock line is high. Then sets the data line
 *          high, keeping the clock line high to stop I/O.
 * @param   void
 * @return  void
 */
static void smbStop( void )
{
  // Wait for clock high and data low
  while ( !( LCD_SCL && !LCD_SDA) );
  smbClock( 0 );
  LCD_DATA_HIGH();
  smbWait();
  smbClock( 1 );
}

/*********************************************************************
 * @fn      smbWait
 * @brief   Wastes a fixed amount of some time.
 * @param   void
 * @return  void
 */
static void smbWait( void )
{
  uint8 i = 0x01;
  while ( i-- );
}
#endif // LCD_HW & HAL_LCD

/**************************************************************************************************
**************************************************************************************************/




