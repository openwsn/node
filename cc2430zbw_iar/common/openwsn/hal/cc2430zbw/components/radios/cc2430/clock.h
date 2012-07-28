/******************************************************************************
    Filename: clock.h

    This file defines interface for clock related functions for the
    CC243x family of RF system-on-chips from Texas Instruments.

******************************************************************************/
#ifndef _CLOCK_H
#define _CLOCK_H


/*******************************************************************************
 * INCLUDES
 */

#include "../../hal_comp_foundation.h"

// Include chip specific IO definition file
#if (chip == 2430)
#include "ioCC2430.h"
#endif
#if (chip == 2431)
#include "ioCC2431.h"
#endif



/*******************************************************************************
 * CONSTANTS
 */

/* SEE DATA SHEET FOR DETAILS ABOUT THE FOLLOWING BIT MASKS */

// Parameters for cc2430ClockSetMainSrc
#define CLOCK_SRC_XOSC      0  // High speed Crystal Oscillator Control
#define CLOCK_SRC_HFRC      1  // Low power RC Oscillator

// Parameters for cc2430ClockSelect32k
#define CLOCK_32K_XTAL      0  // 32.768 Hz crystal oscillator
#define CLOCK_32K_RCOSC     1  // 32 kHz RC oscillator

// Bit masks to check CLKCON register
#define CLKCON_OSC32K_BM    0x80  // bit mask, for the slow 32k clock oscillator
#define CLKCON_OSC_BM       0x40  // bit mask, for the system clock oscillator
#define CLKCON_TICKSPD_BM   0x38  // bit mask, for timer ticks output setting
#define CLKCON_CLKSPD_BM    0x01  // bit maks, for the clock speed

#define TICKSPD_DIV_1       (0x00 << 3)
#define TICKSPD_DIV_2       (0x01 << 3)
#define TICKSPD_DIV_4       (0x02 << 3)
#define TICKSPD_DIV_8       (0x03 << 3)
#define TICKSPD_DIV_16      (0x04 << 3)
#define TICKSPD_DIV_32      (0x05 << 3)
#define TICKSPD_DIV_64      (0x06 << 3)
#define TICKSPD_DIV_128     (0x07 << 3)

// Bit masks to check SLEEP register
#define SLEEP_XOSC_STB_BM   0x40  // bit mask, check the stability of XOSC
#define SLEEP_HFRC_STB_BM   0x20  // bit maks, check the stability of the High-frequency RC oscillator
#define SLEEP_OSC_PD_BM     0x04  // bit mask, power down system clock oscillator(s)


/*******************************************************************************
 * MACROS
 */

// Macro for checking status of the high frequency RC oscillator.
#define CC2430_IS_HFRC_STABLE(x)    (SLEEP & SLEEP_HFRC_STB_BM)

// Macro for checking status of the crystal oscillator
#define CC2430_IS_XOSC_STABLE(x)    (SLEEP & SLEEP_XOSC_STB_BM)

// Macro for getting the clock division factor
#define CC2430_GET_CLKSPD(x)        (CLKCON & CLKCON_CLKSPD_BM)


// Macro for getting the timer tick division factor.
#define CC2430_GET_TICKSPD(x)       ((CLKCON & CLKCON_TICKSPD_BM) >> 3)

// Macro for setting the clock division factor, x value from 0b000 to 0b111
#define CC2430_SET_TICKSPD(x)        st( CLKCON = ((((x) << 3) & 0x38)  \
                                                    | (CLKCON & 0xC7)); \
		)

// Macro for setting the timer tick division factor, x value from 0b000 to 0b111
#define CC2430_SET_CLKSPD(x)         st( CLKCON = (((x) & 0x07)         \
                                                    | (CLKCON & 0xF8)); \
									 )



/*******************************************************************************
 * FUNCTIONS
 */

void clockSetMainSrc(uint8 source);
void clockSelect32k(uint8 source);


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
