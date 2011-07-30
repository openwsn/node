/***********************************************************************************
  Filename:     assy_ccmsp2618_cc2520em.h

  Description:  Defines connections between the CCMSP-EM430F2618 and the CC2520EM

***********************************************************************************/

#ifndef ASSY_CCMSP2618_CC2520EM_H
#define ASSY_CCMSP2618_CC2520EM_H

/*************************************************7**********************************
* INCLUDES
*/
#include <msp430x26x.h>
#include <hal_msp430.h>
#include <hal_digio.h>

/***********************************************************************************
* MACROS
*/

// MCU Pin Access
#ifndef MCU_PIN_DIR_OUT
#define MCU_PIN_DIR_OUT(port,bit)       st( P##port##DIR |= BV(bit); )
#endif
#ifndef MCU_PIN_DIR_IN
#define MCU_PIN_DIR_IN(port,bit)        st( P##port##DIR &= ~BV(bit); )
#endif

// CC2520 I/O Definitions
// Basic I/O pin setup
#define CC2520_BASIC_IO_DIR_INIT()      st( MCU_PIN_DIR_OUT(5,7); MCU_PIN_DIR_OUT(1,0); )

// MCU port control for SPI interface
#define CC2520_DISABLE_SPI_FUNC()       st( P5SEL &= ~(BV(1) | BV(2) | BV(3)); )
#define CC2520_ENABLE_SPI_FUNC()        st( P5SEL |= BV(1) | BV(2) | BV(3); )

// GPIO pin direction control
#define CC2520_GPIO_DIR_OUT(pin) \
    st( \
        if (pin == 0) CC2520_GPIO0_DIR_OUT(); \
            if (pin == 1) CC2520_GPIO1_DIR_OUT(); \
                if (pin == 2) CC2520_GPIO2_DIR_OUT(); \
                    if (pin == 3) CC2520_GPIO3_DIR_OUT(); \
                        if (pin == 4) CC2520_GPIO4_DIR_OUT(); \
                            if (pin == 5) CC2520_GPIO5_DIR_OUT(); \
                                )
#define CC2520_GPIO0_DIR_OUT()          MCU_PIN_DIR_IN(1,3)
#define CC2520_GPIO1_DIR_OUT()          MCU_PIN_DIR_IN(1,5)
#define CC2520_GPIO2_DIR_OUT()          MCU_PIN_DIR_IN(1,6)
#define CC2520_GPIO3_DIR_OUT()          MCU_PIN_DIR_IN(1,1)
#define CC2520_GPIO4_DIR_OUT()          MCU_PIN_DIR_IN(1,2)
#define CC2520_GPIO5_DIR_OUT()          MCU_PIN_DIR_IN(1,7)
#define CC2520_GPIO_DIR_IN(pin) \
    st( \
        if (pin == 0) CC2520_GPIO0_DIR_IN(); \
            if (pin == 1) CC2520_GPIO1_DIR_IN(); \
                if (pin == 2) CC2520_GPIO2_DIR_IN(); \
                    if (pin == 3) CC2520_GPIO3_DIR_IN(); \
                        if (pin == 4) CC2520_GPIO4_DIR_IN(); \
                            if (pin == 5) CC2520_GPIO5_DIR_IN(); \
                                )
#define CC2520_GPIO0_DIR_IN()           MCU_PIN_DIR_OUT(1,3)
#define CC2520_GPIO1_DIR_IN()           MCU_PIN_DIR_OUT(1,5)
#define CC2520_GPIO2_DIR_IN()           MCU_PIN_DIR_OUT(1,6)
#define CC2520_GPIO3_DIR_IN()           MCU_PIN_DIR_OUT(1,1)
#define CC2520_GPIO4_DIR_IN()           MCU_PIN_DIR_OUT(1,2)
#define CC2520_GPIO5_DIR_IN()           MCU_PIN_DIR_OUT(1,7)

// Outputs: Power and reset control
#define CC2520_RESET_OPIN(v)            MCU_IO_SET(5,7,v)
#define CC2520_VREG_EN_OPIN(v)          MCU_IO_SET(1,0,v)

// Outputs: GPIO
#define CC2520_GPIO0_OPIN(v)			MCU_IO_SET(1,3,v)
#define CC2520_GPIO1_OPIN(v)			MCU_IO_SET(1,5,v)
#define CC2520_GPIO2_OPIN(v)			MCU_IO_SET(1,6,v)
#define CC2520_GPIO3_OPIN(v)			MCU_IO_SET(1,1,v)
#define CC2520_GPIO4_OPIN(v)			MCU_IO_SET(1,2,v)
#define CC2520_GPIO5_OPIN(v)			MCU_IO_SET(1,7,v)

// Outputs: SPI interface
#define CC2520_CSN_OPIN(v)              MCU_IO_SET(5,0,v)
#define CC2520_SCLK_OPIN(v)             MCU_IO_SET(5,3,v)
#define CC2520_MOSI_OPIN(v)             MCU_IO_SET(5,1,v)

// Inputs: GPIO
#define CC2520_GPIO0_IPIN				MCU_IO_GET(1,3)
#define CC2520_GPIO1_IPIN				MCU_IO_GET(1,5)
#define CC2520_GPIO2_IPIN				MCU_IO_GET(1,6)
#define CC2520_GPIO3_IPIN				MCU_IO_GET(1,1)
#define CC2520_GPIO4_IPIN				MCU_IO_GET(1,2)
#define CC2520_GPIO5_IPIN				MCU_IO_GET(1,7)

// Inputs: SPI interface
#define CC2520_MISO_IPIN                MCU_IO_GET(5,2)
#define CC2520_MISO_OPIN(v)             MCU_IO_SET(5,2,v) // For use in LPM
#define CC2520_MISO_DIR_IN()            MCU_PIN_DIR_OUT(5,2)
#define CC2520_MISO_DIR_OUT()           MCU_PIN_DIR_IN(5,2)

// SPI register definitions
#define CC2520_SPI_TX_REG               (UCB1TXBUF)
#define CC2520_SPI_RX_REG               (UCB1RXBUF)
#define CC2520_SPI_RX_IS_READY()        (UC1IFG & UCB1RXIFG)
#define CC2520_SPI_RX_NOT_READY()       (UC1IFG &= ~UCB1RXIFG)

// SPI access macros
#define CC2520_SPI_BEGIN()              st( CC2520_CSN_OPIN(0); )
#define CC2520_SPI_TX(x)                st( CC2520_SPI_RX_NOT_READY(); CC2520_SPI_TX_REG = x; )
#define CC2520_SPI_RX()                 (CC2520_SPI_RX_REG)
#define CC2520_SPI_WAIT_RXRDY()         st( while (!CC2520_SPI_RX_IS_READY()); )
#define CC2520_SPI_END()                st( CC2520_CSN_OPIN(1); )


// Platform specific definitions
// IRQ on GPIO0
#define CC2520_GPIO0_IRQ_INIT()         st( P1IES &= ~0x08; CC2520_GPIO0_IRQ_CLEAR(); )
#define CC2520_GPIO0_IRQ_ENABLE()       st( P1IE  |=  0x08; )
#define CC2520_GPIO0_IRQ_DISABLE()      st( P1IE  &= ~0x08; )
#define CC2520_GPIO0_IRQ_CLEAR()        st( P1IFG &= ~0x08; )

// IRQ on GPIO1
#define CC2520_GPIO1_IRQ_INIT()         st( P1IES &= ~0x20; CC2520_GPIO1_IRQ_CLEAR(); )
#define CC2520_GPIO1_IRQ_ENABLE()       st( P1IE  |=  0x20; )
#define CC2520_GPIO1_IRQ_DISABLE()      st( P1IE  &= ~0x20; )
#define CC2520_GPIO1_IRQ_CLEAR()        st( P1IFG &= ~0x20; )

/***********************************************************************************
* GLOBAL VARIABLES
*/
extern const digioConfig pinRadio_GPIO0;


/***********************************************************************************
* PUBLIC FUNCTIONS
*/
void halAssyInit(void);


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


#endif
