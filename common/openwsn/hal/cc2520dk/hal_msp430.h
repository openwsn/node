/***********************************************************************************
    Filename: hal_msp430.h

***********************************************************************************/

#ifndef HAL_MSP430_H
#define HAL_MSP430_H

/***********************************************************************************
* INCLUDES
*/

#include <hal_types.h>
#include <hal_defs.h>
#include <hal_board.h>

/***********************************************************************************
* MACROS
*/

#define MCU_IO_TRISTATE   1
#define MCU_IO_PULLUP     2
#define MCU_IO_PULLDOWN   3


//----------------------------------------------------------------------------------
//  Macros for simple configuration of IO pins on MSP430
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL(port, pin)   MCU_IO_PERIPHERAL_PREP(port, pin)
#define MCU_IO_INPUT(port, pin, func)  MCU_IO_INPUT_PREP(port, pin, func)
#define MCU_IO_OUTPUT(port, pin, val)  MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_SET(port, pin, val)     MCU_IO_SET_PREP(port, pin, val)
#define MCU_IO_SET_HIGH(port, pin)     MCU_IO_SET_HIGH_PREP(port, pin)
#define MCU_IO_SET_LOW(port, pin)      MCU_IO_SET_LOW_PREP(port, pin)
#define MCU_IO_TGL(port, pin)          MCU_IO_TGL_PREP(port, pin)
#define MCU_IO_GET(port, pin)          MCU_IO_GET_PREP(port, pin)



//----------------------------------------------------------------------------------
// Safe polling for state change of variable set in interrupt context
//----------------------------------------------------------------------------------
#define SAFE_POLL(exit_condition, sleep_mode) \
    { istate_t key = halIntLock(); \
      while (!(exit_condition)) \
      { halMcuSetLowPowerMode((sleep_mode)); \
        key = halIntLock(); \
      } \
      halIntUnlock(key); }



//----------------------------------------------------------------------------------
//  Macros for internal use (the MCU_IO macros above need a new round in the preprocessor)
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL_PREP(port, pin)  st( P##port##SEL |= BIT##pin##; )

#ifdef P1REN_
#define MCU_IO_INPUT_PREP(port, pin, func) st( P##port##SEL &= ~BIT##pin##; \
                                               P##port##DIR &= ~BIT##pin##; \
                                               switch (func) { \
                                               case MCU_IO_PULLUP: \
                                                   P##port##REN |= BIT##pin##; \
                                                   P##port##OUT |= BIT##pin##; \
                                                   break; \
                                               case MCU_IO_PULLDOWN: \
                                                   P##port##REN |= BIT##pin##; \
                                                   P##port##OUT &= ~BIT##pin##; \
                                                   break; \
                                               default: \
                                                   P##port##REN &= ~BIT##pin##; \
                                                   break; } )
#else
#define MCU_IO_INPUT_PREP(port, pin, func) st( P##port##SEL &= ~BIT##pin##; \
                                               P##port##DIR &= ~BIT##pin##; )
#endif

#define MCU_IO_OUTPUT_PREP(port, pin, val) st( P##port##SEL &= ~BIT##pin##; \
                                               MCU_IO_SET(port, pin, val); \
                                               P##port##DIR |= BIT##pin##; )

#define MCU_IO_SET_HIGH_PREP(port, pin)    st( P##port##OUT |= BIT##pin##; )
#define MCU_IO_SET_LOW_PREP(port, pin)     st( P##port##OUT &= ~BIT##pin##; )

#define MCU_IO_SET_PREP(port, pin, val)    st( if (val) \
                                                   { MCU_IO_SET_HIGH(port, pin); } \
                                               else \
                                                   { MCU_IO_SET_LOW(port, pin); })

#define MCU_IO_TGL_PREP(port, pin)         st( P##port##OUT ^= BIT##pin##; )
#define MCU_IO_GET_PREP(port, pin)         (P##port##IN & BIT##pin##)


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


/* Code Composer support */

#if defined __TI_COMPILER_VERSION__
#define NOP()  _nop()
#define _PRAGMA(x) _Pragma(#x)

#define HAL_ISR_FUNC_DECLARATION(f,v)   \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
#define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
#define HAL_ISR_FUNCTION(f,v) HAL_ISR_FUNC_DECLARATION(f,v)


/*
 * Control bits in the processor status register, SR.
 */

#define __SR_GIE     (1<<3)
#define __SR_CPU_OFF (1<<4)
#define __SR_OSC_OFF (1<<5)
#define __SR_SCG0    (1<<6)
#define __SR_SCG1    (1<<7)


/*
 * Functions for controlling the processor operation modes.
 */

#define __low_power_mode_0() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF))

#define __low_power_mode_1() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG0))

#define __low_power_mode_2() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG1))

#define __low_power_mode_3()                    \
  (_bis_SR_register(  __SR_GIE                  \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1))

#define __low_power_mode_4()                    \
  (_bis_SR_register(  __SR_GIE                  \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1                \
                     | __SR_OSC_OFF))

#define __low_power_mode_off_on_exit()          \
  (_bic_SR_register_on_exit(  __SR_CPU_OFF      \
                             | __SR_SCG0        \
                             | __SR_SCG1        \
                             | __SR_OSC_OFF))

#endif

/***********************************************************************************
  Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

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
